/*
 * ApplicationManager.cpp
 *
 *  Created on: Sep 21, 2017
 *      Author: root
 */

#include <signal.h>
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
		console.debug ("Load application: [ %s ]", application.getName ().c_str ());

		pid_t pid = fork ();

		if (pid == 0)
		{
			//child process
			int result;
			result = execlp (application.getRunScript ().c_str (), NULL, NULL, NULL);
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

	NS_NaviCommon::Parameter parameter;
	parameter.loadConfigurationFile ("deamon.xml");

	int app_count = parameter.getParameter ("app_count", 4);

	std::string app_names[] = {"SeLidar", "SeController", "SeMapping", "SeNavigation"};
	std::string app_cmds[] = {
			"/home/cybernik/Development/Projects/SeLidar/Debug/SeLidar",
			"/home/cybernik/Development/Projects/SeController/Debug/SeController",
			"/home/cybernik/Development/Projects/SeMapping/Debug/SeMapping",
			"/home/cybernik/Development/Projects/SeNavigation/Debug/SeNavigation"};
	for (int i = 0; i < app_count; i++)
	{
		ApplicationProperties app;
		std::string prop_name;

		prop_name = "name_" + i;
		std::string app_name = parameter.getParameter (prop_name, app_names[i]);
		app.setName (app_name);

		prop_name = "cmd_" + i;
		std::string app_cmd = parameter.getParameter (prop_name, app_cmds[i]);
		app.setRunScript (app_cmd);

		app.setPID (0);

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
