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

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_OBJECTPROCESSOR_HH_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_OBJECTPROCESSOR_HH_

#include "json/json.h"

#include "src/common_cpp/JsonCppProcessors/ProcessorBase.hh"

namespace MAUS {

template <class ObjectType>
class BaseItem; // defined in ObjectProcessor-inl

template <class ObjectType, class ChildType>
class PointerItem; // defined in ObjectProcessor-inl

template <class ObjectType, class ChildType>
class ValueItem; // defined in ObjectProcessor-inl

/** @class ObjectProcessor processes json object types into C++ classes
 *
 *  The object processor converts from a C++ class to a json value and vice 
 *  versa. Member data can be stored as items of the json object. Members are
 *  registered using the RegisterPointerBranch and RegisterValueBranch methods
 *  and these are accessed by the JsonToCpp and CppToJson conversion methods.
 *  The relevant Processor, together with C++ accessors and mutators have to be
 *  stored along with the Branch.
 *
 *  This works by storing the registered branches in a std::vector of BaseItems.
 *  The BaseItem defines an interface for allocating and accessing member data
 *  for the branch based on the registered processor, accessor and mutator.
 *  BaseItem has been overloaded with two specific ways to register a branch,
 *  as a pointer data type and as a value data type.
 *
 *  @tparam ObjectType C++ type that will be converted
 */
template <class ObjectType>
class ObjectProcessor : public ProcessorBase<ObjectType> {
  public:
    /** Convert from a Json object to a C++ instance
     *
     *  Iterate over each registered branch. If the branch is of pointer type,
     *  call the relevant processor and then Set the newly allocated memory
     *  using the registered mutator. If the branch is of value type, call the
     *  relevant processor, copy the data in using the mutator and then clean up
     *  memory allocation afterwards.
     *
     *  If a branch is required but cannot be found, throw a Squeal.
     *
     *  @param json_object object to be converted to C++
     *
     *  @returns C++ representation of the object, caller is responsible for
     *  memory
     */
    ObjectType* JsonToCpp(const Json::Value& json_object);

    /** Convert from a C++ instance to the json object
     *
     *  Iterate over each registered branch, adding items to the json object.
     *  Returned item is newly allocated memory (caller's responsibility).
     *
     *  If a branch is required but has NULL value, throw a Squeal.
     *  
     *  @param C++ object to be converted to json
     *
     *  @returns json representation of the object. Caller is responsible for
     *  memory
     */
    Json::Value* CppToJson(const ObjectType& cpp_instance);

    /** Register a branch for processing
     *
     *  @tparam ChildType of the child object referenced by the branch. Should
     *  use the actual type even if the target is a pointer
     *
     *  @param branch_name name used by json to reference the branch 
     *  @param child_processor processor that will be used to convert the
     *  representation of the child types
     *  @param GetMethod callback that will return a pointer to the child data,
     *  where memory is still owned by the ObjectProcessor
     *  @param SetMethod callback that will set a pointer to the child data,
     *  where memory is given to the ObjectProcessor
     *  @param is_required if the branch doesnt exist in json or is NULL in C++,
     *  throw Squeal if is_required is set to true
     *
     *  Note: don't forget Get method has to be const
     */
    template <class ChildType>
    void RegisterPointerBranch(std::string branch_name,
                    ProcessorBase<ChildType>* child_processor,
                    ChildType* (ObjectType::*GetMethod)() const,
                    void (ObjectType::*SetMethod)(ChildType* value),
                    bool is_required);

    /** Register a branch for processing
     *
     *  @tparam ChildType of the child object referenced by the branch. Should
     *  use the actual type even if the target is a pointer
     *
     *  @param branch_name name used by json to reference the branch 
     *  @param child_processor processor that will be used to convert the
     *  representation of the child types
     *  @param GetMethod callback that will return the value of the child data
     *  associated with this branch
     *  @param SetMethod callback that will set the value of the child data
     *  associated with this branch
     *  @param is_required if the branch doesnt exist in json, throw Squeal if
     *  is_required is set to true
     *
     *  Note: don't forget Get method has to be const
     */
    template <class ChildType>
    void RegisterValueBranch(std::string branch_name,
                    ProcessorBase<ChildType>* child_processor,
                    ChildType (ObjectType::*GetMethod)() const,
                    void (ObjectType::*SetMethod)(ChildType value),
                    bool is_required);
    
    /** Destructor frees memory allocated to items vector
     */
    virtual ~ObjectProcessor();

  private:
    std::vector< BaseItem<ObjectType>* > items;
};

}

#include "src/common_cpp/JsonCppProcessors/ObjectProcessor-inl.hh"

#endif

