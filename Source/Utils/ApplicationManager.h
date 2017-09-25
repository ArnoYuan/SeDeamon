/*
 * ApplicationManager.h
 *
 *  Created on: Sep 21, 2017
 *      Author: root
 */

#ifndef _UTILS_APPLICATIONMANAGER_H_
#define _UTILS_APPLICATIONMANAGER_H_

#include <vector>
#include <Console/Console.h>
#include "ApplicationProperties.h"

class ApplicationManager {
public:
	ApplicationManager();
	virtual ~ApplicationManager();
private:
    std::vector<ApplicationProperties> applications;
    bool running;
    NS_NaviCommon::Console console;
private:

    bool isRunning ()
    {
    	return running;
    }

    void terminate ()
    {
    	running = false;
    }

    bool addApplication (ApplicationProperties& application);

    bool runApplication (ApplicationProperties& application);

    bool runApplications ();

    bool killApplication (std::string app_name);

    bool killApplication (pid_t pid);

    bool killApplications ();

    void removeApplication (pid_t pid);

    static void signalChild (int no);
    static void signalTerm (int no);

    void registerSignals ();

public:

    bool initialize ();

    void pending ();

    void onApplicationQuit (pid_t pid, int status);

};

#endif /* UTILS_APPLICATIONMANAGER_H_ */
