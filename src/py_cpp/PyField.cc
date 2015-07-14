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
#include <sstream>

#include "src/legacy/BeamTools/BTFieldConstructor.hh"

#include "src/common_cpp/Utils/Globals.hh"

#include "src/py_cpp/PyField.hh"

namespace MAUS {
namespace PyField {
std::string GetFieldValue_DocString =
  std::string("get_field_value(x, y, z, t)\n\n")+
  std::string("Get the field value at a point in space, time.\n\n")+
  std::string("Return the field at a point (x, y, z, t), in units of mm/ns. ")+
  std::string("Returns a 6-tuple like (bx, by, bz, ex, ey, ez) in units of ")+
  std::string("kT and GV/mm.\ne.g. get_field_value(1000., 2000., 3000., 20.) ")+
  std::string("will return (bx, by, bz, ex, ey, ez) at (x,y,z)=(1,2,3) m and ")+
  std::string("20 ns");

PyObject* GetFieldValue(PyObject *dummy, PyObject *args) {
  double point[4] = {0., 0., 0., 0.};
  double field[6] = {0., 0., 0., 0., 0., 0.};
  if (!PyArg_ParseTuple(args, "dddd", &point[0], &point[1], &point[2], &point[3])) {
    PyErr_SetString(PyExc_TypeError,
           "Failed to interpret get_field_value arguments as x,y,z,t");
    return NULL;
  }
  try {
    BTFieldConstructor* maus_field =
                          Globals::GetInstance()->GetMCFieldConstructor();
    if (maus_field == NULL) {
      PyErr_SetString(PyExc_RuntimeError,
            "Error - somehow MAUS library was initialised but fields are not.");
      return NULL;
    }
    maus_field->GetFieldValue(point, field);
  } catch (std::exception& exc) {
    PyErr_SetString(PyExc_RuntimeError, (&exc)->what());
    return NULL;
  }
  PyObject* py_field = Py_BuildValue("dddddd", field[0], field[1], field[2],
                                               field[3], field[4], field[5]);
  return py_field;
}

std::string Str_DocString =
  std::string("str(is_mc)\n\n")+
  std::string("Get a string describing the fields in MAUS.\n\n")+
  std::string("- is_mc: set to True to print the MC fields; set to False to\n")+
  std::string("         to print the Recon fields\n")+
  std::string("Returns a multiline string, each line describing the\n")+
  std::string("position, rotation, scale factor and field.\n");

PyObject* Str(PyObject *dummy, PyObject *args) {
  PyObject* py_is_mc;
  if (!PyArg_ParseTuple(args, "O", &py_is_mc)) {
        PyErr_SetString(PyExc_TypeError,
               "Failed to interpret str arguments as bool");
        return NULL;
  }
  int is_mc = PyObject_IsTrue(py_is_mc);

  try {
    BTFieldConstructor* maus_field = NULL;
    if (is_mc)
        maus_field = Globals::GetInstance()->GetMCFieldConstructor();
    else
        maus_field = Globals::GetInstance()->GetReconFieldConstructor();
    if (maus_field == NULL) {
      PyErr_SetString(PyExc_RuntimeError,
            "Error - somehow MAUS library was initialised but fields are not.");
      return NULL;
    }
    std::stringstream str_out;
    maus_field->Print(str_out);
    PyObject* py_str = PyString_FromString(str_out.str().c_str());
    return py_str;
  } catch (std::exception& exc) {
    PyErr_SetString(PyExc_RuntimeError, (&exc)->what());
    return NULL;
  }
}

static PyMethodDef methods[] = {
{"str", (PyCFunction)Str, METH_VARARGS, Str_DocString.c_str()},
{"get_field_value", (PyCFunction)GetFieldValue,
                          METH_VARARGS, GetFieldValue_DocString.c_str()},
{NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC initfield(void) {
  Py_Initialize();
  PyObject* maus_module = Py_InitModule("field", methods);
  if (maus_module == NULL) return;
}
}
}


