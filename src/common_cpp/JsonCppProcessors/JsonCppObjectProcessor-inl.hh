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

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_JSONCPPOBJECTPROCESSOR_INL_HH_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_JSONCPPOBJECTPROCESSOR_INL_HH_

namespace MAUS {

template <class ParentType>
class JsonCppBaseItem {
  public:
    virtual void SetCppChild(const Json::Value& parent_json, ParentType& parent_cpp) = 0;
    virtual void SetJsonChild(const ParentType& parent_cpp, Json::Value& parent_json) = 0;

  protected:
};

template <class ParentType, class ChildType>
class JsonCppPointerItem : public JsonCppBaseItem<ParentType> {
  public:
    typedef void (ParentType::*SetMethod)(ChildType* value);
    typedef ChildType* (ParentType::*GetMethod)() const;

    JsonCppPointerItem(std::string branch_name, JsonCppProcessorBase<ChildType>* child_processor,
                GetMethod getter, SetMethod setter, bool is_required)    : JsonCppBaseItem<ParentType>(), _branch(branch_name), _processor(child_processor), _setter(setter),
      _getter(getter), _required(is_required) {
    }

    // BUG: need to handle is_required
    // Set the child in the ParentInstance
    void SetCppChild(const Json::Value& parent_json, ParentType& parent_cpp) {
        Json::Value child_json = parent_json[_branch];
        ChildType* child_cpp = _processor->JsonToCpp(child_json);
        // syntax is (_object.*_function)(args);
        (parent_cpp.*_setter)(child_cpp);
    }

    // BUG: need to handle is_required
    void SetJsonChild(const ParentType& parent_cpp, Json::Value& parent_json) {
        ChildType* child_cpp = (parent_cpp.*_getter)();
        Json::Value* child_json = _processor->CppToJson(*child_cpp);
        parent_json[_branch] = *child_json;
        delete child_json;
    }

  private:
    std::string _branch;
    JsonCppProcessorBase<ChildType>* _processor;
    SetMethod _setter;
    GetMethod _getter;
    bool      _required;
};

template <class ParentType, class ChildType>
class JsonCppValueItem : public JsonCppBaseItem<ParentType> {
  public:
    typedef void (ParentType::*SetMethod)(ChildType value);
    typedef ChildType (ParentType::*GetMethod)() const;

    JsonCppValueItem(std::string branch_name, JsonCppProcessorBase<ChildType>* child_processor,
                GetMethod getter, SetMethod setter, bool is_required)    : JsonCppBaseItem<ParentType>(), _branch(branch_name), _processor(child_processor), _setter(setter),
      _getter(getter), _required(is_required) {
    }

    // BUG: need to handle is_required
    // Set the child in the ParentInstance
    void SetCppChild(const Json::Value& parent_json, ParentType& parent_cpp) {
        Json::Value child_json = parent_json[_branch];
        ChildType* child_cpp = _processor->JsonToCpp(child_json);
        // syntax is (_object.*_function)(args);
        (parent_cpp.*_setter)(*child_cpp);
        delete child_cpp;
    }

    // BUG: need to handle is_required
    void SetJsonChild(const ParentType& parent_cpp, Json::Value& parent_json) {
        ChildType child_cpp = (parent_cpp.*_getter)();
        Json::Value* child_json = _processor->CppToJson(child_cpp);
        parent_json[_branch] = *child_json;
        delete child_json;
    }

  private:
    std::string _branch;
    JsonCppProcessorBase<ChildType>* _processor;
    SetMethod _setter;
    GetMethod _getter;
    bool      _required;
};



template <class ObjectType>
template <class ChildType>
void JsonCppObjectProcessor<ObjectType>::AddPointerBranch(
                std::string branch_name,
                JsonCppProcessorBase<ChildType>* child_processor,
//                ObjectType* object,
                ChildType* (ObjectType::*GetMethod)() const,
                void (ObjectType::*SetMethod)(ChildType* value),
                bool is_required) {
    JsonCppBaseItem<ObjectType>* item = new JsonCppPointerItem<ObjectType, ChildType>(branch_name, child_processor, GetMethod, SetMethod, is_required);
    items.push_back(item);
}

template <class ObjectType>
template <class ChildType>
void JsonCppObjectProcessor<ObjectType>::AddValueBranch(
                std::string branch_name,
                JsonCppProcessorBase<ChildType>* child_processor,
//                ObjectType* object,
                ChildType (ObjectType::*GetMethod)() const,
                void (ObjectType::*SetMethod)(ChildType value),
                bool is_required) {
    JsonCppBaseItem<ObjectType>* item = new JsonCppValueItem<ObjectType, ChildType>(branch_name, child_processor, GetMethod, SetMethod, is_required);
    items.push_back(item);
}

//    virtual void SetCppChild(ParentType& parent);
//    virtual Json::Value* GetJsonChild(const ParentType& parent);

template <class ObjectType>
ObjectType* JsonCppObjectProcessor<ObjectType>::JsonToCpp(const Json::Value& json_object) {
    ObjectType* cpp_object = new ObjectType();
    for (size_t i = 0; i < items.size(); ++i) {
        try {
            items[i]->SetCppChild(json_object, *cpp_object);
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
            items[i]->SetJsonChild(cpp_object, *json_object);
        } catch (Squeal squee) {
            delete json_object;
            throw squee;
        }
    }
    return json_object;
}

}
#endif

