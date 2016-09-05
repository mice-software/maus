/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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
 */

#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/JsonCppProcessors/Global/EnumeratorProcessors.hh"

namespace MAUS {
namespace Processor {
namespace Global {

MAUS::DataStructure::Global::DetectorPoint*
EnumDetectorPointProcessor::JsonToCpp(
    const Json::Value& json_int) {
  if (json_int.isNumeric()) {
    MAUS::DataStructure::Global::DetectorPoint* dp =
        new MAUS::DataStructure::Global::DetectorPoint();
    (*dp) =
        static_cast<MAUS::DataStructure::Global::DetectorPoint>(
            json_int.asInt());
    return dp;
  } else {
    throw(Exceptions::Exception(
        Exceptions::recoverable,
        "Failed to convert json value to enumerator: DetectorPoint",
        "EnumDetectorPointProcessor::JsonToCpp"
                 ));
  }
  return NULL; // appease gcc
}

Json::Value* EnumDetectorPointProcessor::CppToJson(
    const MAUS::DataStructure::Global::DetectorPoint& cpp_enum) {
  return new Json::Value(static_cast<int>(cpp_enum));
}

Json::Value* EnumDetectorPointProcessor::CppToJson(
    const MAUS::DataStructure::Global::DetectorPoint& cpp_enum,
    std::string path) {
  Json::Value* json_int = new Json::Value(static_cast<int>(cpp_enum));
  JsonWrapper::Path::SetPath(*json_int, path);
  return json_int;
}

MAUS::DataStructure::Global::PID* EnumPIDProcessor::JsonToCpp(
    const Json::Value& json_int) {
  if (json_int.isNumeric()) {
    MAUS::DataStructure::Global::PID* pd =
        new MAUS::DataStructure::Global::PID();
    (*pd) = static_cast<MAUS::DataStructure::Global::PID>(json_int.asInt());
    return pd;
  } else {
    throw(Exceptions::Exception(
        Exceptions::recoverable,
        "Failed to convert json value to enumerator: PID",
        "EnumPIDProcessor::JsonToCpp"
                 ));
  }
  return NULL; // appease gcc
}

Json::Value* EnumPIDProcessor::CppToJson(
    const MAUS::DataStructure::Global::PID& cpp_enum) {
  return new Json::Value(static_cast<int>(cpp_enum));
}

Json::Value* EnumPIDProcessor::CppToJson(
    const MAUS::DataStructure::Global::PID& cpp_enum, std::string path) {
  Json::Value* json_int = new Json::Value(static_cast<int>(cpp_enum));
  JsonWrapper::Path::SetPath(*json_int, path);
  return json_int;
}

} // ~namespace Global
} // ~namespace Processor
} // ~namespace MAUS


