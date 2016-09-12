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

#ifndef _SRC_PY_CPP_PYCOVARIANCEMATRIX_HH_
#define _SRC_PY_CPP_PYCOVARIANCEMATRIX_HH_

// These ifdefs are required to avoid cpp compiler warning
#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif

#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif

#include <Python.h>

#ifdef MAUS_PYCOVARIANCEMATRIX_CC

namespace MAUS {

class CovarianceMatrix; // note this is just in MAUS namespace

namespace PyCovarianceMatrix {

/** PyCovarianceMatrix is the python implementation of the C++ CovarianceMatrix
 */
typedef struct {
    PyObject_HEAD;
    CovarianceMatrix* cov_mat;
} PyCovarianceMatrix;

/** @namespace C_API defines functions that can be accessed by other C libraries
 *
 *  To access these functions call int import_PyCovarianceMatrix() otherwise you
 *  will get a segmentation fault. The import will place the C_API functions in
 *  the MAUS::PyCovarianceMatrix namespace.
 */
namespace C_API {

/** Allocate a new PyCovarianceMatrix
 *
 *  \returns PyCovarianceMatrix* cast as a PyObject* with cm pointer set to
 *  NULL. Caller owns the memory allocated to PyCovarianceMatrix*
 */
static PyObject *create_empty_matrix();

/** Return the C++ covariance matrix associated with a PyCovarianceMatrix
 *
 *  \param py_cm PyCovarianceMatrix* cast as a PyObject*. Python representation
 *         of the covariance matrix
 *
 *  \returns NULL on failure and raises a TypeError. On success returns the
 *  CovarianceMatrix. PyCovarianceMatrix still owns the memory allocated to
 *  CovarianceMatrix.
 */
static CovarianceMatrix* get_covariance_matrix(PyObject* py_cm);

/** Set the C++ covariance matrix associated with a PyCovarianceMatrix
 *
 *  \param py_cm PyCovarianceMatrix* cast as a PyObject*. Python representation
 *               of the covariance matrix
 *  \param cm  C++ representation of the covariance matrix. PyCovarianceMatrix
 *             takes ownership of the memory allocated to cm
 *
 *  \returns 1 on success, 0 on failure and raises a TypeError
 */
static int set_covariance_matrix(PyObject* py_cm, CovarianceMatrix* cm);
}

/** _alloc allocates memory for PyCovarianceMatrix
 *
 *  @param type - pointer to a PyCovarianceMatrixType object, as defined in
 *         PyCovarianceMatrix.cc
 *
 *  returns a PyCovarianceMatrix* (cast as a PyObject*); caller owns this memory
 */
static PyObject *_alloc(PyTypeObject *type, Py_ssize_t nitems);

/** _init initialises an allocated PyCovarianceMatrix object
 *
 *  @param self an initialised PyCovarianceMatrix* cast as a PyObject*; caller
 *         owns this memory
 *  @param args not used
 *  @param kwds not used
 *
 *  @returns 0 on success; -1 on failure
 */
static int _init(PyObject* self, PyObject *args, PyObject *kwds);

/** deallocate memory
 *
 *  @params self an initialised PyCovarianceMatrix*; memory will be freed by
 *          this function
 */
static void _free(PyCovarianceMatrix * self);

/** Return a Python string representation of the PyCovarianceMatrix */
static PyObject* _str(PyObject * self);

/** Initialise covariance_matrix module
 *
 *  This is called by import covariance_matrix; it initialises the
 *  CovarianceMatrix type allowing user to construct and call methods on
 *  CovarianceMatrix objects
 */
PyMODINIT_FUNC initcovariance_matrix(void);

/** Get the value of an element 
 *  
 *  \param self - not used
 *  \param args - not used
 *  \param kwds - keyword arguments; row, column for matrix row, column
 *
 */
static PyObject* get_element(PyObject* self, PyObject *args, PyObject *kwds);

/** Create a PyCovarianceMatrix from penn parameters
 *  
 *  \param self - not used
 *  \param args - value arguments
 *  \param kwds - keyword arguments
 *
 *  \returns PyCovarianceMatrix cast to a PyObject - caller owns this memory
 */
static PyObject* create_from_penn_parameters
                               (PyObject *self, PyObject *args, PyObject *kwds);

/** Create a PyCovarianceMatrix from twiss parameters
 *  
 *  \param self - not used
 *  \param args - value arguments
 *  \param kwds - keyword arguments
 *
 *  \returns PyCovarianceMatrix cast to a PyObject - caller owns this memory
 */
static PyObject* create_from_twiss_parameters
                               (PyObject *self, PyObject *args, PyObject *kwds);

/** Create a PyCovarianceMatrix from a numpy matrix
 *  
 *  \param self - not used
 *  \param args - value arguments
 *  \param kwds - keyword arguments; takes one keywd, matrix which is a 6x6
 *                numpy matrix
 *
 *  \returns PyCovarianceMatrix cast to a PyObject - caller owns this memory
 */
static PyObject* create_from_matrix
                               (PyObject* self, PyObject *args, PyObject *kwds);

/** Create a PyCovarianceMatrix from a numpy matrix
 *
 *  \param numpy_array borrowed reference to a numpy array. Should be 6x6 
 *         matrix, assumed to be symmetric. Borrowed reference means caller
 *         still owns the array.
 *
 *  \returns new CovarianceMatrix; caller owns the memory
 *
 *  \throws MAUS::Exceptions::Exception if array has wrong shape or is not a numpy_array
 */
static CovarianceMatrix* create_from_numpy_matrix(PyObject *numpy_array);
}
}

#else

/** MAUS::PyOpticsModel::PyCovarianceMatrix C API objects
 *
 *  Because of the way python does share libraries, we have to explicitly import
 *  C functions via the Python API, which is done at import time. This mimics 
 *  the functions in MAUS::PyCovarianceMatrix. Full documentation is found
 *  there.
 */
namespace MAUS {
namespace PyCovarianceMatrix {

/** import the PyCovarianceMatrix C_API
 *
 *  Makes the functions in C_API available in the MAUS::PyCovarianceMatrix
 *  namespace, for other python/C code
 *
 *  @returns 0 if the import fails; return 1 if it is a success
 */
int import_PyCovarianceMatrix();


PyObject* (*create_empty_matrix)() = NULL;
int (*set_covariance_matrix)(PyObject* py_cm, CovarianceMatrix* cm) = NULL;
CovarianceMatrix* (*get_covariance_matrix)(PyObject* py_cm) = NULL;
}
}

int MAUS::PyCovarianceMatrix::import_PyCovarianceMatrix() {
  PyObject* cm_module = PyImport_ImportModule("maus_cpp.covariance_matrix");
  if (cm_module == NULL) {
      return 0;
  } else {
    PyObject *cm_dict  = PyModule_GetDict(cm_module);

    PyObject* cem_c_api = PyDict_GetItemString(cm_dict,
                                               "C_API_CREATE_EMPTY_MATRIX");
    void* cem_void = reinterpret_cast<void*>(PyCObject_AsVoidPtr(cem_c_api));
    PyCovarianceMatrix::create_empty_matrix =
                                    reinterpret_cast<PyObject* (*)()>(cem_void);

    PyObject* gcm_c_api = PyDict_GetItemString(cm_dict,
                                               "C_API_GET_COVARIANCE_MATRIX");
    void* gcm_void = reinterpret_cast<void*>(PyCObject_AsVoidPtr(gcm_c_api));
    PyCovarianceMatrix::get_covariance_matrix =
                   reinterpret_cast<CovarianceMatrix* (*)(PyObject*)>(gcm_void);

    PyObject* scm_c_api = PyDict_GetItemString(cm_dict,
                                               "C_API_SET_COVARIANCE_MATRIX");
    void* scm_void = reinterpret_cast<void*>(PyCObject_AsVoidPtr(scm_c_api));
    PyCovarianceMatrix::set_covariance_matrix =
             reinterpret_cast<int (*)(PyObject*, CovarianceMatrix*)>(scm_void);
    if ((create_empty_matrix == NULL) ||
        (set_covariance_matrix == NULL) ||
        (get_covariance_matrix == NULL))
        return 0;
  }
  return 1;
}

#endif  // MAUS_PYCOVARIANCEMATRIX_CC
#endif  // _SRC_PY_CPP_PYCOVARIANCEMATRIX_HH_

