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

#include <string>

#include "json/json.h"

#include "src/cpp_core/Utils/CppErrorHandler.hh"
#include "Interface/Squeak.hh"

Json::Value CppErrorHandler::HandleSqueal
                         (Json::Value val, Squeal exc, std::string class_name) {
  exc.Print();
  val["errors"][class_name] = exc.GetMessage()+": "+exc.GetLocation();
  return val;
}

Json::Value CppErrorHandler::HandleStdExc
                 (Json::Value val, std::exception exc, std::string class_name) {
  Squeak::mout(Squeak::error) << std::string(exc.what()) << std::endl;
  val["errors"][class_name]
                         = "Unhandled std::exception: "+std::string(exc.what());
  return val;
}

void CppErrorHandler::HandleSquealNoJson(Squeal exc, std::string class_name) {
  HandleSqueal(Json::Value(), exc, class_name);
}

void CppErrorHandler::HandleStdExcNoJson
                                  (std::exception exc, std::string class_name) {
  HandleStdExc(Json::Value(), exc, class_name);
}

