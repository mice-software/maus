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
#ifndef _SRC_COMMON_CPP_API_MAPBASE_INL_
#define _SRC_COMMON_CPP_API_MAPBASE_INL_

#include <string>
#include "src/common_cpp/API/APIExceptions.hh"
#include "src/common_cpp/Utils/PyObjectWrapper.hh"
#include "src/common_cpp/Utils/Exception.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"
#include "src/common_cpp/Converter/ConverterFactory.hh"

namespace MAUS {

  template <typename TYPE>
  MapBase<TYPE>::MapBase(const std::string& s) :
    IMap<TYPE>(), ModuleBase(s) {}

  template <typename TYPE>
  MapBase<TYPE>::MapBase(const MapBase& mb) :
    IMap<TYPE>(), ModuleBase(mb._classname) {}

  template <typename TYPE>
  MapBase<TYPE>::~MapBase() {}

  template<typename TYPE>
  PyObject* MapBase<TYPE>::process_pyobj(PyObject* py_input) const {
    // this function owns cpp_data; py_input is still owned by caller
    TYPE* cpp_data = PyObjectWrapper::unwrap<TYPE>(py_input);
    try {
        _process(cpp_data);
    }
    catch(Exception& s) {
        CppErrorHandler::getInstance()->HandleExceptionNoJson(s, _classname);
    } catch(std::exception& e) {
        CppErrorHandler::getInstance()->HandleStdExcNoJson(e, _classname);
    } catch(...) {
        throw Exception(Exception::recoverable,
                        _classname+" threw an unhandled exception",
                        "MapBase::process_pyobj");
    }

    PyObject* py_output = PyObjectWrapper::wrap(cpp_data);
    // py_output now owns cpp_data
    return py_output;
  }



}// end of namespace
#endif

