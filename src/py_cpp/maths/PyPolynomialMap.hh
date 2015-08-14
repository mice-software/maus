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

#ifndef _SRC_PY_CPP_MATHS_PYPOLYNOMIALMAP_HH_
#define _SRC_PY_CPP_MATHS_PYPOLYNOMIALMAP_HH_

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
class PolynomialMap;

namespace PyPolynomialMap {

/** PyPolynomialMap is the python implementation of the C++ PolynomialMap class

 Provides a multivariate polynomial object
 */
typedef struct {
    PyObject_HEAD;
    PolynomialMap* map;
} PyPolynomialMap;

/** _alloc allocates memory for PyPolynomialMap
 *
 *  @param type - pointer to aPyPolynomialMapType object, as defined in
 *         PyPolynomialMap.cc
 *
 *  returns a PyPolynomialMap* (cast as a PyObject*); caller owns this memory
 */
static PyObject *_alloc(PyTypeObject *type, Py_ssize_t nitems);

/** _init initialises an allocated PyPolynomialMap object
 *
 *  @param self an initialised PyPolynomialMap* cast as a PyObject*; caller owns
 *         this memory
 *  @param args not used
 *  @param kwds not used
 *
 *  @returns 0 on success; -1 on failure
 */
static int _init(PyObject* self, PyObject *args, PyObject *kwds);

/** deallocate memory
 *
 *  @params self an initialised PyPolynomialMap*; memory will be freed by this 
 *          function
 */
static void _dealloc(PyPolynomialMap * self);

/** synonym for dealloc */
static void _free(PyPolynomialMap * self);

/** Initialise polynomial_map module
 *
 *  This is called by import polynomial_map; it initialises the PolynomialMap type
 *  allowing user to construct and call methods on PolynomialMap objects
 */
PyMODINIT_FUNC initpolynomial_map(void);

/** Get the PolynomialMap coefficients
 *
 *  \param self a PyPolynomialMap
 *  \param args arguments to the function (not used)
 *  \param kwds keyword arguments to the function - see docstring
 *
 *  \return a list of lists of floats; each corresponding to a value in the
 *          polynomial map
 */
static PyObject* get_coefficients_as_matrix(PyObject *self, PyObject *args,
                                                                PyObject *kwds);

/** Calculate a polynomial vector
 *
 *  \param self a PyPolynomialMap
 *  \param args arguments to the function (not used)
 *  \param kwds keyword arguments to the function - see docstring
 *
 *  \return a PyList of floats with ValueDimension
 */
static PyObject* evaluate(PyObject *self, PyObject *args,
                                                                PyObject *kwds);

/** Get the point dimension
 *
 *  \param self a PyPolynomialMap
 *  \param args arguments to the function (not used)
 *  \param kwds keyword arguments to the function - see docstring
 *
 *  \return a PyLong
 */
static PyObject* point_dimension(PyObject *self, PyObject *args,
                                                                PyObject *kwds);

/** Get the value dimension
 *
 *  \param self a PyPolynomialMap
 *  \param args arguments to the function (not used)
 *  \param kwds keyword arguments to the function - see docstring
 *
 *  \return a PyLong
 */
static PyObject* value_dimension(PyObject *self, PyObject *args,
                                                                PyObject *kwds);

/** Get the string representation of the PolynomialMap
 *
 *  \param self a PyPolynomialMap
 *  \param args arguments to the function (not used)
 *  \param kwds keyword arguments to the function - see docstring
 *
 *  \return a PyString
 */
static PyObject* str(PyObject *self, PyObject *args, PyObject *kwds);

}  // namespace PyPolynomialMap
}  // namespace MAUS

#endif  // _SRC_PY_CPP_MATHS_PYPOLYNOMIALMAP_HH_
