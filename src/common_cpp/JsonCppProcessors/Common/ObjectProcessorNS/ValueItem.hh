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

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_OBJECTPROCESSORNS_VALUEITEM_HH_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_OBJECTPROCESSORNS_VALUEITEM_HH_

#include <string>

#include "src/common_cpp/JsonCppProcessors/ProcessorBase.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/JsonCppProcessors/Common/ReferenceResolverCppToJson.hh"
#include "src/common_cpp/JsonCppProcessors/Common/ReferenceResolverJsonToCpp.hh"

namespace MAUS {
namespace ObjectProcessorNS {
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
     *  @param is_required if the branch doesnt exist in json, throw MAUS::Exceptions::Exception if
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
}  // namespace ObjectProcessorNS
}  // namespace MAUS

#include "src/common_cpp/JsonCppProcessors/Common/ObjectProcessorNS/ValueItem-inl.hh"
#endif
