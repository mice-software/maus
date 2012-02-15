#include "JsonCppConverter.hh" 
#include <algorithm>
#include "src/common_cpp/JsonCppProcessors/DigitsProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/MCProcessor.hh"
#include "Interface/Squeak.hh"
#include "Interface/Squeal.hh"


JsonCppConverter::JsonCppConverter(MausData* md): ConverterBase<Json::Value, MausData>(md){}
JsonCppConverter::JsonCppConverter(MausData& md): ConverterBase<Json::Value, MausData>(&md){}
JsonCppConverter::JsonCppConverter(Json::Value* jv): ConverterBase<Json::Value, MausData>(jv){}
JsonCppConverter::JsonCppConverter(Json::Value& jv): ConverterBase<Json::Value, MausData>(&jv){}


MausData* JsonCppConverter::operator()(const Json::Value& JsonDocument){
  Json::Value root = JsonDocument;
  if(JsonDocument.type()==Json::stringValue){
    m_reader.parse(JsonDocument.asString(),root);
  }

  if(root.type()!=Json::objectValue){
    Squeak::mout(Squeak::error) <<"Expected root conversion object to be of either Json::objectValue or Json::stringValue type"<<std::endl; 
    throw Squeal(Squeal::recoverable,"Json object other than Json::objectValue or Json::stringValue passed to operator()","JsonCppConverter::operator()(const Json::Value&)");
  }

  if(!root.isMember("digits")  ||
     !root.isMember("mc")      ){
    Squeak::mout(Squeak::warning) <<"Cannot find either \"digits\" or \"mc\" branch within Json object"<<std::endl; 
    throw Squeal(Squeal::recoverable,"Json object to convert must contain \"digits\" and \"mc\" branch objects.","JsonCppConverter::operator()(const Json::Value& JsonDocument)");
  }

  if(!m_d2){m_d2 = new MausData();}//lazy loading to stop user messing up
  m_d2->clear();

  
  Json::Value DigitsNode = root.get("digits",Json::Value::null);
  if(DigitsNode.type()==Json::arrayValue){ 
    std::for_each(DigitsNode.begin(),DigitsNode.end(),DigitsProcessor(m_d2)); 
  }
  else{   Squeak::mout(Squeak::warning) <<"Unable to extract \"digits\" object from Json::Value"<<std::endl; }
  
  Json::Value MCNode = root.get("mc",Json::Value::null);
  if(MCNode.type()==Json::arrayValue){
    std::for_each(MCNode.begin(),MCNode.end(),MCProcessor(m_d2));
  }
  else{   Squeak::mout(Squeak::warning) <<"Unable to extract \"mc\" object from Json::Value"<<std::endl; }
  
  
  return m_d2;
}

Json::Value* JsonCppConverter::operator()(const MausData& md){
  if(!m_d1){m_d1 = new Json::Value(Json::objectValue);}//lazy loading to stop user messing up.
  else if(m_d1->type()!=Json::objectValue){
    m_d1 = new (m_d1) Json::Value(Json::objectValue);
  }
  else{
    m_d1->clear();
  }
  
  const Digits& d= md.get_digits();
  Json::Value dv(Json::arrayValue);
  std::for_each(d.begin(),d.end(),DigitsProcessor(&dv));
  (*m_d1)["digits"]=dv;

  const MC& m = md.get_mc();
  Json::Value mv(Json::arrayValue);
  std::for_each(m.begin(),m.end(),MCProcessor(&mv));
  (*m_d1)["mc"]=mv;
  return m_d1;
}
