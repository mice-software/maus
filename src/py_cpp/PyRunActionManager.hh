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

#ifndef _SRC_PY_CPP_PYRUNACTIONMANAGER_HH_
#define _SRC_PY_CPP_PYRUNACTIONMANAGER_HH_

// These ifdefs are required to avoid cpp compiler warning
#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif

#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif

#include "Python.h"

namespace MAUS {

namespace PyRunActionManager {
/* @brief Call start of run given a run number
 *
 * Initialises a StartOfRun object and calls RunActionManager StartOfRun()
 * Returns a ROOT.MAUS.StartOfRun() python object
 *
 * Argument should be an integer that is the run number
 */
PyObject* StartOfRun(PyObject *dummy, PyObject *args);

/* @brief Call end of run given a run number
 *
 * Initialises a EndOfRun object and calls RunActionManager::EndOfRun()
 * Returns a ROOT.MAUS.EndOfRun() python object
 *
 * Ignores all arguments
 */
PyObject* EndOfRun(PyObject *dummy, PyObject *args);
}  // namespace PyRunActionManager
}  // namespace MAUS

#endif  // _SRC_PY_CPP_PYRUNACTIONMANAGER_HH_
