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

#include <Python.h>

#include <string>

#include "json/json.h"

#include "src/common_cpp/Utils/CppErrorHandler.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "Interface/Squeak.hh"

namespace MAUS {

CppErrorHandler* CppErrorHandler::instance = NULL;

Json::Value CppErrorHandler::HandleSqueal
                         (Json::Value val, Squeal exc, std::string class_name) {
  Squeak::mout(Squeak::debug) << "Stack trace:" << exc.GetStackTrace()
                                                                   << std::endl;
  return getInstance()->ExceptionToPython(exc.what(), val, class_name);
}

Json::Value CppErrorHandler::HandleStdExc
                (Json::Value val, std::exception& exc, std::string class_name) {
  return getInstance()->ExceptionToPython((&exc)->what(), val, class_name);
}

void CppErrorHandler::HandleSquealNoJson(Squeal exc, std::string class_name) {
  HandleSqueal(Json::Value(), exc, class_name);
}

void CppErrorHandler::HandleStdExcNoJson
                                 (std::exception& exc, std::string class_name) {
  HandleStdExc(Json::Value(), exc, class_name);
}

Json::Value CppErrorHandler::ExceptionToPython
           (std::string what, Json::Value json_value, std::string class_name) {
  // Logic is:
  // * Make sure the Python error handler is loaded
  // * Clear any existing exceptions
  // * Make json_value into a string
  // * Hand json_value, class name, description of error to python
  // * Convert return PyObject to string
  // * Convert string to json value
  if (!getInstance()->GetPyErrorHandler()) {
    // Importing ErrorHandler calls libMausCpp which calls SetPyErrorHandler
    PyImport_ImportModule("ErrorHandler");
    PyErr_Clear();
    if (CppErrorHandler::getInstance()->GetPyErrorHandler() == 0) {
      Squeak::mout(Squeak::error)
             << "ERROR: Failed to get python error handler" << std::endl;
    }
  }

  PyErr_Clear();  // clear any existing exceptions
  Json::FastWriter writer;
  std::string json_in_cpp = writer.write(json_value);
  char sss[4] = {'s', 's', 's', '\0'};  // gotta love C (dodge compiler warning)
  PyObject* json_out_py = PyObject_CallFunction  // call the Python ErrorHandler
                (getInstance()->GetPyErrorHandler(), &sss[0],
                 json_in_cpp.c_str(), class_name.c_str(), what.c_str());
  const char* json_str;
  if (!json_out_py) {  // error on python side
    Squeak::mout(Squeak::error) << "ERROR: Failed to handle error:" << std::endl;
    PyErr_Print();
    return Json::Value();
  }
  int ok = PyArg_Parse(json_out_py, "s", &json_str);  // convert to string
  if (!ok) {  // we didn't get a string back
    Squeak::mout(Squeak::error)
              << "ERROR: Failed to parse return value from error:" << std::endl;
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
    // Don't initialise PyErrorHandler here - because we can get caught up in
    // initalisation order issues
    instance = new CppErrorHandler();
    Py_Initialize();
  }
  return instance;
}

CppErrorHandler::~CppErrorHandler() {
  if (this == instance) {  // should always be true
    instance = NULL;
  }
}
}  // namespace MAUS

