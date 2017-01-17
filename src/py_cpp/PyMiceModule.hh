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

#ifndef _SRC_PY_CPP_PYMICEMODULE_HH_
#define _SRC_PY_CPP_PYMICEMODULE_HH_

// These ifdefs are required to avoid cpp compiler warning
#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif

#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif

#include <Python.h>

#include <string>

#ifdef MAUS_PYMICEMODULE_CC

class MiceModule;

namespace MAUS {

namespace PyMiceModule {

/** PyMiceModule is the python implementation of the C++ MiceModule
 *
 *  Note about memory management: In order to keep the memory usage self
 *  consistent all functions that access (get) the MiceModule tree deep copy the
 *  associated MiceModule and Python owns the memory. All functions that update
 *  (set) the MiceModule tree deep copy the associated MiceModule and MAUS owns
 *  the memory.
 */
typedef struct {
    PyObject_HEAD;
    MiceModule* mod;
} PyMiceModule;

/** @namespace C_API defines functions that can be accessed by other C libraries
 *
 *  To access these functions, first call int import_PyMiceModule(). This will
 *  also set up and put C_API objects into the MAUS::PyMiceModule namespace
 */
namespace C_API {

/** Allocate a new PyMiceModule
 *
 *  \returns PyMiceModule* cast as a PyObject* with mod pointer set to NULL
 */
static PyObject *create_empty_module();

/** Return the C++ MiceModule associated with a PyMiceModule
 *
 *  \param py_mod PyMiceModule* cast as a PyObject*. Python representation
 *         of the mice module
 *
 *  PyMiceModule still owns the memory allocated to MiceModule
 */
static MiceModule* get_mice_module(PyObject* py_mod);

/** Set the C++ mice module associated with a PyMiceModule
 *
 *  \param py_mod PyMiceModule* cast as a PyObject*. Python representation
 *               of the mice module
 *  \param mod  C++ representation of the mice module. PyMiceModule
 *             takes ownership of the memory allocated to mod. Always sets
 *             mod->mother() as NULL to avoid memory problems.
 *  Returns 1 on success, 0 on failure.
 */
static int set_mice_module(PyObject* py_mod, MiceModule* mod);
}

/** get name of this PyMiceModule
 *   - self a PyObject of type PyMiceModule
 *   - args not used
 *   - kwds not used
 *  Returns a python string with the name of this MiceModule 
 */
static PyObject *get_children(PyObject* self, PyObject *args, PyObject *kwds);


/** get child modules of this PyMiceModule
 *   - self a PyObject of type PyMiceModule
 *   - args not used
 *   - kwds not used
 *  Returns a python list of PyObjects each of which is a PyMiceModule 
 */
static PyObject *get_children(PyObject* self, PyObject *args, PyObject *kwds);

/** get child modules of this PyMiceModule
 *   - self a PyObject of type PyMiceModule
 *   - args not used
 *   - kwds takes one argument, children, which should be a python list whose
 *     members are all of type PyMiceModule
 *  Returns a python list of PyObjects each of which is a PyMiceModule 
 */
static PyObject *set_children(PyObject* self, PyObject *args, PyObject *kwds);

/** get the value of a property of the PyMiceModule
 *    - self a PyObject of type PyMiceModule
 *    - args not used
 *    - kwds takes two arguments, name (PyString name of the property) and type
 *      (PyString type of the property)
 *  Returns PyNone
 */
static PyObject *get_property(PyObject* self, PyObject *args, PyObject *kwds);

/** set the value of a property of the PyMiceModule
 *    - self a PyObject of type PyMiceModule
 *    - kwds takes three arguments, name (PyString name of the property),
 *      type (PyString type of the property), value (PyString value to set the
 *      property to)
 *  Returns the property value
 */
static PyObject *set_property(PyObject* self, PyObject *args, PyObject *kwds);

/** Get the position of the PyMiceModule in the global (top level) coordinates
 *    - self a PyObject of type PyMiceModule
 *    - args not used
 *    - kwds not used
 *  Returns a dict like {"x":x, "y":y, "z":z} corresponding to the module
 *  position
 */
static PyObject *get_global_position(PyObject* self, PyObject *args, PyObject *kwds);

/** Get the rotation of the PyMiceModule in the global (top level) coordinates
 *    - self a PyObject of type PyMiceModule
 *    - args not used
 *    - kwds not used
 *  Returns a dict like {"x":x, "y":y, "z":z} corresponding to the module
 *  rotation; see MiceModule docs for definition
 */
static PyObject *get_global_rotation(PyObject* self, PyObject *args, PyObject *kwds);

/** _alloc allocates memory for PyMiceModule
 *
 *  @param type - pointer to a PyMiceModuleType object, as defined in
 *         PyMiceModule.cc
 *
 *  returns a PyMiceModule* (cast as a PyObject*); caller owns this memory
 */
static PyObject *_alloc(PyTypeObject *type, Py_ssize_t nitems);

/** _init initialises an allocated PyMiceModule object
 *
 *  @param self an initialised PyMiceModule* cast as a PyObject*; caller
 *         owns this memory
 *  @param args not used
 *  @param kwds not used
 *
 *  @returns 0 on success; -1 on failure
 */
static int _init(PyObject* self, PyObject *args, PyObject *kwds);

/** deallocate memory
 *
 *  @params self an initialised PyMiceModule*; memory will be freed by
 *          this function
 */
static void _free(PyMiceModule * self);

/** Return a Python string representation of the PyMiceModule */
static PyObject* _str(PyObject * self);

/** Initialise mice_module module
 *
 *  This is called by import mice_module; it initialises the MiceModule type 
 *  allowing user to construct and call methods on MiceModule objects
 */
PyMODINIT_FUNC initmice_module(void);

/** Return a PyObject boolean with given name from module mod */
static PyObject* get_bool(MiceModule* mod, std::string name);

/** Return a PyObject string with given name from module mod */
static PyObject* get_string(MiceModule* mod, std::string name);

/** Return a PyObject double with given name from module mod */
static PyObject* get_double(MiceModule* mod, std::string name);

/** Return a PyObject int with given name from module mod */
static PyObject* get_int(MiceModule* mod, std::string name);

/** Return a PyObject Hep3Vector with given name from module mod
 *
 *  Returns like {"x":v.x(), "y":v.y(), "z":v.z()}
 */
static PyObject* get_hep3vector(MiceModule* mod, std::string name);

/** Set a MiceModule Hep3Vector with a dict like {"x":1., "y":2., "z":3.} */
static PyObject* set_property_hep3vector
                        (MiceModule* mod, std::string name, PyObject* py_value);

/** Helper function to extract data from Hep3Vector dict */
bool set_property_hep3vector_one
                              (PyObject* py_dict, std::string dim, double* value);


}  // namespace PyMiceModule
}  // namespace MAUS
#else // MAUS_PYMICEMODULE_CC

namespace MAUS {
namespace PyMiceModule {

/** import the PyMiceModule C_API
 *
 *  Makes the functions in C_API available in the MAUS::PyMiceModule
 *  namespace, for other python/C code
 *
 *  @returns 0 if the import fails; return 1 if it is a success
 */
int import_PyMiceModule();

PyObject* (*create_empty_module)() = NULL;
int (*set_mice_module)(PyObject* py_mod, MiceModule* mod) = NULL;
MiceModule* (*get_mice_module)(PyObject* py_mod) = NULL;
}  // namespace PyMiceModule
}  // namespace MAUS

int MAUS::PyMiceModule::import_PyMiceModule() {
  PyObject* mod_module = PyImport_ImportModule("maus_cpp.mice_module");
  if (mod_module == NULL) {
      return 0;
  } else {
    PyObject *mod_dict  = PyModule_GetDict(mod_module);

    PyObject* cem_c_api = PyDict_GetItemString(mod_dict,
                                                 "C_API_CREATE_EMPTY_MODULE");
    void* cem_void = reinterpret_cast<void*>(PyCObject_AsVoidPtr(cem_c_api));
    PyMiceModule::create_empty_module =
                                    reinterpret_cast<PyObject* (*)()>(cem_void);

    PyObject* gmm_c_api = PyDict_GetItemString(mod_dict,
                                               "C_API_GET_MICE_MODULE");
    void* gmm_void = reinterpret_cast<void*>(PyCObject_AsVoidPtr(gmm_c_api));
    PyMiceModule::get_mice_module =
                   reinterpret_cast<MiceModule* (*)(PyObject*)>(gmm_void);

    PyObject* smm_c_api = PyDict_GetItemString(mod_dict,
                                               "C_API_SET_MICE_MODULE");
    void* smm_void = reinterpret_cast<void*>(PyCObject_AsVoidPtr(smm_c_api));
    PyMiceModule::set_mice_module =
             reinterpret_cast<int (*)(PyObject*, MiceModule*)>(smm_void);
    if ((create_empty_module == NULL) ||
        (set_mice_module == NULL) ||
        (get_mice_module == NULL))
        return 0;
  }
  return 1;
}


#endif // MAUS_PYMICEMODULE_CC
#endif // _SRC_PY_CPP_PYMICEMODULE_HH_
