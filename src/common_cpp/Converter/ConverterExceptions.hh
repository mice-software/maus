#ifndef CONVERTER_EXCEPTIONS_H
#define CONVERTER_EXCEPTIONS_H

#include <exception>

namespace MAUS {

  class ConverterNotFoundException: public std::exception {
  public:
    virtual const char* what() const throw() {
      return "The required converter was not found";
    }
  };

}//end of namespace
#endif
