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
#ifndef _MAUS_API_INPUT_BASE_H
#define _MAUS_API_INPUT_BASE_H
#include <string>
#include <exception>
#include "API/IInput.hh"
#include "API/ModuleBase.hh"
#include "API/APIExceptions.hh"
#include "Interface/Squeal.hh"
#include "Utils/CppErrorHandler.hh"

namespace MAUS {

  template <typename T>
  class InputBase : public virtual IInput<T>, public ModuleBase {

  public:
    explicit InputBase(const std::string&);
    InputBase(const InputBase&);
    virtual ~InputBase();

  public:
    T* emitter();

  private:
    virtual T* _emitter() = 0;
  };

  template <typename T>
  InputBase<T>::InputBase(const std::string& s) : IInput<T>(), ModuleBase(s) {}

  template <typename T>
  InputBase<T>::InputBase(const InputBase& ib) : IInput<T>(), ModuleBase(ib._classname) {}

  template <typename T>
  InputBase<T>::~InputBase() {}

  template <typename T>
  T* InputBase<T>::emitter() {
    T* o = 0;
    try {
      o = _emitter();
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

}// end of namespace
#endif

