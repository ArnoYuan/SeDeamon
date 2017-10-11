/*
 * Main.cpp
 *
 *  Created on: Sep 21, 2017
 *      Author: root
 */
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include "Utils/ApplicationManager.h"
#include <boost/interprocess/shared_memory_object.hpp>

ApplicationManager* manager;

static void signalChild(int no)
{
  int status;
  pid_t pid;

  while(((pid = waitpid(-1, &status, WNOHANG)) > 0) && manager->isRunning())
  {
    manager->onApplicationQuit(pid, status);
  }
}

static void signalTerm(int no)
{
  manager->terminate();
}

void registerSignals()
{
  signal(SIGINT, signalTerm);
  signal(SIGKILL, signalTerm);
  signal(SIGQUIT, signalTerm);
  signal(SIGTERM, signalTerm);

  signal(SIGCHLD, signalChild);
  //signal (SIGUSR1, signalChild);
}

void cleanup()
{
  //DATASETS
  boost::interprocess::shared_memory_object::remove("TWIST");
  boost::interprocess::shared_memory_object::remove("LASER_SCAN");
  boost::interprocess::shared_memory_object::remove("GOAL");

  //SERVICES
  boost::interprocess::shared_memory_object::remove("BASE_ODOM");
  boost::interprocess::shared_memory_object::remove("BASE_ODOM_TF");
  boost::interprocess::shared_memory_object::remove("ODOM_MAP_TF");
  boost::interprocess::shared_memory_object::remove("MAP");
}

int main(int argc, char* argv[])
{
  pid_t pid = fork();

  if(pid == 0)
  {
    int lck;
    mode_t fd_mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    lck = open("/tmp/SeDeamon.lck", O_RDWR | O_CREAT, fd_mode);
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    if(fcntl(lck, F_SETLK, &lock) < 0)
    {
      printf("There is an instance has been running!\n");
      exit(EXIT_FAILURE);
      return 0;
    }

    cleanup();

    manager = new ApplicationManager();

    registerSignals();

    if(!manager->initialize())
    {
      delete manager;
      exit(EXIT_FAILURE);
      return 0;
    }

    manager->pending();

    delete manager;

    printf("Quit SeDeamon process.\n");

    cleanup();

    exit(EXIT_SUCCESS);
    return 0;
  }
  else if(pid > 0)
  {
    printf("Run SeDeamon process in background.\n");
    exit(EXIT_SUCCESS);
    return 0;
  }
  else
  {
    exit(EXIT_FAILURE);
    return 0;
  }
}

