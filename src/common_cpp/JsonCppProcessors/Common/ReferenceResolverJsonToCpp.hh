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

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_REFERENCERESOLVERJSONTOCPP_HH_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_REFERENCERESOLVERJSONTOCPP_HH_

#include <map>
#include <string>
#include <vector>

#include "json/value.h"

namespace MAUS {
namespace ReferenceResolver {
namespace JsonToCpp {

/** @class Resolver abstract type for resolving references
 *
 *  By having an untyped base class, we can put all references in the same
 *  vector and iterate without knowing about the type of the pointer referenced
 */
class Resolver {
  public:
     /** Destructor does nothing */
    virtual ~Resolver() {}

    /** Convert from Json pointer to C++ pointer*/
    virtual void ResolveReferences() = 0;

    /** Clear static list of pointers-by-value (called by RefManager delete)
     *
     *  Note this has to be non-static so that it can be called from the base
     *  class and inherited okay. virtual functions can't  be static.
     */
    virtual void ClearData() const = 0;
};

/** @class ChildTypedResolver
 *
 *  Stores a static map of json addresses to C++ addresses. We only need one
 *  type here (the type of the pointer).
 *
 *  @tparam ChildType type of the object pointed to
 */
template <class ChildType>
class ChildTypedResolver : public Resolver {
  public:
    /** Destructor does nothing */
    ~ChildTypedResolver() {}

    /** As parent */
    void ResolveReferences() = 0;

    /** Add a pointer-as-data to the hash table for subsequent dereferencing
     *
     *  @param data_json_address json path to the data
     *  @param data_cpp_address C++ address of the data
     *
     *  throws an exception if the data is already in the hash table - as this
     *  can lead to incorrect resolution of pointer-by-reference and memory
     *  duplication.
     */
    static void AddData(std::string data_json_address,
                        ChildType* data_cpp_address);

    /** Clear the hash table - ready to parse a new data tree
     */
    void ClearData() const;

  protected:
    static std::map<std::string, ChildType*> _data_hash;
};

/** @class FullyTypedResolver
 *
 *  Converts a Json pointer to a C++ pointer
 *
 *  @tparam ParentType type of object that holds the pointer
 *  @tparam ChildType type of object pointed to 
 */
template <class ParentType, class ChildType>
class FullyTypedResolver : public ChildTypedResolver<ChildType> {
  public:
    /** SetMethod function pointer for setting the C++ pointer during
     *  dereference operation
     */
    typedef void (ParentType::*SetMethod)(ChildType* value);

    /** Constructor
     *
     *  @param ref_json_address path to the json pointer-by-value (i.e. actual
     *  data in the json tree. This is stored in the "$ref" field of the json
     *  pointer-by-reference
     *  @param ref_cpp_set_func SetMethod that is used to set the C++ pointer
     *  @param ref_cpp_parent C++ object that will store the C++ pointer.
     */
    FullyTypedResolver(std::string ref_json_address,
                       SetMethod ref_cpp_set_func,
                       ParentType* ref_cpp_parent);

    /** Destructor does nothing */
    ~FullyTypedResolver() {}

    /** Resolve this reference
     *
     *  Lookup the json address in the hash table; call SetMethod to allocate
     *  the resultant C++ pointer to the parent object.
     *
     *  Note that as we store the addresses of C++ objects, deep copy of the C++
     *  data is not allowed during processing of the data tree until after we
     *  have called ResolveReferences (otherwise reference resolution will fail)
     */
    void ResolveReferences();

  private:
    SetMethod _cpp_setter;
    std::string _ref_json_address;
    ParentType* _ref_cpp_parent;
};

/** @class VectorResolver
 *
 *  Converts a Json pointer to a C++ pointer in an array (std::vector)
 *
 *  @tparam ChildType type of object pointed to; so a std::vector<ChildType*>
 */
template <class ChildType>
class VectorResolver : public ChildTypedResolver<ChildType> {
  public:
    /** Constructor
     *
     *  @param ref_json_address Address of json object
     *  @param vector The vector holding child data
     *  @param vector_index Location in the vector holding child data
     */
    VectorResolver(std::string ref_json_address,
                   std::vector<ChildType*>& vector,
                   size_t vector_index);

    /** Destructor - does nothing */
    ~VectorResolver() {}

    /** Resolve this reference
     *
     *  Lookup the json address in the hash table; call vector subscript
     *  operator to allocate the resultant C++ pointer to the parent object.
     *
     *  Note that as we store the addresses of C++ objects, deep copy of the C++
     *  data is not allowed during processing of the data tree until after we
     *  have called ResolveReferences (otherwise reference resolution will fail)
     */
    void ResolveReferences();

  private:
    std::string _ref_json_address;
    std::vector<ChildType*>& _vector;
    size_t _index;
};

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

    /** Destructor - also cleans up static hash tables */
    inline ~RefManager();

    /** Resolve references stored on the refmanager
     *
     *  Iterate over the references and call ResolveReferences() on each
     *  reference
     */
    inline void ResolveReferences();

    /** Append a reference to the manager
     */
    inline void AddReference(Resolver* reference);

  private:
    std::vector<Resolver*> _references;
    static RefManager* _instance;
};
}  // namespace JsonToCpp
}  // namespace ReferenceResolver
}  // namespace MAUS

#include "src/common_cpp/JsonCppProcessors/Common/ReferenceResolverJsonToCpp-inl.hh"

#endif

