#ifndef IREDUCE_H
#define IREDUCE_H
#include "IModule.hh"

namespace MAUS {

  template<typename T>
  class IReduce : public virtual IModule {
  public:
    virtual ~IReduce() {}
    
  public:
    virtual T* process(T*) = 0;
  };

}//end of namespace
#endif
