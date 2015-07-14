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
 *
 */

#ifndef _SRC_PY_CPP_PYCONVERTER_HH_
#define _SRC_PY_CPP_PYCONVERTER_HH_

// These ifdefs are required to avoid cpp compiler warning
#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif

#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif

#include "Python.h"
#include "src/common_cpp/Utils/PyObjectWrapper.hh"

namespace MAUS {
/** @namespace PyConverter converter module for MICE data
 *  
 *  Provides functions to represent MAUS Data structures in various ways.
 *  Functions are
 *    - json_repr: represent data as a python dict
 *    - data_repr: represent data as a ROOT element
 *    - string_repr: represent data as a string element
 *    - del_data_repr: explicitly free memory allocated to a ROOT element
 */

namespace PyConverter {

/** Python function to wrap data as a string
 *
 *  Takes one argument, returns a PyObject* which is the new data
 */
PyObject* py_wrap_string(PyObject* self, PyObject* args);

/** Python function to wrap data as a MAUS::Data
 *
 *  Takes one argument, returns a PyObject* which is the new data
 */
PyObject* py_wrap_data(PyObject* self, PyObject* args);

/** Python function to wrap job header data as a MAUS::JobHeaderData
 *
 *  Takes one argument, returns a PyObject* which is the new job header data
 */
PyObject* py_wrap_job_header_data(PyObject* self, PyObject* args);

/** Python function to wrap job footer data as a MAUS::JobFooterData
 *
 *  Takes one argument, returns a PyObject* which is the new job footer data
 */
PyObject* py_wrap_job_footer_data(PyObject* self, PyObject* args);

/** Python function to wrap run header data as a MAUS::RunHeaderData
 *
 *  Takes one argument, returns a PyObject* which is the new run header data
 */
PyObject* py_wrap_run_header_data(PyObject* self, PyObject* args);

/** Python function to wrap run footer data as a MAUS::RunFooterData
 *
 *  Takes one argument, returns a PyObject* which is the new run footer data
 */
PyObject* py_wrap_run_footer_data(PyObject* self, PyObject* args);

/** Python function to wrap data as a PyObject
 *
 *  Takes one argument, returns a PyObject* which is the new data
 */
PyObject* py_wrap_pyobject(PyObject* self, PyObject* args);

/** Python function to delete MAUS::Data PyObject
 *
 *  Takes one argument which is the MAUS::Data. A type_error will be thrown if 
 *  the argument is not a MAUS::Data. Returns Py_None.
 *
 *  The PyROOT data model wraps everything in a thing called an
 *  ObjectProxy. Python counts the number of pointers to an object, and when
 *  that reaches 0 python calls free on memory allocated; but ObjectProxy does
 *  not delete the associated C++ object, so we have to do it here.
 */
PyObject* py_del_data_repr(PyObject* self, PyObject* args);
}
} // MAUS



#endif

