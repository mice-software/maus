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

#include "src/legacy/Interface/Squeal.hh"

#include "src/common_cpp/JsonCppProcessors/JsonCppProcessorBase.hh"

namespace MAUS {

template <class ArrayContents>
template<class ArrayContentsProcessor>
std::vector<ArrayContents*>* JsonCppArrayProcessor<ArrayContents>::JsonToCpp
                                               (const Json::Value& json_array) {
    if (!json_array.isConvertibleTo(Json::arrayValue)) {
        if (JsonCppProcessorBase<ArrayContents>::_isStrict) {
            // no memory allocated yet...
            throw(Squeal(Squeal::recoverable, 
                        "\nFailed to resolve Json::Value to array",
                        "JsonCppArrayProcessor::JsonToCpp()"
                        ) );
        } else {
            return NULL;
        } 
    }
    std::vector<ArrayContents*>* vec = new std::vector<ArrayContents*>(json_array.size());

    ArrayContentsProcessor proc;
    proc.SetIsStrict(JsonCppProcessorBase<ArrayContents>::_isStrict);
    for (size_t i = 0; i < json_array.size(); ++i) {
        try {
            // allocate the vector
            ArrayContents* data = proc.JsonToCpp(json_array[i]);
            vec.push_back(data);
        } catch (Squeal squee) {
            // if there's a problem, clean up before rethrowing the exception
            for (size_t j = 0; j < vec.size(); ++j) {
                if (vec[j] != NULL) {
                    delete vec[j];
                }
            }
            delete vec;
            throw squee;
        }
    }
    return vec;
}


template <class ArrayContents>
template<class ArrayContentsProcessor>
Json::Value* JsonCppArrayProcessor<ArrayContents>::
                      CppToJson(const std::vector<ArrayContents*>& cpp_array) {
    Json::Value* json_array = new Json::Value(Json::arrayValue);
    json_array->resize(cpp_array.size());

    ArrayContentsProcessor proc;
    proc.SetIsStrict(JsonCppProcessorBase<ArrayContents>::_isStrict);
    for (size_t i = 0; i < cpp_array.size(); ++i) {
        try {
            Json::Value* data = proc.JsonToCpp(cpp_array[i]);
            json_array[i] = *data; // json copies memory here
            delete data; // so we need to clean up here
        } catch (Squeal squee) {
            // if there's a problem, clean up before rethrowing the exception
            delete json_array;
            throw squee;
        }
    }
    return json_array;

}

template <class ObjectType>
template <class ChildType>
void JsonCppObjectProcessor<ObjectType>::push_back(JsonCppItem<ObjectType, ChildType> class_member) {
    JsonCppBaseItem<ObjectType> item = new JsonCppItem<ObjectType, ChildType>(class_member);
    items.push_back(item);
}
//    virtual void SetCppChild(ParentType& parent);
//    virtual Json::Value* GetJsonChild(const ParentType& parent);

template <class ObjectType>
ObjectType* JsonCppObjectProcessor<ObjectType>::JsonToCpp(const Json::Value& json_object) {
    ObjectType* cpp_object = new ObjectType();
    for (size_t i = 0; i < items.size(); ++i) {
        try {
            items[i].SetCppChild(json_object, *cpp_object);
        } catch (Squeal squee) {
            delete cpp_object;
            throw squee;
        }
    }
    return cpp_object;
}

template <class ObjectType>
Json::Value* JsonCppObjectProcessor<ObjectType>::CppToJson(const ObjectType& cpp_object) {
    Json::Value* json_object = new Json::Value(Json::objectValue);
    for (size_t i = 0; i < items.size(); ++i) {
        try {
            items[i].SetJsonChild(cpp_object, *json_object);
        } catch (Squeal squee) {
            delete json_object;
            throw squee;
        }
    }
    return json_object;
}

template <class ObjectType>
template <class ChildType>
JsonCppItem<ObjectType, ChildType>::JsonCppItem<ObjectType, ChildType>
    (std::string branch_name, JsonCppProcessorBase<ObjectType>* child_processor,
                         SetMethod setter, GetMethod getter, bool is_required) 
    : _branch(branch_name), _processor(child_processor), _setter(setter),
      _getter(getter), _require(is_required) {   
}

template <class ObjectType>
template <class ChildType>
void JsonCppItem<ObjectType, ChildType>::SetCppChild(const Json::Value& parent_json, ObjectType& parent_cpp) {
    
}

template <class ObjectType>
template <class ChildType>
void JsonCppItem<ObjectType, ChildType>::GetJsonChild(const ObjectType& parent_cpp, Json::Value& parent_json) {
}


    // Set the child in the ParentInstance
//    virtual void SetCppChild(const Json::Value& parent_json, ParentType& parent_cpp);
//    virtual void GetJsonChild(const ParentType& parent_cpp, Json::Value& parent_json);




}



