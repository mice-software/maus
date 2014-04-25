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

  template <typename INPUT, typename OUTPUT>
  MapBase<INPUT, OUTPUT>::MapBase(const std::string& s) :
    IMap<INPUT, OUTPUT>(), ModuleBase(s) {}

  template <typename INPUT, typename OUTPUT>
  MapBase<INPUT, OUTPUT>::MapBase(const MapBase& mb) :
    IMap<INPUT, OUTPUT>(), ModuleBase(mb._classname) {}

  template <typename INPUT, typename OUTPUT>
  MapBase<INPUT, OUTPUT>::~MapBase() {}

  template<typename INPUT, typename OUTPUT>
  OUTPUT* MapBase<INPUT, OUTPUT>::process(INPUT* i) const {
    if (!i) { throw NullInputException(_classname); }
    OUTPUT* o = 0;
    try {
      o =  _process(i);
    }
    catch(Exception& s) {
      CppErrorHandler::getInstance()->HandleExceptionNoJson(s, _classname);
    }
    catch(std::exception& e) {
      CppErrorHandler::getInstance()->HandleStdExcNoJson(e, _classname);
    }
    catch(...) {
      throw UnhandledException(_classname);
    }
    return o;
  }

  template<typename INPUT, typename OUTPUT>
  template<typename OTHER>
  OUTPUT* MapBase<INPUT, OUTPUT>::process(OTHER* o) const {

    ConverterFactory c;
    INPUT* tmp = 0;
    OUTPUT* ret = 0;
    try {
      tmp = c.convert<OTHER, INPUT>(o);
      ret =  process(tmp);
    }
    catch(std::exception& e) {
      if (tmp) { delete tmp; }
      CppErrorHandler::getInstance()->HandleStdExcNoJson(e, _classname);
      return ret;
    }

    // catch the pass through case
    if (reinterpret_cast<void*>(tmp) != reinterpret_cast<void*>(o)  &&
        reinterpret_cast<void*>(tmp) != reinterpret_cast<void*>(ret)   ) {
      delete tmp;
    }
    return ret;
  }

  template<typename INPUT, typename OUTPUT>
  PyObject* MapBase<INPUT, OUTPUT>::process_pyobj(PyObject* py_input) const {
    INPUT* cpp_input = PyObjectWrapper::unwrap_pyobject<INPUT>(py_input);
    OUTPUT* cpp_output = process(cpp_input);
    PyObject* py_output = PyObjectWrapper::wrap_pyobject(cpp_output);
    return py_output;
  }
}// end of namespace
#endif

