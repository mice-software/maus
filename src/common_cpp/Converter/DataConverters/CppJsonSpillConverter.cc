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

#include "src/common_cpp/Converter/DataConverters/CppJsonSpillConverter.hh"

#include "src/common_cpp/DataStructure/Data.hh"
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/JsonCppProcessors/SpillProcessor.hh"

namespace MAUS {

Json::Value* CppJsonSpillConverter::_convert(const Data* data) const {
  if (data == NULL || data->GetSpill() == NULL)
      return new Json::Value();
  try {
    ReferenceResolver::CppToJson::RefManager::Birth();
    Json::Value* my_json = SpillProcessor().CppToJson(*data->GetSpill(), "");
    ReferenceResolver::CppToJson::RefManager::Death();
    return my_json;
  }
  catch(...) {
    if (ReferenceResolver::CppToJson::RefManager::HasInstance())
      ReferenceResolver::CppToJson::RefManager::Death();
    throw;
  }
}
}

