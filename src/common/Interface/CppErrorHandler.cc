#include "json/json.h"

#include "Interface/CppErrorHandler.hh"
#include "Interface/Squeak.hh"

Json::Value CppErrorHandler::HandleSqueal
                         (Json::Value val, Squeal exc, std::string class_name) {
  exc.Print();
  val["errors"][class_name] = exc.GetMessage()+": "+exc.GetLocation();
  return val;
}

Json::Value CppErrorHandler::HandleStdExc
                 (Json::Value val, std::exception exc, std::string class_name) {
  Squeak::mout(Squeak::error) << std::string(exc.what()) << std::endl;
  val["errors"][class_name]
                         = "Unhandled std::exception: "+std::string(exc.what());
  return val;
}

void CppErrorHandler::HandleSquealNoJson(Squeal exc, std::string class_name) {
  HandleSqueal(Json::Value(), exc, class_name);
}

void CppErrorHandler::HandleStdExcNoJson
                                  (std::exception exc, std::string class_name) {
  HandleStdExc(Json::Value(), exc, class_name);
}

