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
 */

#ifndef _SRC_COMMON_CPP_API_PYWRAPMAPBASE_
#define _SRC_COMMON_CPP_API_PYWRAPMAPBASE_

#include <Python.h>

#include <string>

namespace MAUS {

static PyObject *InvalidModuleError;
static PyObject *InvalidInputError;
static PyObject *InvalidOutputError;
static std::string ModuleClassName;

/** Python wrapper for objects inheriting from MapBase
 *
 *  PyWrapMapBase provides interfaces between C++ API and python code for 
 */

template<class MODULE>
class PyWrapMapBase {

 public:
  /// Initialise a python module with the appropriate methods defined
  static void ModuleInitialisation();

 private:
  /// Create a new instance of the MODULE parameter
  static PyObject* ModuleNew(PyObject *self, PyObject *args);

  /// Delete the MODULE object owned by the wrapper class
  static PyObject* ModuleDelete(PyObject *self, PyObject *args);

  /// Run the MODULE object's birth function.
  static PyObject* ModuleBirth(PyObject *self, PyObject *args);

  /// Run the MODULE object's death function.
  static PyObject* ModuleDeath(PyObject *self, PyObject *args);

  /// Set the MODULE object's input format.
  static PyObject* ModuleSetInput(PyObject *self, PyObject *args);

  /// Get the MODULE object's output format.
  static PyObject* ModuleGetOutput(PyObject *self, PyObject *args);

  /// Run the MODULE object's process function.
  static PyObject* ModuleProcess(PyObject *self, PyObject *args);

  /// Get an array of Methods for calling during module initialisation
  static PyMethodDef* GetModuleMethods();
};
}  // ~MAUS

#include "src/common_cpp/API/PyWrapMapBase-inl.hh"

#endif
