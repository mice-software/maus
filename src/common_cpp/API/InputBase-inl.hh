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
#include "src/common_cpp/Utils/Exception.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"
#include "src/common_cpp/Utils/PyObjectWrapper.hh"


namespace MAUS {

  template <typename T>
  InputBase<T>::InputBase(const std::string& s) : IInput<T>(), ModuleBase(s) {}

  template <typename T>
  InputBase<T>::InputBase(const InputBase& ib) : IInput<T>(), ModuleBase(ib._classname) {}

  template <typename T>
  InputBase<T>::~InputBase() {}

  template <typename T>
  T InputBase<T>::emit_cpp() {
    T o;
    try {
      o = _emit_cpp();
    }
    catch (Exceptions::Exception& s) {
      CppErrorHandler::getInstance()->HandleExceptionNoJson(s, _classname);
      throw NoInputException();
    }
    catch (std::exception& e) {
      CppErrorHandler::getInstance()->HandleStdExcNoJson(e, _classname);
      throw NoInputException();
    }
    catch (...) {
      throw UnhandledException(_classname);
    }
    return o;
  }

  template <typename T>
  PyObject* InputBase<T>::emit_pyobj() {
      PyObject* wrapped = NULL;
      try {
          T temp_o = emit_cpp();
          T* o = new T(temp_o);
          wrapped = PyObjectWrapper::wrap(o);
      } catch (NoInputException& exc) {
          return NULL;
      }
      return wrapped;
  }

}  // end of namespace
#endif

