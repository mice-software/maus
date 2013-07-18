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

namespace MAUS {

class CovarianceMatrix; // note this is just in MAUS namespace

namespace PyCovarianceMatrix {

/** PyCovarianceMatrix is the python implementation of the C++ CovarianceMatrix
 */
typedef struct {
    PyObject_HEAD;
    CovarianceMatrix* cov_mat;
} PyCovarianceMatrix;

/** _alloc allocates memory for PyCovarianceMatrix
 *
 *  @param type - pointer to a PyCovarianceMatrixType object, as defined in
 *         PyCovarianceMatrix.cc
 *
 *  returns a PyCovarianceMatrix* (cast as a PyObject*); caller owns this memory
 */
PyObject *_alloc(PyTypeObject *type, Py_ssize_t nitems);

/** _init initialises an allocated PyCovarianceMatrix object
 *
 *  @param self an initialised PyCovarianceMatrix* cast as a PyObject*; caller
 *         owns this memory
 *  @param args should be PyTuple with one element, a 6*6 numpy matrix
 *  @param kwds not used
 *
 *  @returns 0 on success; -1 on failure
 */
int _init(PyObject* self, PyObject *args, PyObject *kwds);

/** deallocate memory
 *
 *  @params self an initialised PyCovarianceMatrix*; memory will be freed by
 *          this function
 */
void _dealloc(PyCovarianceMatrix * self);

/** synonym for dealloc */
void _free(PyCovarianceMatrix * self);

/** Initialise covariance_matrix module
 *
 *  This is called by import covariance_matrix; it initialises the
 *  CovarianceMatrix type allowing user to construct and call methods on
 *  CovarianceMatrix objects
 */
PyMODINIT_FUNC initcovariance_matrix(void);

/** Return the C++ covariance matrix associated with a PyCovarianceMatrix
 *
 *  \param py_cm Python representation of the covariance matrix
 *
 *  PyCovarianceMatrix still owns the memory allocated to CovarianceMatrix
 */
CovarianceMatrix* get_covariance_matrix(PyCovarianceMatrix* py_cm);

/** Set the C++ covariance matrix associated with a PyCovarianceMatrix
 *
 *  \param py_cm Python representation of the covariance matrix
 *  \param cm  C++ representation of the covariance matrix
 *
 *  PyCovarianceMatrix takes ownership of the memory allocated to cm
 */
void set_covariance_matrix(PyCovarianceMatrix* py_cm, CovarianceMatrix* cm);

/** Create a PyCovarianceMatrix from penn parameters
 */
PyObject* create_from_penn_parameters
                               (PyObject *self, PyObject *args, PyObject *kwds);

/** Create a PyCovarianceMatrix from twiss parameters
 */
PyObject* create_from_twiss_parameters
                               (PyObject *self, PyObject *args, PyObject *kwds);

/** Create a PyCovarianceMatrix from a numpy matrix
 *
 *  \param numpy_array borrowed reference to a numpy array. Should be 6x6 
 *         matrix, assumed to be symmetric. Borrowed reference means caller
 *         still owns the array.
 *
 *  \returns new CovarianceMatrix; caller owns the memory
 *
 *  \throws MAUS::Exception if array has wrong shape or is not a numpy_array
 */
CovarianceMatrix* create_from_numpy_matrix(PyObject *numpy_array);
}
}

#endif
