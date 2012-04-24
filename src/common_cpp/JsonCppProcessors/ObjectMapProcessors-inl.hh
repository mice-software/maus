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
        throw Squeal(Squeal::recoverable,
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
        } catch(Squeal squee) {
            delete my_map;
            throw squee;
        }
        (*my_map)[names[i]] = *cpp_out;
        delete cpp_out;
    }
    return my_map;
}

template <class MapSecond>
Json::Value* ObjectMapValueProcessor<MapSecond>::CppToJson
                             (const std::map<std::string, MapSecond>& cpp_map) {
    Json::Value* json_object = new Json::Value(Json::objectValue);
    // pull this out of the loop def because it is a syntactic mouthful...
    typename std::map<std::string, MapSecond>::const_iterator it;
    for (it = cpp_map.begin(); it != cpp_map.end(); ++it) {
        Json::Value* json_out;
        try {
            json_out = _proc->CppToJson(it->second);
        } catch(Squeal squee) {
            delete json_object;
            throw squee;
        }
        (*json_object)[it->first] = *json_out;
        delete json_out;
    }
    return json_object;
}
}
#endif

