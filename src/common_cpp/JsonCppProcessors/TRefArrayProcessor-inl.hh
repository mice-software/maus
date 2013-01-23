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

#include <string>
#include <vector>

#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/JsonCppProcessors/Common/ReferenceResolverCppToJson.hh"
#include "src/common_cpp/JsonCppProcessors/Common/ReferenceResolverJsonToCpp.hh"

namespace MAUS {

////////////////////////////
template <class ArrayContents>
TRefArray* TRefArrayProcessor<ArrayContents>::JsonToCpp(
    const Json::Value& json_array) {
  using ReferenceResolver::JsonToCpp::RefManager;
  using ReferenceResolver::JsonToCpp::TRefArrayResolver;
  if (!json_array.isConvertibleTo(Json::arrayValue)) {
    // no memory allocated yet...
    throw(Squeal(Squeal::recoverable,
                 "Failed to resolve Json::Value of type "+
                 JsonWrapper::ValueTypeToString(json_array.type())+
                 " to array",
                 "ValueArrayProcessor::JsonToCpp()"
                 ) );
  }
  TRefArray* tref_array = new TRefArray(json_array.size());

  for (size_t i = 0; i < json_array.size(); ++i) {
    try {
      if (json_array[i].type() != Json::nullValue) {
        std::string data_path = JsonWrapper::GetProperty
            (json_array[i], "$ref", JsonWrapper::stringValue).asString();
        // allocate the TRefArray element
        if (RefManager::HasInstance()) {
          TRefArrayResolver<ArrayContents>* res =
              new TRefArrayResolver<ArrayContents>(data_path, tref_array, i);
          RefManager::GetInstance().AddReference(res);
        }
      }
    } catch(Squeal squee) {
      // if there's a problem, clean up before rethrowing the exception
      throw squee;
    }
  }
  return tref_array;
}

template <class ArrayContents>
Json::Value* TRefArrayProcessor<ArrayContents>::CppToJson(
    const TRefArray* tref_array,
    std::string path) {
  using ReferenceResolver::CppToJson::RefManager;
  using ReferenceResolver::CppToJson::TypedResolver;
  Json::Value* array = new Json::Value(Json::arrayValue);
  JsonWrapper::Path::SetPath(*array, path);
  array->resize(tref_array->GetEntries());
  ArrayContents *array_element;
  for(int i = 0; i < tref_array->GetLast()+1; ++i) {
    array_element = (ArrayContents*) tref_array->At(i);
    if(!array_element) continue;
    (*array)[i] = Json::Value();
    JsonWrapper::Path::SetPath((*array)[i], path);
    JsonWrapper::Path::AppendPath((*array)[i], i);
    if (RefManager::HasInstance()) {
      std::string arr_path = JsonWrapper::Path::GetPath((*array)[i]);
      TypedResolver<ArrayContents>* res = new TypedResolver
          <ArrayContents>(array_element, arr_path);
      RefManager::GetInstance().AddReference(res);
    }
  }
  return array;
}

// template <class ArrayContents>
// std::string TRefArrayProcessor<ArrayContents>::GetPath(
//     std::string path, size_t index) {
//   return path+"/"+STLUtils::ToString(index);
// }

}
