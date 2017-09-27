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
#include <sys/wait.h>
#include <Time/Time.h>
#include <Time/Rate.h>
#include <Parameter/Parameter.h>
#include "ApplicationManager.h"

ApplicationManager::ApplicationManager() {
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
		console.message ("Load application: [ %s ]", application.getName ().c_str ());

		pid_t pid = fork ();

		if (pid == 0)
		{
			//child process
			int result;
			std::string cmd = application.getName () + " ";

			if (application.isVerboseMode ())
			{
				cmd += "-v ";
			}

			if (application.getCore () >= 0)
			{
				cmd += "-c ";
				cmd += application.getCore ();
				cmd += " ";
			}

			if (application.getLogFileName () != "")
			{
				cmd += "-l ";
				cmd += application.getLogFileName ();
				cmd += " ";
			}

			char cmd_string[512] = {0};
			char *args[64];
			char **next = args;

			cmd.copy (cmd_string, cmd.size ());

			char *temp = strtok (cmd_string, " \n");
			while (temp != NULL)
			{
				*next++ = temp;
				printf ("%s\n", temp);
				temp = strtok (NULL, " \n");
			}
			*next = NULL;

			result = execvp (application.getRunScript ().c_str (), args);
			if (result < 0)
			{
				return false;
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
	bool result = true;
	for (int i = 0; i < applications.size (); ++i)
	{
		if (!runApplication (applications[i]))
		{
			result = false;
		}
	}

	return result;
}

bool ApplicationManager::killApplication (std::string app_name)
{
	bool result = false;
	for (int i = 0; i < applications.size (); ++i)
	{
		if (applications[i].getName () == app_name)
		{
			result = killApplication (applications[i].getPID ());
		}
	}
	return result;
}

bool ApplicationManager::killApplication (pid_t pid)
{
	if (pid != 0)
	{
		console.message ("Killing application with pid : %d...", pid);
		if (kill (pid, SIGUSR1) != 0)
		{
			return false;
		}else{
			//sleep (1);
			int result = waitpid (pid, NULL, WNOHANG);
			if (result == pid)
			{
				return true;
			}else if (result == 0)
			{
				console.debug ("Application killing might be fail, kill it force.", pid);
				kill (pid, SIGKILL);
				return true;
			}else
			{
				return false;
			}
		}
	}
}

void ApplicationManager::removeApplication (pid_t pid)
{
	std::vector<ApplicationProperties>::iterator vit;

	for (vit = applications.begin (); vit != applications.end ();)
	{
		if (((ApplicationProperties)*vit).getPID () == pid)
		{
			vit = applications.erase (vit);
		}else{
			++vit;
		}
	}
}

void ApplicationManager::onApplicationQuit (pid_t pid, int status)
{
	removeApplication (pid);
}

bool ApplicationManager::killApplications ()
{
	bool result = true;
	for (int i = 0; i < applications.size (); ++i)
	{
		console.message ("Kill!");
		if (!killApplication (applications[i].getPID ()))
		{
			result = false;
		}
	}
	return result;
}

bool ApplicationManager::initialize ()
{
	NS_NaviCommon::Time::init ();

	std::string app_names[] = {"SeLidar", "SeController", "SeMapping", "SeNavigation"};

	for (int i = 0; i < (sizeof (app_names) / sizeof (std::string)); i++)
	{
		ApplicationProperties app (app_names[i]);

		app.load ();

		addApplication (app);
	}

	if (!runApplications ())
	{
		return false;
	}

	running = true;

	return true;
}

void ApplicationManager::pending ()
{
	NS_NaviCommon::Rate rate (10);
	while (running)
	{
		rate.sleep ();
	}

	console.message ("Kill all applications!");

	killApplications ();
}
