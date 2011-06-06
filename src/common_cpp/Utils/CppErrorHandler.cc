/* This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
 *
 * MAUS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MAUS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "Python.h"

#include <string>

#include "json/json.h"

#include "src/common_cpp/Utils/CppErrorHandler.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "Interface/Squeak.hh"

CppErrorHandler* CppErrorHandler::instance = NULL;

Json::Value CppErrorHandler::HandleSqueal
                         (Json::Value val, Squeal exc, std::string class_name) {
  return getInstance()->ExceptionToPython(exc.what(), val, class_name);
}

Json::Value CppErrorHandler::HandleStdExc
                 (Json::Value val, std::exception exc, std::string class_name) {
  return getInstance()->ExceptionToPython(exc.what(), val, class_name);
}

void CppErrorHandler::HandleSquealNoJson(Squeal exc, std::string class_name) {
  HandleSqueal(Json::Value(), exc, class_name);
}

void CppErrorHandler::HandleStdExcNoJson
                                  (std::exception exc, std::string class_name) {
  HandleStdExc(Json::Value(), exc, class_name);
}

Json::Value CppErrorHandler::ExceptionToPython
           (std::string what, Json::Value json_value, std::string class_name) {
  PyErr_Clear();
  Json::FastWriter writer;
  std::string json_in_cpp = writer.write(json_value);
  char* sss = "sss";
  PyObject* json_out_py = PyObject_CallFunction(CppErrorHandler::GetPyErrorHandler(), sss, json_in_cpp.c_str(), class_name.c_str(), what.c_str());
  const char* json_str;
  if (!json_out_py) {
    Squeak::mout(Squeak::error) << "Failed to handle error:" << std::endl;
    PyErr_Print();
    return Json::Value();
  }
  int ok = PyArg_Parse(json_out_py, "s", &json_str);
  if (!ok) {
    Squeak::mout(Squeak::error) << "Failed to parse return value" << std::endl;
    PyErr_Print();
    return Json::Value();
  }
  Json::Value json_out_cpp =  JsonWrapper::StringToJson(std::string(json_str));
  return json_out_cpp;
}

CppErrorHandler::CppErrorHandler() : HandleExceptionFunction(NULL) {

}

CppErrorHandler* CppErrorHandler::getInstance() {
  if (instance == NULL) {
    instance = new CppErrorHandler();
    Py_Initialize();
    PyImport_ImportModule("ErrorHandler");
    PyErr_Clear();
    if (CppErrorHandler::GetPyErrorHandler() == 0) {
      Squeak::mout(Squeak::warning)
         << "Error - failed to get python error handler" << std::endl;
    }
  }
  return instance;
}


