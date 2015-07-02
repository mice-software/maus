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

#ifndef _SRC_PY_CPP_PYGLOBALS_HH_
#define _SRC_PY_CPP_PYGLOBALS_HH_

// These ifdefs are required to avoid cpp compiler warning
#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif

#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif

#include "Python.h"

namespace MAUS {

namespace PyGlobals {
/* @brief Initialise MAUS
 *
 * Initialise MAUS globals. Takes one argument which should be a string set of
 * datacards (as read by Configuration module). Throws a Runtime error if MAUS
 * globals are already set. Returns Py_None.
 */
static PyObject* Birth(PyObject *dummy, PyObject *args);

/* @brief Clean up MAUS
 *
 * Delete MAUS globals. Ignores all arguments. Throws a RuntimeError if MAUS
 * globals were never set. Returns Py_None.
 */
static PyObject* Death(PyObject *dummy, PyObject *args);

/* @brief Check if MAUS has been initialised
 *
 * Check for initalisation of MAUS globals. Ignores all arguments. Returns
 * PyObject integer 1 if library is initialised, 0 if it is not
 */
static PyObject* HasInstance(PyObject *dummy, PyObject *args);

/* @brief Get Json configuration datacards
 *
 * Returns NULL if Globals were not initialised
 */
static PyObject* GetConfigurationCards(PyObject* dummy, PyObject* args);

/* @brief Get Monte Carlo MiceModules
 *
 * Returns NULL if Globals were not initialised
 */
static PyObject* GetMonteCarloMiceModules
                              (PyObject* dummy, PyObject* args, PyObject *kwds);

/* @brief Get Monte Carlo MiceModules
 *
 * Returns NULL if Globals were not initialised
 */
static PyObject* SetMonteCarloMiceModules
                              (PyObject* dummy, PyObject* args, PyObject *kwds);

/* @brief Get Reconstruction MiceModules
 *
 * Returns NULL if Globals were not initialised
 */
static PyObject* GetReconstructionMiceModules
                              (PyObject* dummy, PyObject* args, PyObject *kwds);

/* @brief Get Reconstruction MiceModules
 *
 * Returns NULL if Globals were not initialised
 */
static PyObject* SetReconstructionMiceModules
                              (PyObject* dummy, PyObject* args, PyObject *kwds);

/* @brief Get the MAUS version number as a Python string like x.y.z
 */
static PyObject* GetVersionNumber(PyObject *dummy, PyObject *args);
}  // namespace PyGlobals
}  // namespace MAUS

#endif  // _SRC_PY_CPP_PYGLOBALS_HH_
