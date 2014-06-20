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
TEMP* PyObjectWrapper::unwrap(PyObject *args) {
  // Input should be a single PyStringObject
  std::string** string_ret = new std::string*(NULL);
  Json::Value** json_ret = new Json::Value*(NULL);
  MAUS::Data** data_ret = new MAUS::Data*(NULL);
  PyObject** py_ret = new PyObject*(NULL);
  lazy_unwrap(args, string_ret, json_ret, data_ret, py_ret);
  TEMP* cpp_ret = NULL;
  if (*string_ret != NULL) {
      cpp_ret = ConverterFactory().convert<std::string, TEMP>(*string_ret);
      delete *string_ret;
  } else if (*json_ret != NULL) {
      cpp_ret = ConverterFactory().convert<Json::Value, TEMP>(*json_ret);
      delete *json_ret;
  } else if (*data_ret != NULL) {
      cpp_ret = ConverterFactory().convert<MAUS::Data, TEMP>(*data_ret);
      delete *data_ret;
  } else if (*py_ret != NULL) {
      cpp_ret = ConverterFactory().convert<PyObject, TEMP>(*py_ret);
      Py_DECREF(*py_ret);
  } else {
    throw Exception(Exception::recoverable,
                    "Failed to do the lazy conversion",
                    "PyObjectWrapper::unwrap_pyobject");
  }
  // note here we delete the pointer, not the allocated memory it points to
  delete py_ret;
  delete data_ret;
  delete json_ret;
  delete string_ret;
  return cpp_ret;
}

PyObject* PyObjectWrapper::wrap(MAUS::Data* cpp_data) {
  // Confirm the object exists.
  if (!cpp_data) {
      throw Exception(Exception::recoverable,
                      "Cpp data was NULL",
                      "PyObjectWrapper::wrap_pyobject<MAUS::Data>");
  }
  // Require root_module to be imported for this to work (don't ask me why)
  static PyObject* root_module = PyImport_ImportModule("ROOT");
  if (!root_module)
      throw Exception(Exception::recoverable,
                      "Failed to import ROOT",
                      "PyObjectWrapper::wrap_pyobject<MAUS::Data>");

  // Place the MAUS::MAUSEvent* inside a ROOT Object Proxy
  void* void_data = static_cast<void*>(cpp_data);
  // cpp_data is now owned by python
  PyObject* py_data = TPython::ObjectProxy_FromVoidPtr(void_data,
                                                       "MAUS::Data",
                                                       true);
  if (!py_data) {
      throw Exception(Exception::recoverable,
                      "Could not convert MAUS::Data to PyRoot",
                      "PyObjectWrapper::wrap_pyobject<MAUS::Data>");
  }
  return py_data;
}

PyObject* PyObjectWrapper::wrap(Json::Value* json_value) {
  // Confirm the object exists.
  if (!json_value) {
      throw Exception(Exception::recoverable,
                      "Json::Value was NULL",
                      "PyObjectWrapper::wrap_pyobject<Json::Value>");
  }

  // Place the Json::Value* inside a PyCapsule
  void* vptr = static_cast<void*>(json_value);
  PyObject *py_cap = PyCapsule_New(vptr, "JsonCpp", delete_jsoncpp_pycapsule);
  return py_cap;
}

PyObject* PyObjectWrapper::wrap(std::string* str) {
  // Export the std::string as a Python string.
  if (!str) {
      throw Exception(Exception::recoverable,
                      "str was NULL",
                      "PyObjectWrapper::wrap_pyobject<string>");
  }
  PyObject* py_string = PyString_FromString(str->c_str());
  return py_string;
}

PyObject* PyObjectWrapper::wrap(PyObject* py_object) {
  // We just do an incref (nothing else to do here)
  if (!py_object) {
      throw Exception(Exception::recoverable,
                      "py_object was NULL",
                      "PyObjectWrapper::wrap_pyobject<py_object>");
  }
  return py_object;
}

void PyObjectWrapper::lazy_unwrap(PyObject* py_cpp,
                            std::string** string_ret,
                            Json::Value** json_ret,
                            MAUS::Data** data_ret,
                            PyObject** py_ret) {
    if (*string_ret != NULL || *json_ret != NULL || *data_ret != NULL || *py_ret != NULL) {
      throw Exception(Exception::recoverable,
                      "return values not initialised to NULL",
                      "PyObjectWrapper::lazy_unwrap");
    }

    if (!py_cpp) {
      throw Exception(Exception::recoverable,
                      "py_cpp was NULL",
                      "PyObjectWrapper::lazy_unwrap");
    }

    if (TPython::ObjectProxy_Check(py_cpp)) {
        parse_root_object_proxy(py_cpp, data_ret);
    } else if (PyCapsule_IsValid(py_cpp, "JsonCpp")) {
        void* vptr = PyCapsule_GetPointer(py_cpp, "JsonCpp");
        Json::Value *json_ptr = static_cast<Json::Value*>(vptr);
        if (!json_ptr) {
            throw Exception(Exception::recoverable,
                            "Could not parse JsonCpp capsule to Json::Value",
                            "PyObjectWrapper::lazy_unwrap");
        }
        *json_ret = new Json::Value(*json_ptr);
    } else if (PyString_Check(py_cpp)) {
        *string_ret = new std::string(PyString_AsString(py_cpp));
    } else if (PyDict_Check(py_cpp)) {
        *py_ret = PyDict_Copy(py_cpp); // shallow or deep? unclear...
    } else {
        throw Exception(Exception::recoverable,
                        "Could not parse PyObject as a valid data type",
                        "PyObjectWrapper::lazy_unwrap");
    }
}

void PyObjectWrapper::parse_root_object_proxy(PyObject* py_cpp,
                                              MAUS::Data** data_ret) {
    PyObject* name = PyObject_CallMethod(py_cpp,
                                         const_cast<char*>("Class_Name"),
                                         NULL);
    if (!name) {
        throw Exception(Exception::recoverable,
                        "Class_Name method could not be called on ObjectProxy",
                        "PyObjectWrapper::lazy_unwrap");
    }
    char* c_string = NULL;
    if (!PyArg_Parse(name, "s", &c_string)) {
        throw Exception(Exception::recoverable,
                        "Class_Name method did not return a string",
                        "PyObjectWrapper::lazy_unwrap");
    }
    if (strcmp(c_string, "MAUS::Data") == 0) {
        Py_INCREF(py_cpp);  // TPyReturn decrefs py_cpp; we want to keep it
        void * vptr = static_cast<void*>(TPyReturn(py_cpp));
        Data* data = static_cast<Data*>(vptr); // caller owns this memory
        *data_ret = new Data(*data); // we own this memory
        if (!data_ret) {
            throw Exception(Exception::recoverable,
                        "Failed to cast py_cpp as a MAUS::Data",
                        "PyObjectWrapper::lazy_unwrap");
        }
    } else {
            throw Exception(Exception::recoverable,
                        "Did not recognise ObjectProxy type "+\
                        std::string(c_string),
                        "PyObjectWrapper::lazy_unwrap");
    }
}

void PyObjectWrapper::delete_jsoncpp_pycapsule(PyObject* py_capsule) {
    void* void_json = PyCapsule_GetPointer(py_capsule, "JsonCpp");
    Json::Value* json = static_cast<Json::Value*>(void_json);
    if (!void_json || !json) {
        PyErr_Clear();
        throw Exception(Exception::recoverable,
                    "Attempting to delete a JsonCpp capsule but none was found",
                    "PyObjectWrapper::delete_jsoncpp_pycapsule");
    }
    delete json;
}
}  // namespace MAUS

#endif  // _SRC_COMMON_CPP_UTILS_PYOBJECTWRAPPER_INL_HH

