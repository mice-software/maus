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
#ifndef _SRC_COMMON_CPP_API_REDUCEBASE_INL_
#define _SRC_COMMON_CPP_API_REDUCEBASE_INL_

#include <string>
#include "src/common_cpp/API/APIExceptions.hh"
#include "Utils/Exception.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"
#include "src/common_cpp/Utils/PyObjectWrapper.hh"
#include "src/legacy/Interface/Squeak.hh"

namespace MAUS {

  template <typename T_IN, typename T_OUT>
  ReduceBase<T_IN, T_OUT>::ReduceBase(const std::string& s)
          : IReduce<T_IN, T_OUT>(), ModuleBase(s) {
  }

  template <typename T_IN, typename T_OUT>
  ReduceBase<T_IN, T_OUT>::ReduceBase(const ReduceBase& rb)
          : IReduce<T_IN, T_OUT>(), ModuleBase(rb._classname) {
  }

  template <typename T_IN, typename T_OUT>
  ReduceBase<T_IN, T_OUT>::~ReduceBase() {}

  template <typename T_IN, typename T_OUT>
  T_OUT* ReduceBase<T_IN, T_OUT>::process(T_IN* t_in) {
    if (!t_in) { throw NullInputException(_classname); }
    T_OUT* t_out = _process(t_in);
    return t_out;
  }

  template <typename T_IN, typename T_OUT>
  PyObject* ReduceBase<T_IN, T_OUT>::process_pyobj(PyObject* py_input) {
    T_OUT* cpp_out = NULL;
    try {
        // this function owns cpp_in; py_input is still owned by caller
        // (unwrap performed a deep copy and did any necessary type conversion)
        T_IN* cpp_in = PyObjectWrapper::unwrap<T_IN>(py_input);
        cpp_out = _process(cpp_in);
        delete cpp_in;
    } catch (std::exception& e) {
      CppErrorHandler::getInstance()->HandleStdExcNoJson(e, _classname);
    } catch (...) {
      throw UnhandledException(_classname);
    }

    PyObject* py_output = PyObjectWrapper::wrap(cpp_out);
    // py_output owns cpp_out
    return py_output;
  }

}// end of namespace
#endif

