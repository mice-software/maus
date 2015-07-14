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

#include "src/legacy/Config/MiceModule.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/Globals/GlobalsManager.hh"

#include "src/py_cpp/PyMiceModule.hh"

#include "src/py_cpp/PyGlobals.hh"

namespace MAUS {
namespace PyGlobals {
std::string Birth_DocString =
std::string("Initialise MAUS globals.\n\n")+
std::string("Initialise the MAUS globals such as error handling, C++\n")+
std::string("logging, field maps and GEANT4 interfaces.\n")+
std::string("  Takes one argument which should be the json configuration\n")+
std::string("datacards formatted as a string. Throws an exception if\n")+
std::string("globals are already initialised.");

std::string Death_DocString =
std::string("Destruct MAUS globals.\n\n  Ignores all arguments.\n")+
std::string("Throws an exception if globals are not initialised already.\n");

std::string HasInstance_DocString =
std::string("Check if MAUS globals have been initialised.\n\n")+
std::string("  Ignores all arguments.\n")+
std::string("Returns 1 if globals have been initialised, else 0.");

std::string GetConfigurationCards_DocString =
std::string("Return the configuration cards as a string.\n\n")+
std::string("Throws an exception if globals have not been initialised.");

std::string GetMonteCarloMiceModules_DocString =
std::string("Get the geometry used for simulation.\n\n")+
std::string("  Ignores all arguments.\n")+
std::string("Return a deepcopy of the simulation geometry as a MiceModule\n")+
std::string("object. Note that the actual geometry will only be updated by a\n")+
std::string("call to set_monte_carlo_mice_modules(...)\n")+
std::string("Throws an exception if globals have not been initialised.");

std::string SetMonteCarloMiceModules_DocString =
std::string("Set the simulation geometry and fields.\n\n")+
std::string("  - modules (MiceModule) the new geometry object.\n")+
std::string("Returns None. Throws an exception if globals have not been\n")+
std::string("initialised.");

std::string GetReconstructionMiceModules_DocString =
std::string("Get the geometry used for the reconstruction.\n\n")+
std::string("  Ignores all arguments.\n")+
std::string("Return a deepcopy of the reconstruction geometry as a MiceModule\n")+
std::string("object. Note that the actual geometry will only be updated by a\n")+
std::string("call to set_reconstruction_mice_modules(...)\n")+
std::string("Throws an exception if globals have not been initialised.");

std::string SetReconstructionMiceModules_DocString =
std::string("Set the reconstruction geometry and fields.\n\n")+
std::string("  - modules (MiceModule) the new geometry object.\n")+
std::string("Returns None. Throws an exception if globals have not been\n")+
std::string("initialised.");

std::string GetVersionNumber_DocString =
std::string("Return the MAUS version number as a string like x.y.z.\n\n")+
std::string("Return the MAUS version number as a string like x.y.z where\n")+
std::string("x is the major version number, y is the minor version number,\n")+
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
{"get_monte_carlo_mice_modules", (PyCFunction)GetMonteCarloMiceModules,
    METH_VARARGS|METH_KEYWORDS, GetMonteCarloMiceModules_DocString.c_str()},
{"set_monte_carlo_mice_modules", (PyCFunction)SetMonteCarloMiceModules,
    METH_VARARGS|METH_KEYWORDS, SetMonteCarloMiceModules_DocString.c_str()},
{"get_reconstruction_mice_modules", (PyCFunction)GetReconstructionMiceModules,
    METH_VARARGS|METH_KEYWORDS, GetReconstructionMiceModules_DocString.c_str()},
{"set_reconstruction_mice_modules", (PyCFunction)SetReconstructionMiceModules,
    METH_VARARGS|METH_KEYWORDS, SetReconstructionMiceModules_DocString.c_str()},
{"get_version_number", (PyCFunction)GetVersionNumber,
    METH_VARARGS, GetVersionNumber_DocString.c_str()},
    {NULL, NULL, 0, NULL}
};

PyObject* Birth(PyObject *dummy, PyObject *args) {
  const char* temp = NULL;
  if (!PyArg_ParseTuple(args, "s", &temp)) {
    PyErr_SetString(PyExc_TypeError,
         "Failed to recognise arguments to Globals.birth as a string");
    return NULL;
  }
  std::string cards(temp);
  try {
    GlobalsManager::InitialiseGlobals(cards);
  } catch (std::exception& exc) {
    PyErr_SetString(PyExc_RuntimeError, (&exc)->what());
    return NULL;
  }
  Py_INCREF(Py_None);
  return Py_None;
}

PyObject* Death(PyObject *dummy, PyObject *args) {
  try {
    GlobalsManager::DeleteGlobals();
  } catch (std::exception& exc) {  // shouldn't be able to throw an exception
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
  } catch (std::exception& exc) {  // shouldn't be able to throw an exception
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

PyObject* GetMonteCarloMiceModules
                             (PyObject* dummy, PyObject* args, PyObject *kwds) {
    if (!Globals::HasInstance()) {
        PyErr_SetString(PyExc_RuntimeError,
                  "Attempt to get MC mice modules but globals not birthed");
        return NULL;
    }
    PyObject* py_mod = MAUS::PyMiceModule::create_empty_module();
    if (py_mod == NULL) {
        return NULL;
    }
    MiceModule* mod_orig = Globals::GetMonteCarloMiceModules();
    MiceModule* mod = mod_orig->deepCopy(*mod_orig, false);
    int success = PyMiceModule::set_mice_module(py_mod, mod);
    if (success == 0) {
        delete mod;
        return NULL;
    }
    return py_mod;
}

PyObject* SetMonteCarloMiceModules
                             (PyObject* self, PyObject* args, PyObject *kwds) {
    if (!Globals::HasInstance()) {
        PyErr_SetString(PyExc_RuntimeError,
                  "Attempt to set MC mice modules but globals not birthed");
        return NULL;
    }
    PyObject* py_mod = NULL;
    static char *kwlist[] = {const_cast<char*>("module"), NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O", kwlist, &py_mod)) {
        return NULL;
    }

    MiceModule* mod = MiceModule::deepCopy
                               (*PyMiceModule::get_mice_module(py_mod), false);
    if (mod == NULL) {
        return NULL;
    }
    try {
        GlobalsManager::SetMonteCarloMiceModules(mod);
    } catch (std::exception& exc) {
        std::string message = std::string("Failed to set MiceModules\n")+
                              std::string((&exc)->what());
        PyErr_SetString(PyExc_ValueError, message.c_str());
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}

PyObject* GetReconstructionMiceModules
                             (PyObject* dummy, PyObject* args, PyObject *kwds) {
    if (!Globals::HasInstance()) {
        PyErr_SetString(PyExc_RuntimeError,
                  "Attempt to get recon mice modules but globals not birthed");
        return NULL;
    }
    PyObject* py_mod = MAUS::PyMiceModule::create_empty_module();
    if (py_mod == NULL) {
        return NULL;
    }
    MiceModule* mod_orig = Globals::GetReconstructionMiceModules();
    MiceModule* mod = mod_orig->deepCopy(*mod_orig, false);
    int success = PyMiceModule::set_mice_module(py_mod, mod);
    if (success == 0) {
        delete mod;
        return NULL;
    }
    return py_mod;
}

PyObject* SetReconstructionMiceModules
                             (PyObject* self, PyObject* args, PyObject *kwds) {
    if (!Globals::HasInstance()) {
        PyErr_SetString(PyExc_RuntimeError,
                  "Attempt to set recon mice modules but globals not birthed");
        return NULL;
    }
    PyObject* py_mod = NULL;
    static char *kwlist[] = {const_cast<char*>("module"), NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O", kwlist, &py_mod)) {
        return NULL;
    }

    MiceModule* mod = MiceModule::deepCopy
                               (*PyMiceModule::get_mice_module(py_mod), false);
    if (mod == NULL) {
        return NULL;
    }
    try {
        GlobalsManager::SetReconstructionMiceModules(mod);
    } catch (std::exception& exc) {
        std::string message = std::string("Failed to set MiceModules\n")+
                              std::string((&exc)->what());
        PyErr_SetString(PyExc_ValueError, message.c_str());
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}

PyMODINIT_FUNC initglobals(void) {
  Py_Initialize();
  PyObject* maus_module = Py_InitModule("globals", methods);
  if (maus_module == NULL) return;
  MAUS::PyMiceModule::import_PyMiceModule();
}
}  // namespace PyGlobals
}  // namespace MAUS

