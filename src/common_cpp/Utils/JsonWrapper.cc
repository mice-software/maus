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
 *
 */

#include <algorithm>

#include "json/json.h"

#include "Interface/Squeal.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"

Json::Value JsonWrapper::StringToJson(std::string json_in) throw(Squeal) {
  Json::Reader reader;
  Json::Value  json_out;
  bool parsingSuccessful = reader.parse(json_in, json_out);
  if (!parsingSuccessful) {
    throw(Squeal(Squeal::recoverable,
          "Failed to parse Json configuration. Json reports\n"
                                      +reader.getFormatedErrorMessages(),
          "JsonWrapper::StringToJson()"));
  }
  return json_out;
}

Json::Value JsonWrapper::GetItem
    (Json::Value array, size_t value_index, JsonType value_type) throw(Squeal) {
  if (array.type() != Json::arrayValue) {
    throw(Squeal(Squeal::recoverable,
                 "Attempting to find Json item but not an array",
                 "JsonWrapper::GetPropertyStrict"));
  }
  if (value_index >= array.size()) {
    throw(Squeal(Squeal::recoverable,
                         "Index out of range "+STLUtils::ToString(value_index)
                                                      +" in Json array lookup",
                         "JsonWrapper::GetItemStrict"));
  }
  if (SimilarType(ValueTypeToJsonType(array[value_index].type()), value_type))
    return array[value_index];
  throw(Squeal(Squeal::recoverable,
               "Value of wrong type in Json array lookup",
               "JsonWrapper::GetItemStrict"));
}

Json::Value JsonWrapper::GetProperty
     (Json::Value object, std::string name, JsonType value_type) throw(Squeal) {
  if (object.type() != Json::objectValue) {
    throw(Squeal(Squeal::recoverable,
                 "Attempting to find Json property "+name+" but not an object",
                 "JsonWrapper::GetPropertyStrict"));
  }
  if (object.isMember(name)) {
    if (SimilarType(ValueTypeToJsonType(object[name].type()), value_type)) {
      return object[name];
    } else {  // type incorrect
      throw(Squeal(Squeal::recoverable,
                   "Property "+name+"  had wrong type in Json object lookup",
                   "JsonWrapper::GetPropertyStrict"));
    }
  } else {  // not a member
    throw(Squeal(Squeal::recoverable,
                 "Property "+name+"  not found in Json object lookup",
                 "JsonWrapper::GetPropertyStrict"));
  }
}

JsonWrapper::JsonType JsonWrapper::ValueTypeToJsonType(Json::ValueType tp) {
  switch (tp) {
    case Json::nullValue: return nullValue;
    case Json::intValue: return intValue;
    case Json::uintValue: return uintValue;
    case Json::realValue: return realValue;
    case Json::stringValue: return stringValue;
    case Json::booleanValue: return booleanValue;
    case Json::arrayValue: return arrayValue;
    case Json::objectValue: return objectValue;
  }
}

Json::ValueType JsonWrapper::JsonTypeToValueType(JsonWrapper::JsonType tp)
                                                                throw(Squeal) {
  switch (tp) {
    case nullValue:    return Json::nullValue;
    case intValue:     return Json::intValue;
    case uintValue:    return Json::uintValue;
    case realValue:    return Json::realValue;
    case stringValue:  return Json::stringValue;
    case booleanValue: return Json::booleanValue;
    case arrayValue:   return Json::arrayValue;
    case objectValue:  return Json::objectValue;
    case anyValue:
      throw(Squeal(Squeal::recoverable,
                   "Could not convert anyValue to Json ValueType",
                   "JsonWrapper::JsonTypeToValueType"));
  }
}


bool JsonWrapper::SimilarType(JsonWrapper::JsonType jt1,
                              JsonWrapper::JsonType jt2) {
  return (jt1 == jt2 || jt1 == JsonWrapper::anyValue
                     || jt2 == JsonWrapper::anyValue);
}

