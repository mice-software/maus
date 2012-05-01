#ifndef IOUTPUT_H
#define IOUTPUT_H
#include <string>

namespace MAUS {

  template<typename T>
  class IOutput {
  public:
    virtual ~IOutput() {}
    
  public:
    virtual bool save (T*) = 0;
  };

}//end of namespace
#endif
