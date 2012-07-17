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

#include "src/common_cpp/GlobalsHandling/GlobalsManager.hh"
#include "src/common_cpp/GlobalsHandling/GlobalsManagerFactory.hh"
#include "src/common_cpp/GlobalsHandling/PyMausCpp.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"

namespace MAUS {

std::string GlobalsManager_Initialise_DocString =
  "Initialise MAUS globlas. Takes one argument which should be the json "+
  std::string("datacards formatted as a string");
std::string GlobalsManager_Destruct_DocString =
  "Destruct MAUS globals. Ignores all arguments.";
std::string GlobalsManager_HasInstance_DocString =
  "Check if MAUS globals have been initialised. Ignores all arguments. "+
  std::string("Returns 1 if globals have been initialised, else 0");

std::string PyField_GetFieldValue_DocString =
  std::string("Get the field value at a point in space, time.\n\n")+
  std::string("Return the field at a point (x, y, z, t), in units of mm/ns.")+
  std::string("Returns a 6-tuple like (bx, by, bz, ex, ey, ez) in units of")+
  std::string("kT and GV/mm");


static PyMethodDef MausCpp_methods[] = {
{"initialise", (PyCFunction)GlobalsManager_Initialise,
    METH_VARARGS, GlobalsManager_Initialise_DocString.c_str()},
{"destruct", (PyCFunction)GlobalsManager_Destruct,
    METH_VARARGS, GlobalsManager_Destruct_DocString.c_str()},
{"has_instance", (PyCFunction)GlobalsManager_HasInstance,
    METH_VARARGS, GlobalsManager_HasInstance_DocString.c_str()},
{"SetHandleException", (PyCFunction)CppErrorHandler_SetHandleExceptionFunction,
    METH_VARARGS, "Set the python function that is called to handle exceptions"},
{NULL, NULL, 0, NULL}
};

PyObject* GlobalsManager_Initialise(PyObject *dummy, PyObject *args) {
  const char* temp = NULL;
  if (!PyArg_ParseTuple(args, "s", &temp)) {
    PyErr_SetString(PyExc_TypeError,
         "Failed to recognise arguments to libMausCpp.Initialise as a string");
    return NULL;
  }
  std::string cards(temp); 
  try {
    GlobalsManagerFactory::InitialiseGlobalsManager(cards);
  } catch(std::exception& exc) {
    PyErr_SetString(PyExc_RuntimeError, (&exc)->what());
    return NULL;
  }
  Py_INCREF(Py_None);
  return Py_None;
}

PyObject* GlobalsManager_Destruct(PyObject *dummy, PyObject *args) {
  try {
    GlobalsManagerFactory::DeleteGlobalsManager();
  } catch(std::exception& exc) {  // shouldn't be able to throw an exception
    PyErr_SetString(PyExc_RuntimeError, (&exc)->what());
    return NULL;
  }
  Py_INCREF(Py_None);
  return Py_None;
}


PyObject* GlobalsManager_HasInstance(PyObject *dummy, PyObject *args) {
  try {
    bool has_instance = GlobalsManager::HasInstance();
    if (has_instance)
      return Py_BuildValue("i", 1);
    else
      return Py_BuildValue("i", 0);
  } catch(std::exception& exc) {  // shouldn't be able to throw an exception
    PyErr_SetString(PyExc_RuntimeError, (&exc)->what());
    return NULL;
  }
}

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

PyObject* PyField_GetFieldValue(PyObject *dummy, PyObject *args) {
  double point[4] = {0., 0., 0., 0.};
  double field[6] = {0., 0., 0., 0., 0., 0.};
  if (!PyArg_ParseTuple(args, "dddd", &point[0], &point[1], &point[2], &point[3])) {
    PyErr_SetString(PyExc_TypeError,
           "Failed to interpret get_field_value arguments as x,y,z,t");
    return NULL;
  }
  if (!GlobalsManager::HasInstance()) {
    PyErr_SetString(PyExc_RuntimeError,
           "Attempt to get field when MAUS library has not been initialised");
    return NULL;
  }
  BTFieldConstructor* maus_field =
                        GlobalsManager::GetInstance()->GetBTFieldConstructor();
  if (maus_field == NULL) {
    PyErr_SetString(PyExc_RuntimeError,
           "Error - somehow MAUS library was initialised but fields are not.");
    return NULL;
  }
  try {
    maus_field->GetFieldValue(point, field);
  } catch(std::exception& exc) {
    PyErr_SetString(PyExc_RuntimeError, (&exc)->what());
    return NULL;
  }
  PyObject* py_field = Py_BuildValue("dddddd", field[0], field[1], field[2],
                                               field[3], field[4], field[5]);
  return py_field;
}

static PyMethodDef PyField_methods[] = {
{"get_field_value", (PyCFunction)PyField_GetFieldValue,
   METH_VARARGS, PyField_GetFieldValue_DocString.c_str()},
{NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC initlibMausCpp(void) {
  Py_Initialize();
  PyObject* maus_module = Py_InitModule("libMausCpp", MausCpp_methods);
  if (maus_module == NULL) return;
  PyObject* field_module = Py_InitModule("libMausCpp.Field", PyField_methods);
  if (field_module == NULL) return;
  int success = PyModule_AddObject(maus_module, "Field", field_module);
  if (success == -1) {
    PyErr_SetString(PyExc_TypeError, "Failed to set up MAUS field module");
    // I think memory should be handled by Python garbage collection
  }
}

}  // namespace MAUS

