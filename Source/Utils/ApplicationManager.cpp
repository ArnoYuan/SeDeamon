/*
 * ApplicationManager.cpp
 *
 *  Created on: Sep 21, 2017
 *      Author: root
 */

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <Time/Time.h>
#include <Time/Rate.h>
#include <Parameter/Parameter.h>
#include "ApplicationManager.h"

ApplicationManager::ApplicationManager()
{
  console = NS_NaviCommon::Console("SeDeamon");
  console.showDebug(true);
  applications.clear();
}

ApplicationManager::~ApplicationManager()
{
  log_threads.join_all();
  close(log_sender_id);
}

void ApplicationManager::loadParameters()
{
  NS_NaviCommon::Parameter parameter;
  parameter.loadConfigurationFile("log_server.xml");

  log_server_ip_ = parameter.getParameter("log_server_ip", "127.0.0.1");
  log_server_port_ = parameter.getParameter("log_server_port", 12349);

}

bool ApplicationManager::addApplication(ApplicationProperties& application)
{
  applications.push_back(application);
  return true;
}

bool ApplicationManager::runApplication(ApplicationProperties& application)
{
  if(application.getPID() == 0)
  {
    console.message("Load application: [ %s ]", application.getName().c_str());

    pid_t pid = fork();

    if(pid == 0)
    {
      //child process
      int result;

      std::stringstream cmdstr;

      std::string cmd;

      cmdstr << application.getName();
      cmdstr << " ";

      if(application.isVerboseMode())
      {
        cmdstr << "-v ";
      }

      if(application.getCore() >= 0)
      {

        cmdstr << "-c ";
        cmdstr << application.getCore();
        cmdstr << " ";
      }

      if(application.isRemoteLog())
      {
        cmdstr << "-l ";
      }

      cmd = cmdstr.str();

      char cmd_string[512] =
        {0};
      char *args[64];
      char **next = args;

      cmd.copy(cmd_string, cmd.size());

      console.debug("Prepare to run command: [%s]", cmd_string);

      char *temp = strtok(cmd_string, " \n");
      while(temp != NULL)
      {
        *next++ = temp;
        temp = strtok(NULL, " \n");
      }
      *next = NULL;

      result = execvp(application.getRunScript().c_str(), args);
      if(result < 0)
      {
        console.error("Run application [%s] failure!",
                      application.getRunScript().c_str());
        exit(EXIT_FAILURE);
        return false;
      }

    }
    else if(pid > 0)
    {
      //parent process
      application.setPID(pid);
    }
    else
    {
      //failure
      return false;
    }
  }
  return true;
}

bool ApplicationManager::runApplications()
{
  bool result = true;
  for(int i = 0; i < applications.size(); ++i)
  {
    if(!runApplication(applications[i]))
    {
      result = false;
    }
  }

  return result;
}

bool ApplicationManager::killApplication(std::string app_name)
{
  bool result = false;
  for(int i = 0; i < applications.size(); ++i)
  {
    if(applications[i].getName() == app_name)
    {
      result = killApplication(applications[i].getPID());
    }
  }
  return result;
}

bool ApplicationManager::killApplication(pid_t pid)
{
  if(pid != 0)
  {
    console.message("Killing application with pid : %d...", pid);
    if(kill(pid, SIGUSR1) != 0)
    {
      return false;
    }
    else
    {
      int try_times = 5;
      while(try_times--)
      {
        sleep (1);
        int result = waitpid(pid, NULL, WNOHANG);
        if(result == pid)
        {
          break;
        }
        else if(result == 0)
        {
          continue;
        }
        else
        {
          return false;
        }
      }

      if (try_times <= 0)
      {
        console.message("Application killing might be fail, kill it force.", pid);
        kill(pid, SIGKILL);
        return true;
      }
    }
  }
}

void ApplicationManager::removeApplication(pid_t pid)
{
  std::vector< ApplicationProperties >::iterator vit;

  for(vit = applications.begin(); vit != applications.end();)
  {
    if(((ApplicationProperties)*vit).getPID() == pid)
    {
      vit = applications.erase(vit);
    }
    else
    {
      ++vit;
    }
  }
}

void ApplicationManager::onApplicationQuit(pid_t pid, int status)
{
  console.warning("Application with PID: %d has been quit!", pid);
  removeApplication(pid);
  running = false;
}

bool ApplicationManager::killApplications()
{
  bool result = true;
  for(size_t i = 0; i < applications.size(); ++i)
  {
    if(!killApplication(applications[i].getPID()))
    {
      result = false;
    }
  }
  return result;
}

bool ApplicationManager::createLogSender()
{
  log_sender_id = socket(AF_INET, SOCK_DGRAM, 0);
  if(log_sender_id < 0)
  {
    console.debug("Create log sender fail!");
    return false;
  }

  int optval = 1;
  if(setsockopt(log_sender_id, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) < 0)
  {
    console.debug("Set log sender fail!, because %d.", errno);
    return false;
  }

  return true;
}

bool ApplicationManager::createLogRedirector(std::string log_name)
{
  std::string log_path;
  log_path = "/tmp/" + log_name + ".log";

  if(mkfifo(log_path.c_str(), S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) < 0 && errno != EEXIST)
  {
    console.debug("Make log queue %s fail!", log_path.c_str());
    return false;
  }

  log_threads.create_thread(boost::bind (&ApplicationManager::logFifoLoop, this, log_path));

  return true;
}

void ApplicationManager::logFifoLoop(std::string log_file)
{
  int log_fifo_id = open(log_file.c_str(), O_RDONLY);
  if(log_fifo_id < 0)
  {
    console.debug("Make log queue %s fail!", log_file.c_str());
    return;
  }

  console.message("Wait log stream on %s.", log_file.c_str());

  while(running)
  {
    struct timeval timeout = {0, 100000};
    fd_set read_set;
    FD_ZERO(&read_set);
    FD_SET(log_fifo_id, &read_set);
    int result;
    if ((result = select(log_fifo_id + 1, &read_set, NULL, NULL, &timeout)) > 0)
    {
      if(result > 0)
      {
        char buffer[512] = {0};
        int got = 0;
        if ((got = read(log_fifo_id, buffer, sizeof(buffer))) > 0)
        {
          if(got > 0)
          {
            struct sockaddr_in rmt_addr;
            socklen_t sin_len = sizeof(struct sockaddr_in);

            bzero(&rmt_addr, sizeof(struct sockaddr_in));
            rmt_addr.sin_family = AF_INET;
            rmt_addr.sin_port = htons(log_server_port_);
            rmt_addr.sin_addr.s_addr = inet_addr(log_server_ip_.c_str());

            boost::mutex::scoped_lock locker(log_sender_lock);

            if(sendto(log_sender_id, buffer, got, 0, (struct sockaddr*)&rmt_addr, sin_len) < 0)
            {
              printf("{e:%d}%s", errno, buffer);
            }

          }
        }
      }
    }
  }
  return;
}

bool ApplicationManager::initialize()
{
  NS_NaviCommon::Time::init();

  loadParameters();

  if(!createLogSender())
  {
    console.error("Create log sender error.");
    return false;
  }

  std::string app_names[] =
    {"SeLidar", "SeController", "SeMapping", "SeNavigation", "SeTrunk"};

  for(size_t i = 0; i < (sizeof(app_names) / sizeof(std::string)); i++)
  {
    ApplicationProperties app(app_names[i]);

    app.load();

    addApplication(app);

    createLogRedirector(app_names[i]);
  }

  if(!runApplications())
  {
    console.error("Run application error.");
    return false;
  }

  running = true;

  return true;
}

void ApplicationManager::pending()
{
  NS_NaviCommon::Rate rate(10);
  while(running)
  {
    rate.sleep();
  }

  console.message("Kill all applications!");

  killApplications();
}
