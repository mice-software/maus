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
 *
 */

#ifndef _SRC_PY_CPP_PYPHASESPACEVECTOR_HH_
#define _SRC_PY_CPP_PYPHASESPACEVECTOR_HH_

// These ifdefs are required to avoid cpp compiler warning
#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif

#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif

#include <Python.h>

#ifdef MAUS_PYPHASESPACEVECTOR_CC

namespace MAUS {

class PhaseSpaceVector; // note this is just in MAUS namespace

/** Methods for the PyPhaseSpaceVector object
 */
namespace PyPhaseSpaceVector {

/** PyPhaseSpaceVector is the python implementation of the C++ PhaseSpaceVector
 */
typedef struct {
    PyObject_HEAD;
    PhaseSpaceVector* psv;
} PyPhaseSpaceVector;

/** @namespace C_API defines functions that can be accessed by other C libraries
 *
 *  To access these functions, you must call import_PyPhaseSpaceVector()
 *  (otherwise you will get segmentation fault)
 */
namespace C_API {

/** Create a new empty PyPhaseSpaceVector (psv is NULL)
 *
 *  Caller owns the returned memory
 */

static PyObject* create_empty_vector();

/** Return the C++ PhaseSpaceVector associated with a PyPhaseSpaceVector
 *
 *  \param py_psv PyPhaseSpaceVector* cast as a PyObject*. Python representation
 *         of the phase space vector
 *
 *  PyPhaseSpaceVector still owns the memory allocated to PhaseSpaceVector
 */
static PhaseSpaceVector* get_phase_space_vector(PyObject* py_psv);

/** Set the C++ phase space vector associated with a PyPhaseSpaceVector
 *
 *  \param py_psv PyPhaseSpaceVector* cast as a PyObject*. Python representation
 *               of the phase space vector
 *  \param psv  C++ representation of the phase space vector. PyPhaseSpaceVector
 *             takes ownership of the memory allocated to psv
 */
static void set_phase_space_vector(PyObject* py_psv, PhaseSpaceVector* psv);
}

// ~~~~~~~~~~~~~~~~~~~~ PyPhaseSpaceVector private methods ~~~~~~~~~~~~~~~~~~~~~
// most are available as native python functions

/** _alloc allocates memory for PyPhaseSpaceVector
 *
 *  @param type - pointer to a PyPhaseSpaceVectorType object, as defined in
 *         PyPhaseSpaceVectorType.cc
 *
 *  returns a PyPhaseSpaceVector* (cast as a PyObject*); caller owns this memory
 */
static PyObject *_alloc(PyTypeObject *type, Py_ssize_t nitems);

/** _init initialises an allocated PyPhaseSpaceVector object
 *
 *  @param self an initialised PyPhaseSpaceVector* cast as a PyObject*; caller
 *         owns this memory
 *  @param args not used
 *  @param kwds accepts t, energy, x, px, y, py keywords; see python docstring
 *
 *  @returns 0 on success; -1 on failure
 */
static int _init(PyObject* self, PyObject *args, PyObject *kwds);

/** deallocate memory
 *
 *  @params self an initialised PyPhaseSpaceVector*; memory will be freed by
 *          this function
 */
static void _free(PyPhaseSpaceVector * self);

/** Initialise phase_space_vector module
 *
 *  This is called by import phase_space_vector; it initialises the
 *  PhaseSpaceVector type allowing user to construct and call methods on
 *  PhaseSpaceVector objects
 */
PyMODINIT_FUNC initphase_space_vector(void);

/** get time t [ns]
 *
 *  @params self a PyPhaseSpaceVector
 *  @params args not used
 *  @params kwds not used
 *
 *  @returns a PyObject float containing time t
 */
static PyObject* get_t(PyObject* self, PyObject *args, PyObject *kwds);

/** get energy E [MeV]
 *
 *  @params self a PyPhaseSpaceVector
 *  @params args not used
 *  @params kwds not used
 *
 *  @returns a PyObject float containing energy E
 */
static PyObject* get_energy(PyObject* self, PyObject *args, PyObject *kwds);

/** get x (horizontal) position [mm]
 *
 *  @params self a PyPhaseSpaceVector
 *  @params args not used
 *  @params kwds not used
 *
 *  @returns a PyObject float containing position x
 */
static PyObject* get_x(PyObject* self, PyObject *args, PyObject *kwds);

/** get px (horizontal component of momentum) [MeV/c]
 *
 *  @params self a PyPhaseSpaceVector
 *  @params args not used
 *  @params kwds not used
 *
 *  @returns a PyObject float containing momentum px
 */
static PyObject* get_px(PyObject* self, PyObject *args, PyObject *kwds);

/** get y (vertical position) [mm]
 *
 *  @params self a PyPhaseSpaceVector
 *  @params args not used
 *  @params kwds not used
 *
 *  @returns a PyObject float containing position y
 */
static PyObject* get_y(PyObject* self, PyObject *args, PyObject *kwds);

/** get py (vertical component of momentum) [MeV/c]
 *
 *  @params self a PyPhaseSpaceVector
 *  @params args not used
 *  @params kwds not used
 *
 *  @returns a PyObject float containing momentum py
 */
static PyObject* get_py(PyObject* self, PyObject *args, PyObject *kwds);

/** set t (time) [ns]
 *
 *  @params self a PyPhaseSpaceVector
 *  @params args not used
 *  @params kwds accepts keyword value (float)
 *
 *  @returns None
 */
static PyObject* set_t(PyObject* self, PyObject *args, PyObject *kwds);

/** set E (energy) [MeV/c]
 *
 *  @params self a PyPhaseSpaceVector
 *  @params args not used
 *  @params kwds accepts keyword value (float)
 *
 *  @returns None
 */
static PyObject* set_energy(PyObject* self, PyObject *args, PyObject *kwds);

/** set x (horizontal position) [mm]
 *
 *  @params self a PyPhaseSpaceVector
 *  @params args not used
 *  @params kwds accepts keyword value (float)
 *
 *  @returns None
 */
static PyObject* set_x(PyObject* self, PyObject *args, PyObject *kwds);

/** set px (horizontal component of momentum) [MeV/c]
 *
 *  @params self a PyPhaseSpaceVector
 *  @params args not used
 *  @params kwds accepts keyword value (float)
 *
 *  @returns None
 */
static PyObject* set_px(PyObject* self, PyObject *args, PyObject *kwds);

/** set y (vertical position) [mm]
 *
 *  @params self a PyPhaseSpaceVector
 *  @params args not used
 *  @params kwds accepts keyword value (float)
 *
 *  @returns None
 */
static PyObject* set_y(PyObject* self, PyObject *args, PyObject *kwds);

/** set py (vertical component of momentum) [MeV/c]
 *
 *  @params self a PyPhaseSpaceVector
 *  @params args not used
 *  @params kwds accepts keyword value (float)
 *
 *  @returns None
 */
static PyObject* set_py(PyObject* self, PyObject *args, PyObject *kwds);

/** Template for get functions */
static PyObject* get(PyObject* self,
                     double (PhaseSpaceVector::*get_function)() const);

/** Template for set functions */
static PyObject* set(PyObject* self,
                     PyObject *args,
                     PyObject *kwds,
                     void (PhaseSpaceVector::*set_function)(double value));

/** Return a python string describing the phase space vector
 *
 *  Returns string formatted like a python list with contents:
 *    time [ns], energy [MeV], x [mm], px [MeV/c], y [mm], py [MeV/c]
 */
static PyObject* _str(PyObject * self);
}
}

#else // ifdef MAUS_PYPHASESPACEVECTOR_CC

/** MAUS::PyOpticsModel::PyPhaseSpaceVector C API objects
 *
 *  Because of the way python does share libraries, we have to explicitly import
 *  C functions via the Python API, which is done at import time. This mimics 
 *  the functions in MAUS::PyPhaseSpaceVector. Full documentation is found
 *  there.
 *
 *  Hacky implementation of public keyword in C++
 */
namespace MAUS {
namespace PyPhaseSpaceVector {
/** import the PyPhaseSpaceVector C_API
 *
 *  Make the functions in C_API namespace available to other C modules.
 *  Functions will be in namespace MAUS::PyPhaseSpaceVector.
 *
 *  @returns 0 if the import fails; return 1 if it is a success
 */
int import_PyPhaseSpaceVector();

void (*set_phase_space_vector)(PyObject* py_psv, PhaseSpaceVector* psv) = NULL;
PhaseSpaceVector* (*get_phase_space_vector)(PyObject* py_psv) = NULL;
PyObject* (*create_empty_vector)() = NULL;
}
}

int MAUS::PyPhaseSpaceVector::import_PyPhaseSpaceVector() {
  PyObject* psv_module = PyImport_ImportModule("maus_cpp.phase_space_vector");
  if(psv_module == NULL) {
      return 0;
  } else {
    PyObject *psv_dict  = PyModule_GetDict(psv_module);

    PyObject* gpsv_c_api = PyDict_GetItemString(psv_dict,
                                               "C_API_GET_PHASE_SPACE_VECTOR");
    void* gpsv_void = (void*)PyCObject_AsVoidPtr(gpsv_c_api);
    PyPhaseSpaceVector::get_phase_space_vector =
                  reinterpret_cast<PhaseSpaceVector* (*)(PyObject*)>(gpsv_void);

    PyObject* spsv_c_api = PyDict_GetItemString(psv_dict,
                                               "C_API_SET_PHASE_SPACE_VECTOR");
    void* spsv_void = (void*)PyCObject_AsVoidPtr(spsv_c_api);
    PyPhaseSpaceVector::set_phase_space_vector =
            reinterpret_cast<void (*)(PyObject*, PhaseSpaceVector*)>(spsv_void);

    PyObject* cev_c_api = PyDict_GetItemString(psv_dict,
                                               "C_API_CREATE_EMPTY_VECTOR");
    void* cev_void = (void*)PyCObject_AsVoidPtr(cev_c_api);
    PyPhaseSpaceVector::create_empty_vector =
            reinterpret_cast<PyObject* (*)()>(cev_void);

    if ((create_empty_vector == NULL) ||
        (set_phase_space_vector == NULL) ||
        (get_phase_space_vector == NULL))
        return 0;
  }
  return 1;
}
#endif  // #ifdef MAUS_PYPHASESPACEVECTOR_CC
#endif  // _SRC_PY_CPP_PYPHASESPACEVECTOR_HH_

