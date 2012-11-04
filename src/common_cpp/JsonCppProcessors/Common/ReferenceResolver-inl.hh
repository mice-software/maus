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
namespace CppToJson {
template <class ChildType>
TypedResolver<ChildType>::TypedResolver
(ChildType* ref_cpp_address, std::string ref_json_address)
  : _cpp_pointer(ref_cpp_address), _json_pointer(ref_json_address) {
}

template <class ChildType>
void TypedResolver<ChildType>::ResolveReferences(Json::Value& json_root) {
    if (_data_hash.find(_cpp_pointer) == _data_hash.end()) {  // NullValue
        Json::Value& child =
                   JsonWrapper::Path::DereferencePath(json_root, _json_pointer);
        child = Json::Value();
    } else {  // set json reference to point at json value
        Json::Value& child =
                   JsonWrapper::Path::DereferencePath(json_root, _json_pointer);
        Json::Value reference_value(Json::objectValue); 
        reference_value["$ref"] = _data_hash[_cpp_pointer];
        child = reference_value;
    }
}

template <class ChildType>
void TypedResolver<ChildType>::AddData(ChildType* data_cpp_address, std::string data_json_address) {
    _data_hash[data_cpp_address] = data_json_address;
}

template <class ChildType>
std::map<ChildType*, std::string> TypedResolver<ChildType>::_data_hash;

template <class ChildType>
void TypedResolver<ChildType>::ClearData() const {
    _data_hash = std::map<ChildType*, std::string>();
}

RefManager::~RefManager() {
    for (size_t i = 0; i < _references.size(); ++i) {
        _references[i]->ClearData();
        delete _references[i];
    }
    if (this == _instance) {
        _instance = NULL;
    }
}

void RefManager::ResolveReferences(Json::Value& json_root) {
    for (size_t i = 0; i < _references.size(); ++i) {
        _references[i]->ResolveReferences(json_root);
    }
}

void RefManager::AddReference(Resolver* reference) {
    if (reference == NULL) return;
    _references.push_back(reference);
}
}  // namespace CppToJson

///////////////////////////////////////////////////////////////

namespace JsonToCpp {
template <class ParentType, class ChildType>
FullyTypedResolver<ParentType, ChildType>::FullyTypedResolver(
      std::string ref_json_address,
      FullyTypedResolver<ParentType, ChildType>::SetMethod cpp_setter,
      ParentType* ref_cpp_parent)
  : _cpp_setter(cpp_setter), _ref_json_address(ref_json_address),
    _ref_cpp_parent(ref_cpp_parent) {
}

template <class ParentType, class ChildType>
void FullyTypedResolver<ParentType, ChildType>::ResolveReferences() {
    std::map<std::string, ChildType*>& data_hash =
                             ChildTypedResolver<ChildType>::_data_hash;
    if (data_hash.find(_ref_json_address) == data_hash.end()) {
        (*_ref_cpp_parent.*_cpp_setter)(NULL);
    } else {
        (*_ref_cpp_parent.*_cpp_setter)(data_hash[_ref_json_address]);
    }
}

////////////////////////////////////////////////////////////////

template <class ChildType>
VectorResolver<ChildType>::VectorResolver(
                            std::string ref_json_address,
                            std::vector<ChildType*>& vector,
                            size_t vector_index)
  : _ref_json_address(ref_json_address), _vector(vector), _index(vector_index) {   
}

template <class ChildType>
void VectorResolver<ChildType>::ResolveReferences() {
    std::map<std::string, ChildType*>& data_hash = 
                             ChildTypedResolver<ChildType>::_data_hash;
    if (_index >= _vector.size())
        throw(Squeal(Squeal::recoverable,
                     "Index out of range while resolving pointer to array "+
                     _ref_json_address,
                     "VectorResolver<ChildType>::ResolverReferences"));
    if (data_hash.find(_ref_json_address) == data_hash.end()) {
        _vector[_index] = NULL;
    } else {
        _vector[_index] = data_hash[_ref_json_address];
    }
}

void RefManager::ResolveReferences() {
    for (size_t i = 0; i < _references.size(); ++i) {
        _references[i]->ResolveReferences();
    }
}

void RefManager::AddReference(Resolver* reference) {
    if (reference == NULL) return;
    _references.push_back(reference);
}

RefManager::~RefManager() {
    for (size_t i = 0; i < _references.size(); ++i) {
        _references[i]->ClearData();
        delete _references[i];
    }
}

template <class ChildType>
void ChildTypedResolver<ChildType>::AddData
                  (std::string data_json_address, ChildType* data_cpp_address) {
    _data_hash[data_json_address] = data_cpp_address;
}

template <class ChildType>
void ChildTypedResolver<ChildType>::ClearData() const {
    _data_hash = std::map<std::string, ChildType*>();
}

template <class ChildType>
std::map<std::string, ChildType*> ChildTypedResolver<ChildType>::_data_hash;
}  // namespace JsonToCpp
}  // namespace ReferenceResolver
}  // namespace MAUS

#endif

