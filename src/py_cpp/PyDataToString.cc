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

#include "src/py_cpp/PyDataToString.hh"

#include <string>

#include "src/common_cpp/Converter/DataConverters/CppJsonSpillConverter.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"

namespace MAUS {
namespace PyDataToString {

std::string Convert_DocString =
  std::string("Convert an input PyCapsule object, containing either\n\n")+
    std::string("MAUS::Data or Json::Value pointer into a std::string.\n\n")+
    std::string("This can then be used by any of the MapPyXXX methods.");
    
PyObject* Convert(PyObject *dummy, PyObject *args) {
  // Input shoudl be a single PyCapsule
  PyObject* obj0 = NULL;
  if (!PyArg_ParseTuple(args,"O:Convert", &obj0)) {
    return NULL;
  }

  // Read the Input into a Json::Value type
  Json::Value* json_value = NULL;
  void* vptr;
  if(PyCapsule_IsValid(obj0, "Json::Value")) {
    // Input is Json::Value type, so fill the json_value pointer.
    vptr = PyCapsule_GetPointer(obj0, "Json::Value");
    json_value = static_cast<Json::Value*>(vptr);
  } else if(PyCapsule_IsValid(obj0, "MAUS::Data")) {
    // Input is MAUS::Data type, so call a converter to produce a Json::Value.
    vptr = PyCapsule_GetPointer(obj0, "MAUS::Data");
    MAUS::Data* data_cpp = static_cast<MAUS::Data*>(vptr);
    CppJsonSpillConverter c2j_converter;
    json_value = c2j_converter(data_cpp);
  }

  // Convert the Json::Value to a std::string
  std::string json_str = JsonWrapper::JsonToString(*json_value);

  // Export the std::string is a Python string.
  PyObject* py_string = PyString_FromString(json_str.c_str());
  return py_string;
}

static PyMethodDef methods[] = {
{"convert", (PyCFunction)Convert,
 METH_VARARGS, Convert_DocString.c_str()},
{NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC initdata_to_string(void) {
  Py_Initialize();
  PyObject* maus_module = Py_InitModule("data_to_string", methods);
  if (maus_module == NULL) return;
}
}
}


