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
#ifndef _SRC_COMMON_CPP_API_INPUTBASE_INL_
#define _SRC_COMMON_CPP_API_INPUTBASE_INL_

#include <string>
#include "src/common_cpp/API/APIExceptions.hh"
#include "src/legacy/Interface/Squeal.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"

namespace MAUS {

  template <typename T>
  InputBase<T>::InputBase(const std::string& s) : IInput<T>(), ModuleBase(s) {}

  template <typename T>
  InputBase<T>::InputBase(const InputBase& ib) : IInput<T>(), ModuleBase(ib._classname) {}

  template <typename T>
  InputBase<T>::~InputBase() {}

  template <typename T>
  T InputBase<T>::emitter_cpp() {
    T o;
    try {
      o = _emitter_cpp();
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

  template <typename T>
  T InputBase<T>::load_job_header() {
    T o;
    try {
      o = _load_job_header();
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
}  // end of namespace
#endif

