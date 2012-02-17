#include "JsonCppConverter.hh" 

#include "src/common_cpp/JsonCppProcessors/SpillProcessor.hh"

namespace MAUS {

Spill* JsonCppConverter::operator()(const Json::Value& data){
  return SpillProcessor()(data);
}

Json::Value* JsonCppConverter::operator()(const Spill& data){
  return SpillProcessor()(data);
}

}

