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

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_OBJECTMAPPROCESSORS_INL_HH_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_OBJECTMAPPROCESSORS_INL_HH_

#include <map>
#include <string>
#include <vector>

#include "src/common_cpp/Utils/JsonWrapper.hh"

namespace MAUS {

template <class MapSecond>
ObjectMapValueProcessor<MapSecond>::ObjectMapValueProcessor
        (ProcessorBase<MapSecond>* second_processor) : _proc(second_processor) {
}

template <class MapSecond>
ObjectMapValueProcessor<MapSecond>::~ObjectMapValueProcessor() {
    delete _proc;
}

template <class MapSecond>
std::map<std::string, MapSecond>* ObjectMapValueProcessor<MapSecond>::JsonToCpp
                                              (const Json::Value& json_object) {
    if (json_object.type() != Json::objectValue &&
        json_object.type() != Json::nullValue) {
        throw MAUS::Exceptions::Exception(Exceptions::recoverable,
                    "Can only convert json objects to maps",
                    "ObjectMapValueProcessor::JsonToCpp");
    }
    std::map<std::string, MapSecond>* my_map =
                                         new std::map<std::string, MapSecond>();
    std::vector<std::string> names = json_object.getMemberNames();
    for (size_t i = 0; i < names.size(); ++i) {
        MapSecond* cpp_out;
        try {
            cpp_out = _proc->JsonToCpp(json_object[names[i]]);
        } catch (Exceptions::Exception exc) {
            delete my_map;
            throw exc;
        }
        (*my_map)[names[i]] = *cpp_out;
        delete cpp_out;
    }
    return my_map;
}

template <class MapSecond>
Json::Value* ObjectMapValueProcessor<MapSecond>::CppToJson
                             (const std::map<std::string, MapSecond>& cpp_map) {
    return CppToJson(cpp_map, "");
}

template <class MapSecond>
Json::Value* ObjectMapValueProcessor<MapSecond>::CppToJson
                             (const std::map<std::string, MapSecond>& cpp_map,
                              std::string path) {
    Json::Value* json_object = new Json::Value(Json::objectValue);
    JsonWrapper::Path::SetPath(*json_object, path);
    // pull this out of the loop def because it is a syntactic mouthful...
    typename std::map<std::string, MapSecond>::const_iterator it;
    for (it = cpp_map.begin(); it != cpp_map.end(); ++it) {
        Json::Value* json_out;
        std::string child_path = GetPath(path, it->first);
        try {
            json_out = _proc->CppToJson(it->second, child_path);
        } catch (Exceptions::Exception exc) {
            delete json_object;
            throw exc;
        }
        (*json_object)[it->first] = *json_out;
        JsonWrapper::Path::SetPath((*json_object)[it->first], child_path);
        delete json_out;
    }
    return json_object;
}
template <class MapSecond>
std::string ObjectMapValueProcessor<MapSecond>::
                                 GetPath(std::string path, std::string branch) {
    return path+"/"+branch;
}
}
#endif

