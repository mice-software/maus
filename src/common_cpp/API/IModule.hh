#ifndef IMODULE_H
#define IMODULE_H
#include <string>

namespace MAUS {

  class IModule{
  public:
    virtual ~IModule() {}
    
  public:
    virtual void birth (const std::string&) = 0;
    virtual void death ()                   = 0;
  };
  
}//end of namespace
#endif

