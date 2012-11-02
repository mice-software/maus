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
                  (const Json::Value& parent_json, ParentType& parent_cpp) {
        _SetCppChild(parent_json, parent_cpp);
    }
    /** SetCppChild set the child data on the json representation
     *
     *  Uses the json comment field to store metadata (e.g. path)
     */
    virtual void SetJsonChild
                  (const ParentType& parent_cpp, Json::Value& parent_json) {
        _SetJsonChild(parent_cpp, parent_json);
    }

    /** Get the branch name */
    virtual std::string GetBranchName() const = 0;

  protected:
    /** Called by SetCppChild - overload here */
    virtual void _SetCppChild
                  (const Json::Value& parent_json, ParentType& parent_cpp) = 0;

    /** Called by SetJsonChild - overload here */
    virtual void _SetJsonChild
                  (const ParentType& parent_cpp, Json::Value& parent_json) = 0;

    std::string GetPath(Json::Value& parent) {
        std::string branch = GetBranchName();
        Json::Value test;
        JsonWrapper::SetPath(test, JsonWrapper::GetPath(parent));
        JsonWrapper::AppendPath(test, GetBranchName());
        return JsonWrapper::GetPath(test);
    }

    void SetPath(Json::Value& parent) {
        std::string branch = GetBranchName();
        Json::Value test;
        JsonWrapper::SetPath(parent[branch], JsonWrapper::GetPath(parent));
        JsonWrapper::AppendPath(parent[branch], GetBranchName());
    }
};

/** @class PointerRefItem pointer branch for a Cpp object
 *
 *  Probably don't want to use this class, instead use Register...Branch methods
 *  directly on ObjectProcessor
 *
 *  @tparam ParentType type of the parent object
 *  @tparam ChildType type of the child object referenced by the branch. Should
 *  use the actual type even if the target is a pointer
 *
 *  For resolving references; logic is - for conversion from Json to C++
 *
 *  At first pass conversion
 *
 *  \li We store a C++ pointer, Json::Value pair 
 *  \li We store the Json address of the target (as string)
 *  \li We store the C++ pointer of the target (as string)
 *
 *  On the PointerItem,
 *
 *  \li We store a static map of Json address to C++ pointer
 *
 *  We store a static list of PointerRefs. 
 *
 *  When we attempt to resolve references; 
 *  for pointer_ref in PointerRefs:
 *    get json address from pointer_ref;
 *    extract C++ pointer from pointer item map;
 *    fill C++ pointer
 *  
 *  Problem: how do we deal with types? Abstraction which has 
 *  ResolveReferences() function... the caller doesn't need to know the pointer
 *  type, only the child class does (so templates are all in the child class);
 *  then we store a vector of reference resolvers and list of PointerRefItems.
 *  Propose then this is wrapped up in a reference manager class.
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
                   bool is_required)
        : BaseItem<ParentType>(), _branch(branch_name),
          _processor(child_processor), _setter(setter),
          _getter(getter), _required(is_required) {
    }

    /** SetCppChild using data from parent_json
     *
     *  @param parent_json json object whose child branch is used to make C++
     *  representation
     *  @param parent_cpp C++ object where C++ data is put
     */
    void _SetCppChild(const Json::Value& parent_json, ParentType& parent_cpp) {
        if (!parent_json.isMember(_branch)) {
            if (_required) {
                throw Squeal(Squeal::recoverable,
                "Missing required branch "+_branch+" converting json->cpp",
                "PointerRefItem::SetCppChild");
            } else {
                return;
            }
        }
        if (parent_json[_branch].isNull()) {
            if (_required) {
                throw Squeal(Squeal::recoverable,
                "Null branch "+_branch+" converting json->cpp",
                "PointerRefItem::SetCppChild");
            } else {
                return;
            }
        }
        Json::Value child_json = parent_json[_branch];
        std::string data_path = JsonWrapper::GetProperty
                      (child_json, "$ref", JsonWrapper::stringValue).asString();
        using namespace ReferenceResolver::JsonToCpp;
        if (RefManager::HasInstance()) {
            FullyTypedResolver<ParentType, ChildType>* res =
                                   new FullyTypedResolver<ParentType, ChildType>
                                              (data_path, _setter, &parent_cpp);
            RefManager::GetInstance().AddReference(res);
        }
        // syntax is (_object.*_function)(args);
        (parent_cpp.*_setter)(NULL);
    }

    /** SetJsonChild fills the branch with a Null json value and stores the
     *  branch location and pointer in the reference manager for filling later.
     *
     *  @param parent_cpp C++ object from whence C++ data is got
     *  @param parent_json json object whose child branch is set using C++
     *  representation
     */
    void _SetJsonChild(const ParentType& parent_cpp, Json::Value& parent_json) {
        ChildType* child_cpp = (parent_cpp.*_getter)();
        if (child_cpp == NULL) {
            if (_required) {
                throw Squeal(Squeal::recoverable,
                "Failed to find branch "+_branch+": class data was NULL",
                "PointerRefItem::GetCppChild");
            } else {
                return;
            }
        }
        parent_json[_branch] = Json::Value();
        BaseItem<ParentType>::SetPath(parent_json);
        using namespace ReferenceResolver::CppToJson;
        if (RefManager::HasInstance()) {
            TypedResolver<ChildType>* res = new TypedResolver<ChildType>
                        (child_cpp, JsonWrapper::GetPath(parent_json[_branch]));
            RefManager::GetInstance().AddReference(res);
        }
    }

    /** Get the branch name */
    std::string GetBranchName() const {return _branch;}

  private:
    std::string _branch;
    ProcessorBase<ChildType>* _processor;
    SetMethod _setter;
    GetMethod _getter;
    bool      _required;
    // for CppToJson, we want to map the cpp pointer to the address of the json
    // data which needs to be filled; and map the json path to the cpp pointer.
    // as we can have many references to the same cpp pointer, this needs to be
    // a multimap; but the pointer only exists once, so this is a map.
    //
    // When we are ready to fill pointers, we loop over items in the multimap;
    // and for each ChildType* we look for the appropriate string pointer on the
    // PointerItem... if it can't be found then we wait; else we Set the
    // ChildType and pop that pair off the multimap
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
                GetMethod getter, SetMethod setter, bool is_required)
        : BaseItem<ParentType>(), _branch(branch_name),
          _processor(child_processor), _setter(setter),
          _getter(getter), _required(is_required) {
    }

    /** _SetCppChild using data from parent_json
     *
     *  @param parent_json json object whose child branch is used to make C++
     *  representation
     *  @param parent_cpp C++ object where C++ data is put
     */
    void _SetCppChild(const Json::Value& parent_json, ParentType& parent_cpp) {
        if (!parent_json.isMember(_branch)) {
            if (_required) {
                throw Squeal(Squeal::recoverable,
                "Missing required branch "+_branch+" converting json->cpp",
                "PointerItem::_SetCppChild");
            } else {
                return;
            }
        }
        if (parent_json[_branch].isNull()) {
            if (_required) {
                throw Squeal(Squeal::recoverable,
                "Null branch "+_branch+" converting json->cpp",
                "PointerItem::_SetCppChild");
            } else {
                return;
            }
        }
        Json::Value child_json = parent_json[_branch];
        ChildType* child_cpp = _processor->JsonToCpp(child_json);
        using namespace ReferenceResolver::JsonToCpp;
        if (RefManager::HasInstance())
            ChildTypedResolver<ChildType>
               ::AddData(JsonWrapper::GetPath(parent_json[_branch]), child_cpp);
        // syntax is (_object.*_function)(args);
        (parent_cpp.*_setter)(child_cpp);
    }

    /** SetJsonChild using data from parent_cpp
     *
     *  @param parent_cpp C++ object from whence C++ data is got
     *  @param parent_json json object whose child branch is set using C++
     *  representation
     */
    void _SetJsonChild(const ParentType& parent_cpp, Json::Value& parent_json) {
        ChildType* child_cpp = (parent_cpp.*_getter)();
        if (child_cpp == NULL) {
            if (_required) {
                throw Squeal(Squeal::recoverable,
                "Failed to find branch "+_branch+": class data was NULL",
                "PointerItem::GetCppChild");
            } else {
                return;
            }
        }
        std::string path = BaseItem<ParentType>::GetPath(parent_json);
        Json::Value* child_json = _processor->CppToJson(*child_cpp, path);
        parent_json[_branch] = *child_json;
        delete child_json;
        // slightly worrying, the path doesn't seem to get pulled through here
        BaseItem<ParentType>::SetPath(parent_json);
        using namespace ReferenceResolver::CppToJson;
        if (RefManager::HasInstance())
            TypedResolver<ChildType>::
                 AddData(child_cpp, JsonWrapper::GetPath(parent_json[_branch]));
    }

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
                GetMethod getter, SetMethod setter, bool is_required)
                          : BaseItem<ParentType>(), _branch(branch_name),
                            _processor(child_processor), _setter(setter),
      _getter(getter), _required(is_required) {
    }

    // Set the child in the ParentInstance
    /** _SetCppChild using data from parent_json
     *
     *  @param parent_json json object whose child branch is used to make C++
     *  representation
     *  @param parent_cpp C++ object where C++ data is put
     */
    void _SetCppChild(const Json::Value& parent_json, ParentType& parent_cpp) {
        if (!parent_json.isMember(_branch)) {
            if (_required) {
                throw Squeal(Squeal::recoverable,
                "Missing required branch "+_branch+" converting json->cpp",
                "ValueItem::_SetCppChild");
            } else {
                return;
            }
        }
        Json::Value child_json = parent_json[_branch];
        ChildType* child_cpp = _processor->JsonToCpp(child_json);
        // syntax is (_object.*_function)(args);
        (parent_cpp.*_setter)(*child_cpp);
        delete child_cpp;
    }

    /** SetJsonChild using data from parent_cpp
     *
     *  @param parent_cpp C++ object from whence C++ data is got
     *  @param parent_json json object whose child branch is set using C++
     *  representation
     */
    void _SetJsonChild(const ParentType& parent_cpp, Json::Value& parent_json) {
        ChildType child_cpp = (parent_cpp.*_getter)();
        std::string path = BaseItem<ParentType>::GetPath(parent_json);
        Json::Value* child_json = _processor->CppToJson(child_cpp, path);
        parent_json[_branch] = *child_json;
        delete child_json;
        // slightly worrying, the path doesn't seem to get pulled through here
        BaseItem<ParentType>::SetPath(parent_json);
    }

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
    ConstantItem(std::string branch_name, Json::Value child_value, bool is_required)
                          : BaseItem<ParentType>(), _branch(branch_name),
                            _child_value(child_value), _required(is_required) {
    }

    /** _SetCppChild using data from parent_json
     *
     *  Merely throws if is_required is set and parent_json does not contain the
     *  branch with the correct value. Else does nothing.
     *
     *  @param parent_json json object whose child branch is used to make C++
     *  representation
     *  @param parent_cpp C++ object - ignored in this case
     */
    void _SetCppChild(const Json::Value& parent_json, ParentType& parent_cpp) {
        if (_required && !parent_json.isMember(_branch)) {
                throw Squeal(Squeal::recoverable,
                "Missing required branch "+_branch+" converting json->cpp",
                "ConstantItem::_SetCppChild");
        } else if (parent_json.isMember(_branch) && !JsonWrapper::AlmostEqual
                                   (parent_json[_branch], _child_value, 1e-9)) {
                throw Squeal(Squeal::recoverable,
                "Wrong value in branch "+_branch+": "+
                                JsonWrapper::JsonToString(parent_json[_branch]),
                "ConstantItem::_SetCppChild");
        }
    }

    /** SetJsonChild using data from parent_cpp
     *
     *  @param parent_cpp C++ object from whence C++ data is got
     *  @param parent_json json object whose child branch is set using C++
     *  representation
     */
    void _SetJsonChild(const ParentType& parent_cpp, Json::Value& parent_json) {
        parent_json[_branch] = _child_value;
        BaseItem<ParentType>::SetPath(parent_json);
    }

    /** Get the branch name */
    std::string GetBranchName() const {return _branch;}

  private:
    std::string _branch;
    Json::Value _child_value;
    bool      _required;
};
// }  // namespace ObjectProcessorNS
}  // namespace MAUS
#endif
