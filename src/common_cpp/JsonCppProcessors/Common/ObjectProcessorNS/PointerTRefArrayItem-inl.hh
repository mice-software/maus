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

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_OBJECTPROCESSORNS_POINTERTREFARRAYITEM_INL_HH_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_OBJECTPROCESSORNS_POINTERTREFARRAYITEM_INL_HH_

#include <set>
#include <string>

#include "src/common_cpp/JsonCppProcessors/ProcessorBase.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"

namespace MAUS {
namespace ObjectProcessorNS {
template <class ParentType>
PointerTRefArrayItem<ParentType>::PointerTRefArrayItem(
    std::string branch_name,
    TRefArrayProcessor* child_processor,
    GetMethod getter,
    SetMethod setter,
    bool is_required)
    : BaseItem<ParentType>(), _branch(branch_name),
      _processor(child_processor),
      _setter(setter),
      _getter(getter), _required(is_required) {
}

template <class ParentType>
void PointerTRefArrayItem<ParentType>::_SetCppChild
(const Json::Value& parent_json, ParentType& parent_cpp) {
  if (!parent_json.isMember(_branch)) {
    if (_required) {
      throw MAUS::Exceptions::Exception(Exceptions::recoverable,
                   "Missing required branch "+_branch+" converting json->cpp",
                   "PointerTRefArrayItem::SetCppChild");
    } else {
      (parent_cpp.*_setter)(new TRefArray());
      return;
    }
  }
  if (parent_json[_branch].isNull()) {
    if (_required) {
      throw MAUS::Exceptions::Exception(Exceptions::recoverable,
                   "Null branch "+_branch+" converting json->cpp",
                   "PointerTRefArrayItem::SetCppChild");
    } else {
      (parent_cpp.*_setter)(new TRefArray());
      return;
    }
  }

  Json::Value child_json = parent_json[_branch];
  TRefArray *child_cpp = _processor->JsonToCpp(child_json);
  // syntax is (_object.*_function)(args);
  (parent_cpp.*_setter)(child_cpp);
}

template <class ParentType>
void PointerTRefArrayItem<ParentType>::_SetJsonChild
(const ParentType& parent_cpp, Json::Value& parent_json) {
  TRefArray *child_cpp = (parent_cpp.*_getter)();
  std::string path = BaseItem<ParentType>::GetPath(parent_json);
  Json::Value* child_json = _processor->CppToJson(child_cpp, path);
  parent_json[_branch] = *child_json;
  delete child_json;
  // slightly worrying, the path doesn't seem to get pulled through here
  BaseItem<ParentType>::SetPath(parent_json);
}
}  // namespace ObjectProcessorNS
}  // namespace MAUS
#endif
