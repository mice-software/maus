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

#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/Globals/GlobalsManager.hh"

#include "src/py_cpp/PyGlobals.hh"

namespace MAUS {
namespace PyGlobals {
std::string Birth_DocString =
  std::string("Initialise MAUS globals.\n\n")+
  std::string("Initialise the MAUS globals such as error handling, C++ ")+
  std::string("logging, field maps and GEANT4 interfaces. ")+
  std::string("Takes one argument which should be the json configuration ")+
  std::string("datacards formatted as a string. Throws an exception if ")+
  std::string("globals are already initialised.");

std::string Death_DocString =
  std::string("Destruct MAUS globals.\n\nIgnores all arguments. ")+
  std::string("Throws an exception if globals are not initialised already.");

std::string HasInstance_DocString =
  std::string("Check if MAUS globals have been initialised. Ignores all ")+
  std::string("arguments. Returns 1 if globals have been initialised, else 0");

std::string GetConfigurationCards_DocString =
  std::string("Return the configuration cards as a string. Throws an exception if globals have")+
  std::string(" not been initialised.");

std::string GetVersionNumber_DocString =
  std::string("Return the MAUS version number as a string like x.y.z.\n\n")+
  std::string("Return the MAUS version number as a string like x.y.z where.")+
  std::string("x is the major version number, y is the minor version number,")+
  std::string("and z is the patch number. Ignores all arguments.");

static PyMethodDef methods[] = {
{"birth", (PyCFunction)Birth,
    METH_VARARGS, Birth_DocString.c_str()},
{"death", (PyCFunction)Death,
    METH_VARARGS, Death_DocString.c_str()},
{"has_instance", (PyCFunction)HasInstance,
    METH_VARARGS, HasInstance_DocString.c_str()},
{"get_configuration_cards", (PyCFunction)GetConfigurationCards,
    METH_VARARGS, GetConfigurationCards_DocString.c_str()},
{"get_version_number", (PyCFunction)GetVersionNumber,
    METH_VARARGS, GetVersionNumber_DocString.c_str()},
    {NULL, NULL, 0, NULL}
};

PyObject* Birth(PyObject *dummy, PyObject *args) {
  const char* temp = NULL;
  if (!PyArg_ParseTuple(args, "s", &temp)) {
    PyErr_SetString(PyExc_TypeError,
         "Failed to recognise arghas_instanceuments to Globals.birth as a string");
    return NULL;
  }
  std::string cards(temp);
  try {
    GlobalsManager::InitialiseGlobals(cards);
  } catch(std::exception& exc) {
    PyErr_SetString(PyExc_RuntimeError, (&exc)->what());
    return NULL;
  }
  Py_INCREF(Py_None);
  return Py_None;
}

PyObject* Death(PyObject *dummy, PyObject *args) {
  try {
    GlobalsManager::DeleteGlobals();
  } catch(std::exception& exc) {  // shouldn't be able to throw an exception
    PyErr_SetString(PyExc_RuntimeError, (&exc)->what());
    return NULL;
  }
  Py_INCREF(Py_None);
  return Py_None;
}

PyObject* HasInstance(PyObject *dummy, PyObject *args) {
  try {
    bool has_instance = Globals::HasInstance();
    if (has_instance)
      return Py_BuildValue("i", 1);
    else
      return Py_BuildValue("i", 0);
  } catch(std::exception& exc) {  // shouldn't be able to throw an exception
    PyErr_SetString(PyExc_RuntimeError, (&exc)->what());
    return NULL;
  }
}

PyObject* GetConfigurationCards(PyObject* dummy, PyObject* args) {
    if (!Globals::HasInstance()) {
        PyErr_SetString(PyExc_RuntimeError,
            "Attempt to get configuration cards but globals not birthed");
        return NULL;
    }
    Json::Value* cards = Globals::GetInstance()->GetConfigurationCards();
    if (cards == NULL) {
        PyErr_SetString(PyExc_RuntimeError,
         "Attempt to get configuration cards failed but cards were not initialised");            
        return NULL;
    }
    std::string json_str = JsonWrapper::JsonToString(*cards);
    const char* json = json_str.c_str();
    PyObject* py_cards = Py_BuildValue("s", json);
    if (py_cards == NULL) {
        PyErr_SetString(PyExc_RuntimeError,
            "Attempt to get configuration cards failed unexpectedly");
        return NULL;
    }
    return py_cards;
}

PyObject* GetVersionNumber(PyObject *dummy, PyObject *args) {
    std::string version_str = Globals::GetVersionNumberString();
    const char* version_cstr = version_str.c_str();
    PyObject* version_py = Py_BuildValue("s", version_cstr);
    if (version_py == NULL) {
        PyErr_SetString(PyExc_RuntimeError,
            "Attempt to get version number failed unexpectedly");
        return NULL;
    }
    return version_py;
}

PyMODINIT_FUNC initglobals(void) {
  Py_Initialize();
  PyObject* maus_module = Py_InitModule("globals", methods);
  if (maus_module == NULL) return;
}
}  // namespace PyGlobals
}  // namespace MAUS

