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

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_REFERENCERESOLVERJSONTOCPP_INL_HH_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_REFERENCERESOLVERJSONTOCPP_INL_HH_

#include <set>
#include <map>
#include <string>
#include <vector>

#include "src/common_cpp/JsonCppProcessors/ProcessorBase.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/legacy/Interface/Squeal.hh"
#include "src/legacy/Interface/STLUtils.hh"

namespace MAUS {
namespace ReferenceResolver {
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
    ChildType* data_address = RefManager::GetInstance().
                                GetPointerAsValue<ChildType>(_ref_json_address);
    (*_ref_cpp_parent.*_cpp_setter)(data_address);
    if (data_address == NULL)
        throw(Squeal(Squeal::recoverable,
              "Failed to resolve reference at "+_ref_json_address+
              " on C++ object "+STLUtils::ToString(_ref_cpp_parent),
              "ReferenceResolver::FullyTypedResolver::ResolveReferences"));
}

template <class ParentType, class ChildType>
TRefResolver<ParentType, ChildType>::TRefResolver(
      std::string ref_json_address,
      TRefResolver<ParentType, ChildType>::SetMethod cpp_setter,
      ParentType* ref_cpp_parent)
  : _cpp_setter(cpp_setter), _ref_json_address(ref_json_address),
    _ref_cpp_parent(ref_cpp_parent) {
}

template <class ParentType, class ChildType>
void TRefResolver<ParentType, ChildType>::ResolveReferences() {
    ChildType* data_address = RefManager::GetInstance().
                                GetPointerAsValue<ChildType>(_ref_json_address);
    (*_ref_cpp_parent.*_cpp_setter)(TRef(data_address));
    if (data_address == NULL)
        throw(Squeal(Squeal::recoverable,
              "Failed to resolve reference at "+_ref_json_address+
              " on C++ object "+STLUtils::ToString(_ref_cpp_parent),
              "ReferenceResolver::TRefResolver::ResolveReferences"));
}

////////////////////////////////////////////////////////////////

template <class ChildType>
VectorResolver<ChildType>::VectorResolver(
                            std::string ref_json_address,
                            std::vector<ChildType*>* vector,
                            size_t vector_index)
  : _ref_json_address(ref_json_address), _vector(vector), _index(vector_index) {
}

template <class ChildType>
void VectorResolver<ChildType>::ResolveReferences() {
    if (_vector == NULL)
        return;
    if (_index >= _vector->size())
        throw(Squeal(Squeal::recoverable,
                     "Index out of range while resolving pointer to array "+
                     _ref_json_address,
                     "ReferenceResolver::VectorResolver::ResolveReferences"));
    ChildType* data_address = RefManager::GetInstance().
                                GetPointerAsValue<ChildType>(_ref_json_address);
    (*_vector)[_index] = data_address;
    if (data_address == NULL)
        throw(Squeal(Squeal::recoverable,
              "Failed to resolve reference at "+_ref_json_address+
              " on C++ vector "+STLUtils::ToString(_vector)+" element "+
              STLUtils::ToString(_index),
              "ReferenceResolver::VectorResolver::ResolveReferences"));
  }

////////////////////////////////////////////////////////////////

template <class ChildType>
TRefArrayResolver<ChildType>::TRefArrayResolver(
    std::string ref_json_address,
    TRefArray* tref_array,
    size_t vector_index)
    : _ref_json_address(ref_json_address),
      _tref_array(tref_array),
      _index(vector_index) {
}

template <class ChildType>
void TRefArrayResolver<ChildType>::ResolveReferences() {
  if (_tref_array == NULL)
    return;
  if ((int)_index > _tref_array->GetSize())
    throw(Squeal(Squeal::recoverable,
                 "Index out of range while resolving pointer to array "+
                 _ref_json_address,
                 "ReferenceResolver::TRefArrayResolver::ResolveReferences"));
  ChildType* data_address = RefManager::GetInstance().
      GetPointerAsValue<ChildType>(_ref_json_address);
  if (data_address == NULL)
    throw(Squeal(Squeal::recoverable,
                 "Failed to resolve reference at "+_ref_json_address+
                 " on TRefArray element "+
                 STLUtils::ToString(_index),
                 "ReferenceResolver::TRefArrayResolver::ResolveReferences"));
  _tref_array->AddAt(data_address, _index);
  data_address->ls();
}

//////////////////////////////////////////////////////////////////

class RefManager::PointerValueTable {
  public:
    virtual ~PointerValueTable() {}
    virtual void ClearData() = 0;
  private:
};

template <class PointerType>
class RefManager::TypedPointerValueTable : public RefManager::PointerValueTable {
  public:
    virtual ~TypedPointerValueTable() {}
    void ClearData() {_data_hash.erase(_data_hash.begin(), _data_hash.end());}
    std::map<std::string, PointerType*> _data_hash;
};

void RefManager::ResolveReferences() {
    for (size_t i = 0; i < _references.size(); ++i) {
        _references[i]->ResolveReferences();
    }
}

template <class PointerType>
void RefManager::SetPointerAsValue
                              (std::string json_address, PointerType* pointer) {
    TypedPointerValueTable<PointerType>* table =
                                       GetTypedPointerValueTable<PointerType>();
    if (table->_data_hash.find(json_address) != table->_data_hash.end())
        throw(Squeal(Squeal::recoverable,
                     "Attempt to add json pointer "+json_address+
                     " to hash table when it was already added",
                     "JsonToCpp::RefManager::SetPointerAsValue(...)"));
    table->_data_hash[json_address] = pointer;
}

template <class PointerType>
PointerType* RefManager::GetPointerAsValue(std::string json_address) {
    TypedPointerValueTable<PointerType>* table =
                                       GetTypedPointerValueTable<PointerType>();
    if (table->_data_hash.find(json_address) == table->_data_hash.end())
        return NULL;
    return table->_data_hash[json_address];
}

template <class PointerType>
RefManager::TypedPointerValueTable<PointerType>*
                                       RefManager::GetTypedPointerValueTable() {
    static TypedPointerValueTable<PointerType> table;
    _value_tables.insert(&table);
    return &table;
}

void RefManager::AddReference(Resolver* reference) {
    if (reference == NULL) return;
    _references.push_back(reference);
}

RefManager::~RefManager() {
    for (size_t i = 0; i < _references.size(); ++i) {
        delete _references[i];
    }
    for (std::set<PointerValueTable*>::iterator it = _value_tables.begin();
         it != _value_tables.end();
         ++it) {
        (*it)->ClearData();
    }
}
}  // namespace JsonToCpp
}  // namespace ReferenceResolver
}  // namespace MAUS

#endif

