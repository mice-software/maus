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
#ifndef _SRC_COMMON_CPP_API_OUTPUTBASE_INL_
#define _SRC_COMMON_CPP_API_OUTPUTBASE_INL_

#include <string>
#include "src/common_cpp/API/APIExceptions.hh"
#include "Utils/Exception.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"

namespace MAUS {


  template <typename T>
  OutputBase<T>::OutputBase(const std::string& s) : IOutput<T>(), ModuleBase(s) {}

  template <typename T>
  OutputBase<T>::OutputBase(const OutputBase& ob) : IOutput<T>(), ModuleBase(ob._classname) {}

  template <typename T>
  OutputBase<T>::~OutputBase() {}

  template <typename T>
  bool OutputBase<T>::save(T t) {
    bool ret = false;
    try {
      std::cerr << "OutputBase::save" << std::endl;
      ret = _save(t);
      std::cerr << "OutputBase::save done" << std::endl;
    }
    catch (std::exception& e) {
      std::cerr << "OutputBase::save exc 2" << std::endl;
      CppErrorHandler::getInstance()->HandleStdExcNoJson(e, _classname);
    }
    catch (...) {
      std::cerr << "OutputBase::save exc 3" << std::endl;
      throw UnhandledException(_classname);
    }
    return ret;
  }

}// end of namespace
#endif
