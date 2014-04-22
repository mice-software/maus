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

#include "TPython.h"
#include "TPyReturn.h"

#include "src/common_cpp/Converter/DataConverters/CppJsonSpillConverter.hh"
#include "src/common_cpp/Converter/DataConverters/JsonCppSpillConverter.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/DataStructure/Data.hh"
#include "src/py_cpp/converter/ObjectProxy.h"

#include "src/py_cpp/converter/PyConverter.hh"

namespace MAUS {
namespace PyConverter{

PyObject* JsonCppSpillConvert(PyObject* dummy, PyObject *args) {
  if (!args) {
      PyErr_SetString(PyExc_ValueError, "No arguments passed");
      return NULL;
  }

  char* c_string = NULL;
  if (!PyArg_ParseTuple(args, "s", &c_string)) {
      PyErr_SetString(PyExc_ValueError, "Could not parse arguments");
      return NULL;
  }

  Json::Value json_value;
  try {
      json_value = JsonWrapper::StringToJson(c_string);
  } catch (std::exception& exc) {
      PyErr_SetString(PyExc_ValueError, (&exc)->what());
      return NULL;
  }
  Data* data = JsonCppSpillConverter().convert(&json_value);
  if (!data) {
      PyErr_SetString(PyExc_ValueError, "Could not convert json to Cpp Spill");
      return NULL;
  }
  PyObject* py_data = TPython::ObjectProxy_FromVoidPtr
                                      (static_cast<void*>(data), "MAUS::Data");
  if (!py_data) {
      PyErr_SetString(PyExc_ValueError, "Could not convert Cpp Spill to PyRoot");
      return NULL;
  }
  return py_data;
}


PyObject* CppJsonSpillConvert(PyObject* dummy, PyObject *args) {
  if (!args) {
      PyErr_SetString(PyExc_ValueError, "No arguments passed");
      return NULL;
  }

  PyObject* py_cpp = NULL;
  if (!PyArg_ParseTuple(args, "O", &py_cpp)) {
      PyErr_SetString(PyExc_ValueError, "Could not parse arguments");
      return NULL;
  }

  if (!TPython::ObjectProxy_Check(py_cpp)) {
      PyErr_SetString(PyExc_ValueError, "Could not convert to C++");
      return NULL;
  }

  PyObject* name = PyObject_CallMethod(py_cpp, (char*)"Class_Name", NULL);
  if (!name) {
      PyErr_SetString(PyExc_ValueError, "ClassName method could not be called");
      return NULL;
  }
  char* c_string = NULL;
  if (!PyArg_Parse(name, "s", &c_string)) {
      PyErr_SetString(PyExc_ValueError, "ClassName method did not return a string");
      return NULL;
  }
  if (strcmp(c_string, "MAUS::Data") != 0) {
      PyErr_SetString(PyExc_ValueError, "Not a MAUS::Data object");
      return NULL;
  }

  void * vptr = (void*)TPyReturn(py_cpp);
  Data* data = static_cast<Data*>(vptr);
  if (!data) {
      PyErr_SetString(PyExc_ValueError, "Failed to make the cast");
      return NULL;
  }

  Json::Value* json_value = CppJsonSpillConverter().convert(data);
  if (!json_value) {
      PyErr_SetString(PyExc_ValueError, "Failed to convert from cpp to json");
      return NULL;
  }

  std::string str_value = JsonWrapper::JsonToString(*json_value);
  delete json_value;
  json_value = NULL;
  
  PyObject* py_value = Py_BuildValue("s", str_value.c_str());
  if (!py_value) {
      PyErr_SetString(PyExc_ValueError, "Failed to convert from json to string");
      return NULL;
  }
  return py_value;
}

static PyMethodDef methods[] = {
    {"_json_to_cpp_spill_convert", (PyCFunction)JsonCppSpillConvert,
    METH_VARARGS, "Please look elsewhere"},
    {"_cpp_to_json_spill_convert", (PyCFunction)CppJsonSpillConvert,
    METH_VARARGS, "Please look elsewhere"},
    {NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC initconverter(void) {
  Py_Initialize();
  PyObject* converter_module = Py_InitModule("converter", methods);
  if (converter_module == NULL) return;
}

}
}

