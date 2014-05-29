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

#ifndef _SRC_COMMON_CPP_GLOBALS_PYMAUSCPP_HH_
#define _SRC_COMMON_CPP_GLOBALS_PYMAUSCPP_HH_

// These ifdefs are required to avoid cpp compiler warning
#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif

#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif

#include "Python.h"

namespace MAUS {

namespace PyLibMausCpp {
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
}
}  // namespace MAUS

#endif  // _SRC_COMMON_CPP_GLOBALS_PYMAUSCPP_HH_

