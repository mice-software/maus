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

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_JSONCPPPROCESSORBASE_HH_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_JSONCPPPROCESSORBASE_HH_

#include "json/json.h"

#include "src/legacy/Interface/Squeal.hh"

namespace MAUS {

template <class CppRepresentation>
class IJsonCppProcessor {
  public:
    virtual ~IJsonCppProcessor() {}
    virtual CppRepresentation* JsonToCpp(const Json::Value& json_representation) = 0;
    virtual Json::Value* CppToJson(const CppRepresentation& cpp_representation) = 0;
};

template <class CppRepresentation>
class JsonCppProcessorBase : IJsonCppProcessor<CppRepresentation> {
  public:
    virtual CppRepresentation* JsonToCpp(const Json::Value& json_representation) = 0;
    virtual Json::Value* CppToJson(const CppRepresentation& cpp_representation) = 0;

    virtual void SetIsStrict(bool isStrict) {
        _isStrict = isStrict;
    }
    virtual bool GetIsStrict() {
        return _isStrict;
    }
  protected:
    bool _isStrict;
};

class JsonCppDoubleProcessor : public JsonCppProcessorBase<double> {
  public:
    virtual double* JsonToCpp(const Json::Value& json_double);
    virtual Json::Value* CppToJson(const double& cpp_double);
};

class JsonCppStringProcessor : public IJsonCppProcessor<std::string> {
  public:
    virtual std::string* JsonToCpp(const Json::Value& json_string);
    virtual Json::Value* CppToJson(const std::string& cpp_string);
};

class JsonCppIntProcessor : public JsonCppProcessorBase<int> {
  public:
    virtual int* JsonToCpp(const Json::Value& json_int);
    virtual Json::Value* CppToJson(const int& cpp_int);
};

class JsonCppUIntProcessor : public JsonCppProcessorBase<unsigned int> {
  public:
    virtual unsigned int* JsonToCpp(const Json::Value& json_uint);
    virtual Json::Value* CppToJson(const unsigned int& cpp_double);
};

class JsonCppBoolProcessor : public JsonCppProcessorBase<bool> {
  public:
    virtual bool* JsonToCpp(const Json::Value& json_bool);
    virtual Json::Value* CppToJson(const bool& cpp_bool);
};

template <class ArrayContents>
class JsonCppArrayProcessor : public JsonCppProcessorBase<std::vector<ArrayContents> > {
  public:
    template <class ArrayContentsProcessor>
    std::vector<ArrayContents*>* JsonToCpp(const Json::Value& json_array);
    template <class ArrayContentsProcessor>
    Json::Value* CppToJson(const std::vector<ArrayContents*>& cpp_array);
};

template <class ParentType>
class JsonCppBaseItem {
  public:
    virtual void SetCppChild(const Json::Value& parent_json, ParentType& parent_cpp) = 0;
    virtual void SetJsonChild(const ParentType& parent_cpp, Json::Value& parent_json) = 0;

  protected:
};

template <class ParentType, class ChildType>
class JsonCppItem : public JsonCppBaseItem<ParentType> {
  public:
    typedef void (ParentType::*SetMethod)(ChildType* value);
    typedef ChildType* (ParentType::*GetMethod)() const;

    JsonCppItem(std::string branch_name, JsonCppProcessorBase<ChildType>* child_processor,
                SetMethod setter, GetMethod getter, bool is_required)    : JsonCppBaseItem<ParentType>(), _branch(branch_name), _processor(child_processor), _setter(setter),
      _getter(getter), _required(is_required) {
    }

    // BUG: need to handle is_required
    // Set the child in the ParentInstance
    void SetCppChild(const Json::Value& parent_json, ParentType& parent_cpp) {
        Json::Value child_json = parent_json[_branch];
        ChildType* child_cpp = _processor->JsonToCpp(child_json);
        parent_cpp._setter(child_cpp);
    }

    // BUG: need to handle is_required
    void SetJsonChild(const ParentType& parent_cpp, Json::Value& parent_json) {
        ChildType* child_cpp = parent_cpp._getter();
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
class JsonCppObjectProcessor : public JsonCppProcessorBase<ObjectType> {
  public:
    ObjectType* JsonToCpp(const Json::Value& json_object);
    Json::Value* CppToJson(const ObjectType& cpp_instance);

    template <class ChildType>
    void add_branch(std::string branch_name,
                    JsonCppProcessorBase<ChildType>* child_processor,
                    ObjectType* object,
                    ChildType* (ObjectType::*GetMethod)() const,
                    void (ObjectType::*SetMethod)(ChildType* value),
                    bool is_required);
  private:
    std::vector< JsonCppBaseItem<ObjectType>* > items;
};

template <class ObjectType>
template <class ChildType>
void JsonCppObjectProcessor<ObjectType>::add_branch(
                std::string branch_name,
                JsonCppProcessorBase<ChildType>* child_processor,
                ObjectType* object,
                ChildType* (ObjectType::*GetMethod)() const,
                void (ObjectType::*SetMethod)(ChildType* value),
                bool is_required) {
    JsonCppBaseItem<ObjectType> item = new JsonCppItem<ObjectType, ChildType>(branch_name, child_processor, GetMethod, SetMethod, is_required);
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

/*
      nullValue = 0, ///< 'null' value
      intValue,      ///< signed integer value
      uintValue,     ///< unsigned integer value
      realValue,     ///< double value
      stringValue,   ///< UTF-8 string value
      booleanValue,  ///< bool value
      arrayValue,    ///< array value (ordered list)
      objectValue    ///< object value (collection of name/value pairs).
*/

