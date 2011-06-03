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

#include <string>

#include "json/json.h"
#include "Python.h"

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
  CppErrorHandler::getInstance();
  PyErr_SetString(PyExc_Exception, what.c_str());
  Json::FastWriter writer;
  std::string json_in_cpp = writer.write(json_value);
  PyObject* py_args = Py_BuildValue("ss", json_in_cpp.c_str(), class_name.c_str());
  PyObject* json_out_py = PyEval_CallObject(CppErrorHandler::GetPyErrorHandler(), py_args);
  PyErr_Clear();
  char* json_string;
  PyArg_ParseTuple(json_out_py, "s", json_string);
  Json::Value json_out_cpp =  JsonWrapper::StringToJson(std::string(json_string));
}

CppErrorHandler::CppErrorHandler() : HandleExceptionFunction(NULL) {
  std::cerr << "CppErrorHandler ctor" << std::endl;
  const char* error_handler_name = "sys";
  PyEval_InitThreads;
  // why do I get a segmentation fault when I attempt to import sys?
  PyObject* error_handler_module = PyImport_ImportModule(error_handler_name);
  if (error_handler_module == NULL) std::cerr << "ERROR_HANDLER_NOT_SET" << std::endl;
  std::cerr << "CppErrorHandler ctor done" << std::endl;
}

CppErrorHandler* CppErrorHandler::getInstance() {
  if (instance == NULL) instance = new CppErrorHandler();
  return instance;
}


