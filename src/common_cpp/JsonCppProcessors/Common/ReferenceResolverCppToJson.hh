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

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_REFERENCERESOLVERCPPTOJSON_HH_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_REFERENCERESOLVERCPPTOJSON_HH_

#include <map>
#include <string>
#include <vector>

#include "json/value.h"

namespace MAUS {
namespace ReferenceResolver {
namespace CppToJson {

/** @class Resolver abstract type for resolving references
 *
 *  By having an untyped base class, we can put all references in the same
 *  vector and iterate without knowing about the type of the pointer referenced
 */
class Resolver {
  public:
    /** Destructor does nothing */
    virtual ~Resolver() {}

    /** Convert from Cpp pointer to json pointer*/
    virtual void ResolveReferences(Json::Value& json_root) = 0;

    /** Clear static list of pointers-by-value (called by RefManager delete)
     *
     *  Note this has to be non-static so that it can be called from the base
     *  class and inherited okay. virtual functions can't  be static.
     */
    virtual void ClearData() const = 0;
};

/** @class TypedResolver real type for resolving references
 *
 *  Holds C++ pointer-by-reference of type ChildType* to a Json object and a
 *  static map of all pointers-by-value to associated json object. When the data
 *  tree is filled we can call ResolveReferences to fill the references.
 *
 *  @tparam ChildType the type pointed to by the C++ pointer-by-reference
 */
template <class ChildType>
class TypedResolver : public Resolver {
  public:
    /** Constructor
     *
     *  @param ref_cpp_address C++ address of the reference
     *  @param ref_json_address path through the json tree to the point where
     *  the reference should be written
     */
    TypedResolver(ChildType* ref_cpp_address, std::string ref_json_address);

    /** Destructor does nothing */
    ~TypedResolver() {}

    /** Resolve this reference
     *
     *  Lookup the C++ pointer-as-reference in the hash table and find the
     *  json path where the data is stored. Write the path to the data at
     *  the reference position in the json data structure (walking up from
     *  json_root).
     *
     *  @param json_root the ref_json_address is dereferenced using json_root
     *  as the root of the json tree.
     *
     *  throws an exception if the reference isn't in the hash table - i.e. a
     *  pointer-by-reference was stored but no pointer-by-value
     */
    void ResolveReferences(Json::Value& json_root);

    /** Add a pointer-as-data to the map for subsequent dereferencing
     *
     *  @param data_cpp_address C++ address of the data
     *  @param data_json_address json path to the data
     *
     *  throws an exception if the data is already in the hash table - as this
     *  can lead to incorrect resolution of pointer-by-reference and memory
     *  duplication.
     */
    static void AddData(ChildType* data_cpp_address,
                        std::string data_json_address);

    /** @copydoc
     *  as parent
     */
    void ClearData() const;

  private:
    ChildType* _cpp_pointer;
    std::string _json_pointer;
    static std::map<ChildType*, std::string> _data_hash;
};

/** @class RefManager stores and subsequently resolves a list of references
 *
 *  As we traverse the data tree we collect a list of pointer-by-references to 
 *  json and a map of pointer-by-values to json. When the data tree is fully
 *  traversed we can resolve the pointer-by-references to the appropriate json
 *  address.
 *
 *  Probably should be called statically using GetInstance() but this is not
 *  enforced. The issue here is the static data on child references is stored
 *  and cleared globally - if several data trees need to be resolved at the same
 *  time then it is possible to get them confused.
 *
 *  Call Birth() before parsing the data tree and Death() when you are finished
 *  parsing. Otherwise you will leave stray data that MAUS will attempt to reuse
 *  next time ResolveReferences() is called. We are reasonably strict about
 *  throwing Squeals to encourage the user to be careful about Birth() and
 *  Death() for this reason.
 */
class RefManager {
  public:
    /** Get the static instance of the RefManager
     *
     *  throws a Squeal if RefManager has not been Birthed.
     */
    static RefManager& GetInstance();

    /** Return true if RefManager has been birthed
     */
    static bool HasInstance();

    /** Allocate a static instance of the RefManager
     *
     *  throws a Squeal if RefManager has already been birthed
     */
    static void Birth();

    /** Delete the static instance of the RefManager
     *
     *  throws a Squeal if RefManager has not been birthed
     */
    static void Death();

    /** delete RefManager; if this == static instance, delete the static
     *  instance also
     */
    inline ~RefManager();

    /** Resolve references
     *
     *  Iterate over the list of references and call ResolveReferences on
     *  each child reference.
     *
     *  @param json_root the paths of the json references will be
     *  dereferenced starting from json_root.
     */
    inline void ResolveReferences(Json::Value& json_root);

    /** Add a new reference
     *
     *  @param reference Append this reference to the list of references
     */
    inline void AddReference(Resolver* reference);

  private:
    std::vector<Resolver*> _references;
    static RefManager* _instance;
};
}  // namespace CppToJson
}  // namespace ReferenceResolver
}  // namespace MAUS

#include "src/common_cpp/JsonCppProcessors/Common/ReferenceResolverCppToJson-inl.hh"

#endif

