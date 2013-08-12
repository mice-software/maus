#include <Python.h>

#include "src/common_cpp/API/WrapMapBase.hh"
#include "MapCppExampleJSONValueInput.hh"

namespace MAUS {

PyMethodDef ModuleMethods[] = {
  {"module_new",    WrapMapBase<MAUS::MapCppExampleJSONValueInput>::ModuleNew,
   METH_VARARGS,    "Creating a new instance of the Module"},
  {"module_delete", WrapMapBase<MAUS::MapCppExampleJSONValueInput>::ModuleDelete,
   METH_VARARGS,    "Deleting the instance of the Module"},
  {"birth",         WrapMapBase<MAUS::MapCppExampleJSONValueInput>::ModuleBirth,
   METH_VARARGS,    "Running ModuleBase::birth()."},
  {"death",         WrapMapBase<MAUS::MapCppExampleJSONValueInput>::ModuleDeath,
   METH_VARARGS,    "Running ModuleBase::death()."},
  {"set_input",     WrapMapBase<MAUS::MapCppExampleJSONValueInput>::ModuleSetInput,
   METH_VARARGS,    "Running ModuleBase::set_input()."},
  {"get_output",    WrapMapBase<MAUS::MapCppExampleJSONValueInput>::ModuleGetOutput,
   METH_VARARGS,    "Running ModuleBase::get_output()."},
  {"process",       WrapMapBase<MAUS::MapCppExampleJSONValueInput>::ModuleProcess,
   METH_VARARGS,    "Running ModuleBase::process()."},
  {NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC init_MapCppExampleJSONValueInput(void) {
  ModuleClassName = "MapCppExampleJSONValueInput";
  
  PyObject *m;
  m = Py_InitModule("_MapCppExampleJSONValueInput", ModuleMethods);
  if (m == NULL)
    return;

  InvalidModuleError =
      PyErr_NewExceptionWithDoc((char *)"_MapCppExampleJSONValueInput.InvalidModule",
                                (char *)"Invalid Module pointer passed in.",
                                NULL, NULL);
  Py_INCREF(InvalidModuleError);
  PyModule_AddObject(m, "InvalidModule", InvalidModuleError);

  InvalidInputError =
      PyErr_NewExceptionWithDoc((char *)"_MapCppExampleJSONValueInput.InvalidInput",
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
