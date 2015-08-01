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

#ifndef _SRC_COMMON_CPP_API_PYWRAPREDUCEBASE_
#define _SRC_COMMON_CPP_API_PYWRAPREDUCEBASE_

#include <string>

// These ifdefs are required to avoid cpp compiler warning
#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif

#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif

#include "Python.h"

namespace MAUS {

/** @class PyWrapReduceBase Python wrapper for objects inheriting from REduceBase
 *
 *  PyWrapReduceBase provides interfaces between C++ API and python code for a
 *  ReduceBase class
 */

typedef struct {
    PyObject_HEAD
    void* reduce;
} WrapReduce;


template<class REDUCECLASS>
class PyWrapReduceBase {
  public:
    /* \brief Initialise python module for this class
     *
     * \param class_name name string for the python class and module.
     * \param class_docstring docstring for the python class.
     * \param birth_docstring docstring for the birth function.
     * \param death_docstring docstring for the death function.
     * \param process_docstring docstring for the process function.
     * 
     * If docstrings are left empty ("") then some reasonable default docs will
     * be generated.
     *
     * PyWrapReduceBase initialises a python module for this class. This
     * function should be called in a function in the MAUS namespace called
     * init<ReduceName> e.g.
     * PyMODINIT_FUNC init_ReduceCppMyPlots(void) {
     *   PyWrapReduceBase("ReduceCppMyPlots", "", "", "", "");
     * }
     * When user calls import MAUS, MAUS looks in the library 
     * build/_ReduceCppMyPlots.so for a function called
     * init_ReduceCppMyPlots and runs that function to load the python
     * module. The class_name string must correspond to the .so name, so
     * class_name must be "ReduceCppMyPlots"
     * in this case.
     */
    static void PyWrapReduceBaseModInit(
                  std::string class_name,
                  std::string class_docstring,
                  std::string birth_docstring,
                  std::string death_docstring,
                  std::string process_docstring);

    /** PyWrappedReduce is the python implementation of the C++ Reduce
     */
    typedef struct {
        PyObject_HEAD;
        REDUCECLASS* reduce;
        PyObject* can_convert;
    } PyWrappedReduce;

 private:
  // python wrappers for reduce functions
  static PyObject* birth(PyObject* self, PyObject *args, PyObject *kwds);
  static PyObject* death(PyObject* self, PyObject *args, PyObject *kwds);
  static PyObject* process(PyObject* self, PyObject *args, PyObject *kwds);

  // memory allocation etc
  static PyObject* _new(PyTypeObject* type, PyObject *args, PyObject *kwds);
  static int _init(PyWrappedReduce* self, PyObject *args, PyObject *kwds);
  static void _dealloc(PyWrappedReduce* self);

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

#include "src/common_cpp/API/PyWrapReduceBase-inl.hh"

#endif
