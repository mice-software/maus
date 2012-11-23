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

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_ARRAYPROCESSOR_HH_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_ARRAYPROCESSOR_HH_

#include <vector>
#include <string>

#include "json/json.h"

#include "src/common_cpp/JsonCppProcessors/ProcessorBase.hh"

namespace MAUS {

/** @class PointerArrayProcessor
 *
 *  PointerArrayProcessor provides bindings for converting between a C++ vector
 *  of pointers and a json array (json's vector type). Note that this class is
 *  specifically for converting a vector of pointers where the std::vector owns
 *  the memory allocated to the pointers. New memory will be allocated for each
 *  element in the array.
 */
template <class ArrayContents>
class PointerArrayProcessor : public ProcessorBase<std::vector<ArrayContents*> > {
  public:
    /** @Constructor
     *
     *  @param contents_processor is a pointer that will be used to process the 
     *  target contents. PointerArrayProcessor takes ownership of the memory
     *  allocated to contents_processor.
     */
    explicit PointerArrayProcessor
                             (ProcessorBase<ArrayContents>* contents_processor);

    /** @Destructor deletes memory allocted to _proc
     */
    ~PointerArrayProcessor();

    /** Convert from a json array type to a C++ std::vector type.
     *
     *  @param json_array Json arrayValue
     *
     *  @returns std::vector C++ representation of the data. As with all
     *  processors, caller has ownership of this memory. NULL values in the json
     *  representation always give NULL values in the cpp representation.
     */
    std::vector<ArrayContents*>* JsonToCpp(const Json::Value& json_array);

    /** Convert from a C++ vector type to a json array type.
     *
     *  @param cpp_array C++ vector
     *
     *  @returns Json::Value arrayValue with vector contents in Json
     *  representation. As with all processors, caller has ownership of this
     *  memory. NULL values in the cpp representation always give NULL values in
     *  the json representation.
     */
    Json::Value* CppToJson(const std::vector<ArrayContents*>& cpp_array);


    /** Convert from a C++ vector type to a json array type.
     *
     *  @param cpp_array C++ vector
     *  @param path path to this element
     *
     *  @returns Json::Value arrayValue with vector contents in Json
     *  representation. As with all processors, caller has ownership of this
     *  memory. NULL values in the cpp representation always give NULL values in
     *  the json representation.
     */
    Json::Value* CppToJson(const std::vector<ArrayContents*>& cpp_array,
                           std::string path);

    /** Return string for ith array item given array path 
     *
     *  @param path json path to the array
     *  @param index position of the element in the array
     *
     *  @returns string like path+"/"+index
     */
    std::string GetPath(std::string path, size_t index);

  private:
    ProcessorBase<ArrayContents>* _proc;
};

/** @class ValueArrayProcessor
 *
 *  ValueArrayProcessor provides bindings for converting between a C++ vector
 *  of values and a json array (json's vector type). Note that this class is
 *  specifically for converting a vector of values.
 */
template <class ArrayContents>
class ValueArrayProcessor : public ProcessorBase<std::vector<ArrayContents> > {
  public:
    /** @Constructor
     *
     *  @param contents_processor is a pointer that will be used to process the 
     *  target contents. PointerArrayProcessor takes ownership of the memory
     *  allocated to contents_processor.
     */
    explicit ValueArrayProcessor
                         (ProcessorBase<ArrayContents>* ArrayContentsProcessor);

    /** @Destructor deletes memory allocated to _proc
     */
    ~ValueArrayProcessor();

    /** Convert from a json array type to a C++ std::vector type.
     *
     *  @param json_array Json arrayValue
     *
     *  @returns std::vector C++ representation of the data. As with all
     *  processors, caller has ownership of this memory. Note json null values
     *  usually result in an exception, depending on the ArrayContentsProcessor.
     */
    std::vector<ArrayContents>* JsonToCpp(const Json::Value& json_array);

    /** Convert from a C++ vector type to a json array type.
     *
     *  @param cpp_array C++ vector
     *
     *  @returns Json::Value arrayValue with vector contents in Json
     *  representation. As with all processors, caller has ownership of this
     *  memory.
     */
    Json::Value* CppToJson(const std::vector<ArrayContents>& cpp_array);

    /** Convert from a C++ vector type to a json array type.
     *
     *  @param cpp_array C++ vector
     *  @param path path to this element
     *
     *  @returns Json::Value arrayValue with vector contents in Json
     *  representation. As with all processors, caller has ownership of this
     *  memory.
     */
    Json::Value* CppToJson(const std::vector<ArrayContents>& cpp_array,
                           std::string path);

    /** Return string for ith array item given array path 
     *
     *  @param path json path to the array
     *  @param index position of the element in the array
     *
     *  @returns string like path+"/"+index
     */
    std::string GetPath(std::string path, size_t index);

  private:
    ProcessorBase<ArrayContents>* _proc;
};

/** @class ReferenceArrayProcessor
 *
 *  Note that ReferenceArrayProcessor should only be used on an array that is
 *  stored in C++ as a pointer-by-value. Otherwise you will get a
 *  segmentation fault (ack).
 *
 *  ReferenceArrayProcessor provides bindings for converting between a C++
 *  vector of pointers and a json array (json's vector type). Note that here the
 *  pointers are distinct from PointerArrayProcessor because the std::vector
 *  should not own the memory - no memory will be allocated or deleted, and some
 *  other data type (either another class or another array) should own the
 *  memory.
 *
 *  In json we store a reference to the data; in C++ we store a pointer. Needs
 *  the vector memory address to be persistent as this is how we reference the
 *  vector, hence need a pointer-by-value
 */
template <class ArrayContents>
class ReferenceArrayProcessor
                          : public ProcessorBase<std::vector<ArrayContents*> > {
  public:
    /** @Constructor
     *
     *  Unlike other processors, no need for the target processor (as we are
     *  just handing pointers around).
     */
    ReferenceArrayProcessor() {}

    /** @Destructor does nothing
     */
    ~ReferenceArrayProcessor() {}

    /** Convert from a json array type to a C++ std::vector type.
     *
     *  @param json_array Json arrayValue
     *
     *  @returns std::vector C++ representation of the data. As with all
     *  processors, caller has ownership of this memory. Note json null values
     *  usually result in an exception, depending on the ArrayContentsProcessor.
     */
    std::vector<ArrayContents*>* JsonToCpp(const Json::Value& json_array);

    /** Convert from a C++ vector type to a json array type.
     *
     *  @param cpp_array C++ vector
     *  @param path path to this array
     *
     *  @returns Json::Value arrayValue with vector contents in Json
     *  representation. As with all processors, caller has ownership of this
     *  memory.
     */
    Json::Value* CppToJson(const std::vector<ArrayContents*>& cpp_array,
                           std::string path);

    /** Return string for ith array item given array path 
     *
     *  @param path json path to the array
     *  @param index position of the element in the array
     *
     *  @returns string like path+"/"+index
     */
    std::string GetPath(std::string path, size_t index);

  private:
};
}

#include "src/common_cpp/JsonCppProcessors/ArrayProcessors-inl.hh"

#endif

