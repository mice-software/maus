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

    virtual void SetIsStrict(bool isStrict);
    virtual bool GetIsStrict();
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
    virtual void GetJsonChild(const ParentType& parent_cpp, Json::Value& parent_json) = 0;

  protected:
};

template <class ParentType, class ChildType>
class JsonCppItem : public JsonCppBaseItem<ParentType> {
  public:
    typedef void (ParentType::*SetMethod)(ChildType* value);
    typedef ChildType* (ParentType::*GetMethod)();

    JsonCppItem(std::string branch_name, JsonCppProcessorBase<ParentType>* child_processor,
                SetMethod setter, GetMethod getter, bool is_required);

    // Set the child in the ParentInstance
    virtual void SetCppChild(const Json::Value& parent_json, ParentType& parent_cpp);
    virtual void GetJsonChild(const ParentType& parent_cpp, Json::Value& parent_json);

  private:
    std::string _branch;
    JsonCppProcessorBase<ParentType>* _processor;
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
    void push_back(JsonCppItem<ObjectType, ChildType> class_member);
  private:
    std::vector< JsonCppBaseItem<ObjectType>* > items;

};

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

