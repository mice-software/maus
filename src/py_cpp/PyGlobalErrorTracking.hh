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

#ifndef _SRC_PY_CPP_PyGlobalErrorTracking_HH_
#define _SRC_PY_CPP_PyGlobalErrorTracking_HH_

// These ifdefs are required to avoid cpp compiler warning
#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif

#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif

#include "Python.h"

namespace MAUS {
namespace PyGlobalErrorTracking {
/* @brief Track a central trajectory and error through the Geant4 geometry
 *
 * Track a central trajectory and error through the Geant4 geometry. Return a MC
 * event encoded as a json string.
 */
static PyObject* propagate_errors
                               (PyObject *self, PyObject *args, PyObject *kwds);

/** Initialise error_propagation module
 *
 *  This is called by import error_propagation
 */
PyMODINIT_FUNC initerror_propagation(void);

}  // namespace PyGlobalErrorTracking
}  // namespace MAUS

#endif  // _SRC_PY_CPP_PyGlobalErrorTracking_HH_


