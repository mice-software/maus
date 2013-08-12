#include <Python.h>

#include "src/common_cpp/API/WrapMapBase.hh"
#include "MapCppGlobalRecon.hh"

namespace MAUS {

PyMethodDef ModuleMethods[] = {
  {"module_new",    WrapMapBase<MAUS::MapCppGlobalRecon>::ModuleNew,
   METH_VARARGS,    "Creating a new instance of the Module"},
  {"module_delete", WrapMapBase<MAUS::MapCppGlobalRecon>::ModuleDelete,
   METH_VARARGS,    "Deleting the instance of the Module"},
  {"birth",         WrapMapBase<MAUS::MapCppGlobalRecon>::ModuleBirth,
   METH_VARARGS,    "Running ModuleBase::birth()."},
  {"death",         WrapMapBase<MAUS::MapCppGlobalRecon>::ModuleDeath,
   METH_VARARGS,    "Running ModuleBase::death()."},
  {"set_input",     WrapMapBase<MAUS::MapCppGlobalRecon>::ModuleSetInput,
   METH_VARARGS,    "Running ModuleBase::set_input()."},
  {"get_output",    WrapMapBase<MAUS::MapCppGlobalRecon>::ModuleGetOutput,
   METH_VARARGS,    "Running ModuleBase::get_output()."},
  {"process",       WrapMapBase<MAUS::MapCppGlobalRecon>::ModuleProcess,
   METH_VARARGS,    "Running ModuleBase::process()."},
  {NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC init_MapCppGlobalRecon(void) {
  ModuleClassName = "MapCppGlobalRecon";
  
  PyObject *m;
  m = Py_InitModule("_MapCppGlobalRecon", ModuleMethods);
  if (m == NULL)
    return;

  InvalidModuleError =
      PyErr_NewExceptionWithDoc((char *)"_MapCppGlobalRecon.InvalidModule",
                                (char *)"Invalid Module pointer passed in.",
                                NULL, NULL);
  Py_INCREF(InvalidModuleError);
  PyModule_AddObject(m, "InvalidModule", InvalidModuleError);

  InvalidInputError =
      PyErr_NewExceptionWithDoc((char *)"_MapCppGlobalRecon.InvalidInput",
                                (char *)"Invalid Input passed in.",
                                NULL, NULL);
  Py_INCREF(InvalidInputError);
  PyModule_AddObject(m, "InvalidInput", InvalidInputError);

  InvalidOutputError =
      PyErr_NewExceptionWithDoc((char *)"_MapCppGlobalRecon.InvalidOutput",
                                (char *)"Invalid Output defined.",
                                NULL, NULL);
  Py_INCREF(InvalidOutputError);
  PyModule_AddObject(m, "InvalidOutput", InvalidOutputError);

}

} // ~MAUS
