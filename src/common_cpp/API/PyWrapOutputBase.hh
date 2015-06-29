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

// C. T. Rogers, 2015

#ifndef _SRC_COMMON_CPP_API_PYWRAPOUTPUTBASE_
#define _SRC_COMMON_CPP_API_PYWRAPOUTPUTBASE_

#include <Python.h>

#include <string>

namespace MAUS {

/** @class PyWrapOutputBase Python wrapper for objects inheriting from OutputBase
 *
 *  PyWrapOutputBase provides interfaces between C++ API and python code for a
 *  OutputBase class
 */

typedef struct {
    PyObject_HEAD
    void* output;
} WrapOutput;


template <class OUTPUTCLASS>
class PyWrapOutputBase {
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
     * PyWrapOutputBase initialises a python module for this class. This
     * function should be called in a function in the MAUS namespace called
     * init<OutputName> e.g.
     * PyMODINIT_FUNC init_OutputCppRoot(void) {
     *   PyWrapOutputBase("OutputCppRoot", "", "", "", "");
     * }
     * When user calls import MAUS, MAUS looks in the library 
     * build/_OutputCppRoot.so for a function called init_OutputCppRoot
     * and runs that function to load the python module. The class_name string
     * must correspond to the .so name, so class_name must be "OutputCppRoot"
     * in this case.
     */
    static void PyWrapOutputBaseModInit(
                  std::string class_name,
                  std::string class_docstring,
                  std::string birth_docstring,
                  std::string death_docstring,
                  std::string process_docstring);

    /** PyWrappedOutput is the python implementation of the C++ Output
     */
    typedef struct {
        PyObject_HEAD;
        OUTPUTCLASS* output;
        PyObject* can_convert;
    } PyWrappedOutput;

 private:
  // python wrappers for output functions
  static PyObject* birth(PyObject* self, PyObject *args, PyObject *kwds);
  static PyObject* death(PyObject* self, PyObject *args, PyObject *kwds);
  static PyObject* save(PyObject* self, PyObject *args, PyObject *kwds);

  // memory allocation etc
  static PyObject* _new(PyTypeObject* type, PyObject *args, PyObject *kwds);
  static int _init(PyWrappedOutput* self, PyObject *args, PyObject *kwds);
  static void _dealloc(PyWrappedOutput* self);

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
  static std::string _save_docstring;
};
}  // ~MAUS

#include "src/common_cpp/API/PyWrapOutputBase-inl.hh"

#endif
