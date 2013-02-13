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
#include "src/common_cpp/JsonCppProcessors/RunHeaderProcessor.hh"
#include "src/common_cpp/DataStructure/RunFooter.hh"
#include "src/common_cpp/JsonCppProcessors/RunFooterProcessor.hh"

#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/Utils/RunActionManager.hh"

#include "src/py_cpp/PyRunActionManager.hh"

namespace MAUS {
namespace PyRunActionManager {
std::string StartOfRun_DocString =
  std::string("start_of_run(run_number)\n\n")+
  std::string("Call the start of run action for all RunActions registered ")+
  std::string("with the run action manager.\n\n")+
  std::string("\\param run_number is an integer corresponding to the run ")+
  std::string("number of the new run.\n\n")+
  std::string("\\returns string representation of the RunHeader (json format)");

std::string EndOfRun_DocString =
  std::string("end_of_run(run_number)\n\n")+
  std::string("Call the end of run action for all RunActions registered ")+
  std::string("with the run action manager.\n\n")+
  std::string("\\param run_number is an integer corresponding to the run ")+
  std::string("number of the new run.\n\n")+
  std::string("\\returns string representation of the RunFooter (json format)");

PyObject* StartOfRun(PyObject *dummy, PyObject *args) {
  int run_number;
  std::string head_str;
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
    Json::Value* head_json = RunHeaderProcessor().CppToJson(*run_header, "");
    Py_BuildValue("s", head_str.c_str());
    head_str = JsonWrapper::JsonToString(*head_json);
    delete head_json;
    delete run_header;
  } catch(std::exception& exc) {
    PyErr_SetString(PyExc_RuntimeError, (&exc)->what());
    return NULL;
  }
  return Py_BuildValue("s", head_str.c_str());
}

PyObject* EndOfRun(PyObject *dummy, PyObject *args) {
  int run_number;
  std::string foot_str;
  if (!PyArg_ParseTuple(args, "i", &run_number)) {
    PyErr_SetString(PyExc_TypeError,
           "Failed to interpret end_of_run argument as an integer");
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
    RunFooter* run_footer = new RunFooter();
    run_footer->SetRunNumber(run_number);
    maus_run_action_manager->EndOfRun(run_footer);
    Json::Value* foot_json = RunFooterProcessor().CppToJson(*run_footer, "");
    Py_BuildValue("s", foot_str.c_str());
    foot_str = JsonWrapper::JsonToString(*foot_json);
    delete foot_json;
    delete run_footer;
  } catch(std::exception& exc) {
    PyErr_SetString(PyExc_RuntimeError, (&exc)->what());
    return NULL;
  }
  return Py_BuildValue("s", foot_str.c_str());
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


