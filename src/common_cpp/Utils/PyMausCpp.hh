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

#ifndef PYMAUSCPP
#define PYMAUSCPP

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
 * Tell C which python function to call in the event of an error by calling the
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

}  // namespace MAUS

#endif

