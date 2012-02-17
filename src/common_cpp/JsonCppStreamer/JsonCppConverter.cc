#include "JsonCppConverter.hh" 

#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/JsonCppProcessors/SpillProcessor.hh"

namespace MAUS {

Spill* JsonCppConverter::operator()(const Json::Value& data){
  return SpillProcessor().JsonToCpp(data);
}

Json::Value* JsonCppConverter::operator()(const Spill& data){
  return SpillProcessor().CppToJson(data);
}

}

