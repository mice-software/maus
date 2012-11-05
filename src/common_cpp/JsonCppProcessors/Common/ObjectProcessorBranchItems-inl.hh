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

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_OBJECTPROCESSORBRANCHITEMS_INL_HH_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_OBJECTPROCESSORBRANCHITEMS_INL_HH_

#include "src/common_cpp/JsonCppProcessors/ProcessorBase.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/JsonCppProcessors/Common/ReferenceResolver.hh"

namespace MAUS {
// namespace ObjectProcessorNS {
template <class ParentType>
void BaseItem<ParentType>::SetCppChild
                  (const Json::Value& parent_json, ParentType& parent_cpp) {
    _SetCppChild(parent_json, parent_cpp);
}

template <class ParentType>
void BaseItem<ParentType>::SetJsonChild
                  (const ParentType& parent_cpp, Json::Value& parent_json) {
    _SetJsonChild(parent_cpp, parent_json);
}

template <class ParentType>
std::string BaseItem<ParentType>::GetPath(Json::Value& parent) {
    std::string branch = GetBranchName();
    Json::Value test;
    JsonWrapper::Path::SetPath(test, JsonWrapper::Path::GetPath(parent));
    JsonWrapper::Path::AppendPath(test, GetBranchName());
    return JsonWrapper::Path::GetPath(test);
}

template <class ParentType>
void BaseItem<ParentType>::SetPath(Json::Value& parent) {
    std::string branch = GetBranchName();
    Json::Value test;
    JsonWrapper::Path::SetPath(parent[branch],
                                        JsonWrapper::Path::GetPath(parent));
    JsonWrapper::Path::AppendPath(parent[branch], GetBranchName());
}

///////////////////////////////////////////////////////////////////////

template <class ParentType, class ChildType>
PointerRefItem<ParentType, ChildType>::PointerRefItem(
                   std::string branch_name,
                   ProcessorBase<ChildType>* child_processor,
                   GetMethod getter,
                   SetMethod setter,
                   bool is_required)
    : BaseItem<ParentType>(), _branch(branch_name),
      _processor(child_processor), _setter(setter),
      _getter(getter), _required(is_required) {
}

template <class ParentType, class ChildType>
void PointerRefItem<ParentType, ChildType>::_SetCppChild
                      (const Json::Value& parent_json, ParentType& parent_cpp) {
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

template <class ParentType, class ChildType>
void PointerRefItem<ParentType, ChildType>::_SetJsonChild
                      (const ParentType& parent_cpp, Json::Value& parent_json) {
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
              (child_cpp, JsonWrapper::Path::GetPath(parent_json[_branch]));
        RefManager::GetInstance().AddReference(res);
    }
}

template <class ParentType, class ChildType>
PointerItem<ParentType, ChildType>::PointerItem(
            std::string branch_name, ProcessorBase<ChildType>* child_processor,
            GetMethod getter, SetMethod setter, bool is_required)
    : BaseItem<ParentType>(), _branch(branch_name),
      _processor(child_processor), _setter(setter),
      _getter(getter), _required(is_required) {
}

template <class ParentType, class ChildType>
void PointerItem<ParentType, ChildType>::_SetCppChild
                      (const Json::Value& parent_json, ParentType& parent_cpp) {
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
    if (RefManager::HasInstance()) {
        std::string pth = JsonWrapper::Path::GetPath(parent_json[_branch]);
        ChildTypedResolver<ChildType>::AddData(pth, child_cpp);
    }
    // syntax is (_object.*_function)(args);
    (parent_cpp.*_setter)(child_cpp);
}

template <class ParentType, class ChildType>
void PointerItem<ParentType, ChildType>::_SetJsonChild
                      (const ParentType& parent_cpp, Json::Value& parent_json) {
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
    if (RefManager::HasInstance()) {
        std::string pth = JsonWrapper::Path::GetPath(parent_json[_branch]);
        TypedResolver<ChildType>::AddData(child_cpp, pth);
    }
}

template <class ParentType, class ChildType>
ValueItem<ParentType, ChildType>::ValueItem
          (std::string branch_name, ProcessorBase<ChildType>* child_processor,
           GetMethod getter, SetMethod setter, bool is_required)
          : BaseItem<ParentType>(), _branch(branch_name),
            _processor(child_processor), _setter(setter),
  _getter(getter), _required(is_required) {
}

template <class ParentType, class ChildType>
void ValueItem<ParentType, ChildType>::_SetCppChild
                      (const Json::Value& parent_json, ParentType& parent_cpp) {
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

template <class ParentType, class ChildType>
void ValueItem<ParentType, ChildType>::_SetJsonChild
                      (const ParentType& parent_cpp, Json::Value& parent_json) {
    ChildType child_cpp = (parent_cpp.*_getter)();
    std::string path = BaseItem<ParentType>::GetPath(parent_json);
    Json::Value* child_json = _processor->CppToJson(child_cpp, path);
    parent_json[_branch] = *child_json;
    delete child_json;
    // slightly worrying, the path doesn't seem to get pulled through here
    BaseItem<ParentType>::SetPath(parent_json);
}

template <class ParentType>
ConstantItem<ParentType>::ConstantItem
        (std::string branch_name, Json::Value child_value, bool is_required)
        : BaseItem<ParentType>(), _branch(branch_name),
          _child_value(child_value), _required(is_required) {
}

template <class ParentType>
void ConstantItem<ParentType>::_SetCppChild
                      (const Json::Value& parent_json, ParentType& parent_cpp) {
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

template <class ParentType>
void ConstantItem<ParentType>::_SetJsonChild
                      (const ParentType& parent_cpp, Json::Value& parent_json) {
    parent_json[_branch] = _child_value;
    BaseItem<ParentType>::SetPath(parent_json);
}

// }  // namespace ObjectProcessorNS
}  // namespace MAUS
#endif
