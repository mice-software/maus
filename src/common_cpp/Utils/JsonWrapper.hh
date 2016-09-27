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
 */

#ifndef _SRC_CPP_CORE_UTILS_JSONWRAPPER_HH
#define _SRC_CPP_CORE_UTILS_JSONWRAPPER_HH

#include <ostream>
#include <string>

#include "json/json.h"

#include "CLHEP/Vector/ThreeVector.h"

#include "src/legacy/Interface/STLUtils.hh"
#include "Utils/Exception.hh"

namespace JsonWrapper {
  /** @brief List of data types allowed by json
   *
   *  List of data types allowed by json, with additional type "anyValue" for
   *  when we don't care.
   *
   *  Note Json always assumes integers are signed
   */
  enum JsonType {
    nullValue,     // 'null' value
    uintValue,     // unsigned integer value
    intValue,      // signed integer value
    realValue,     // double value
    stringValue,   // UTF-8 string value
    booleanValue,  // bool value
    arrayValue,    // array value (ordered list)
    objectValue,   // object value (collection of name/value pairs)
    anyValue       // object value (collection of name/value pairs)
  };

  /** @brief Convert a string to a Json::Value tree 
   *
   *  @param json_in raw string holding the configuration information
   *
   *  Read in a string and return as a Json value. Throw a Exception if the reader
   *  fails to parse the string. The configuration is a dict of Json::Value,
   *  which is in itself a Json::Value.
   */
  Json::Value StringToJson(const std::string& json_in) throw(MAUS::Exceptions::Exception);

  /** @brief Convert a Json::Value tree to a std::string
   *
   *  @param json_in Json::Value to convert to a string
   */
  std::string JsonToString(const Json::Value& val);

  /** @brief Get an item from a Json array (variable length array)
   *
   *  @param array array of values from which we want to get a value
   *  @param value_index index of the value in the array
   *  @param value_type type of the value we want to get
   *
   *  Returns the Json::Value on success. Throws an exception of type Exception on
   *  failure
   */
  Json::Value GetItem(const Json::Value& array,
                      const size_t value_index,
                      const JsonType value_type) throw(MAUS::Exceptions::Exception);

  /** @brief Get a property from a Json object (hash)
   *
   *  @param object the Json data tree
   *  @param value_name name of the branch we want to get
   *  @param value_type type of the value we want to get
   *
   *  Attempt to access a branch from Json. If the branch is not found, throw a
   *  Exception.
   */
  Json::Value GetProperty(const Json::Value& object,
                          const std::string& value_name,
                          const JsonType value_type) throw(MAUS::Exceptions::Exception);


  /** @brief Convert from a json three vector to a CLHEP three vector
   *
   *  @param json_vec objectValue with realValue children "x", "y", "z". Throws
   *         an exception if the conversion fails.
   */
  CLHEP::Hep3Vector JsonToThreeVector(const Json::Value& json_vec)
      throw(MAUS::Exceptions::Exception);

  /** @brief Convert from Json::ValueType to JsonType
   */
  JsonType ValueTypeToJsonType(const Json::ValueType tp);

  /** @brief Convert from Json::Value type to string
   */
  std::string ValueTypeToString(const Json::ValueType tp);

  /** @brief Convert from JsonType to Json::ValueType
   */
  Json::ValueType JsonTypeToValueType(const JsonType tp) throw(MAUS::Exceptions::Exception);

  /** @brief Return true if cast_target can be cast to cast_type in a safe way
   *
   *  Return true if the target can be cast to cast_type without loss of data,
   *  i.e. int, uint can be cast to float; uint can be cast to int; anything can
   *  be cast to anyValue and we allow anyValue to be cast back to anything.
   */
  bool SimilarType(const JsonType cast_target, const JsonType cast_type);

  /** @brief Return true if types are numeric (integer or float)
   */
  bool IsNumeric(const JsonType jt);


  /** @brief Print the Json value to an ostream
   *
   *  Prints in json format to ostream out; so if passed to a stringstream, then
   *  StringToJson should read back in with no change
   */
  void Print(std::ostream& out, const Json::Value& val);

  /** @brief Check for equality between json values
   *
   *  Check that value_1 == value_2; when comparing reals, we allow a float
   *  tolerance given by tolerance. Recursively search through arrays and
   *  objects.
   *
   *  @param tolerance float tolerance - requirement is that
   *         fabs(float_1-float_2) < tolerance
   *  @param int_permissive return true even if a is an int and b is a
   *         uint (recursively)
   */
  bool AlmostEqual(const Json::Value& value_1,
                   const Json::Value& value_2,
                   const double tolerance,
                   bool int_permissive = false);

  /** @brief Check for equality between json arrays
   *
   *  Check that value_1 == value_2, within float tolerance. Note that there is
   *  no type checking done here.
   *
   *  @param tolerance float tolerance - requirement is that
   *         fabs(float_1-float_2) < tolerance
   *  @param int_permissive return true even if a is an int and b is a
   *         uint (recursively)
   */
  bool ArrayEqual(const Json::Value& value_1,
                  const Json::Value& value_2,
                  const double tolerance,
                  bool int_permissive = false);

  /** @brief Check for equality between json objects
   *
   *  Check that value_1 == value_2, within float tolerance. Note that there is
   *  no type checking done here.
   *
   *  @param tolerance float tolerance - requirement is that
   *         fabs(float_1-float_2) < tolerance
   *  @param int_permissive return true even if a is an int and b is a
   *         uint (recursively)
   */
  bool ObjectEqual(const Json::Value& value_1,
                   const Json::Value& value_2,
                   const double tolerance,
                   bool int_permissive = false);


  /** @brief Merge two json objects
   *
   *  Merge two json objects. For each property in the object; if the property
   *  exists in object_1 or object_2 and is an array, append items from object_2
   *  array onto the back of the object_1 array; if the property exists in one
   *  object but not the other put it in the merged object; if the property
   *  exists in both objects but is not an array, throw an exception.
   */
  Json::Value ObjectMerge(const Json::Value& object_1,
                          const Json::Value& object_2);

  /** @brief Merge two json arrays
   *
   *  Put items from array_2 onto the back of array_1
   */
  Json::Value ArrayMerge(const Json::Value& array_1,
                         const Json::Value& array_2);

namespace Path {
  // Nb: Path could be a class? But then we lose "using" functionality; OTOH we
  // can make things properly private...

  /** @brief Get the path from a json value
   *
   *  Path is stored in the comment field
   */
  std::string GetPath(const Json::Value& json);

  /** @brief Set the path to a json value
   *
   *  Path is stored in the comment field
   */
  void SetPath(Json::Value& json, const std::string& path);

  /** @brief Append the path to a json value
   *
   *  Path is stored in the comment field
   *  \param json Json::Value to which the path should be appended
   *  \param branch_name name of a branch in a Json object
   */
  void AppendPath(Json::Value& json, const std::string& branch_name);

  /** @brief Append the path to a json value
   *
   *  Path is stored in the comment field
   *  \param json Json::Value to which the path should be appended
   *  \param array_index index of a branch in a Json array
   */
  void AppendPath(Json::Value& json, const size_t array_index);

  /** @brief Return the json value corresponding to a given path
   *
   *  Return the json value located at a given path. If the path cannot be
   *  accessed, throw a Exception.
   */
  Json::Value& DereferencePath(Json::Value& json, const std::string& path);

  /** @brief walk the Json structure setting path on all data
   *
   *  @param value json tree that will have path set
   *  @param root_path path from root; all subsequent paths will be appended to
   *         this one (set to "" to start from root)
   */
  void SetPathRecursive(Json::Value& json, const std::string& root_path);

  /** @brief walk the Json structure stripping comments from all data
   *
   *  Metadata on json objects is stored in path information. This function
   *  walks the data structure and strips out those comments.
   */
  void StripPathRecursive(Json::Value& json);

  /** Private - do not call */
  void _SetPathRecursive(Json::Value& json);
}  // namespace Path
}  // namespace JsonWrapper

#endif


