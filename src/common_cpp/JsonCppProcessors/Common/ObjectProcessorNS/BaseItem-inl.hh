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

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_OBJECTPROCESSORNS_BASEITEM_INL_HH_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_OBJECTPROCESSORNS_BASEITEM_INL_HH_

#include <string>

#include "src/common_cpp/JsonCppProcessors/ProcessorBase.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"

namespace MAUS {
namespace ObjectProcessorNS {
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
}  // namespace ObjectProcessorNS
}  // namespace MAUS
#endif
