/*
 * ApplicationProperties.h
 *
 *  Created on: Sep 21, 2017
 *      Author: root
 */

#ifndef _UTILS_APPLICATIONPROPERTIES_H_
#define _UTILS_APPLICATIONPROPERTIES_H_

#include <string>
#include <signal.h>
#include <Parameter/Parameter.h>

class ApplicationProperties
{
public:
  ApplicationProperties(std::string name)
      : name_(name), pid_(0), run_script_(""), verbose_(false), core_(0),
        remote_log_(true)
  {

  }
  ~ApplicationProperties()
  {

  }
private:
  std::string name_;
  pid_t pid_;
  std::string run_script_;
  bool verbose_;
  int core_;
  bool remote_log_;
public:
  void load()
  {
    if(name_ != "")
    {
      NS_NaviCommon::Parameter parameter;
      parameter.loadConfigurationFile(name_ + ".xml");

      run_script_ = parameter.getParameter("script", "/usr/sbin/" + name_);

      if(parameter.getParameter("verbose", 1) == 1)
      {
        verbose_ = true;
      }
      else
      {
        verbose_ = false;
      }

      core_ = parameter.getParameter("core", 0);

      if(parameter.getParameter("remote_log", 1) == 1)
        remote_log_ = 1;
      else remote_log_ = 0;
    }
  }

  std::string getName()
  {
    return name_;
  }

  void setPID(pid_t pid)
  {
    pid_ = pid;
  }

  pid_t getPID()
  {
    return pid_;
  }

  int getCore()
  {
    return core_;
  }

  std::string getRunScript()
  {
    return run_script_;
  }

  bool isRemoteLog()
  {
    return remote_log_;
  }

  bool isVerboseMode()
  {
    return verbose_;
  }

};

#endif /* UTILS_APPLICATIONPROPERTIES_H_ */
