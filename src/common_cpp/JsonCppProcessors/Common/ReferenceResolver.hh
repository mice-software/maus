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

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_REFERENCERESOLVER_HH_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_REFERENCERESOLVER_HH_

#include <map>
#include <string>
#include <vector>

#include "json/value.h"

namespace MAUS {
namespace ReferenceResolver {

class CppToJsonResolver {
  public:
      ~CppToJsonResolver() {}
      virtual void ResolveReferences(Json::Value& json_root) = 0;
      virtual void ClearData() const = 0;
};

template <class ChildType>
class TypedCppToJsonResolver : public CppToJsonResolver {
  public:
      TypedCppToJsonResolver(ChildType* ref_cpp_address, std::string ref_json_address);
      ~TypedCppToJsonResolver() {}
      void ResolveReferences(Json::Value& json_root);
      static void AddData(ChildType* data_cpp_address, std::string data_json_address);
      void ClearData() const;

  private:
      ChildType* _cpp_pointer;
      std::string _json_pointer;
      static std::map<ChildType*, std::string> _data_hash;
};

class CppToJsonManager {
    public:
        static CppToJsonManager& GetInstance();
        inline ~CppToJsonManager();
        inline void ResolveReferences(Json::Value& json_root);
        inline void AddReference(CppToJsonResolver* reference);
        
    private:
        std::vector<CppToJsonResolver*> _references;
        static CppToJsonManager* _instance;
};

class JsonToCppResolver {
  public:
    ~JsonToCppResolver() {}
    virtual void ResolveReferences() = 0;
    virtual void ClearData() const = 0;
};

template <class ChildType>
class ChildTypedJsonToCppResolver : public JsonToCppResolver {
  public:
    ~ChildTypedJsonToCppResolver() {}
    void ResolveReferences() = 0;
    static void AddData(std::string data_json_address, ChildType* data_cpp_address);
    void ClearData() const;

  protected:
    static std::map<std::string, ChildType*> _data_hash;
};

template <class ParentType, class ChildType>
class FullyTypedJsonToCppResolver : public ChildTypedJsonToCppResolver<ChildType> {
  public:
    typedef void (ParentType::*SetMethod)(ChildType* value);

    FullyTypedJsonToCppResolver(std::string ref_json_address,
                                SetMethod ref_cpp_set_func,
                                ParentType* ref_cpp_parent);
    ~FullyTypedJsonToCppResolver() {}
    void ResolveReferences();

  private:
    SetMethod _cpp_setter;
    std::string _ref_json_address;
    ParentType* _ref_cpp_parent;
};

namespace JsonToCpp {
template <class ChildType>
class VectorResolver : public ChildTypedJsonToCppResolver<ChildType> {
  public:
    VectorResolver(std::string ref_json_address,
                            std::vector<ChildType*>& vector,
                            size_t vector_index);
    ~VectorResolver() {}
    void ResolveReferences();

  private:
    std::string _ref_json_address;
    std::vector<ChildType*>& _vector;
    size_t _index;
};
}

class JsonToCppManager {
    public:
        static JsonToCppManager& GetInstance();
        inline ~JsonToCppManager();
        inline void ResolveReferences();
        inline void AddReference(JsonToCppResolver* reference);
        
    private:
        std::vector<JsonToCppResolver*> _references;
        static JsonToCppManager* _instance;
};

}  // namespace ReferenceResolver
}  // namespace MAUS

#include "src/common_cpp/JsonCppProcessors/Common/ReferenceResolver-inl.hh"

#endif

