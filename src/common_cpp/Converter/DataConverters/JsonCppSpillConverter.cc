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
 *
 */

#include "src/common_cpp/Converter/DataConverters/JsonCppSpillConverter.hh"

#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/JsonCppProcessors/SpillProcessor.hh"

namespace MAUS {

Data* JsonCppSpillConverter::_convert(const Json::Value* json_data) const {
  try {
    ReferenceResolver::JsonToCpp::RefManager::Birth();
    Json::Value my_data = *json_data;  // need non-const copy to set path data
    JsonWrapper::Path::SetPathRecursive(my_data, "");
    Spill* spill = SpillProcessor().JsonToCpp(my_data);
    ReferenceResolver::JsonToCpp::RefManager::GetInstance().ResolveReferences();
    ReferenceResolver::JsonToCpp::RefManager::Death();
    Data *cpp_data = new Data();
    cpp_data->SetEvent(spill);
    cpp_data->SetEventType(spill->GetDaqEventType());
    return cpp_data;
  }
  catch(...) {
    if (ReferenceResolver::JsonToCpp::RefManager::HasInstance())
      ReferenceResolver::JsonToCpp::RefManager::Death();
    throw;
  }
}
}

