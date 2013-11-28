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

#ifndef _SRC_PY_CPP_PYOPTICSMODEL_HH_
#define _SRC_PY_CPP_PYOPTICSMODEL_HH_

// These ifdefs are required to avoid cpp compiler warning
#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif

#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif

#include <Python.h>

namespace MAUS {

// note following are in MAUS namespace
class OpticsModel;
class CovarianceMatrix;

namespace PyOpticsModel {

/** PyOpticsModel is the python implementation of the C++ OpticsModel class

 Provides bindings for transporting particles and beam ellipses
 */
typedef struct {
    PyObject_HEAD;
    OpticsModel* model;
} PyOpticsModel;

/** _alloc allocates memory for PyOpticsModel
 *
 *  @param type - pointer to a PyOpticsModelType object, as defined in
 *         PyOpticsModel.cc
 *
 *  returns a PyOpticsModel* (cast as a PyObject*); caller owns this memory
 */
static PyObject *_alloc(PyTypeObject *type, Py_ssize_t nitems);

/** _init initialises an allocated PyOpticsModel object
 *
 *  @param self an initialised PyOpticsModel* cast as a PyObject*; caller owns
 *         this memory
 *  @param args not used
 *  @param kwds not used
 *
 *  @returns 0 on success; -1 on failure
 */
static int _init(PyObject* self, PyObject *args, PyObject *kwds);

/** deallocate memory
 *
 *  @params self an initialised PyOpticsModel*; memory will be freed by this 
 *          function
 */
static void _dealloc(PyOpticsModel * self);

/** synonym for dealloc */
static void _free(PyOpticsModel * self);

/** Initialise optics_model module
 *
 *  This is called by import optics_model; it initialises the OpticsModel type
 *  allowing user to construct and call methods on OpticsModel objects
 */
PyMODINIT_FUNC initoptics_model(void);

/** Return the C++ optics model associated with a PyOpticsModel 
 *
 *  PyOpticsModel still owns the memory allocated to OpticsModel
 */
static OpticsModel* get_optics_model(PyOpticsModel* py_model);

/** Transport a CovarianceMatrix from a to b
 *
 *  \param self a PyOpticsModel
 *  \param args arguments to the function (not used)
 *  \param kwds keyword arguments to the function - see docstring
 *
 *  \return a new transported object
 *
 *  Note that we do not overload functions as in the C++ library as python
 *  support for overloading is poor.
 */
static PyObject* transport_covariance_matrix(PyObject *self, PyObject *args,
                                                                PyObject *kwds);

/** Transport a PhaseSpaceVector from a to b
 *
 *  \param self a PyOpticsModel
 *  \param args arguments to the function (not used)
 *  \param kwds keyword arguments to the function - see docstring
 *
 *  \return a new transported object
 *
 *  Note that we do not overload functions as in the C++ library as python
 *  support for overloading is poor.
 */
static PyObject* transport_phase_space_vector(PyObject *self, PyObject *args,
                                                                PyObject *kwds);
}  // namespace PyOpticsModel
}  // namespace MAUS

#endif  // _SRC_PY_CPP_PYFIELDS_HH_
