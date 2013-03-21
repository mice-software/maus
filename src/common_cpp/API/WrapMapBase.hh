#ifndef _SRC_COMMON_CPP_API_WRAPMAPBASE_
#define _SRC_COMMON_CPP_API_WRAPMAPBASE_

#include <Python.h>

#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/DataStructure/Data.hh"

namespace MAUS {

static PyObject *InvalidModuleError;
static PyObject *InvalidInputError;

static std::string ModuleClassName;

template<class MODULE>
class WrapMapBase {
 public:

  static PyObject* ModuleNew(PyObject *self, PyObject *args) {
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
  
  static PyObject* ModuleDelete(PyObject *self, PyObject *args) {
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

  static PyObject* ModuleBirth(PyObject *self, PyObject *args) {
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

  static PyObject* ModuleDeath(PyObject *self, PyObject *args) {
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

  static PyObject* ModuleProcess(PyObject *self, PyObject *args) {
    // We need to check for one of three inputs: string, a Json::Value
    // inside a PyCapsule or a MAUS::Data inside a PyCapsule.
    bool input_is_string = false;
    bool input_is_json = false;
    bool input_is_data = false;
  
    PyObject* obj0 = NULL;
    const char *input_char = NULL;
    PyObject* obj1 = NULL;

    while(true) {
      if (PyArg_ParseTuple(args,
                           (std::string("Os:ModuleProcess")+
                            ModuleClassName).c_str(),
                           &obj0, &input_char)) {
        if (!PyCapsule_IsValid(obj0, "Module")) {
          std::string error;
          error  = "First input PyCapsule must contain the pointer";
          error += " to the Module";
          PyErr_SetString(InvalidModuleError, error.c_str());
          return NULL;
        }
        input_is_string = true;
        break;
      }

      // The failed conversion will have raised a Python Error
      // exception.  Let's clear that now, as we have other options.
      PyErr_Clear();
    
      if (PyArg_ParseTuple(args,
                           (std::string("OO:ModuleProcess")+
                            ModuleClassName).c_str(),
                           &obj0, &obj1)) {
        if(!PyCapsule_IsValid(obj0, "Module")) {
          std::string error;
          error  = "First input PyCapsule must contain the pointer";
          error += " to the Module";
          PyErr_SetString(InvalidModuleError, error.c_str());
          return NULL;
        }
        if(PyCapsule_IsValid(obj1, "Json::Value")) {
          input_is_json = true;
          break;
        } else if(PyCapsule_IsValid(obj1, "MAUS::Data")) {
          input_is_data = true;
          break;
        } else {
          std::string error;
          error  = "Second input PyCapsule must contain 'Json::Value'";
          error += " or 'MAUS::Data'";
          PyErr_SetString(InvalidInputError, error.c_str());
          return NULL;
        }
      }
      std::string error;
      error  = "Input parameters must be 'Module'";
      error +=" + 'string' or 'Module' + 'PyCapsule'";
      PyErr_SetString(InvalidInputError, error.c_str());
      return NULL;
    }
  
    // Get the module
    void* vptr0 = PyCapsule_GetPointer(obj0, "Module");
    MODULE* module = static_cast<MODULE*>(vptr0);
  
    // Get the correct input object, and run process
    MAUS::Data* result = NULL;
    if(input_is_string) {
      std::string input_str(input_char);
      // Convert the string to a Json::Value
      Json::Value imported_json = JsonWrapper::StringToJson(input_str);
      result = module->process(&imported_json);
    } else if(input_is_json) {
      void* vptr1 = PyCapsule_GetPointer(obj1, "Json::Value");
      Json::Value* input_json = static_cast<Json::Value*>(vptr1);
      result = module->process(input_json);
    } else if(input_is_data) {
      void* vptr2 = PyCapsule_GetPointer(obj1, "MAUS::Data");
      MAUS::Data* input_data = static_cast<MAUS::Data*>(vptr2);
      result = module->process(input_data);
    } else {
      PyErr_SetString(InvalidInputError, "No Valid Input selection found");    
      return NULL;
    }

    void* vptr3 = static_cast<void*>(result);
    PyObject *resultobj = PyCapsule_New(vptr3, "MAUS::Data", NULL);
    return resultobj;
  }
};

} // ~MAUS
#endif
