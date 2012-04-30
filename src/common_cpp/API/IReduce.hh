#ifndef IREDUCE_H
#define IREDUCE_H
#include "IModule.hh"

template<typename T>
class IReduce : public virtual IModule {
public:
  virtual ~IReduce() {}

public:
  virtual T* process(T*) = 0;
};
#endif
