#include <exception>

#include "gtest/gtest.h"

#include "json/json.h"

#include "Interface/Squeal.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"

namespace {

using namespace MAUS;
class CppErrorHandlerTest : public ::testing::Test {
protected:
  CppErrorHandlerTest() : obj(Json::objectValue), 
                          squee(Squeal::recoverable, "a_test", "exc::test"),
                          std_exc(&squee) {
  }
  virtual ~CppErrorHandlerTest() {}
  Json::Value obj;
  Squeal      squee;
  std::exception* std_exc;  
};

TEST_F(CppErrorHandlerTest, HandleSquealTest) {
  Json::Value value = CppErrorHandler::HandleSqueal(obj, squee, "exc_test");
  EXPECT_EQ(value["errors"]["exc_test"][Json::UInt(0)],
           Json::Value("<class 'ErrorHandler.CppError'>: a_test at exc::test"));
}

TEST_F(CppErrorHandlerTest, HandleStdExcTest) {
  Json::Value value = CppErrorHandler::HandleStdExc(obj, *std_exc, "exc_test");
  EXPECT_EQ(value["errors"]["exc_test"][Json::UInt(0)],
           Json::Value("<class 'ErrorHandler.CppError'>: std::exception"));
}

TEST_F(CppErrorHandlerTest, HandleStdExcNoJsonTest) {
  CppErrorHandler::HandleStdExcNoJson(*std_exc, "exc_test");
  // not much to be done here... could check stderr or so?
}

TEST_F(CppErrorHandlerTest, HandleSquealNoJsonTest) {
  CppErrorHandler::HandleSquealNoJson(squee, "exc_test");
  // not much to be done here... could check stderr or so?
}

}
