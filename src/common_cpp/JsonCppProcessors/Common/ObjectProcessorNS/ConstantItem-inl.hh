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

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_OBJECTPROCESSORNS_CONSTANTITEM_INL_HH_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_OBJECTPROCESSORNS_CONSTANTITEM_INL_HH_

#include <string>
#include "src/common_cpp/JsonCppProcessors/ProcessorBase.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"

namespace MAUS {
namespace ObjectProcessorNS {
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
            throw MAUS::Exceptions::Exception(Exceptions::recoverable,
            "Missing required branch "+_branch+" converting json->cpp",
            "ConstantItem::_SetCppChild");
    } else if (parent_json.isMember(_branch) && !JsonWrapper::AlmostEqual
                               (parent_json[_branch], _child_value, 1e-9)) {
            throw MAUS::Exceptions::Exception(Exceptions::recoverable,
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
}  // namespace ObjectProcessorNS
}  // namespace MAUS
#endif
