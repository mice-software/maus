#include "JsonCppConverter.hh" 

#include "src/common_cpp/DataStructure/TestSpill.hh"

namespace MAUS {

TestSpill* JsonCppConverter::operator()(const Json::Value& data){
  return new TestSpill();
}

Json::Value* JsonCppConverter::operator()(const TestSpill& data){
  return new Json::Value;
}

}

