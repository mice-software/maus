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
#include "src/common_cpp/JsonCppProcessors/PrimitivesProcessors.hh"

namespace MAUS {

double* DoubleProcessor::JsonToCpp(const Json::Value& json_double) {
  if (json_double.isNumeric()) {
      return new double (json_double.asDouble());
  } else {
      throw(Squeal(
          Squeal::recoverable,
          "Failed to convert json value to double",
          "DoubleProcessor::JsonToCpp"
      ));
  }
  return NULL; // appease gcc
}

Json::Value* DoubleProcessor::CppToJson(const double& cpp_double) {
  return new Json::Value(cpp_double);
}

Json::Value* DoubleProcessor::CppToJson
                                  (const double& cpp_double, std::string path) {
  Json::Value* json_double = new Json::Value(cpp_double);
  JsonWrapper::Path::SetPath(*json_double, path);
  return json_double;
}

std::string* StringProcessor::JsonToCpp(const Json::Value& json_string) {
  if (json_string.isString()) {
      return new std::string (json_string.asString());
  } else {
      throw(Squeal(
          Squeal::recoverable,
          "Failed to convert json value to string",
          "StringProcessor::JsonToCpp"
      ));
  }
}

Json::Value* StringProcessor::CppToJson(const std::string& cpp_string) {
  return new Json::Value(cpp_string);
}

Json::Value* StringProcessor::CppToJson
                             (const std::string& cpp_string, std::string path) {
  Json::Value* json_string = new Json::Value(cpp_string);
  JsonWrapper::Path::SetPath(*json_string, path);
  return json_string;
}

int* IntProcessor::JsonToCpp(const Json::Value& json_int) {
  if (json_int.isInt()) {
      return new int (json_int.asInt());
  } else {
      throw(Squeal(
          Squeal::recoverable,
          "Failed to convert json value to integer",
          "IntProcessor::JsonToCpp"
      ));
  }
}

Json::Value* IntProcessor::CppToJson(const int& cpp_int) {
  return new Json::Value(cpp_int);
}

Json::Value* IntProcessor::CppToJson
                                  (const int& cpp_int, std::string path) {
  Json::Value* json_int = new Json::Value(cpp_int);
  JsonWrapper::Path::SetPath(*json_int, path);
  return json_int;
}

unsigned int* UIntProcessor::JsonToCpp(const Json::Value& json_uint) {
  if (json_uint.isUInt()) {
      return new unsigned int (json_uint.asUInt());
  } else if (json_uint.isInt() && json_uint.asInt() >= 0) {
      return new unsigned int (json_uint.asUInt());
  } else {
      throw(Squeal(
          Squeal::recoverable,
          "Failed to convert json value to unsigned int",
          "UIntProcessor::JsonToCpp"
      ));
  }
}

Json::Value* UIntProcessor::CppToJson(const unsigned int& cpp_uint) {
  return new Json::Value(cpp_uint);
}

Json::Value* UIntProcessor::CppToJson
                              (const unsigned int& cpp_uint, std::string path) {
  Json::Value* json_uint = new Json::Value(Json::UInt(cpp_uint));
  JsonWrapper::Path::SetPath(*json_uint, path);
  return json_uint;
}

bool* BoolProcessor::JsonToCpp(const Json::Value& json_bool) {
  if (json_bool.isBool()) {
      return new bool (json_bool.asBool());
  } else {
      throw(Squeal(
          Squeal::recoverable,
          "Failed to convert json value to bool",
          "BoolProcessor::JsonToCpp"
      ));
  }
}

Json::Value* BoolProcessor::CppToJson(const bool& cpp_bool) {
  return new Json::Value(cpp_bool);
}

Json::Value* BoolProcessor::CppToJson
                                  (const bool& cpp_bool, std::string path) {
  Json::Value* json_bool = new Json::Value(cpp_bool);
  JsonWrapper::Path::SetPath(*json_bool, path);
  return json_bool;
}
}

