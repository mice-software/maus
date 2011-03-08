#include <json/json.h>

#include "Squeal.hh"
#include "JsonWrapper.hh"

Json::Value JsonWrapper::ReadConfiguration(std::string configuration_in) 
                                                                throw (Squeal) {
  Json::Reader reader;
  Json::Value  configuration_out;
  bool parsingSuccessful = reader.parse(configuration_in, configuration_out);
  if (!parsingSuccessful) {
    throw(Squeal(Squeal::recoverable,
          "Failed to parse Json configuration",
          "JsonWrapper::ReadConfiguration()"));
  }
  return configuration_out;
}

Json::Value GetValue(const Json::Value& value_list, std::string value_name)
                                                                throw (Squeal) {
  if (!value_list.isMember(value_name)) {
    throw(Squeal(Squeal::recoverable,
                 "Failed to find "+value_name+" in Json value list",
                 "JsonWrapper::GetValue"));
  }
  return value_list[value_name];
}

Json::Value JsonWrapper::ReadData(std::string data_in) throw (Squeal) {
  Json::Reader reader;
  Json::Value  data_out;
  bool parsingSuccessful = reader.parse(data_in, data_out);
  if (!parsingSuccessful) {
    throw(Squeal(Squeal::recoverable,
          "Failed to parse Json data",
          "JsonWrapper::ReadData()"));
  }
  return data_out;
}

bool JsonWrapper::GetChild
        (Json::Value data, JsonObject object, int max_depth, Json::Value& val) {
  if (data.isMember(object.GetName()) && 
      data[object.GetName()].type() == object.GetType()) {
    val = data[object.GetName()];
    return true;
  }
  if (max_depth > 1 or max_depth < 0)
    for (Json::Value::iterator it = data.begin(); it != data.end(); ++it) {
      return GetChild(*it, object, max_depth-1, val);
    }
  return false;
}


Json::Value JsonWrapper::GetChildStrict
           (Json::Value data, JsonObject object, int max_depth) throw (Squeal) {
  // nb we don't want the exception in the recursion so we only make exceptions
  // at the top level
  Json::Value value;
  if(!GetChild(data, object, max_depth, value)) {
    throw(Squeal(Squeal::recoverable, "Failed to get value "+
                  object.GetName()+" from json tree", 
                  "JsonWrapper::GetChildStrict"));
  }
  return value;  
}

bool JsonWrapper::IsChild
                        (Json::Value parent, JsonObject object, int max_depth) {
  if (parent.isMember(object.GetName()) && 
      parent[object.GetName()].type() == object.GetType()) return true;
  if (max_depth > 1 or max_depth < 0)
    for (Json::Value::iterator it = parent.begin(); it != parent.end(); ++it) {
      return IsChild(*it, object, max_depth-1);
    }
  return false;
}

/*
void JsonWrapper::JsonTree::Initialise(Json::Value encodedTree) {
  for ();
}

Json::Value JsonWrapper::JsonTree::
  EncodeTree(Json::Value val, std::string ancestors) {
  for (std::vector<JsonObject>::iterator it = children.begin();
                                                   it != children.end(); ++it) {
    (*it)->
  }
}

  for (Json::Members::iterator it = member_list.begin(); 
                                              it != member_list.end() ++it) {
    std::string new_ancestor = ancestors+spacer+(*it);
    if (ancestors == "") new_ancestor = (*it); // no spacer for top level
    ConvertToString(val[*it], new_ancestor, spacer);
  }
}
*/

Json::Value JsonWrapper::JsonTree::EncodeNode(std::string node_name) {

  Json::Value node(Json::objectValue);
  node[type_id] = Json::Value(leaf.GetType());
  node[docs_id] = Json::Value(DocString(node_name));
  if (leaf.GetType() == Json::objectValue) {
    Json::Value val(Json::arrayValue);
    for (size_t i=0; i<children.size(); ++i) {
      val[i] = Json::Value(children[i].leaf.GetName());
    }
    node[children_id] = val;
  } else if (leaf.GetType() == Json::arrayValue) {
    Json::Value val(Json::arrayValue);
    val[static_cast<unsigned int>(0)] = Json::Value(array_name);
    node[children_id] = val;
  }
  return node;
}

bool JsonWrapper::JsonTree::CheckTree(Json::Value val) {
  typedef Json::Value::Members MList; // is a std::vector<std::string>
  if (val.type() != leaf.GetType()) return false;
  bool testpass = true;
  // if value is iterable (Json::array or Json::object) then iterate over
  // children and return result. Else just return true (we already checked this
  // lead)
  switch (val.type()) {
    case Json::objectValue: { // stupid C++ scoping problem so need braces
      MList ml = val.getMemberNames();
      for (MList::iterator it = ml.begin(); it != ml.end(); ++it) {
        if (!HasChild(*it)) return false;
        else testpass &= GetChild(*it).CheckTree(val[*it]);
      }
      break;
    }
    case Json::arrayValue: // CHECK - what do I want to put here?
      for (size_t i = 0; i < val.size(); ++i) {
        std::string i_str = to_string(i);
        if (!HasChild(i_str)) return false;
        else testpass &= GetChild(i_str).CheckTree(val[i]);
      }
      break;
    default: // not an iterable type so no need to check children
      return true;
  }
  return testpass;
}




