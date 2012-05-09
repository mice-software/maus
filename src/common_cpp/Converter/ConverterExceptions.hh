#ifndef CONVERTER_EXCEPTIONS_H
#define CONVERTER_EXCEPTIONS_H

#include <exception>

class ConverterNotFoundException: public std::exception {
public:
  virtual const char* what() const throw() {
    return "The required converter was not found";
  }
};

#endif
