#ifndef IINPUT_H
#define IINPUT_H
#include "IModule.hh"

namespace MAUS {

  template<typename T>
  class IInput : public virtual IModule {
  public:
    virtual ~IInput() {}
    
  public:
    virtual T* emitter() = 0;
  };
  
}//end of namespace
#endif
