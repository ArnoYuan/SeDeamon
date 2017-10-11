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

class ApplicationManager
{
public:
  ApplicationManager();
  virtual ~ApplicationManager();
private:
  std::vector< ApplicationProperties > applications;
  bool running;
  NS_NaviCommon::Console console;
private:

  bool addApplication(ApplicationProperties& application);

  bool runApplication(ApplicationProperties& application);

  bool runApplications();

  bool killApplication(std::string app_name);

  bool killApplication(pid_t pid);

  bool killApplications();

  void removeApplication(pid_t pid);

public:

  bool initialize();

  void pending();

  void onApplicationQuit(pid_t pid, int status);

  bool isRunning()
  {
    return running;
  }

  void terminate()
  {
    console.debug("Application is quitting!");
    running = false;
  }

};

#endif /* UTILS_APPLICATIONMANAGER_H_ */
