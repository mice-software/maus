#ifndef _SRC_COMMON_CPP_API_WRAPMAPBASE_
#define _SRC_COMMON_CPP_API_WRAPMAPBASE_

#include <Python.h>

#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/DataStructure/Data.hh"

namespace MAUS {

static PyObject *InvalidModuleError;
static PyObject *InvalidInputError;
static PyObject *InvalidOutputError;

static std::string ModuleClassName;

template<class MODULE>
class WrapMapBase {
 public:

  /// Create a new instance of the MODULE parameter
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

  /// Delete the MODULE object owned by the wrapper class
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

  /// Run the MODULE object's birth function.
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

  /// Run the MODULE object's death function.
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

  /// Set the MODULE object's input format.
  static PyObject* ModuleSetInput(PyObject *self, PyObject *args) {

    PyObject* obj0 = NULL;
    const char *input_char = NULL;

    // Interpret the input as a PyObject* and a string, otherwise fail.
    if (!PyArg_ParseTuple(args,
                          (std::string("Os:ModuleSetInput")+
                           ModuleClassName).c_str(),
                          &obj0, &input_char)) {
      std::string error;
      error  = "Input parameters must be 'Module'";
      error +=" + 'string', where string defines the input type.";
      PyErr_SetString(InvalidInputError, error.c_str());
      return NULL;
    }

    // Check the PyObject* is a MODULE*
    if (!PyCapsule_IsValid(obj0, "Module")) {
      std::string error;
      error  = "First input PyCapsule must contain the pointer";
      error += " to the Module";
      PyErr_SetString(InvalidModuleError, error.c_str());
      return NULL;
    }

    // Get the module
    void* vptr0 = PyCapsule_GetPointer(obj0, "Module");
    MODULE* module = static_cast<MODULE*>(vptr0);

    // Check the MODULE* is valid
    if (!module) {
      std::string error;
      error  = "First input Module is invalid pointer";
      PyErr_SetString(InvalidModuleError, error.c_str());
      return NULL;
    }

    module->set_input(input_char);
    Py_RETURN_NONE;
  }

  /// Get the MODULE object's output format.
  static PyObject* ModuleGetOutput(PyObject *self, PyObject *args) {

    PyObject* obj0 = NULL;

    // Interpret the input as a PyObject*, otherwise fail.
    if (!PyArg_ParseTuple(args,
                          (std::string("O:ModuleGetOutput")+
                           ModuleClassName).c_str(),
                          &obj0)) {
      std::string error;
      error  = "Input parameters must be 'Module'";
      PyErr_SetString(InvalidInputError, error.c_str());
      return NULL;
    }

    // Check the PyObject* is a MODULE*,
    if (!PyCapsule_IsValid(obj0, "Module")) {
      std::string error;
      error  = "First input PyCapsule must contain the pointer";
      error += " to the Module";
      PyErr_SetString(InvalidModuleError, error.c_str());
      return NULL;
    }

    // Get the module
    void* vptr0 = PyCapsule_GetPointer(obj0, "Module");
    MODULE* module = static_cast<MODULE*>(vptr0);

    // Check the MODULE* is valid
    if (!module) {
      std::string error;
      error  = "First input Module is invalid pointer";
      PyErr_SetString(InvalidModuleError, error.c_str());
      return NULL;
    }

    std::string temp = module->get_output();

    return PyString_FromString(temp.c_str());
  }

  /// Run the MODULE object's process function.
  static PyObject* ModuleProcess(PyObject *self, PyObject *args) {
    // Parse two elements, the module and the data object
    PyObject* obj0 = NULL;
    PyObject* obj1 = NULL;
    std::string format = "OO:ModuleProcess";
    format += ModuleClassName;
    if (!PyArg_ParseTuple(args,
                          format.c_str(),
                          &obj0, &obj1)) {
      std::string error;
      error  = "Unable to interpret inputs as two PyObject pointers";
      PyErr_SetString(InvalidInputError, error.c_str());
      return NULL;
    }

    // First object is going to be the module
    if (!PyCapsule_IsValid(obj0, "Module")) {
      std::string error;
      error  = "First input PyCapsule must contain the pointer";
      error += " to the Module";
      PyErr_SetString(InvalidModuleError, error.c_str());
      return NULL;
    }
    void* vptr0 = PyCapsule_GetPointer(obj0, "Module");
    MODULE* module = static_cast<MODULE*>(vptr0);
  
    // input_type can be either: std::string, MAUS::Data* or Json::Value*.
    std::string input_type  = module->get_input();
    
    if(input_type.compare("std::string") == 0) {
      // std::string input
      if(!PyString_Check(obj1)) {
        std::string error;
        error  = "Expected std::string, data of wrong type";
        PyErr_SetString(InvalidInputError, error.c_str());
        return NULL;
      }        
      std::string event_string(PyString_AsString(obj1));
      
      // Convert the string to a Json::Value
      Json::Value imported_json = JsonWrapper::StringToJson(event_string);
      PyObject *resultobj = module->process_pyobj(&imported_json);
      return resultobj;
    }
    
    if(input_type.compare("Json::Value") == 0) {
      // Json::Value* input
      if(!PyCapsule_IsValid(obj1, "Json::Value")) {
        std::string error;
        error  = "Expected Json::Value pointer, data of wrong type";
        PyErr_SetString(InvalidInputError, error.c_str());
        return NULL;
      }        
      void* vptr1 = PyCapsule_GetPointer(obj1, "Json::Value");
      Json::Value* input_json = static_cast<Json::Value*>(vptr1);
      PyObject *resultobj = module->process_pyobj(input_json);
      return resultobj;
    }
    
    if(input_type.compare("MAUS::Data") == 0) {
      // MAUS::Data* input
      if(!PyCapsule_IsValid(obj1, "MAUS::Data")) {
        std::string error;
        error  = "Expected MAUS::Data pointer, data of wrong type";
        PyErr_SetString(InvalidInputError, error.c_str());
        return NULL;
      }        
      void* vptr1 = PyCapsule_GetPointer(obj1, "MAUS::Data");
      MAUS::Data* input_data = static_cast<MAUS::Data*>(vptr1);
      PyObject *resultobj = module->process_pyobj(input_data);
      return resultobj;
    }
    
    std::string error;
    error  = "Second input PyCapsule must contain; 'std::string',";
    error += " 'Json::Value' or 'MAUS::Data'";
    PyErr_SetString(InvalidInputError, error.c_str());
    return NULL;
    
  }
};
} // ~MAUS
#endif
