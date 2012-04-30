#ifndef IINPUT_H
#define IINPUT_H
#include "IModule.hh"

template<typename T>
class IInput : public virtual IModule {
public:
  virtual ~IInput() {}

public:
  virtual T* emitter() = 0;
};
#endif
