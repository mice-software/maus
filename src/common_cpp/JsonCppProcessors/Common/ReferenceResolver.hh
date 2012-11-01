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
}  // namespace ReferenceResolver
}  // namespace MAUS

#include "src/common_cpp/JsonCppProcessors/Common/ReferenceResolver-inl.hh"

#endif

