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
#include "src/legacy/Interface/Squeal.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"
#include "src/common_cpp/Converter/ConverterFactory.hh"

namespace MAUS {

  template <typename INPUT, typename OUTPUT>
  MapBase<INPUT, OUTPUT>::MapBase(const std::string& s) :
    IMap<INPUT, OUTPUT>(), ModuleBase(s) {}
  template <typename OUTPUT>
  MapBase<Json::Value, OUTPUT>::MapBase(const std::string& s) :
    IMap<Json::Value, OUTPUT>(), ModuleBase(s) {}

  template <typename INPUT, typename OUTPUT>
  MapBase<INPUT, OUTPUT>::MapBase(const MapBase& mb) :
    IMap<INPUT, OUTPUT>(), ModuleBase(mb._classname) {}
  template <typename OUTPUT>
  MapBase<Json::Value, OUTPUT>::MapBase(const MapBase& mb) :
    IMap<Json::Value, OUTPUT>(), ModuleBase(mb._classname) {}

  template <typename INPUT, typename OUTPUT>
  MapBase<INPUT, OUTPUT>::~MapBase() {}
  template <typename OUTPUT>
  MapBase<Json::Value, OUTPUT>::~MapBase() {}

  template<typename INPUT, typename OUTPUT>
  OUTPUT* MapBase<INPUT, OUTPUT>::process(INPUT* i) const {
    if (!i) { throw NullInputException(_classname); }
    OUTPUT* o = 0;
    try {
      o =  _process(i);
    }
    catch(Squeal& s) {
      CppErrorHandler::getInstance()->HandleSquealNoJson(s, _classname);
    }
    catch(std::exception& e) {
      CppErrorHandler::getInstance()->HandleStdExcNoJson(e, _classname);
    }
    catch(...) {
      throw UnhandledException(_classname);
    }
    return o;
  }
  template<typename OUTPUT>
  OUTPUT* MapBase<Json::Value, OUTPUT>::process(Json::Value* i) const {
    if (!i) { throw NullInputException(_classname); }
    OUTPUT* o = 0;
    try {
      o = _process(i);
    }
    catch(Squeal& s) {
      CppErrorHandler::getInstance()->HandleSqueal(*i, s, _classname);
    }
    catch(std::exception& e) {
      CppErrorHandler::getInstance()->HandleStdExc(*i, e, _classname);
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

    if (reinterpret_cast<void*>(tmp) != reinterpret_cast<void*>(o)  &&
        reinterpret_cast<void*>(tmp) != reinterpret_cast<void*>(ret)   ) { // catch the pass through case
      delete tmp;
    }
    return ret;
  }
  template<typename OUTPUT>
  template<typename OTHER>
  OUTPUT* MapBase<Json::Value, OUTPUT>::process(OTHER* o) const {

    ConverterFactory c;
    Json::Value* tmp;
    OUTPUT* ret;
    try {
      tmp = c.convert<OTHER, Json::Value>(o);
      ret =  process(tmp);
    }
    catch(std::exception& e) {
      if (tmp) { delete tmp; }
      CppErrorHandler::getInstance()->HandleStdExcNoJson(e, _classname);
      return ret;
    }

    if (reinterpret_cast<void*>(tmp) != reinterpret_cast<void*>(o)  &&
        reinterpret_cast<void*>(tmp) != reinterpret_cast<void*>(ret)   ) { // catch the pass through case
      delete tmp;
    }
    return ret;
  }

}// end of namespace
#endif

