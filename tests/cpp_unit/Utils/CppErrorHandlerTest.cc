#include <exception>

#include "gtest/gtest.h"

#include "json/json.h"

#include "Utils/Exception.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"

namespace {

using namespace MAUS;
class CppErrorHandlerTest : public ::testing::Test {
protected:
  CppErrorHandlerTest() : obj(Json::objectValue), 
                          exception(Exceptions::recoverable, "a_test", "exc::test"),
                          std_exc(&exception) {
  }
  virtual ~CppErrorHandlerTest() {}
  Json::Value obj;
  Exceptions::Exception      exception;
  std::exception* std_exc;  
};

TEST_F(CppErrorHandlerTest, HandleExceptionTest) {
  Json::Value value = 
           CppErrorHandler::getInstance()->HandleException(obj, exception, "exc_test");
  EXPECT_EQ(value["errors"]["exc_test"],
           Json::Value("<class 'ErrorHandler.CppError'>: a_test at exc::test"));
}

TEST_F(CppErrorHandlerTest, HandleStdExcTest) {
  Json::Value value = 
        CppErrorHandler::getInstance()->HandleStdExc(obj, *std_exc, "exc_test");
  std::string err = value["errors"]["exc_test"].asString();
  EXPECT_EQ(err.substr(0, 33), "<class 'ErrorHandler.CppError'>: ");
}

TEST_F(CppErrorHandlerTest, HandleStdExcNoJsonTest) {
  CppErrorHandler::getInstance()->HandleStdExcNoJson(*std_exc, "exc_test");
  // not much to be done here... could check stderr or so?
}

TEST_F(CppErrorHandlerTest, HandleExceptionNoJsonTest) {
  CppErrorHandler::getInstance()->HandleExceptionNoJson(exception, "exc_test");
  // not much to be done here... could check stderr or so?
}

}
