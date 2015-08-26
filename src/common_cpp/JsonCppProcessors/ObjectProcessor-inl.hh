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

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_OBJECTPROCESSOR_INL_HH_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_OBJECTPROCESSOR_INL_HH_

#include <string>

#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/JsonCppProcessors/Common/ObjectProcessorNS/BaseItem.hh"
#include "src/common_cpp/JsonCppProcessors/Common/ObjectProcessorNS/PointerItem.hh"
#include "src/common_cpp/JsonCppProcessors/Common/ObjectProcessorNS/ConstantItem.hh"
#include "src/common_cpp/JsonCppProcessors/Common/ObjectProcessorNS/ValueItem.hh"
#include "src/common_cpp/JsonCppProcessors/Common/ObjectProcessorNS/BaseClassItem.hh"
#include "src/common_cpp/JsonCppProcessors/Common/ObjectProcessorNS/PointerRefItem.hh"
#include "src/common_cpp/JsonCppProcessors/Common/ObjectProcessorNS/PointerTRefItem.hh"
#include "src/common_cpp/JsonCppProcessors/Common/ObjectProcessorNS/PointerTRefArrayItem.hh"
#include "src/common_cpp/JsonCppProcessors/Common/ObjectProcessorNS/IgnoreItem.hh"

namespace MAUS {

template <class ObjectType>
ObjectProcessor<ObjectType>::ObjectProcessor()
    : _throws_if_unknown_branches(true), _items() {
}

template <class ObjectType>
template <class ChildType>
void ObjectProcessor<ObjectType>::RegisterPointerBranch(
                std::string branch_name,
                ProcessorBase<ChildType>* child_processor,
                ChildType* (ObjectType::*GetMethod)() const,
                void (ObjectType::*SetMethod)(ChildType* value),
                bool is_required) {
    using ObjectProcessorNS::BaseItem;
    using ObjectProcessorNS::PointerItem;
    BaseItem<ObjectType>* item = new PointerItem<ObjectType, ChildType>
              (branch_name, child_processor, GetMethod, SetMethod, is_required);
    _items[branch_name] = item;
}

template <class ObjectType>
template <class ChildType>
void ObjectProcessor<ObjectType>::RegisterPointerReference(
                std::string branch_name,
                ChildType* (ObjectType::*GetMethod)() const,
                void (ObjectType::*SetMethod)(ChildType* value),
                bool is_required) {
    using ObjectProcessorNS::BaseItem;
    using ObjectProcessorNS::PointerRefItem;
    BaseItem<ObjectType>* item = new PointerRefItem<ObjectType, ChildType>
              (branch_name, GetMethod, SetMethod, is_required);
    _items[branch_name] = item;
}

template <class ObjectType>
void ObjectProcessor<ObjectType>::RegisterTRef(
    std::string branch_name,
    TObject* (ObjectType::*GetMethod)() const,
    void (ObjectType::*SetMethod)(TObject* value),
    bool is_required) {
  using ObjectProcessorNS::BaseItem;
  using ObjectProcessorNS::PointerTRefItem;
  BaseItem<ObjectType>* item = new PointerTRefItem<ObjectType>
      (branch_name, GetMethod, SetMethod, is_required);
  _items[branch_name] = item;
}

template <class ObjectType>
void ObjectProcessor<ObjectType>::RegisterTRefArray(
    std::string branch_name,
    TRefArrayProcessor* child_processor,
    TRefArray* (ObjectType::*GetMethod)() const,
    void (ObjectType::*SetMethod)(TRefArray* value),
    bool is_required) {
  using ObjectProcessorNS::BaseItem;
  using ObjectProcessorNS::PointerTRefArrayItem;
  BaseItem<ObjectType>* item = new PointerTRefArrayItem<ObjectType>
      (branch_name, child_processor, GetMethod, SetMethod, is_required);
  _items[branch_name] = item;
}

template <class ObjectType>
template <class ChildType>
void ObjectProcessor<ObjectType>::RegisterValueBranch(
                std::string branch_name,
                ProcessorBase<ChildType>* child_processor,
                ChildType (ObjectType::*GetMethod)() const,
                void (ObjectType::*SetMethod)(ChildType value),
                bool is_required) {
    using ObjectProcessorNS::BaseItem;
    using ObjectProcessorNS::ValueItem;
    BaseItem<ObjectType>* item = new ValueItem<ObjectType, ChildType>
              (branch_name, child_processor, GetMethod, SetMethod, is_required);
    _items[branch_name] = item;
}

template <class ObjectType>
template <class ChildType>
void ObjectProcessor<ObjectType>::RegisterBaseClass(
                std::string branch_name,
                ProcessorBase<ChildType>* child_processor,
                void (ChildType::*SetMethod)(ChildType value),
                bool is_required) {
    using ObjectProcessorNS::BaseItem;
    using ObjectProcessorNS::BaseClassItem;
    BaseItem<ObjectType>* item = new BaseClassItem<ObjectType, ChildType>
        (branch_name, child_processor, SetMethod, is_required);
    _items[branch_name] = item;
}

template <class ObjectType>
void ObjectProcessor<ObjectType>::RegisterConstantBranch(
                    std::string branch_name,
                    Json::Value child_value,
                    bool is_required) {
    using ObjectProcessorNS::BaseItem;
    using ObjectProcessorNS::ConstantItem;
    BaseItem<ObjectType>* item = new ConstantItem<ObjectType>
              (branch_name, child_value, is_required);
    _items[branch_name] = item;
}

template <class ObjectType>
void ObjectProcessor<ObjectType>::RegisterIgnoredBranch(
                    std::string branch_name,
                    bool is_required) {
    using ObjectProcessorNS::BaseItem;
    using ObjectProcessorNS::IgnoreItem;
    BaseItem<ObjectType>* item = new IgnoreItem<ObjectType>
                                                    (branch_name, is_required);
    _items[branch_name] = item;
}

template <class ObjectType>
ObjectType* ObjectProcessor<ObjectType>::JsonToCpp(
    const Json::Value& json_object) {
    if (json_object.type() != Json::objectValue) {
        std::string tp = JsonWrapper::ValueTypeToString(json_object.type());
        throw(Exception(Exception::recoverable,
                     "Attempt to pass a json "+tp+" type as an object", 
                     "ObjectProcessor<ObjectType>::JsonToCpp"));
    }
    if (_throws_if_unknown_branches && HasUnknownBranches(json_object)) {
        Json::Value::Members members = json_object.getMemberNames();
        std::string unknown = "";
        for (Json::Value::Members::iterator it = members.begin();
                                                    it != members.end(); ++it) {
            if (_items.find(*it) == _items.end()) {
                unknown += *it+" ";
            }
        }
        throw(Exception(Exception::recoverable,
                     "Failed to recognise all json properties "+unknown,
                     "ObjectProcessor<ObjectType>::JsonToCpp"));
    }
    ObjectType* cpp_object = new ObjectType();
    for (my_iter it = _items.begin(); it != _items.end(); ++it) {
        try {
            it->second->SetCppChild(json_object, *cpp_object);
        } catch (Exception exc) {
            delete cpp_object;
            exc.SetMessage("In branch "+it->first+"\n"
                            +exc.GetMessage());
            throw exc;
        }
    }
    return cpp_object;
}

template <class ObjectType>
Json::Value* ObjectProcessor<ObjectType>::CppToJson
                              (const ObjectType& cpp_object, std::string path) {
    Json::Value* json_object = new Json::Value(Json::objectValue);
    JsonWrapper::Path::SetPath(*json_object, path);
    for (my_iter it = _items.begin(); it != _items.end(); ++it) {
        try {
            it->second->SetJsonChild(cpp_object, *json_object);
        } catch (Exception exc) {
            delete json_object;
            exc.SetMessage("In branch "+it->first+"\n"
                            +exc.GetMessage());
            throw exc;
        }
    }
    return json_object;
}

template <class ObjectType>
ObjectProcessor<ObjectType>::~ObjectProcessor() {
    for (my_iter it = _items.begin(); it != _items.end(); ++it) {
        delete it->second;
    }
}

template <class ObjectType>
bool ObjectProcessor<ObjectType>::HasUnknownBranches
                                              (const Json::Value& value) const {
    if (!value.isObject()) {
        std::string tp = JsonWrapper::ValueTypeToString(value.type());
        throw(Exception(Exception::recoverable,
                     "Comparison value must be a json object type - found "+tp,
                     "ObjectProcessor::HasUnknownBranches(...)"));
    }
    Json::Value::Members members = value.getMemberNames();
    for (Json::Value::Members::iterator it = members.begin();
                                                    it != members.end(); ++it) {
        if (_items.find(*it) == _items.end()) {
            return true;
        }
    }
    return false;
}

template <class ObjectType>
void ObjectProcessor<ObjectType>::SetThrowsIfUnknownBranches(bool will_throw) {
    _throws_if_unknown_branches = will_throw;
}

template <class ObjectType>
bool ObjectProcessor<ObjectType>::GetThrowsIfUnknownBranches() const {
    return _throws_if_unknown_branches;
}
}  // namespace MAUS
#endif  // #ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_OBJECTPROCESSOR_INL_HH_

