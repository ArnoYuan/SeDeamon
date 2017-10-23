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
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
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

  std::string log_server_ip_;
  int log_server_port_;

  boost::thread_group log_threads;
  int log_sender_id;
  boost::mutex log_sender_lock;

private:
  void loadParameters();

  bool addApplication(ApplicationProperties& application);

  bool runApplication(ApplicationProperties& application);

  bool runApplications();

  bool killApplication(std::string app_name);

  bool killApplication(pid_t pid);

  bool killApplications();

  void removeApplication(pid_t pid);

  bool createLogSender();

  bool createLogRedirector(std::string log_name);

  void logFifoLoop(std::string log_file);

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
