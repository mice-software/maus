/* This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
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
 *
 */

/** @class JsonWrapper
 *  \brief Wrap the Json function calls with additional error handling
 *
 *  Convenience wrapper for Json functionality. Use of Json is somewhat verbose
 *  and requires lots of error checking etc. I wrap the Json function calls here
 *  in order to reduce verbosity.
 *
 *  I am also working up some functions for hard coding the Json tree - to
 *  provide some "default" structure with as much as possible checked at compile
 *  time. Here I am worried that e.g. one guy uses "mc" branch and another guy
 *  uses e.g. "MC" branch and we end up in a mess (more likely one guy uses "px"
 *  branch and another guy uses "p_x" branch and then we have to write a whole
 *  load of converters from _this data type to _that data type - we end up in a
 *  mess.
 *
 *  Also then we can start working up to automated documentation of the
 *  "default" tree structure...
 *
 *  Nb: Json "object" is like a map<std::string, Json::Value>
 *
 *  @authors Chris Rogers <chris.rogers@stfc.ac.uk>
 */

#include <json/json.h>

#include "Interface/STLUtils.hh"

template <class T> class KDTree
{
 private:
  std::vector< KDTree<T> > children;
  T& leaf;
};

class JsonWrapper {
 public:
  /** \brief List of data types allowed by json
   */
  typedef Json::ValueType JsonType;

  /** \brief Defines a name-type pair that defines a Json Value
   */
  class JsonObject;

  /** \brief Defines a KDTree of JsonObjects that defines a Json tree
   */
  class JsonTree;

  /** \brief Read the Json configuration (global run controls)
   *
   *  \param configuration_in raw string holding the configuration information
   *
   *  Read in the configuration information and return as a Json value. Throw a
   *  Squeal if the reader fails to parse the configuration. The configuration
   *  is a dict of Json::Value, which is in itself a Json::Value.
   */
  static Json::Value ReadConfiguration(std::string configuration_in)
                                                                 throw (Squeal);

  /** \brief Get a value from a list of values
   *
   *  \param value_list list of values from which we want to find a particular
   *                    datum
   *  \param value_name name of the datum we want to get
   *
   *  Read in the configuration information and return as a Json value. Throw a
   *  Squeal if the value does not exist.
   */
  static Json::Value GetValue
         (const Json::Value& value_list, std::string value_name) throw (Squeal);

  /** \brief Read the Json data structure (hits, etc)
   *
   *  \param data_in raw string holding the configuration information
   *
   *  Read in the data information and return as a Json::Value. Throw a Squeal
   *  if the reader fails to parse the data. The configuration is a dict of
   *  Json::Value, which is in itself a Json::Value.
   */
  static Json::Value ReadData(std::string data_in) throw (Squeal);

  /** \brief Get a child branch from the Json data structure
   *
   *  \param data the Json data tree
   *  \param branch_name raw string holding the name of the branch you want
   *
   *  Attempt to access a branch from Json. If the branch does not exist, throw
   *  a Squeal.
   */
  static Json::Value GetChild
      (Json::Value data, std::string datum_name, int max_depth) throw (Squeal);

  /** \brief Get a branch from the Json data structure
   *
   *  \param parent the Json data tree
   *  \param branch_type enumeration holding the name of the branch you want
   *  \param max_depth maximum depth to search for the branch
   *
   *  Attempt to access a branch from Json and put in value. Return true if the
   *  branch was found else false.
   */
  static bool GetChild
 (Json::Value parent, JsonObject datum_object, int max_depth, Json::Value& val);

  /** \brief Get a branch from the Json data structure
   *
   *  \param parent the Json data tree
   *  \param branch_type enumeration holding the name of the branch you want
   *  \param max_depth maximum depth to search for the branch
   *
   *  Attempt to access a branch from Json. If the branch does not exist, throw
   *  a Squeal.
   */
  static Json::Value GetChildStrict
     (Json::Value parent, JsonObject datum_object, int max_depth) throw (Squeal);


  /** \brief Return true if object is in the tree below parent
   *
   *  \param parent look for object here
   *  \param object look for parent here
   *  \param depth search the tree to a maximum depth (e.g. max_depth = 1 will
   *         only search the children of parent). If max_depth is < 0, will
   *         continue until the tree is exhausted.
   *
   *  Recursively searches the tree looking for object.
   */
  static bool IsChild(Json::Value parent, JsonObject object, int max_depth);

  /** \brief Return true if the tree is reflected in the json input
   *
   *  \param json_input input tree from json
   *  \param tree input KDTree that encodes the actual structure in the tree
   *
   */
  static bool CheckTree(Json::Value json_input, JsonTree tree);

  static bool isIterable(Json::ValueType val);
 private:
  ~JsonWrapper();

  DISALLOW_COPY_AND_ASSIGN(JsonWrapper);
};

/** @class JsonObject
 *  \brief encode mapping of json value name to a particular type
 *
 *  To encode the json tree, we want to write down the permitted value names and
 *  associated types at each step of the tree
 *
 *  @authors Chris Rogers <chris.rogers@stfc.ac.uk>
 */
class JsonWrapper::JsonObject {
 public:
  JsonObject() : _name(""), _type(Json::nullValue) {;}
  JsonObject(std::string name, JsonType type) : _name(name), _type(type) {}
  ~JsonObject() {}
  std::string GetName() {return _name;}
  JsonType    GetType() {return _type;}
 private:
  std::string _name;
  JsonType    _type;
  void operator=(const JsonObject&);
};

/** @class JsonTree
 *  \brief encode json tree structure
 *
 *  Encodes the permitted value names and types at each step of the tree. At
 *  each level we want to remember:- the value name, value type, any children.
 *  We flatten the JsonTree and encode it as a Json::Value
 *
 *  @authors Chris Rogers <chris.rogers@stfc.ac.uk>
 */
class JsonWrapper::JsonTree {
  public:
    JsonTree() {}
    ~JsonTree() {}
    void Initialise(Json::Value encodedTree);
    Json::Value EncodeTree();
    Json::Value EncodeNode(std::string node_name);
    void PrintWiki(std::ostream& out);
    bool CheckTree(Json::Value json_input);
    bool HasChild(std::string) {throw("Not implemented");}
    JsonTree& GetChild(std::string) {throw("Not implemented");}
    std::vector<Json::Value> ChildrenAsValue();
    std::string DocString(std::string node_name) {throw("Not implemented");}
 private:
  // ------------- KDTree -------------- //
  std::vector<JsonTree> children;
  JsonObject leaf;

  std::string to_string(int i) {throw("Not implemented");} 

  // ------------- Formatting information ------------- //

  std::string anc_id; // string that ids the Json Value that stores ancestors
  std::string type_id;  // string that ids the Json Value as a value type
  std::string children_id;  // string that ids the Json Value as a list of
                            // permitted children
  std::string docs_id;  // string that ids the Json Value as documentation
  std::string array_name; //name to put in place of array names

  const std::map<std::string, std::string> doc_strings;
  // e.g. a tree like mc -> tracks (array) -> (array of steps) -> (object) \
  //                                                -> x y z px py pz (floats)
  //                     -> errors (object) -> bad_type (string)
  // would be encoded as
  // mc                       | object | tracks, errors | Monte Carlo output
  // mc::tracks               | array  | int            | Array of MC tracks
  // mc::tracks::int          | array  | int            | Array of track steps
  // mc::tracks::int::int     | array  | x y z px py pz | Track step pos/mom
  // mc::tracks::int::int::x  | float  |                | x position
  // mc::tracks::int::int::y  | float  |                | y position
  // mc::tracks::int::int::z  | float  |                | z position
  // mc::tracks::int::int::px | float  |                | x momentum
  // mc::tracks::int::int::py | float  |                | y momentum
  // mc::tracks::int::int::pz | float  |                | z momentum
  // mc::errors               | object | bad_type       | Allowed errors
  // mc::errors::bad_type     | string |                | Bad type found in Json
  //
  // Two issues:
  // * If a parameter name has ancestor_spacer in it then we throw an exception
  // * If a Value can take two different child types to the same parameter the
  //   parameter will be just repeated with the different child types also
  //   repeated - e.g. if mc::tracks can be an array or an object then we do:
  // mc::tracks               | array  | int             | Array of MC tracks
  // mc::tracks::int          | object | some_name       | Object of MC tracks
};


