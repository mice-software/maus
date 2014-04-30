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

#include "src/py_cpp/PyConverter.hh"

namespace MAUS {
namespace PyConverter{
template <class DATAREPR>
PyObject* py_wrap(PyObject* self, PyObject* args) {
  PyObject* py_data_in;
  if (!PyArg_ParseTuple(args, "O", &py_data_in)) {
    return NULL;
  }
  DATAREPR* data_out = NULL;
  PyObject* py_data_out = NULL;
  try {
      // for a moment MAUS owns this memory
      data_out = PyObjectWrapper::unwrap<DATAREPR>(py_data_in);
      // now python owns this memory
      py_data_out = PyObjectWrapper::wrap(data_out);
  } catch(Exception& exc) {
      if (data_out != NULL && py_data_out == NULL)
          throw; // memory was lost, raise it up...
      PyErr_SetString(PyExc_ValueError, (&exc)->what());
  }
  return py_data_out;
}

PyObject* py_wrap_data(PyObject* self, PyObject* args) {
    return py_wrap<Data>(self, args);
}

PyObject* py_wrap_string(PyObject* self, PyObject* args) {
    return py_wrap<std::string>(self, args);
}

PyObject* py_wrap_pyobject(PyObject* self, PyObject* args) {
    return py_wrap<PyObject>(self, args);
}

PyMethodDef methods[] = {
    {"json_repr", (PyCFunction)py_wrap_pyobject, METH_VARARGS,
     "Represent data as a (py)json object\n - data: MICE data in any representation"},
    {"data_repr", (PyCFunction)py_wrap_data, METH_VARARGS,
     "Represent data as a MAUS.Data object\n - data: MICE data in any representation"},
    {"string_repr", (PyCFunction)py_wrap_string, METH_VARARGS,
     "Represent data as a string object\n - data: MICE data in any representation"},
    {NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC initconverter(void) {
  Py_Initialize();
  PyObject* converter_module = Py_InitModule3("converter", methods,
  "Handle data conversions between different representations of the MICE data");
  if (converter_module == NULL) return;
}
}
}

