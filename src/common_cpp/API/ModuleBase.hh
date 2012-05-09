#ifndef MODULE_BASE_H
#define MODULE_BASE_H
#include "IModule.hh"
#include "gtest/gtest_prod.h"

namespace MAUS {

  class ModuleBase : public virtual IModule {
    
  public:
    explicit ModuleBase(const std::string&);
    ModuleBase(const ModuleBase& mb);
    virtual ~ModuleBase();
    
  public:
    void birth  (const std::string&);
    void death  ();
    
  protected:
    std::string _classname;
    
  private:
    virtual void _birth (const std::string&) = 0;
    virtual void _death ()                   = 0;
  };

}//end of namespace 
#endif
