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

#include "json/json.h"

#include "src/common_cpp/JsonCppProcessors/ProcessorBase.hh"

namespace MAUS {

/** @class PointerArrayProcessor
 *
 *  PointerArrayProcessor provides bindings for converting between a C++ vector
 *  of pointers and a json array (json's vector type). Note that this class is
 *  specifically for converting a vector of pointers.
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
  private:
    ProcessorBase<ArrayContents>* _proc;
};
}

#include "src/common_cpp/JsonCppProcessors/ArrayProcessors-inl.hh"

#endif

