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

#include <Python.h>

#include "src/common_cpp/API/WrapMapBase.hh"
#include "src/map/MapCppExampleMAUSDataInput/MapCppExampleMAUSDataInput.hh"

namespace MAUS {

PyMethodDef ModuleMethods[] = {
  { "module_new",    WrapMapBase<MAUS::MapCppExampleMAUSDataInput>::ModuleNew,
    METH_VARARGS,    "Creating a new instance of the Module"},
  { "module_delete", WrapMapBase<MAUS::MapCppExampleMAUSDataInput>::ModuleDelete,
    METH_VARARGS,    "Deleting the instance of the Module"},
  { "birth",         WrapMapBase<MAUS::MapCppExampleMAUSDataInput>::ModuleBirth,
    METH_VARARGS,    "Running ModuleBase::birth()."},
  { "death",         WrapMapBase<MAUS::MapCppExampleMAUSDataInput>::ModuleDeath,
    METH_VARARGS,    "Running ModuleBase::death()."},
  { "set_input",     WrapMapBase<MAUS::MapCppExampleMAUSDataInput>::ModuleSetInput,
    METH_VARARGS,    "Running ModuleBase::set_input()."},
  { "get_output",    WrapMapBase<MAUS::MapCppExampleMAUSDataInput>::ModuleGetOutput,
    METH_VARARGS,    "Running ModuleBase::get_output()."},
  { "process",       WrapMapBase<MAUS::MapCppExampleMAUSDataInput>::ModuleProcess,
    METH_VARARGS,    "Running ModuleBase::process()."},
  {NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC init_MapCppExampleMAUSDataInput(void) {
  ModuleClassName = "MapCppExampleMAUSDataInput";

  PyObject *m;
  m = Py_InitModule("_MapCppExampleMAUSDataInput", ModuleMethods);
  if (m == NULL)
    return;

  InvalidModuleError =
      PyErr_NewExceptionWithDoc((char *)"_MapCppExampleMAUSDataInput.InvalidModule",
                                (char *)"Invalid Module pointer passed in.",
                                NULL, NULL);
  Py_INCREF(InvalidModuleError);
  PyModule_AddObject(m, "InvalidModule", InvalidModuleError);

  InvalidInputError =
      PyErr_NewExceptionWithDoc((char *)"_MapCppExampleMAUSDataInput.InvalidInput",
                                (char *)"Invalid Input passed in.",
                                NULL, NULL);
  Py_INCREF(InvalidInputError);
  PyModule_AddObject(m, "InvalidInput", InvalidInputError);

  InvalidOutputError =
      PyErr_NewExceptionWithDoc((char *)"_MapCppExampleMAUSDataInput.InvalidOutput",
                                (char *)"Invalid Output defined.",
                                NULL, NULL);
  Py_INCREF(InvalidOutputError);
  PyModule_AddObject(m, "InvalidOutput", InvalidOutputError);
}
}  // ~MAUS
