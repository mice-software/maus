#ifndef API_EXCEPTIONS
#define API_EXCEPTIONS
#include <exception>
#include <string>
#include "gtest/gtest_prod.h"

namespace MAUS {

  class NullInputException: public std::exception {

  public:
    NullInputException(const std::string& classname) : std::exception(), _classname(classname){}
    virtual ~NullInputException() throw() {}

  public:
    virtual const char* what() const throw() {
      std::string* ret = new std::string();
      *ret += "The input to '";
      *ret += _classname;
      *ret += "' was a null pointer";
      return ret->c_str();
    }

  protected:
    std::string _classname;

  private:
    FRIEND_TEST(APIExceptionsTest, TestNullInputExceptionConstructor);
    FRIEND_TEST(APIExceptionsTest, TestNullInputExceptionWhat);
  };
  
  class UnhandledException: public std::exception {

  public:
    UnhandledException(const std::string& classname) : std::exception(), _classname(classname){}
    virtual ~UnhandledException() throw() {}

  public:
    virtual const char* what() const throw() {
      std::string* ret = new std::string();
      *ret +="An unhandled exception was thrown by '";
      *ret += _classname;
      *ret += "' which was not of type 'Squeal' or deriving from 'std::exception'";
      return ret->c_str();
    }

  protected:
    std::string _classname;

  private:
    FRIEND_TEST(APIExceptionsTest, TestUnhandledExceptionConstructor);
    FRIEND_TEST(APIExceptionsTest, TestUnhandledExceptionWhat);
  };
  
}//end of namespace

#endif
