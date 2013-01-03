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

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_OBJECTMAPPROCESSORS_HH_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_OBJECTMAPPROCESSORS_HH_

#include <map>
#include <string>

#include "json/json.h"

#include "src/common_cpp/JsonCppProcessors/ProcessorBase.hh"

namespace MAUS {

/** @class ObjectMapValueProcessor converts a json object to a C++ std::map
 *
 *  @class ObjectMapValueProcessor has routines to convert a json object to a
 *  C++ std::map<std::string, some_type>. Json::object maps from a string, so
 *  the first value always has to be a string, but the second type is user
 *  defined.
 *
 *  @template MapSecond the second value in the C++ std::map
 *
 *  Note the significance of the name - MapSecond must be a value (not a pointer
 *  or reference), otherwise it probably gets memory allocation etc screwed up.
 */
template <class MapSecond>
class ObjectMapValueProcessor
                    : public ProcessorBase<std::map<std::string, MapSecond> > {
  public:
    /** Construct the processor
     *
     *  @param second_processor the processor object that will be used for
     *  processing the json values into a C++ objects. ObjectMapValueProcessor
     *  will call second_processor->CppToJson and second_processor->JsonToCpp on
     *  each item to generate the map or json object. ObjectMapValueProcessor
     *  takes ownership of the memory pointed to by second_processor.
     */
    explicit ObjectMapValueProcessor
                             (ProcessorBase<MapSecond>* second_processor);

    /** Deletes the second_processor */
    ~ObjectMapValueProcessor();

    /** JsonToCpp converts the json_object to a std::map
     *
     *  @param json_object the json object to be converted; calls
     *  second_processor->JsonToCpp on each of the properties and allocates the
     *  resultant C++ object to the std::map. 
     *
     *  @return Returns the C++ representation of json_object. Caller owns the
     *  memory pointed to by the return value.
     */
    std::map<std::string, MapSecond>* JsonToCpp(const Json::Value& json_object);

    /** CppToJson converts the std::map to a json_object
     *
     *  @param cpp_map the json object to be converted; calls
     *  second_processor->JsonToCpp on each of the properties and allocates the
     *  resultant C++ object to the std::map.
     * 
     *  @return Returns the json representation of cpp_map. Caller owns the
     *  memory pointed to by the return value.
     */
    Json::Value* CppToJson(const std::map<std::string, MapSecond>& cpp_map);

    /** Convert the std::map to a json_object passing path information
     *
     *  @param cpp_map the json object to be converted; calls
     *  second_processor->JsonToCpp on each of the properties and allocates the
     *  resultant C++ object to the std::map.
     *  @param path string containing path information
     * 
     *  @return Returns the json representation of cpp_map. Caller owns the
     *  memory pointed to by the return value.
     */
    Json::Value* CppToJson(const std::map<std::string, MapSecond>& cpp_map,
                           std::string path);

    /** Get a path object for child branches of the std::map
     *
     *  @param path Path through the Json tree to the parent object
     *         (json representation of the C++ map)
     *  @param branch name of the branch
     *
     *  @returns path to the branch i.e. path+"/"+branch
     */
    std::string GetPath(std::string path, std::string branch);

  private:
    ProcessorBase<MapSecond>* _proc;
};


// class ObjectMapPointerProcessor NOT IMPLEMENTED
}

#include "src/common_cpp/JsonCppProcessors/ObjectMapProcessors-inl.hh"

#endif

