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
#include "Utils/Squeak.hh"

namespace MAUS {

CppErrorHandler* CppErrorHandler::instance = NULL;

CppErrorHandler* CppErrorHandler::getInstance() {
    if (!instance)
        instance = new CppErrorHandler();
    return instance;
}

Json::Value CppErrorHandler::HandleException
                         (Json::Value val, Exceptions::Exception exc, std::string class_name) {
  Squeak::mout(Squeak::debug) << "Stack trace:" << exc.GetStackTrace()
                                                                   << std::endl;
  return getInstance()->ExceptionToPython(exc.what(), val, class_name);
}

Json::Value CppErrorHandler::HandleStdExc
                (Json::Value val, std::exception& exc, std::string class_name) {
  Json::Value out = getInstance()->ExceptionToPython((&exc)->what(), val, class_name);
  return out;
}

void CppErrorHandler::HandleExceptionNoJson(Exceptions::Exception exc, std::string class_name) {
  Json::Value json = Json::Value();
  HandleException(json, exc, class_name);
}

void CppErrorHandler::HandleStdExcNoJson
                                 (std::exception& exc, std::string class_name) {
  Json::Value json = Json::Value();
  HandleStdExc(json, exc, class_name);
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
  PyErr_Clear();  // clear any existing exceptions
  Json::FastWriter writer;
  std::string json_in_cpp = writer.write(json_value);
  char* sss = const_cast<char*>("sss");  // gotta love C (dodge compiler warning)
  PyObject* error_handler_function = GetPyErrorHandler();
  PyObject* json_out_py = PyObject_CallFunction  // call the Python ErrorHandler
                (error_handler_function, sss,
                 json_in_cpp.c_str(), class_name.c_str(), what.c_str());
  Py_DECREF(error_handler_function);
  const char* json_str;
  if (!json_out_py) {  // python ErrorHandler was set to raise the error
    Squeak::mout(Squeak::error) << "ERROR: Failed to handle error:" << std::endl;
    PyErr_Print();
    throw std::exception();
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

CppErrorHandler::CppErrorHandler() {
}

PyObject* CppErrorHandler::GetPyErrorHandler() {
    // import ErrorHandler
    PyObject* py_error_handler_module = PyImport_ImportModule("ErrorHandler");
    if (!py_error_handler_module) {
        throw Exceptions::Exception(Exceptions::recoverable,
                        "Failed to import ErrorHandler module",
                        "CppErrorHandler::GetPyErrorHandler");
    }
    // ErrorHandler.getattr("HandleCppException")
    PyObject* handle_error_fnc = PyObject_GetAttrString(py_error_handler_module,
                                                       "HandleCppException");
    Py_DECREF(py_error_handler_module);
    if (!PyCallable_Check(handle_error_fnc))
        throw Exceptions::Exception(Exceptions::recoverable,
                        "Failed to get HandleCppException function",
                        "CppErrorHandler::GetPyErrorHandler");
    return handle_error_fnc;
}

CppErrorHandler::~CppErrorHandler() {
  instance = NULL;
}
}  // namespace MAUS

