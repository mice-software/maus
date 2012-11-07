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

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_OBJECTPROCESSORNS_BASEITEM_HH_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_OBJECTPROCESSORNS_BASEITEM_HH_

#include "src/common_cpp/JsonCppProcessors/ProcessorBase.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/JsonCppProcessors/Common/ReferenceResolverCppToJson.hh"
#include "src/common_cpp/JsonCppProcessors/Common/ReferenceResolverJsonToCpp.hh"

namespace MAUS {
namespace ObjectProcessorNS {

/** BaseItem is base representation of a particular data structure branch
 *
 *  The processor holds information on each of the branches for a particular C++
 *  type in a vector. Different sorts of branches (is it stored as a pointer? is
 *  it stored by a value) share this interface so they can be stored in the same
 *  vector. For use with ObjectProcessor.
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
}  // namespace ObjectProcessorNS
}  // namespace MAUS

#include "src/common_cpp/JsonCppProcessors/Common/ObjectProcessorNS/BaseItem-inl.hh"
#endif
