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

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_REFERENCERESOLVERCPPTOJSON_INL_HH_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_REFERENCERESOLVERCPPTOJSON_INL_HH_

#include <map>
#include <set>
#include <string>

#include "TObject.h"

#include "src/common_cpp/JsonCppProcessors/ProcessorBase.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "Utils/Exception.hh"
#include "src/legacy/Interface/STLUtils.hh"

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
    Json::Value& child =
                   JsonWrapper::Path::DereferencePath(json_root, _json_pointer);
    child = Json::Value();
    // will throw if pointer is not in the RefManager
    try {
        std::string json_address =
                      RefManager::GetInstance().GetPointerAsValue(_cpp_pointer);
        child["$ref"] = Json::Value(json_address);
    } catch (MAUS::Exceptions::Exception& exc) {
        exc.SetMessage(exc.GetMessage()+"\n"+"Error at address "+_json_pointer);
        throw exc;
    }
}


///////////////////////////////////////////////////////

class RefManager::PointerValueTable {
  public:
    virtual ~PointerValueTable() {}
    virtual void ClearData() = 0;
    virtual std::string Dump() = 0;
  private:
};

template <class PointerType>
class RefManager::TypedPointerValueTable
    : public RefManager::PointerValueTable {
  public:
    virtual ~TypedPointerValueTable() {}
    void ClearData() {_data_hash.erase(_data_hash.begin(), _data_hash.end());}
    std::string Dump();
    std::map<PointerType*, std::string> _data_hash;
};

template <class PointerType>
std::string RefManager::TypedPointerValueTable<PointerType>::Dump() {
        std::string dump_str = "PointerValueTable\n";
        typename std::map<PointerType*, std::string>::iterator it;
        for (it = _data_hash.begin(); it != _data_hash.end(); ++it)
            dump_str += "  "+STLUtils::ToString(it->first)+" "+it->second+"\n";
        return dump_str;
}

template <class PointerType>
void RefManager::SetPointerAsValue
                              (PointerType* pointer, std::string json_address) {
    if (pointer == NULL)
        return;
    TypedPointerValueTable<PointerType>* table =
                                       GetTypedPointerValueTable<PointerType>();
    if (table->_data_hash.find(pointer) != table->_data_hash.end())
        throw(Exceptions::Exception(Exceptions::recoverable,
                     "Attempt to add pointer for C++ address "+
                     STLUtils::ToString(pointer)+
                     " to C++ hash table at json address "+json_address+
                     " when it was already added at json address "+
                     table->_data_hash[pointer],
                     "CppToJson::RefManager::SetPointerAsValue(...)"));
    table->_data_hash[pointer] = json_address;

    // If the object inherits from a TObject, add a second entry in a
    // second table, so that we can find the pointer from a TRef.
    TObject* tobject_pointer = (TObject*) pointer;
    if (tobject_pointer) {
      TypedPointerValueTable<TObject>* tableTObject =
          GetTypedPointerValueTable<TObject>();
      if (tableTObject->_data_hash.find(tobject_pointer) !=
          tableTObject->_data_hash.end())
        throw(Exceptions::Exception(Exceptions::recoverable,
                     "Attempt to add pointer for C++ address "+
                     STLUtils::ToString(pointer)+
                     " to TObject hash table at json address "+json_address+
                     " when it was already added at json address "+
                     table->_data_hash[pointer],
                     "CppToJson::RefManager::SetPointerAsValue(...)"));
      tableTObject->_data_hash[tobject_pointer] = json_address;
    }
}

template <class PointerType>
std::string RefManager::GetPointerAsValue(PointerType* pointer) {
    TypedPointerValueTable<PointerType>* table =
                                       GetTypedPointerValueTable<PointerType>();
    if (!table) {
        throw(Exceptions::Exception(Exceptions::recoverable,
                     "Attempt to get pointer for C++ address "+
                     STLUtils::ToString(pointer)+
                     " when no pointers of this type were ever added",
                     "CppToJson::RefManager::GetPointerAsValue(...)"));
    }
    if (table->_data_hash.find(pointer) == table->_data_hash.end())
        throw(Exceptions::Exception(Exceptions::recoverable,
                     "Attempt to get pointer for C++ address "+
                     STLUtils::ToString(pointer)+
                     " when it was never added. "+table->Dump(),
                     "CppToJson::RefManager::GetPointerAsValue(...)"));
    return table->_data_hash[pointer];
}

template <class PointerType>
RefManager::TypedPointerValueTable<PointerType>*
                                       RefManager::GetTypedPointerValueTable() {
    static TypedPointerValueTable<PointerType> table;
    _value_tables.insert(&table);
    return &table;
}

////////////////////////////////////////////////////////

RefManager::~RefManager() {
    for (size_t i = 0; i < _references.size(); ++i) {
        delete _references[i];
    }
    for (std::set<PointerValueTable*>::iterator it = _value_tables.begin();
         it != _value_tables.end();
         ++it) {
        (*it)->ClearData();
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
}  // namespace ReferenceResolver
}  // namespace MAUS

#endif

