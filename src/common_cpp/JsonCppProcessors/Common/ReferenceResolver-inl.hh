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

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_REFERENCERESOLVER_INL_HH_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_REFERENCERESOLVER_INL_HH_

#include "src/common_cpp/JsonCppProcessors/ProcessorBase.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"

namespace MAUS {
namespace ReferenceResolver {

template <class ChildType>
TypedCppToJsonResolver<ChildType>::TypedCppToJsonResolver
(ChildType* ref_cpp_address, std::string ref_json_address)
  : _cpp_pointer(ref_cpp_address), _json_pointer(ref_json_address) {
  std::cerr << "TypedConstructor for " << _cpp_pointer << std::endl;
}

template <class ChildType>
void TypedCppToJsonResolver<ChildType>::ResolveReferences(Json::Value& json_root) {
    if (_data_hash.find(_cpp_pointer) == _data_hash.end()) {
        Json::Value& child = JsonWrapper::DereferencePath(json_root, _json_pointer);
        child = Json::Value();
    } else {
        std::cerr << "Attempting to allocate new value to " << _json_pointer << std::endl;
        Json::Value& child = JsonWrapper::DereferencePath(json_root, _json_pointer);
        Json::Value reference_value(Json::objectValue);
        reference_value["$ref"] = _data_hash[_cpp_pointer];
        child = reference_value;
        std::cerr << "Resolved " << _cpp_pointer << " as " << reference_value << std::endl;
    }
}

template <class ChildType>
void TypedCppToJsonResolver<ChildType>::AddData(ChildType* data_cpp_address, std::string data_json_address) {
    std::cerr << "AddData for " << data_cpp_address << " address " << data_json_address << std::endl;
    _data_hash[data_cpp_address] = data_json_address;
}

template <class ChildType>
void TypedCppToJsonResolver<ChildType>::ClearData() const {
    _data_hash = std::map<ChildType*, std::string>();
}

CppToJsonManager::~CppToJsonManager() {
    for (size_t i = 0; i < _references.size(); ++i) {
        _references[i]->ClearData();
        delete _references[i];
    }
    if (this == _instance) {
        _instance = NULL;
    }
}

void CppToJsonManager::ResolveReferences(Json::Value& json_root) {
    std::cerr << "Resolving " << std::endl;
    for (size_t i = 0; i < _references.size(); ++i) {
        _references[i]->ResolveReferences(json_root);
    }
}

void CppToJsonManager::AddReference(CppToJsonResolver* reference) {
    if (reference == NULL) return;
    _references.push_back(reference);
}

template <class ChildType>
std::map<ChildType*, std::string> TypedCppToJsonResolver<ChildType>::_data_hash;
}  // namespace ReferenceResolver
}  // namespace MAUS

#endif

