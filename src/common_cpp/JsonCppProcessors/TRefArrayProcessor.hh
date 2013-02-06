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

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_TREFARRAYPROCESSOR_HH_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_TREFARRAYPROCESSOR_HH_

#include <vector>
#include <string>

#include <TRefArray.h>

#include "json/json.h"

namespace MAUS {

/** @class TRefArrayProcessor
 *
 *  Note that TRefArrayProcessor can only be used on an array that is
 *  stored in C++ as a pointer-by-value, so memory ownership is clear.
 *
 *  TRefArrayProcessor provides bindings for converting between a C++
 *  TRefArray, containing a set of C++ pointers (to objects inherited
 *  from TObject) and a json array (json's vector type). Note that
 *  here the pointers are distinct from PointerArrayProcessor because
 *  the TRefArray can not own the memory - no memory will be allocated
 *  or deleted, and some other data type (either another class or
 *  another array) will own the memory.
 *
 *  N.B. Doesn't inherit from ProcessorBase, as it rather rigidly
 *  defines the structure of JsonToCpp and CppToJson...
 */

class TRefArrayProcessor {
 public:
  /** @Constructor
   *
   *  Unlike other array processors, no need for the target processor
   *  (as we are just handing pointers around).  Do need to identify
   *  the object inside the TRef, beyond the template parameter.
   *
   */
  TRefArrayProcessor() {}

  /** @Destructor does nothing
   */
  ~TRefArrayProcessor() {}

  /** Convert from a json array type to a TRefArray type.
   *
   *  @param json_array Json arrayValue
   *
   *  @returns TRefArray, containing pointers to C++ objects, which
   *  must inherit from TObject, owned elsewhere.
   */
  TRefArray* JsonToCpp(const Json::Value& json_array);

  /** Convert from a C++ TRefArray to a json array type.
   *
   *  @param cpp_trefarray TRefArray of TObjects
   *  @param path path to this array
   *
   *  @returns Json::Value arrayValue with vector contents in Json
   *  representation. As with all processors, caller has ownership of this
   *  memory.
   */
  Json::Value* CppToJson(const TRefArray* cpp_trefarray,
                         std::string path);

  // /** Return string for ith array item given array path 
  //  *
  //  *  @param path json path to the array
  //  *  @param index position of the element in the array
  //  *
  //  *  @returns string like path+"/"+index
  //  */
  // std::string GetPath(std::string path, size_t index);

 private:
};
}

#include "src/common_cpp/JsonCppProcessors/TRefArrayProcessor-inl.hh"

#endif
