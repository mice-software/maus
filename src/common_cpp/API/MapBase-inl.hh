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
#include "Utils/Squeak.hh"
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
    TYPE* cpp_data = NULL;
    try {
        cpp_data = PyObjectWrapper::unwrap<TYPE>(py_input);
        _process(cpp_data);
    } catch (MAUS::Exceptions::Exception& exc) {
        Squeak::mout(Squeak::debug) << "Stack trace:" << exc.GetStackTrace()
                                                                   << std::endl;
        HandleException(&cpp_data, &exc, _classname);
    } catch (std::exception& exc) {
        HandleException(&cpp_data, &exc, _classname);
    } catch (...) {
        throw Exceptions::Exception(Exceptions::recoverable,
                        _classname+" threw an unhandled exception",
                        "MapBase::process_pyobj");
    }
    PyObject* py_output = PyObjectWrapper::wrap(cpp_data);
    // py_output now owns cpp_data
    return py_output;
  }

  // Need some wrapper code for the exception handler as the interface is
  // assumed to be json. As conversion is error prone, need to be a bit careful
  // and do the conversion here.
  template <typename TYPE>
  void MapBase<TYPE>::HandleException(TYPE** data,
                               std::exception* exc,
                               std::string class_name) const {
      if (!data) {
          throw *exc;
      }
      Json::Value* val = NULL;
      try {
          try {
              val = ConverterFactory().convert<TYPE, Json::Value>(*data);
          } catch (...) {
              // do nothing; catch data == NULL or failed conversion to json
          }
          delete *data;  // we don't need it any more
          if (val == NULL) {  // conversion failed, try to build from scratch
              MAUS::Data data_temp;
              data_temp.SetSpill(new Spill());
              val = ConverterFactory().convert<MAUS::Data, Json::Value>(&data_temp);
          }
          *val = CppErrorHandler::getInstance()->HandleStdExc(*val, *exc, class_name);
          *data = ConverterFactory().convert<Json::Value, TYPE>(val);
          delete val;
      } catch (...) {
          std::cerr << "Unhandled exception" << std::endl;
          throw;
      }
  }

}// end of namespace
#endif

