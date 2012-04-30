#ifndef IOUTPUT_H
#define IOUTPUT_H
#include <string>

template<typename T>
class IOutput {
public:
  virtual ~IOutput() {}

public:
  virtual bool save (T*) = 0;
};
#endif
