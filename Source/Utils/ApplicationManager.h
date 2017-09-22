/*
 * ApplicationManager.h
 *
 *  Created on: Sep 21, 2017
 *      Author: root
 */

#ifndef _UTILS_APPLICATIONMANAGER_H_
#define _UTILS_APPLICATIONMANAGER_H_

#include <vector>
#include "ApplicationProperties.h"

class ApplicationManager {
public:
	ApplicationManager();
	virtual ~ApplicationManager();
private:
    std::vector<ApplicationProperties> applications;
    bool running;
private:
    bool addApplication (ApplicationProperties& application);

    bool runApplication (ApplicationProperties& application);

    bool runApplications ();

    bool killApplicationByName (std::string app_name);

    bool killApplicationByPID (pid_t pid);

    bool killApplications ();

    void onApplicationQuit (pid_t pid, int status);

    static void signalChild (int no);
    static void signalTerm (int no);

    void registerSignals ();

public:

    bool initialize ();

    void pending ();

};

#endif /* UTILS_APPLICATIONMANAGER_H_ */
