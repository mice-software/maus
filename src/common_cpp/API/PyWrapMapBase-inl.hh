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
 */

#ifndef _SRC_COMMON_CPP_API_PYWRAPMAPBASE_INL_
#define _SRC_COMMON_CPP_API_PYWRAPMAPBASE_INL_

#include <Python.h>

#include <string>

#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/DataStructure/Data.hh"

namespace MAUS {
template <class MODULE>
PyObject* PyWrapMapBase<MODULE>::ModuleNew(PyObject *self, PyObject *args) {
  if (!PyArg_ParseTuple(args,
                        (std::string(":ModuleNew_")+
                         ModuleClassName).c_str())) {
    return NULL;
  }

  MODULE* module = new MODULE();
  void* vptr = static_cast<void*>(module);
  PyObject *resultobj = PyCapsule_New(vptr, "Module", NULL);
  return resultobj;
}

template <class MODULE>
PyObject* PyWrapMapBase<MODULE>::ModuleDelete(PyObject *self, PyObject *args) {
  PyObject* obj0 = NULL;
  if (!PyArg_ParseTuple(args,
                        (std::string("O:ModuleDelete_")+
                         ModuleClassName).c_str(),
                        &obj0)) {
    return NULL;
  }

  if (!PyCapsule_IsValid(obj0, "Module")) {
    std::string error;
    error  = "First input PyCapsule must contain the pointer";
    error += " to the Module";
    PyErr_SetString(InvalidModuleError, error.c_str());
    return NULL;
  }

  void* vptr = PyCapsule_GetPointer(obj0, "Module");
  MODULE* module = static_cast<MODULE*>(vptr);
  delete module;

  Py_RETURN_NONE;
}

template <class MODULE>
PyObject* PyWrapMapBase<MODULE>::ModuleBirth(PyObject *self, PyObject *args) {
  PyObject* obj0 = NULL;
  char* birth_arg = NULL;
  if (!PyArg_ParseTuple(args,
                        (std::string("Os:ModuleBirth_")+
                         ModuleClassName).c_str(),
                        &obj0, &birth_arg)) {
    return NULL;
  }

  if (!PyCapsule_IsValid(obj0, "Module")) {
    std::string error;
    error  = "First input PyCapsule must contain the pointer";
    error += " to the Module";
    PyErr_SetString(InvalidModuleError, error.c_str());
    return NULL;
  }
  void* vptr = PyCapsule_GetPointer(obj0, "Module");
  MODULE* module = static_cast<MODULE*>(vptr);
  std::string argJsonConfigDocument(birth_arg);

  module->birth(argJsonConfigDocument);

  Py_RETURN_NONE;
}

template <class MODULE>
PyObject* PyWrapMapBase<MODULE>::ModuleDeath(PyObject *self, PyObject *args) {
  PyObject* obj0 = NULL;
  if (!PyArg_ParseTuple(args,
                        (std::string("O:ModuleDeath_")+
                         ModuleClassName).c_str(),
                        &obj0)) {
    return NULL;
  }

  if (!PyCapsule_IsValid(obj0, "Module")) {
    std::string error;
    error  = "First input PyCapsule must contain the pointer";
    error += " to the Module";
    PyErr_SetString(InvalidModuleError, error.c_str());
    return NULL;
  }
  void* vptr = PyCapsule_GetPointer(obj0, "Module");
  MODULE* module = static_cast<MODULE*>(vptr);

  module->death();

  Py_RETURN_NONE;
}

template <class MODULE>
PyObject* PyWrapMapBase<MODULE>::ModuleProcess(PyObject *self, PyObject *args) {

  return NULL;
}

template <class MODULE>
PyMethodDef* PyWrapMapBase<MODULE>::GetModuleMethods() {
  PyMethodDef ModuleMethods[] = {
    { "module_new",    ModuleNew,
      METH_VARARGS,    "Creating a new instance of the Module"},
    { "module_delete", ModuleDelete,
      METH_VARARGS,    "Deleting the instance of the Module"},
    { "birth",         ModuleBirth,
      METH_VARARGS,    "Running ModuleBase::birth()."},
    { "death",         ModuleDeath,
      METH_VARARGS,    "Running ModuleBase::death()."},
    { "process",       ModuleProcess,
      METH_VARARGS,    "Running ModuleBase::process()."},
    {NULL, NULL, 0, NULL}
  };
  std::vector<PyMethodDef>* method_vec = new std::vector<PyMethodDef>(8);
  for (size_t i = 0; i < 5; ++i) {
      (*method_vec)[i] = ModuleMethods[i];
  }
  return &((*method_vec)[0]);
}

template <class MODULE>
void PyWrapMapBase<MODULE>::ModuleInitialisation() {
  ModuleClassName = "_"+MODULE().get_classname();

  PyObject *m;
  m = Py_InitModule(ModuleClassName.c_str(), GetModuleMethods());
  if (m == NULL)
    return;

  InvalidModuleError =
      PyErr_NewExceptionWithDoc((char *)(ModuleClassName+".InvalidModule").c_str(),
                                (char *)"Invalid Module pointer passed in.",
                                NULL, NULL);
  Py_INCREF(InvalidModuleError);
  PyModule_AddObject(m, "InvalidModule", InvalidModuleError);

  InvalidInputError =
      PyErr_NewExceptionWithDoc((char *)(ModuleClassName+".InvalidInput").c_str(),
                                (char *)"Invalid Input passed in.",
                                NULL, NULL);
  Py_INCREF(InvalidInputError);
  PyModule_AddObject(m, "InvalidInput", InvalidInputError);

  InvalidOutputError =
      PyErr_NewExceptionWithDoc((char *)(ModuleClassName+".InvalidOutput").c_str(),
                                (char *)"Invalid Output defined.",
                                NULL, NULL);
  Py_INCREF(InvalidOutputError);
  PyModule_AddObject(m, "InvalidOutput", InvalidOutputError);
}
}  // ~MAUS
#endif
