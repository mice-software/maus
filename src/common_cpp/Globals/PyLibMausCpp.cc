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

#include "src/legacy/BeamTools/BTFieldConstructor.hh"

#include "src/common_cpp/Utils/CppErrorHandler.hh"
#include "src/common_cpp/Utils/Globals.hh"

#include "src/common_cpp/Globals/GlobalsManager.hh"
#include "src/common_cpp/Globals/PyLibMausCpp.hh"

namespace MAUS {

namespace PyLibMausCpp {
std::string PyField_GetFieldValue_DocString =
  std::string("Get the field value at a point in space, time.\n\n")+
  std::string("Return the field at a point (x, y, z, t), in units of mm/ns. ")+
  std::string("Returns a 6-tuple like (bx, by, bz, ex, ey, ez) in units of ")+
  std::string("kT and GV/mm.\ne.g. get_field_value(1000., 2000., 3000., 20.) ")+
  std::string("will return (bx, by, bz, ex, ey, ez) at (x,y,z)=(1,2,3) m and ")+
  std::string("20 ns");


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
    CppErrorHandler::getInstance()->SetPyErrorHandler(temp);
  } else {
    PyErr_SetString(PyExc_TypeError,
         "Failed to recognise arguments to libMausCpp.SetHandleException");
    return NULL;
  }
  Py_INCREF(Py_None);
  return Py_None;
}


PyMODINIT_FUNC initlibMausCpp(void) {
  Py_Initialize();
  PyObject* maus_module = Py_InitModule("libMausCpp", MausCpp_methods);
  if (maus_module == NULL) return;
}
}  // namespace PyLibMausCpp
}  // namespace MAUS

