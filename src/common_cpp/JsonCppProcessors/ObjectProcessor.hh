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

#include <vector>
#include <string>
#include <map>

#include "json/json.h"

#include "src/common_cpp/JsonCppProcessors/ProcessorBase.hh"
#include "src/common_cpp/JsonCppProcessors/TRefArrayProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/Common/ObjectProcessorNS/BaseItem.hh"

namespace MAUS {
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
 *  BaseItem has been overloaded with four specific ways to register a branch,
 *  \li RegisterPointerBranch for pointer data type where the ObjectType object
 *      owns the memory
 *  \li RegisterPointerReference for pointer data type where another C++ object
 *      in the tree owns the memory (borrowed reference)
 *  \li RegisterValueBranch for value data
 *  \li RegisterConstantBranch for static const data
 *
 *  @tparam ObjectType C++ type that will be converted
 */
template <class ObjectType>
class ObjectProcessor : public ProcessorBase<ObjectType> {
  public:
    /** Default constructor sets _throws_if_different_properties to true
     */
    ObjectProcessor();

    /** Convert from a Json object to a C++ instance
     *
     *  Iterate over each registered branch. If the branch is of pointer type,
     *  call the relevant processor and then Set the newly allocated memory
     *  using the registered mutator. If the branch is of value type, call the
     *  relevant processor, copy the data in using the mutator and then clean up
     *  memory allocation afterwards.
     *
     *  If a branch is required but cannot be found, throw a Exception.
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
     *  If a branch is required but has NULL value, throw a Exception.
     *
     *  @param cpp_instance C++ object to be converted to json
     *  @param path Path to the newly created json object
     *
     *  @returns json representation of the object. Caller is responsible for
     *  memory
     */
    Json::Value* CppToJson(const ObjectType& cpp_instance, std::string path);

    /** Register a branch for processing, where the data is stored in a pointer
     *  and the ParentType owns the memory pointed to. 
     *
     *  @tparam ChildType of the child object referenced by the branch. Should
     *  use the actual type even if the target is a pointer
     *
     *  @param branch_name name used by json to reference the branch
     *  @param child_processor processor that will be used to convert the
     *  representation of the child types
     *  @param GetMethod callback that will return a pointer to the child data,
     *  where memory is still owned by the ParentType
     *  @param SetMethod callback that will set a pointer to the child data,
     *  where memory ownership is given to the ParentType
     *  @param is_required if the branch doesnt exist in json or is NULL in C++,
     *  throw MAUS::Exceptions::Exception if is_required is set to true
     *
     *  Note: don't forget Get method has to be const
     */
    template <class ChildType>
    void RegisterPointerBranch(std::string branch_name,
                    ProcessorBase<ChildType>* child_processor,
                    ChildType* (ObjectType::*GetMethod)() const,
                    void (ObjectType::*SetMethod)(ChildType* value),
                    bool is_required);

    /** Register a branch reference for processing, where the data is stored in
     *  a pointer and another class in the tree owns the memory pointed to. 
     *
     *  For a PointerReference we only convert the address and assume that the
     *  value is passed elsewhere in the tree as a PointerBranch.
     *  
     *  Pointers are resolved after all other processing is complete. Memory is
     *  assumed to be owned by the ParentType of the RegisterPointerBranch, NOT
     *  the ParentType of this RegisterPointerReference.
     *
     *  @tparam ChildType of the child object referenced by the branch. Should
     *  use the actual type even if the target is a pointer (i.e. 
     *  MyProcessor<Child>, NOT MyProcess<Child*>
     *
     *  @param branch_name name used by json to reference the branch
     *  @param GetMethod callback that will return a pointer to the child data,
     *  where memory is still owned by the ParentType
     *  @param SetMethod callback that will set a pointer to the child data,
     *  where memory is owned by the PointerBranch parent
     *  @param is_required if the branch doesnt exist in json, is None in json,
     *  or is NULL in C++, throw MAUS::Exceptions::Exception if is_required is set to true; else set
     *  the branch to NULL/None as appropriate
     *item
     *  Note: don't forget Get method has to be const
     */
    template <class ChildType>
    void RegisterPointerReference(std::string branch_name,
                    ChildType* (ObjectType::*GetMethod)() const,
                    void (ObjectType::*SetMethod)(ChildType* value),
                    bool is_required);

    /** Register a ROOT TRef for processing, which contains a pointer
     *  to memory owned by another class in the tree.
     *
     *  We only convert the address and assume that the value is
     *  passed elsewhere in the tree as a PointerBranch.
     *  
     *  Pointers are resolved after all other processing is
     *  complete. To be stored in a TRef, a data member must inherit
     *  from TObject.  This removes the need to template this method,
     *  as all pointers can be resolved as TObjects.
     *
     *  @param branch_name name used by json to reference the branch
     *  @param GetMethod callback that will return a pointer to the child data,
     *  where memory is still owned by the ParentType
     *  @param SetMethod callback that will set a pointer to the child data,
     *  where memory is owned by the PointerBranch parent
     *  @param is_required if the branch doesnt exist in json, is None in json,
     *  or is NULL in C++, throw MAUS::Exceptions::Exception if is_required is set to true; else set
     *  the branch to NULL/None as appropriate
     *item
     *  Note: don't forget Get method has to be const
     */
    void RegisterTRef(std::string branch_name,
                      TObject* (ObjectType::*GetMethod)() const,
                      void (ObjectType::*SetMethod)(TObject* value),
                      bool is_required);

    /** Register a ROOT TRefArray for processing, which contains a
     *  pointer to memory owned by another class in the tree.
     *
     *  We only convert the address and assume that the values are
     *  passed elsewhere in the tree as a PointerBranch.
     *  
     *  References are resolved after all other processing is
     *  complete. Memory is assumed to be owned by the ParentType of
     *  the RegisterPointerBranch, NOT the ParentType of this
     *  RegisterTRefArray.  All objects referenced this way must
     *  inherit from TObject.  This is required by the associated ROOT
     *  code, but also helpful, as it removes the need for templating
     *  the ChildType.
     *
     *  @param branch_name name used by json to reference the branch
     *  @param GetMethod callback that will return a pointer to the TRefArray,
     *  which contains references to memory still owned by the ParentType
     *  @param SetMethod callback that will set a pointer to the TRefArray,
     *  where memory is owned by the PointerBranch parent
     *  @param is_required if the branch doesnt exist in json, is None in json,
     *  or is NULL in C++, throw MAUS::Exceptions::Exception if is_required is set to true; else set
     *  the branch to NULL/None as appropriate
     *item
     *  Note: don't forget Get method has to be const
     */
    void RegisterTRefArray(std::string branch_name,
                           TRefArrayProcessor* child_processor,
                           TRefArray* (ObjectType::*GetMethod)() const,
                           void (ObjectType::*SetMethod)(TRefArray* value),
                           bool is_required);

  /** Register a branch for processing where the data is stored by value.
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
     *  @param is_required if the branch doesnt exist in json, throw MAUS::Exceptions::Exception if
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

    /** Register a branch for processing where the data is the base class component of a derived class.
     *
     *  @tparam ChildType The base class, which must have it's own processor.
     *
     *  @param branch_name name used by json to reference the branch
     *  @param child_processor processor that will be used to convert the
     *  representation of the child types
     *  @param SetMethod callback that will set the value of the child data
     *  associated with this branch
     *  @param is_required if the branch doesnt exist in json, throw MAUS::Exceptions::Exception if
     *  is_required is set to true
     *
     *  Note: A Get method is not required, as the inherited class
     *        will be passed through to the relevant processor.
     */
    template <class ChildType>
    void RegisterBaseClass(std::string branch_name,
                           ProcessorBase<ChildType>* child_processor,
                           void (ChildType::*SetMethod)(ChildType value),
                           bool is_required);

    /** Register a branch for processing, where json value is always the same
     *
     *  @param branch_name name used by json to reference the branch
     *  @param child_value the branch will always be filled with this value
     *  @param is_required if the branch doesnt exist in json, throw MAUS::Exceptions::Exception if
     *  is_required is set to true
     *
     *  This method will never fill anything in the parent C++ class
     */
    void RegisterConstantBranch(std::string branch_name,
                    Json::Value child_value,
                    bool is_required);

    /** Register a branch for ignoring
     *
     *  @param branch_name name used by json to reference the branch
     *  @param is_required if the branch doesnt exist in json, throw MAUS::Exceptions::Exception if
     *  is_required is set to true
     *
     *  This method will never fill anything in the parent C++ class or the Json
     *  value; it merely registers that there is a branch which should be 
     *  ignored.
     */
    void RegisterIgnoredBranch(std::string branch_name,
                    bool is_required);

    /** Return true if json value properties not the same as branches
     *
     *  Compare the list of properties on the json value and compare with the
     *  list of registered branches. If they are different return false. Else
     *  return true.
     *
     *  @param value Json::Value to check against. Throw an exception if value
     *         is not an object type.
     */
    bool HasUnknownBranches(const Json::Value& value) const;

    /** Set _throws_if_different_properties
     */
    void SetThrowsIfUnknownBranches(bool will_throw);

    /** Get _throws_if_different_properties
     */
    bool GetThrowsIfUnknownBranches() const;

    /** Destructor frees memory allocated to items vector
     */
    virtual ~ObjectProcessor();

  protected:
    /** Set this to true to throw if json value has extra properties
     *
     *  When converting from json to cpp, if this is set to true the class will
     *  throw an exception if the json branch has properties that are not
     *  registered in the processor. Default is true.
     */
    bool _throws_if_unknown_branches;

    /** Set the path of the json object
     *
     *  Use the branch name to set the path of the json object
     */
    void SetPath();

  private:
    typedef typename std::map< std::string,
                   ObjectProcessorNS::BaseItem<ObjectType>* >::iterator my_iter;
    std::map< std::string, ObjectProcessorNS::BaseItem<ObjectType>* > _items;
};
} // namespace MAUS

#include "src/common_cpp/JsonCppProcessors/ObjectProcessor-inl.hh"

#endif

