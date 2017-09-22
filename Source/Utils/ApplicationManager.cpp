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

}

ApplicationManager::~ApplicationManager() {
	// TODO Auto-generated destructor stub
}

bool ApplicationManager::addApplication (ApplicationProperties& application)
{

}

bool ApplicationManager::runApplication (ApplicationProperties& application)
{

}

bool ApplicationManager::runApplications ()
{

}

bool ApplicationManager::killApplicationByName (std::string app_name)
{

}

bool ApplicationManager::killApplicationByPID (pid_t pid)
{

}

bool ApplicationManager::killApplications ()
{

}

void ApplicationManager::signalChild (int no)
{

}

void ApplicationManager::signalTerm (int no)
{

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
}
