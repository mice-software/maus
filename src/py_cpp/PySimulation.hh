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

#ifndef _SRC_PY_CPP_PYSIMULATION_HH_
#define _SRC_PY_CPP_PYSIMULATION_HH_

// These ifdefs are required to avoid cpp compiler warning
#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif

#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif

#include "Python.h"

namespace MAUS {
namespace PySimulation {
/* @brief Track a list of particles through the Geant4 geometry
 *
 * Push a list of particles through the Geant4 geometry. Return a MC event 
 * encoded as a json string.
 */
static PyObject* track_particles
                               (PyObject *self, PyObject *args, PyObject *kwds);

/** Initialise simulation module
 *
 *  This is called by import simulation
 */
PyMODINIT_FUNC initsimulation(void);

}  // namespace PySimulation
}  // namespace MAUS

#endif  // _SRC_PY_CPP_PYSIMULATION_HH_
