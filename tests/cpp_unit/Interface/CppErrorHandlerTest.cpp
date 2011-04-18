#include <exception>

#include "gtest/gtest.h"

#include "json/json.h"

#include "Interface/Squeal.hh"
#include "Interface/CppErrorHandler.hh"

namespace {

class MyException: public std::exception {
 public:
  virtual const char* what() const throw() {
    return "Some exception";
  }
};

TEST(CppErrorHandlerTest, HandleSquealTest) {
  Json::Value obj = Json::Value(Json::objectValue);
  Squeal squee1(Squeal::recoverable, "a_test", "exc::test");
  Json::Value value  = CppErrorHandler::HandleSqueal(obj, squee1, "exc_test");
  CppErrorHandler::HandleSquealNoJson(squee1, "exc_test");
  EXPECT_EQ(value["errors"]["exc_test"], Json::Value("a_test: exc::test"));
  Squeal squee2(Squeal::nonRecoverable, "a_test", "exc::test");
  CppErrorHandler::HandleSqueal(obj, squee2, "exc_test");
  CppErrorHandler::HandleSquealNoJson(squee2, "exc_test");
}

class Base {virtual void Member(){}};
class Derived : Base {};

TEST(CppErrorHandlerTest, HandleStdExcTest) {
  try {
    Base* b;
    Derived* rd = dynamic_cast<Derived*>(b);
  } catch (std::exception e) {
      Json::Value obj = Json::Value(Json::objectValue);
      Json::Value value  = CppErrorHandler::HandleStdExc(obj, e, "exc_test");
      EXPECT_EQ(value["Errors"]["exc_test"], 
             Json::Value("Unhandled std::exception: "+std::string(e.what())));
      CppErrorHandler::HandleStdExcNoJson(e, "exc_test");
  }
}

}

