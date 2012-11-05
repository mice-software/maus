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

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_OBJECTPROCESSORBRANCHITEMS_HH_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_OBJECTPROCESSORBRANCHITEMS_HH_

#include "src/common_cpp/JsonCppProcessors/ProcessorBase.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/JsonCppProcessors/Common/ReferenceResolver.hh"

namespace MAUS {
// namespace ObjectProcessorNS {

/** BaseItem is base representation of a particular data structure branch
 *
 *  The processor holds information on each of the branches for a particular C++
 *  type in a vector. Different sorts of branches (is it stored as a pointer? is
 *  it stored by a value) share this interface so they can be stored in the same
 *  vector.
 *
 *  @tparam ParentType C++ parent object on which branches are to be registered
 */
template <class ParentType>
class BaseItem {
  public:
    /** Destructor
     */
    virtual ~BaseItem() {}

    /** SetCppChild set the child data on the C++ representation
     */
    virtual void SetCppChild
                  (const Json::Value& parent_json, ParentType& parent_cpp);

    /** SetCppChild set the child data on the json representation
     *
     *  Uses the json comment field to store metadata (e.g. path)
     */
    virtual void SetJsonChild
                  (const ParentType& parent_cpp, Json::Value& parent_json);

    /** Get the branch name */
    virtual std::string GetBranchName() const = 0;

  protected:
    /** Called by SetCppChild - overload here */
    virtual void _SetCppChild
                  (const Json::Value& parent_json, ParentType& parent_cpp) = 0;

    /** Called by SetJsonChild - overload here */
    virtual void _SetJsonChild
                  (const ParentType& parent_cpp, Json::Value& parent_json) = 0;

    /** Get the path to the child branch based on parent path and branch name */
    std::string GetPath(Json::Value& parent);

    /** Set the path to the child branch based on parent path and branch name */
    void SetPath(Json::Value& parent);
};

/** @class PointerRefItem pointer reference branch for a Cpp object
 *
 *  Probably don't want to use this class, instead use Register...Branch methods
 *  directly on ObjectProcessor
 *
 *  @tparam ParentType type of the parent object
 *  @tparam ChildType type of the child object referenced by the branch. Should
 *  use the actual type even if the target is a pointer
 *
 *  References the location of a PointerValueItem type branch.
 */
template <class ParentType, class ChildType>
class PointerRefItem : public BaseItem<ParentType> {
  public:
    typedef void (ParentType::*SetMethod)(ChildType* value);
    typedef ChildType* (ParentType::*GetMethod)() const;

    /** Constructor
     *
     *  @param branch_name name used by json to reference the branch
     *  @param child_processor processor that will be used to convert the
     *  representation of the child types
     *  @param GetMethod callback that will return a pointer to the child data,
     *  where memory is still owned by the ObjectProcessor
     *  @param SetMethod callback that will set a pointer to the child data,
     *  where memory is given to the ObjectProcessor
     *  @param is_required if the branch doesnt exist in json, is null in json
     *  or is NULL in C++, throw Squeal if is_required is set to true when
     *  Set...Child methods are called
     */
    PointerRefItem(std::string branch_name,
                   ProcessorBase<ChildType>* child_processor,
                   GetMethod getter,
                   SetMethod setter,
                   bool is_required);

    /** SetCppChild using data from parent_json
     *
     *  @param parent_json json object whose child branch is used to make C++
     *  representation
     *  @param parent_cpp C++ object where C++ data is put
     *
     *  Performs some error checking then adds a reference to the RefManager. If
     *  RefManager has not been birthed then does nothing. Actual pointer
     *  allocation is done after the entire tree has been parsed by
     *  RefManager::SetReferences()
     */
    void _SetCppChild(const Json::Value& parent_json, ParentType& parent_cpp);

    /** SetJsonChild fills the branch with a Null json value and stores the
     *  branch location and pointer in the reference manager for filling later.
     *
     *  @param parent_cpp C++ object from whence C++ data is got
     *  @param parent_json json object whose child branch is set using C++
     *  representation
     *
     *  Performs some error checking then adds a reference to the RefManager. If
     *  RefManager has not been birthed then does nothing. Actual pointer
     *  allocation is done after the entire tree has been parsed by
     *  RefManager::SetReferences()
     */
    void _SetJsonChild(const ParentType& parent_cpp, Json::Value& parent_json);

    /** Get the branch name */
    std::string GetBranchName() const {return _branch;}

  private:
    std::string _branch;
    ProcessorBase<ChildType>* _processor;
    SetMethod _setter;
    GetMethod _getter;
    bool      _required;
};

/** @class PointerItem pointer branch for a Cpp object
 *
 *  Probably don't want to use this class, instead use Register...Branch methods
 *  directly on ObjectProcessor
 *
 *  @tparam ParentType type of the parent object
 *  @tparam ChildType type of the child object referenced by the branch. Should
 *  use the actual type even if the target is a pointer
 */
template <class ParentType, class ChildType>
class PointerItem : public BaseItem<ParentType> {
  public:
    typedef void (ParentType::*SetMethod)(ChildType* value);
    typedef ChildType* (ParentType::*GetMethod)() const;

    /** Constructor
     *
     *  @param branch_name name used by json to reference the branch
     *  @param child_processor processor that will be used to convert the
     *  representation of the child types
     *  @param GetMethod callback that will return a pointer to the child data,
     *  where memory is still owned by the ObjectProcessor
     *  @param SetMethod callback that will set a pointer to the child data,
     *  where memory is given to the ObjectProcessor
     *  @param is_required if the branch doesnt exist in json, is null in json
     *  or is NULL in C++, throw Squeal if is_required is set to true when
     *  Set...Child methods are called
     */
    PointerItem(std::string branch_name, ProcessorBase<ChildType>* child_processor,
                GetMethod getter, SetMethod setter, bool is_required);

    /** _SetCppChild using data from parent_json
     *
     *  @param parent_json json object whose child branch is used to make C++
     *  representation
     *  @param parent_cpp C++ object where C++ data is put
     */
    void _SetCppChild(const Json::Value& parent_json, ParentType& parent_cpp);

    /** SetJsonChild using data from parent_cpp
     *
     *  @param parent_cpp C++ object from whence C++ data is got
     *  @param parent_json json object whose child branch is set using C++
     *  representation
     */
    void _SetJsonChild(const ParentType& parent_cpp, Json::Value& parent_json);

    /** Get the branch name */
    std::string GetBranchName() const {return _branch;}

  private:
    std::string _branch;
    ProcessorBase<ChildType>* _processor;
    SetMethod _setter;
    GetMethod _getter;
    bool      _required;
};

/** @class ValueItem value branch for a Cpp object
 *
 *  Probably don't want to use this class, instead use Register...Branch methods
 *  directly on ObjectProcessor
 *
 *  @tparam ParentType type of the parent object
 *  @tparam ChildType type of the child object referenced by the branch. Should
 *  use the actual type even if the target is a pointer
 */
template <class ParentType, class ChildType>
class ValueItem : public BaseItem<ParentType> {
  public:
    typedef void (ParentType::*SetMethod)(ChildType value);
    typedef ChildType (ParentType::*GetMethod)() const;

    /** Constructor
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
     */
    ValueItem(std::string branch_name, ProcessorBase<ChildType>* child_processor,
                GetMethod getter, SetMethod setter, bool is_required);

    // Set the child in the ParentInstance
    /** _SetCppChild using data from parent_json
     *
     *  @param parent_json json object whose child branch is used to make C++
     *  representation
     *  @param parent_cpp C++ object where C++ data is put
     */
    void _SetCppChild(const Json::Value& parent_json, ParentType& parent_cpp);

    /** SetJsonChild using data from parent_cpp
     *
     *  @param parent_cpp C++ object from whence C++ data is got
     *  @param parent_json json object whose child branch is set using C++
     *  representation
     */
    void _SetJsonChild(const ParentType& parent_cpp, Json::Value& parent_json);

    /** Get the branch name */
    std::string GetBranchName() const {return _branch;}

  private:
    std::string _branch;
    ProcessorBase<ChildType>* _processor;
    SetMethod _setter;
    GetMethod _getter;
    bool      _required;
};

/** @class ConstantItem constant branch for a Cpp object
 *
 *  Probably don't want to use this class, instead use Register...Branch methods
 *  directly on ObjectProcessor
 *
 *  @tparam ParentType type of the parent object
 */
template <class ParentType>
class ConstantItem : public BaseItem<ParentType> {
  public:
    /** Constructor
     *
     *  @param branch_name name used by json to reference the branch
     *  @param json_value value that will always be filled in this branch
     *  @param is_required if the branch doesnt exist in json, throw Squeal if
     *  is_required is set to true
     */
    ConstantItem(std::string branch_name, Json::Value child_value, bool is_required);

    /** _SetCppChild using data from parent_json
     *
     *  Merely throws if is_required is set and parent_json does not contain the
     *  branch with the correct value. Else does nothing.
     *
     *  @param parent_json json object whose child branch is used to make C++
     *  representation
     *  @param parent_cpp C++ object - ignored in this case
     */
    void _SetCppChild(const Json::Value& parent_json, ParentType& parent_cpp);

    /** SetJsonChild using data from parent_cpp
     *
     *  @param parent_cpp C++ object from whence C++ data is got
     *  @param parent_json json object whose child branch is set using C++
     *  representation
     */
    void _SetJsonChild(const ParentType& parent_cpp, Json::Value& parent_json);

    /** Get the branch name */
    std::string GetBranchName() const {return _branch;}

  private:
    std::string _branch;
    Json::Value _child_value;
    bool      _required;
};
// }  // namespace ObjectProcessorNS
}  // namespace MAUS

#include "src/common_cpp/JsonCppProcessors/Common/ObjectProcessorBranchItems-inl.hh"
#endif
