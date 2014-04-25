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

#include <string>

#include "TPython.h"
#include "json/json.h"

#include "src/common_cpp/Utils/Exception.hh"
#include "src/common_cpp/Utils/PyObjectWrapper.hh"

namespace MAUS {

void PyObjectWrapper::lazy_unwrap(PyObject* py_input,
                            std::string** string_ret,
                            Json::Value** json_ret,
                            MAUS::Data** data_ret,
                            PyObject** py_ret) {
    if (*string_ret != NULL || *json_ret != NULL || *data_ret != NULL || *py_ret != NULL) {
      throw Exception(Exception::recoverable,
                      "py_input return values not initialised to NULL",
                      "PyObjectWrapper::lazy_unwrap");
    }

    if (!py_input) {
      throw Exception(Exception::recoverable,
                      "py_input was NULL",
                      "PyObjectWrapper::lazy_unwrap");
    }
    PyObject* py_cpp = NULL;
    if (!PyArg_ParseTuple(py_input, "O", &py_cpp)) {
        throw Exception(Exception::recoverable,
                      "Could not parse py_input as a tuple of a single PyObject",
                      "PyObjectWrapper::lazy_unwrap");
    }
    if (TPython::ObjectProxy_Check(py_cpp)) {
        parse_root_object_proxy(py_cpp, data_ret);
    } else if (PyCapsule_IsValid(py_cpp, "JsonCpp")) {
        void* vptr = PyCapsule_GetPointer(py_cpp, "json");
        Json::Value *json_ptr = static_cast<Json::Value*>(vptr);
        if(!json_ptr) {
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
    PyObject* name = PyObject_CallMethod(py_cpp, (char*)"Class_Name", NULL);
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
        void * vptr = (void*)TPyReturn(py_cpp);
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
}
