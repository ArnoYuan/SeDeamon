/*
 * ApplicationManager.cpp
 *
 *  Created on: Sep 21, 2017
 *      Author: root
 */

#include <signal.h>
#include <Time/Time.h>
#include <Rate/Rate.h>
#include "ApplicationManager.h"

static ApplicationManager* instance;

ApplicationManager::ApplicationManager() {
	instance = this;
	applications.clear ();

}

ApplicationManager::~ApplicationManager() {

}

bool ApplicationManager::addApplication (ApplicationProperties& application)
{
	applications.push_back (application);
}

bool ApplicationManager::runApplication (ApplicationProperties& application)
{
	if (application.getPID () == 0)
	{
		pid_t pid = fork ();

		if (pid == 0)
		{
			//child process
			int result;
			result = execlp (application.getRunScript ().c_str (), NULL, NULL, NULL);
			if (result < 0)
			{
				exit (-1);
			}

		}else if (pid > 0)
		{
			//parent process
			application.setPID (pid);
		}else{
			//failure
			return false;
		}
	}
	return true;
}

bool ApplicationManager::runApplications ()
{

}

bool ApplicationManager::killApplicationByName (std::string app_name)
{

}

bool ApplicationManager::killApplicationByPID (pid_t pid)
{
	if (pid != 0)
	{
		if (kill (pid, SIGTERM) != 0)
		{
			return false;
		}else{
			sleep (1);
			int result = waitpid (pid, NULL, WNOHANG);
			if (result == pid)
			{
				return true;
			}else if (result == 0)
			{
				kill (pid, SIGKILL);
				return true;
			}else
			{
				return false;
			}
		}
	}
}

bool ApplicationManager::killApplications ()
{

}

void ApplicationManager::signalChild (int no)
{
	int status;
	pid_t pid;

	while (((pid = waitpid(-1, &status, WNOHANG)) > 0) && running)
	{
		sleep(1);
		instance->onApplicationQuit(pid, status);
	}
}

void ApplicationManager::signalTerm (int no)
{
	running = false;
}

void ApplicationManager::registerSignals ()
{
	signal (SIGINT, signalTerm);
	signal (SIGKILL, signalTerm);
	signal (SIGQUIT, signalTerm);
	signal (SIGTERM, signalTerm);

	signal (SIGCHLD, signalChild);
	//signal (SIGUSR1, signalChild);
}

bool ApplicationManager::initialize ()
{
	registerSignals ();
}

void ApplicationManager::pending ()
{
	NS_NaviCommon::Rate rate (10);
	while (running)
	{
		rate.sleep ();
	}
	killApplications ();
}
