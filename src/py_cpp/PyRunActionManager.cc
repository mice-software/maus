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

#include "src/common_cpp/DataStructure/RunHeader.hh"
#include "src/common_cpp/DataStructure/RunFooter.hh"

#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/Utils/RunActionManager.hh"

#include "src/py_cpp/PyRunActionManager.hh"

namespace MAUS {
namespace PyRunActionManager {
std::string StartOfRun_DocString =
  std::string("start_of_run(run_number)\n\n")+
  std::string("Call the start of run action for all RunActions registered ")+
  std::string("with the run action manager.\n\n  - run_number is an integer ")+
  std::string("corresponding to the run number of the new run.\n\n")+
  std::string("  - returns None");

std::string EndOfRun_DocString =
  std::string("end_of_run()\n\n")+
  std::string("Call the end of run action for all RunActions registered ")+
  std::string("with the run action manager.\n\n  - takes no arguments.\n\n")+
  std::string("  - returns None");

PyObject* StartOfRun(PyObject *dummy, PyObject *args) {
  int run_number;
  if (!PyArg_ParseTuple(args, "i", &run_number)) {
    PyErr_SetString(PyExc_TypeError,
           "Failed to interpret start_of_run argument as an integer");
    return NULL;
  }
  try {
    RunActionManager* maus_run_action_manager =
                          Globals::GetInstance()->GetRunActionManager();
    if (maus_run_action_manager == NULL) {
      PyErr_SetString(PyExc_RuntimeError,
         "Error - somehow MAUS library was initialised but run action is not.");
      return NULL;
    }
    RunHeader* run_header = new RunHeader();
    run_header->SetRunNumber(run_number);
    maus_run_action_manager->StartOfRun(run_header);
    delete run_header;
  } catch(std::exception& exc) {
    PyErr_SetString(PyExc_RuntimeError, (&exc)->what());
    return NULL;
  }
  Py_INCREF(Py_None);
  return Py_None;
}

PyObject* EndOfRun(PyObject *dummy, PyObject *args) {
  try {
    RunActionManager* maus_run_action_manager =
                          Globals::GetInstance()->GetRunActionManager();
    if (maus_run_action_manager == NULL) {
      PyErr_SetString(PyExc_RuntimeError,
         "Error - somehow MAUS library was initialised but run action is not.");
      return NULL;
    }
    RunFooter* run_footer = new RunFooter();
    maus_run_action_manager->EndOfRun(run_footer);
    delete run_footer;
  } catch(std::exception& exc) {
    PyErr_SetString(PyExc_RuntimeError, (&exc)->what());
    return NULL;
  }
  Py_INCREF(Py_None);
  return Py_None;
}

static PyMethodDef methods[] = {
{"start_of_run", (PyCFunction)StartOfRun,
                         METH_VARARGS, StartOfRun_DocString.c_str()},
{"end_of_run", (PyCFunction)EndOfRun,
                         METH_VARARGS, EndOfRun_DocString.c_str()},
{NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC initrun_action_manager(void) {
  Py_Initialize();
  PyObject* maus_module = Py_InitModule("run_action_manager", methods);
  if (maus_module == NULL) return;
}
}  // namespace PyRunActionManager
}  // namespace MAUS


