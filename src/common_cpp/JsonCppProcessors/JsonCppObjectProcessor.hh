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

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_JSONCPPOBJECTPROCESSOR_HH_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_JSONCPPOBJECTPROCESSOR_HH_

#include "json/json.h"

#include "src/common_cpp/JsonCppProcessors/JsonCppProcessorBase.hh"

namespace MAUS {

template <class ObjectType>
class JsonCppBaseItem; // defined in JsonCppObjectProcessor-inl

template <class ObjectType, class ChildType>
class JsonCppPointerItem; // defined in JsonCppObjectProcessor-inl

template <class ObjectType, class ChildType>
class JsonCppValueItem; // defined in JsonCppObjectProcessor-inl

template <class ObjectType>
class JsonCppObjectProcessor : public JsonCppProcessorBase<ObjectType> {
  public:
    ObjectType* JsonToCpp(const Json::Value& json_object);
    Json::Value* CppToJson(const ObjectType& cpp_instance);

    template <class ChildType>
    void AddPointerBranch(std::string branch_name,
                    JsonCppProcessorBase<ChildType>* child_processor,
                    ChildType* (ObjectType::*GetMethod)() const,
                    void (ObjectType::*SetMethod)(ChildType* value),
                    bool is_required);


    template <class ChildType>
    void AddValueBranch(std::string branch_name,
                    JsonCppProcessorBase<ChildType>* child_processor,
                    ChildType (ObjectType::*GetMethod)() const,
                    void (ObjectType::*SetMethod)(ChildType value),
                    bool is_required);


  private:
    std::vector< JsonCppBaseItem<ObjectType>* > items;
};

}

#include "src/common_cpp/JsonCppProcessors/JsonCppObjectProcessor-inl.hh"

#endif

