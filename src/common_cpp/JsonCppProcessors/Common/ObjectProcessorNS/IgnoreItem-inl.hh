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

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_OBJECTPROCESSORNS_IGNOREITEM_INL_HH_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_OBJECTPROCESSORNS_IGNOREITEM_INL_HH_

#include <string>
#include "src/common_cpp/JsonCppProcessors/ProcessorBase.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"

namespace MAUS {
namespace ObjectProcessorNS {
template <class ParentType>
IgnoreItem<ParentType>::IgnoreItem
        (std::string branch_name, bool is_required)
        : BaseItem<ParentType>(), _branch(branch_name), _required(is_required) {
}

template <class ParentType>
void IgnoreItem<ParentType>::_SetCppChild
                      (const Json::Value& parent_json, ParentType& parent_cpp) {
    if (_required && !parent_json.isMember(_branch)) {
            throw MAUS::Exceptions::Exception(Exceptions::recoverable,
            "Missing required branch "+_branch+" converting json->cpp",
            "IgnoreItem::_SetCppChild");
    }
}

template <class ParentType>
void IgnoreItem<ParentType>::_SetJsonChild
                      (const ParentType& parent_cpp, Json::Value& parent_json) {
}
}  // namespace ObjectProcessorNS
}  // namespace MAUS
#endif
