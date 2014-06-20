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

#include <string>

#include "src/legacy/BeamTools/BTFieldConstructor.hh"

#include "src/common_cpp/Utils/CppErrorHandler.hh"
#include "src/common_cpp/Utils/Globals.hh"

#include "src/common_cpp/Globals/GlobalsManager.hh"
#include "src/common_cpp/Globals/PyLibMausCpp.hh"

namespace MAUS {

namespace PyLibMausCpp {
static PyMethodDef MausCpp_methods[] = {
{NULL, NULL, 0, NULL}
};


PyMODINIT_FUNC initlibMausCpp(void) {
  Py_Initialize();
  PyObject* maus_module = Py_InitModule("libMausCpp", MausCpp_methods);
  if (maus_module == NULL) return;
}
}  // namespace PyLibMausCpp
}  // namespace MAUS

