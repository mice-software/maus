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
#include <vector>
#include <map>

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

std::string JsonWrapper::JsonToString(const Json::Value& val) {
    Json::FastWriter writer;
    std::string json_out = std::string(writer.write(val));
    return json_out.substr(0, json_out.size()-1);  // strip carriage return
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
    default:
      throw(Squeal(Squeal::recoverable,
                   "Json ValueType not recognised",
                   "JsonWrapper::ValueTypeToJsonType"));
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
    case anyValue: default:
      throw(Squeal(Squeal::recoverable,
                   "Could not convert anyValue to Json ValueType",
                   "JsonWrapper::JsonTypeToValueType"));
  }
  return Json::nullValue;  // appease gcc
}

CLHEP::Hep3Vector JsonWrapper::JsonToThreeVector
                                       (Json::Value j_vec) throw(Squeal) {
    if (!j_vec.isObject())
      throw(Squeal(Squeal::recoverable,
                  "Could not convert Json value of non-object type to three vector",
                  "JsonWrapper::JsonToThreeVector(...)"
            ));
    CLHEP::Hep3Vector c_vec;
    c_vec[0] = JsonWrapper::GetProperty(j_vec, "x", JsonWrapper::realValue).asDouble();
    c_vec[1] = JsonWrapper::GetProperty(j_vec, "y", JsonWrapper::realValue).asDouble();
    c_vec[2] = JsonWrapper::GetProperty(j_vec, "z", JsonWrapper::realValue).asDouble();
    return c_vec;
}

bool JsonWrapper::SimilarType(JsonWrapper::JsonType jt1,
                              JsonWrapper::JsonType jt2) {
    return (jt1 == jt2 || jt1 == JsonWrapper::anyValue
                       || jt2 == JsonWrapper::anyValue);
}

void JsonWrapper::Print(std::ostream& out, const Json::Value& val) {
    Json::FastWriter writer;
    out << writer.write(val);
}

bool JsonWrapper::AlmostEqual(Json::Value value_1, Json::Value value_2, double tolerance) {
    if (value_1.type() != value_2.type()) {
        return false;
    }
    switch (value_1.type()) {
        case Json::objectValue:
            return ObjectEqual(value_1, value_2, tolerance);
        case Json::arrayValue:
            return ArrayEqual(value_1, value_2, tolerance);
        case Json::realValue:
            return fabs(value_1.asDouble() - value_2.asDouble()) < tolerance;
        case Json::stringValue:
            return value_1.asString() == value_2.asString();
        case Json::uintValue:
            return value_1.asUInt() == value_2.asUInt();
        case Json::intValue:
            return value_1.asInt() == value_2.asInt();
        case Json::booleanValue:
            return value_1.asBool() == value_2.asBool();
        case Json::nullValue:
            return true;
        default:
            return false;
    }
}

bool JsonWrapper::ObjectEqual
                  (Json::Value value_1, Json::Value value_2, double tolerance) {
    // get keys, assure that ordering is same
    std::vector<std::string> keys_1 = value_1.getMemberNames();
    std::vector<std::string> keys_2 = value_2.getMemberNames();
    std::sort(keys_1.begin(), keys_1.end());
    std::sort(keys_2.begin(), keys_2.end());
    // check keys are same
    if (keys_1 != keys_2) {
        return false;
    }
    // check values are same
    for (size_t i = 0; i < keys_1.size(); ++i) {
        if (!AlmostEqual(value_1[keys_1[i]], value_2[keys_1[i]], tolerance)) {
            return false;
        }
    }
    // return true
    return true;
}

bool JsonWrapper::ArrayEqual
                  (Json::Value value_1, Json::Value value_2, double tolerance) {
    // check length is same
    if (value_1.size() != value_2.size()) {
        return false;
    }
    // check each item is the same (recursively)
    for (size_t i = 0; i < value_1.size(); ++i) {
        if (!AlmostEqual(value_1[i], value_2[i], tolerance)) {
            return false;
        }
    }
    return true;
}

Json::Value JsonWrapper::ObjectMerge
                                  (Json::Value object_1, Json::Value object_2) {
    // check we have objects
    if (object_1.type() != Json::objectValue ||
        object_2.type() != Json::objectValue) {
        throw(Squeal(Squeal::recoverable,
                     "Expecting object type for object merge",
                     "JsonWrapper::ObjectMerge"));
    }
    Json::Value object_merged(object_1);
    std::vector<std::string> names_1 = object_1.getMemberNames();
    std::vector<std::string> names_2 = object_2.getMemberNames();
    std::map<std::string, int> names_merged;

    // find out which key is in which object
    // int = 1 => in object 1, int = 2 => in object 2; int = 3 => in both
    for (size_t i = 0; i < names_1.size(); ++i) {
        names_merged[names_1[i]] = 1;
    }
    for (size_t i = 0; i < names_2.size(); ++i) {
        if (names_merged.find(names_2[i]) != names_merged.end()) {
          names_merged[names_2[i]] = 3;
        } else {
          names_merged[names_2[i]] = 2;
        }
    }
    // loop over all keys and pull from relevant object into the merge target
    typedef std::map<std::string, int>::iterator my_iter;
    for (my_iter it = names_merged.begin(); it != names_merged.end(); ++it) {
        if (it->second == 1) {
          // do nothing, merge target is already a copy of object_1 (faster?)
        } else if (it->second == 2) {
            object_merged[it->first] = object_2[it->first];
        } else {
            object_merged[it->first] = JsonWrapper::ArrayMerge
                                     (object_1[it->first], object_2[it->first]);
        }
    }

    return object_merged;
}

Json::Value JsonWrapper::ArrayMerge(Json::Value array_1, Json::Value array_2) {
    if (array_1.type() != Json::arrayValue ||
        array_2.type() != Json::arrayValue) {
        throw(Squeal(Squeal::recoverable,
                     "Expecting array type for array merge",
                     "JsonWrapper::ArrayMerge"));
    }
    Json::Value array_merge(array_1);
    for (size_t i = 0; i < array_2.size(); ++i) {
        array_merge.append(array_2[i]);
    }
    return array_merge;
}

std::string JsonWrapper::ValueTypeToString(Json::ValueType tp) {
  switch (tp) {
    case Json::nullValue: return "nullValue";
    case Json::intValue: return "intValue";
    case Json::uintValue: return "uintValue";
    case Json::realValue: return "realValue";
    case Json::stringValue: return "stringValue";
    case Json::booleanValue: return "booleanValue";
    case Json::arrayValue: return "arrayValue";
    case Json::objectValue: return "objectValue";
    default:
      throw(Squeal(Squeal::recoverable,
                   "Json ValueType not recognised",
                   "JsonWrapper::ValueTypeToJsonType"));
  }
  return "";
}

std::string JsonWrapper::Path::GetPath(Json::Value json) {
    if (!json.hasComment(Json::commentAfter)) {
        return "";
    }
    std::string comment = json.getComment(Json::commentAfter);
    Json::Value comment_json = StringToJson(comment.substr(3, comment.size()));
    return comment_json["path"].asString();
}

void JsonWrapper::Path::SetPath(Json::Value& json, std::string path) {
    Json::Value comment(Json::objectValue);
    comment["path"] = path;
    json.setComment("// "+JsonToString(comment), Json::commentAfter);
}

void JsonWrapper::Path::AppendPath(Json::Value& json, std::string branch_name) {
    if (branch_name.find("/") != std::string::npos)
        throw(Squeal(Squeal::recoverable,
                     "/ not allowed in branch names",
                     "JsonWrapper::AppendPath"));
    std::string old_path = GetPath(json);
    if (old_path == "") {
        SetPath(json, "#"+branch_name);
    } else {
        SetPath(json, old_path+"/"+branch_name);
    }    
}

void JsonWrapper::Path::AppendPath(Json::Value& json, size_t array_index) {
    AppendPath(json, STLUtils::ToString(array_index));
}

Json::Value& JsonWrapper::Path::DereferencePath
                                         (Json::Value& json, std::string path) {
    if (path[0] == '#')
        path = path.substr(1, path.size()); //lstrip #
    if (path[path.size()-1] == '/')
        path = path.substr(0, path.size()-1); //rstrip /
    if (path == "")
        return json;
    std::string this_path = path;
    std::string child_path = "";
    if (path.find("/") != std::string::npos) {
        this_path = path.substr(0, path.find('/'));
        child_path = path.substr(path.find('/')+1, path.size());
    }
    switch (json.type()) {
        case Json::arrayValue: {
            Json::Value::UInt index = STLUtils::FromString<size_t>(this_path);
            if (json.isValidIndex(index)) {
                return DereferencePath(json[index], child_path);
            }
            break;
        }
        case Json::objectValue: {
            if (json.isMember(this_path)) {
                return DereferencePath(json[this_path], child_path);
            }
            break;
        }
        default:
            break;
    }
    throw(Squeal(Squeal::recoverable,
                 "Could not dig through path "+path+
                 " from json value "+JsonToString(json),
                 "JsonWrapper::DereferencePath"));
}

void JsonWrapper::Path::SetPathRecursive(Json::Value& json,
                                         std::string root_path) {
  SetPath(json, root_path);
  _SetPathRecursive(json);
}

void JsonWrapper::Path::_SetPathRecursive(Json::Value& tree) {
  std::string path = GetPath(tree);
  switch (tree.type()) {
      case Json::arrayValue: {
          for (size_t i = 0; i < tree.size(); ++i) {
              SetPath(tree[i], path);
              AppendPath(tree[i], i);
              _SetPathRecursive(tree[i]);
          }
          break;
      }
      case Json::objectValue: {
          std::vector<std::string> properties = tree.getMemberNames();
          for (size_t i = 0; i < properties.size(); ++i) {
              SetPath(tree[properties[i]], path);
              AppendPath(tree[properties[i]], properties[i]);
              _SetPathRecursive(tree[properties[i]]);
          }
          break;
      }
      default: {
          break;
      }
  }
}

void JsonWrapper::Path::StripPathRecursive(Json::Value& tree) {
      tree.setComment("", Json::commentAfter);
      for (Json::Value::iterator it = tree.begin(); it != tree.end(); ++it) {
          StripPathRecursive(*it);
      }
}


