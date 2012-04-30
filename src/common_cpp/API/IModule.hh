#ifndef IMODULE_H
#define IMODULE_H
#include <string>

class IModule{
public:
  virtual ~IModule() {}

public:
  virtual void birth (const std::string&) = 0;
  virtual void death ()                   = 0;
};

#endif

