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

// I thought about putting this straight into MapBase, but decided against it;
// I thought it better to split the C API and the python API more strongly.
//
// C. T. Rogers, 2014

#ifndef _SRC_COMMON_CPP_API_PYWRAPMAPBASE_
#define _SRC_COMMON_CPP_API_PYWRAPMAPBASE_

#include <string>

// These ifdefs are required to avoid cpp compiler warning
#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif

#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif

#include <Python.h>

namespace MAUS {

/** @class PyWrapMapBase Python wrapper for objects inheriting from MapBase
 *
 *  PyWrapMapBase provides interfaces between C++ API and python code for a
 *  MapBase class
 */

typedef struct {
    PyObject_HEAD
    void* map;
} WrapMap;


template<class MAPCLASS>
class PyWrapMapBase {
  public:
    /* \brief Initialise python module for this class
     *
     * \param class_docstring docstring for the python class.
     * \param birth_docstring docstring for the birth function.
     * \param death_docstring docstring for the death function.
     * \param process_docstring docstring for the process function.
     * 
     * If docstrings are left empty ("") then some reasonable default docs will
     * be generated.
     *
     * PyWrapMapBase initialises a python module for this class. This
     * function should be called in a function in the MAUS namespace called
     * init<MapName> e.g.
     * PyMODINIT_FUNC init_MapCppSimulation(void) {
     *   PyWrapMapBase("", "", "", "");
     * }
     * When user calls import MAUS, MAUS looks in the library 
     * build/_MapCppSimulation.so for a function called init_MapCppSimulation
     * and runs that function to load the python module.
     */
    static void PyWrapMapBaseModInit(std::string class_docstring,
                  std::string birth_docstring,
                  std::string death_docstring,
                  std::string process_docstring);

    /** PyWrappedMap is the python implementation of the C++ Map
     */
    typedef struct {
        PyObject_HEAD;
        MAPCLASS* map;
        PyObject* can_convert;
    } PyWrappedMap;

 private:
  // python wrappers for map functions
  static PyObject* birth(PyObject* self, PyObject *args, PyObject *kwds);
  static PyObject* death(PyObject* self, PyObject *args, PyObject *kwds);
  static PyObject* process(PyObject* self, PyObject *args, PyObject *kwds);

  // memory allocation etc
  static PyObject* _new(PyTypeObject* type, PyObject *args, PyObject *kwds);
  static int _init(PyWrappedMap* self, PyObject *args, PyObject *kwds);
  static void _dealloc(PyWrappedMap* self);

  // python stuff
  static PyMethodDef _module_methods[];
  static PyMethodDef _methods[];
  static PyMemberDef _members[];
  static PyTypeObject _class_type;

  // class name
  static std::string _class_name;  // Class
  static std::string _module_name;  // module
  static std::string _path_name;  // module.Class

  // docstrings
  static std::string _class_docstring;
  static std::string _birth_docstring;
  static std::string _death_docstring;
  static std::string _process_docstring;
};
}  // ~MAUS

#include "src/common_cpp/API/PyWrapMapBase-inl.hh"

#endif
