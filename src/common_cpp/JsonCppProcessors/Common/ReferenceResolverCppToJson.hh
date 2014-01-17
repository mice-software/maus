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

#include <set>
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
};

/** @class TypedResolver real type for resolving references
 *
 *  Holds C++ pointer-by-reference of type ChildType* to a Json object. When the
 *  data tree is filled we can call ResolveReferences to fill the references
 *  using the pointer-by-value information on the refManager
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

  private:
    ChildType* _cpp_pointer;
    std::string _json_pointer;
};

/** @class RefManager stores and subsequently resolves a list of 
 *  pointer-as-reference and a (set of) map of pointer-as-value.
 *
 *  As we traverse the data tree we collect a list of pointer-by-references to 
 *  json and a map of pointer-by-values to json. When the data tree is fully
 *  traversed we can resolve the pointer-by-references to the appropriate json
 *  address.
 *
 *  Should be called statically using GetInstance(). The issue here is
 *  that I use function overloading to determine type leading to static storage
 *  of pointer-as-value - if several data trees need to be resolved at the same
 *  time then it is possible to get them confused.
 *
 *  Call Birth() before parsing the data tree and Death() when you are finished
 *  parsing. Otherwise you will leave stray data that MAUS will attempt to reuse
 *  next time ResolveReferences() is called. We are reasonably strict about
 *  throwing Exceptions to encourage the user to be careful about Birth() and
 *  Death() for this reason.
 */
class RefManager {
  public:
    /** Get the static instance of the RefManager
     *
     *  throws a Exception if RefManager has not been Birthed.
     */
    static RefManager& GetInstance();

    /** Return true if RefManager has been birthed
     */
    static bool HasInstance();

    /** Allocate a static instance of the RefManager
     *
     *  throws a Exception if RefManager has already been birthed
     */
    static void Birth();

    /** Delete the static instance of the RefManager
     *
     *  throws a Exception if RefManager has not been birthed
     */
    static void Death();

    /** delete RefManager; if this == static instance, delete the static
     *  instance also. At the same time clears the PointerAsValue tables.
     */

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

    /** Add a mapping from Json address to C++ pointer to the manager 
     *
     *  @param json_address Json representation of the pointer-as-value (path to
     *  the data
     *  @param pointer C++ representation of the pointer-as-value
     */
    template <class PointerType>
    void SetPointerAsValue(PointerType* pointer, std::string json_address);

    /** Access the C++ representation of a json address
     *
     *  @param cpp_address C++ address of the data
     *
     *  @returns Json representation of the pointer-as-value (path to the data)
     */
    template <class PointerType>
    std::string GetPointerAsValue(PointerType* cpp_address);

  private:
    class PointerValueTable;
    template <class PointerType>
    class TypedPointerValueTable;

    template <class PointerType>
    TypedPointerValueTable<PointerType>* GetTypedPointerValueTable();

    std::set<PointerValueTable*> _value_tables;
    std::vector<Resolver*> _references;
    static RefManager* _instance;

    RefManager() {}
    inline ~RefManager();

    // disabled
    RefManager& operator=(const RefManager&);
    RefManager(const RefManager&);
};
}  // namespace CppToJson
}  // namespace ReferenceResolver
}  // namespace MAUS

#include "src/common_cpp/JsonCppProcessors/Common/ReferenceResolverCppToJson-inl.hh"

#endif

