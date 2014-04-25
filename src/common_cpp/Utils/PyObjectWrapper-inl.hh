/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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

#ifndef _SRC_COMMON_CPP_UTILS_PYOBJECTWRAPPER_INL_HH
#define _SRC_COMMON_CPP_UTILS_PYOBJECTWRAPPER_INL_HH

#include <string>

#include "TPython.h" // root
#include "json/json.h"
#include "src/common_cpp/DataStructure/Data.hh"
#include "src/common_cpp/Converter/ConverterFactory.hh"

#include "src/common_cpp/Utils/PyObjectWrapper.hh"

namespace MAUS {
template <typename TEMP>
TEMP* PyObjectWrapper::unwrap_pyobject(PyObject *args) {
  // Input should be a single PyStringObject
  std::string** string_ret = new std::string*(NULL);
  Json::Value** json_ret = new Json::Value*(NULL);
  MAUS::Data** data_ret = new MAUS::Data*(NULL);
  PyObject** py_ret = new PyObject*(NULL);
  lazy_unwrap(args, string_ret, json_ret, data_ret, py_ret);
  TEMP* cpp_ret = NULL;
  if (*string_ret != NULL) {
      cpp_ret = ConverterFactory().convert<TEMP, std::string>(*string_ret);
  } else if (*json_ret != NULL) {
      cpp_ret = ConverterFactory().convert<TEMP, Json::Value>(*json_ret);
  } else if (*data_ret != NULL) {
      cpp_ret = ConverterFactory().convert<TEMP, MAUS::Data>(*data_ret);
  } else if (*py_ret != NULL) {
      cpp_ret = ConverterFactory().convert<TEMP, PyObject>(*py_ret);
  } else {
    throw Exception(Exception::recoverable,
                    "Failed to do the lazy conversion",
                    "PyObjectWrapper::unwrap_pyobject");
  }
  return cpp_ret;
}

template <>
PyObject* PyObjectWrapper::wrap_pyobject(MAUS::Data* cpp_data) {
  // Confirm the object exists.
  if(!cpp_data) {
      throw Exception(Exception::recoverable,
                      "Cpp data was NULL",
                      "PyObjectWrapper::wrap_pyobject<MAUS::Data>");
  }

  // Place the MAUS::MAUSEvent* inside a ROOT Object Proxy
  PyObject* py_data = TPython::ObjectProxy_FromVoidPtr
                                  (static_cast<void*>(cpp_data), "MAUS::Data");
  if (!py_data) {
      throw Exception(Exception::recoverable,
                      "Could not convert MAUS::Data to PyRoot",
                      "PyObjectWrapper::wrap_pyobject<MAUS::Data>");
  }
  return py_data;
}

template <>
PyObject* PyObjectWrapper::wrap_pyobject(Json::Value* json_value) {
  // Confirm the object exists.
  if(!json_value) {
      throw Exception(Exception::recoverable,
                      "Json::Value was NULL",
                      "PyObjectWrapper::wrap_pyobject<Json::Value>");
  }

  // Place the Json::Value* inside a PyCapsule
  void* vptr = static_cast<void*>(json_value);
  PyObject *py_cap = PyCapsule_New(vptr, "JsonCpp", NULL);
  return py_cap;
}

template <>
PyObject* PyObjectWrapper::wrap_pyobject(std::string* str) {
  // Export the std::string as a Python string.
  PyObject* py_string = PyString_FromString(str->c_str());
  return py_string;
}

template <>
PyObject* PyObjectWrapper::wrap_pyobject(PyObject* py_object) {
  // Export the std::string as a Python string.
  return py_object;  
}
}  // namespace MAUS

#endif  // _SRC_COMMON_CPP_UTILS_PYOBJECTWRAPPER_INL_HH

