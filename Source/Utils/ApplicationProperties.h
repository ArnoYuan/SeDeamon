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

class ApplicationProperties
{
public:
  ApplicationProperties () :
	  name_ (""), pid_ (0),
	  run_script_ ("")
  {

  }
  ~ApplicationProperties ()
  {

  }
private:
  std::string name_;
  pid_t pid_;
  std::string run_script_;
public:
  void setName (std::string name)
  {
	  name_ = name;
  }

  std::string getName ()
  {
	  return name_;
  }

  void setPID (pid_t pid)
  {
	  pid_ = pid;
  }

  pid_t getPID ()
  {
	  return pid_;
  }

  void setRunScript (std::string run_script)
  {
	  run_script_ = run_script;
  }

  std::string getRunScript ()
  {
	  return run_script_;
  }

};



#endif /* UTILS_APPLICATIONPROPERTIES_H_ */
