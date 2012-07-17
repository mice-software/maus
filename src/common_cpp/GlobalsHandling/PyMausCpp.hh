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

#ifndef _SRC_COMMON_CPP_GLOBALSHANDLING_PYMAUSCPP_HH_
#define _SRC_COMMON_CPP_GLOBALSHANDLING_PYMAUSCPP_HH_

// These ifdefs are required to avoid cpp compiler warning
#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif

#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif

#include "Python.h"

namespace MAUS {

/*  @brief Defines some interfaces available in Python from C
 *
 *  The interface between Python and C in MAUS is usually done using text files
 *  (e.g. json). In a few places we need to make some function calls available
 *  to python. Those function calls are defined here.
 */

// Nb: Following tested in tests/py_unit/test_error_handler.py

/* @brief Initialise the libMausCpp module
 *
 * Called by "import libMausCpp" - here we just define the list of functions
 * available in libMausCpp.
 */
PyMODINIT_FUNC initlibMausCpp(void);


// Nb: Following tested in tests/py_unit/test_error_handler.py and
//                         tests/cpp_unit/CppErrorHandlerTest.cc

/* @brief Call back to make Python ErrorHandler by the CppErrorHandler
 *
 * Tell C++ which python function to call in the event of an error. Calls the
 * SetHandleExceptionFunction. Aim is to use the same python code to manage
 * errors arising from both C and Python; argument is a function with callsign
 * like\n
 *
 *    def HandleExceptionStrings(doc, caller, error_message)
 *
 * See also: src/common_py/ErrorHandler, src/common_cpp/CppErrorHandler
 */
static PyObject* CppErrorHandler_SetHandleExceptionFunction
                                              (PyObject *dummy, PyObject *args);

/* @brief Initialise MAUS
 *
 * Initialise MAUS globals. Takes one argument which should be a string set of
 * datacards (as read by Configuration module). Throws a Runtime error if MAUS
 * globals are already set. Returns Py_None.
 */
static PyObject* GlobalsManager_Initialise(PyObject *dummy, PyObject *args);

/* @brief Destruct MAUS
 *
 * Destruct MAUS globals. Ignores all arguments. Throws a RuntimeError if MAUS
 * globals were never set. Returns Py_None.
 */
PyObject* GlobalsManager_Destruct(PyObject *dummy, PyObject *args);

/* @brief GetFieldValue at a point in space, time
 *
 * Get the field value at a point in x,y,z,time. args should be a tuple of 
 * (x,y,z,t). Returns a tuple of (bx,by,bz,ex,ey,ez).
 */
PyObject* PyField_GetFieldValue(PyObject *dummy, PyObject *args);

/* @brief Check if MAUS has been initialised
 *
 * Check for initalisation of MAUS globals. Ignores all arguments. Returns
 * PyObject integer 1 if library is initialised, 0 if it is not
 */
PyObject* GlobalsManager_HasInstance(PyObject *dummy, PyObject *args);

}  // namespace MAUS

#endif  // _SRC_COMMON_CPP_GLOBALSHANDLING_PYMAUSCPP_HH_

