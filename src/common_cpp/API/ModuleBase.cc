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
#include "src/common_cpp/API/ModuleBase.hh"
#include "src/common_cpp/API/APIExceptions.hh"
#include "Utils/Exception.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"

namespace MAUS {

  ModuleBase::ModuleBase(const std::string& s) : IModule(), _classname(s) {}
  ModuleBase::ModuleBase(const ModuleBase& mb) : IModule(), _classname(mb._classname) {}
  ModuleBase::~ModuleBase() {}

  void ModuleBase::birth(const std::string& s) {
    try {
      _birth(s);
    }
    catch (Exception& s) {
      CppErrorHandler::getInstance()->HandleExceptionNoJson(s, _classname);
    }
    catch (std::exception & e) {
      CppErrorHandler::getInstance()->HandleStdExcNoJson(e, _classname);
    }
    catch (...) {
      throw UnhandledException(_classname);
    }
  }

  void ModuleBase::death() {
    try {
      _death();
    }
    catch (Exception& s) {
      CppErrorHandler::getInstance()->HandleExceptionNoJson(s, _classname);
    }
    catch (std::exception & e) {
      CppErrorHandler::getInstance()->HandleStdExcNoJson(e, _classname);
    }
    catch (...) {
      throw UnhandledException(_classname);
    }
  }

}// end of namespace
