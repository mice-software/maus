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

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_OBJECTPROCESSORNS_POINTERREFITEM_INL_HH_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_OBJECTPROCESSORNS_POINTERREFITEM_INL_HH_

#include "src/common_cpp/JsonCppProcessors/ProcessorBase.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"

namespace MAUS {
namespace ObjectProcessorNS {
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
            (parent_cpp.*_setter)(NULL);
            return;
        }
    }
    if (parent_json[_branch].isNull()) {
        if (_required) {
            throw Squeal(Squeal::recoverable,
            "Null branch "+_branch+" converting json->cpp",
            "PointerRefItem::SetCppChild");
        } else {
            (parent_cpp.*_setter)(NULL);
            return;
        }
    }
    if (parent_json[_branch]["$ref"].isNull()) {
        if (_required) {
            throw Squeal(Squeal::recoverable,
            "Null branch "+_branch+" converting json->cpp",
            "PointerRefItem::SetCppChild");
        } else {
            (parent_cpp.*_setter)(NULL);
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
            parent_json[_branch] = Json::Value();
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
}  // namespace ObjectProcessorNS
}  // namespace MAUS
#endif
