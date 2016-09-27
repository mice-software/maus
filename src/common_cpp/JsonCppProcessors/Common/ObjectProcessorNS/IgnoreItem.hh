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

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_OBJECTPROCESSORNS_IGNOREITEM_HH_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_OBJECTPROCESSORNS_IGNOREITEM_HH_

#include <string>

#include "src/common_cpp/JsonCppProcessors/ProcessorBase.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"

namespace MAUS {
namespace ObjectProcessorNS {
/** @class IgnoreItem ignore branch for a Cpp object
 *
 *  Probably don't want to use this class, instead use Register...Branch methods
 *  directly on ObjectProcessor
 *
 *  @tparam ParentType type of the parent object
 */
template <class ParentType>
class IgnoreItem : public BaseItem<ParentType> {
  public:
    /** Constructor
     *
     *  @param branch_name name used by json to reference the branch
     *  @param is_required if the branch doesnt exist in json, throw MAUS::Exceptions::Exception if
     *  is_required is set to true
     */
    IgnoreItem(std::string branch_name, bool is_required);

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
     *  Does nothing.
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
}  // namespace ObjectProcessorNS
}  // namespace MAUS

#include "src/common_cpp/JsonCppProcessors/Common/ObjectProcessorNS/IgnoreItem-inl.hh"
#endif
