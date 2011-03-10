/* Copyright Chris Rogers 2011
 *
 * This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
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

/** @class JsonWrapper
 *  \brief Wrap the Json function calls with additional error handling
 *
 *  Convenience wrapper for Json functionality. Use of Json is somewhat verbose
 *  and requires lots of error checking etc. I wrap the Json function calls here
 *  in order to reduce verbosity.
 *
 *  Nb jargon:
 *  * Json "object" is like a map<std::string, Json::Value>.
 *  * Json "array" is a dynamic length array
 *
 *  @authors Chris Rogers <chris.rogers@stfc.ac.uk>
 */

#ifndef INTERFACE_JSONWRAPPER_HH

#include <string>

#include "json/json.h"

#include "src/common/Interface/STLUtils.hh"
#include "src/common/Interface/Squeal.hh"

class JsonWrapper {
 public:
  /** \brief List of data types allowed by json
   *
   *  List of data types allowed by json, with additional type "anyValue" for
   *  when we don't care.
   *
   *  Note Json always assumes integers are signed
   */
  enum JsonType {
    nullValue,     // 'null' value
    uintValue,     // unsigned integer value;
    intValue,      // signed integer value
    realValue,     // double value
    stringValue,   // UTF-8 string value.
    booleanValue,  // bool value
    arrayValue,    // array value (ordered list)
    objectValue,   // object value (collection of name/value pairs).
    anyValue       // object value (collection of name/value pairs).
  };

  /** \brief Convert a string to a Json::Value tree 
   *
   *  \param json_in raw string holding the configuration information
   *
   *  Read in a string and return as a Json value. Throw a Squeal if the reader
   *  fails to parse the string. The configuration is a dict of Json::Value,
   *  which is in itself a Json::Value.
   */
  static Json::Value StringToJson(std::string json_in) throw(Squeal);

  /** \brief Get an item from a Json array (variable length array)
   *
   *  \param value_list array of values from which we want to get a value
   *  \param value_index index of the value we want to get
   *  \param value_type type of the value we want to get
   *
   *  Returns the Json::Value on success. Throws an exception of type Squeal on
   *  failure
   */
  static Json::Value GetItem(Json::Value array, size_t value_index,
                                            JsonType value_type) throw(Squeal);

  /** \brief Get a property from a Json object (hash)
   *
   *  \param parent the Json data tree
   *  \param branch_type enumeration holding the name of the branch you want
   *  \param max_depth maximum depth to search for the branch. Set to negative
   *                   to search the whole tree.
   *
   *  Attempt to access a branch from Json. If the branch is not found, throw a
   *  Squeal.
   */
  static Json::Value GetProperty
     (Json::Value object, std::string value_name,
                          JsonType value_type) throw(Squeal);


  /** \brief Convert from Json::ValueType to JsonType
   */
  static JsonType ValueTypeToJsonType(Json::ValueType tp);

  /** \brief Convert from JsonType to Json::ValueType
   */
  static Json::ValueType JsonTypeToValueType(JsonType tp) throw(Squeal);

  /** \brief Return true if types are equal or anyValue
   */
  static bool SimilarType(JsonType jt1, JsonType jt2);

 private:
  JsonWrapper();
  ~JsonWrapper();
  DISALLOW_COPY_AND_ASSIGN(JsonWrapper);
};

#endif


