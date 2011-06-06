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

#include "src/common_cpp/Utils/PyMausCpp.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"

namespace MAUS {

static PyMethodDef MausCpp_methods[] = {
{"SetHandleException", (PyCFunction)CppErrorHandler_SetHandleExceptionFunction,
    METH_VARARGS, "Set the python function that is called to handle exceptions"},
{NULL, NULL, 0, NULL}
};

PyObject* CppErrorHandler_SetHandleExceptionFunction
                                             (PyObject *dummy, PyObject *args) {
  PyObject* temp = NULL;
  if (PyArg_ParseTuple(args, "O:HandleExceptionFunction", &temp)) {
    if (!PyCallable_Check(temp)) {
        PyErr_SetString(PyExc_TypeError,
             "Attempt to set HandleExceptionFunction to non-callable PyObject");
        return NULL;
    }
    Py_XINCREF(temp);
    CppErrorHandler::SetPyErrorHandler(temp);
  }
  Py_INCREF(Py_None);
  return Py_None;
}

PyMODINIT_FUNC initlibMausCpp(void) {
  Py_Initialize();
  PyObject* m = Py_InitModule("libMausCpp", MausCpp_methods);
  if (m == NULL) return;
}

}  // namespace MAUS

