#include <exception>

#include "gtest/gtest.h"

#include "Interface/Squeal.hh"
#include "Interface/CppErrorHandler.hh"

namespace {

class MyException: public std::exception {
 public:
  virtual const char* what() const throw() {
    return "Some exception";
  }
};

TEST(CppErrorHandlerTest, HandleExceptionTest) {
/*
  Json::Value val;
  std::stringstream inout;
  Squeal(Squeal::recoverable, "Error message", "CppErrorHandlerTest");
  Squeak::setOutput(Squeak::fatal, inout);
  HandleException(val, Squeal);
  HandleException(val, myexception());
  std::cout << inout.str();
*/
  EXPECT_TRUE(false);  
}

}

