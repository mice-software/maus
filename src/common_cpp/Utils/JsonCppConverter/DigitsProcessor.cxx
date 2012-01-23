#include "DigitsProcessor.h"
#include <algorithm>
#include "Interface/Squeak.hh"
#include "Interface/Squeal.hh"

DigitsProcessor::DigitsProcessor(MausData* md): m_md(md),m_jv(0){}
DigitsProcessor::DigitsProcessor(MausData& md): m_md(&md),m_jv(0){}
DigitsProcessor::DigitsProcessor(Json::Value* jv): m_md(0),m_jv(jv){}
DigitsProcessor::DigitsProcessor(Json::Value& jv): m_md(0),m_jv(&jv){}
MausData* DigitsProcessor::operator()(const Json::Value& jv){
  if (jv.type()!=Json::nullValue && jv.type()!=Json::arrayValue){
    Squeak::mout(Squeak::error) <<"Expected either type Json::nullValue or Json::arrayValue"<<std::endl; 
    throw Squeal(Squeal::recoverable,"Json::Value input should be either of type Json::nullValue or Json::arrayValue","DigitsProcessor::operator()(const Json::Value&)");
  }
  if(!m_md){ m_md = new MausData(); } //Lazy loading if user picks wrong one.


  if (jv.type()==Json::nullValue){
    m_md->digits_push_back(0);
    return m_md;
  }
  DigitsElement* de = new DigitsElement();
  m_md->digits_push_back(de);
  std::for_each(jv.begin(),jv.end(),DigitProcessor(de));
  return m_md;
}

Json::Value* DigitsProcessor::operator()(const DigitsElement* de){
  if(!m_jv){ m_jv = new Json::Value(Json::arrayValue); }//lazy loading if user picks wrong one
  
  if(!de){
    m_jv->append(Json::Value(Json::nullValue));
    return m_jv;
  }

  Json::Value dv(Json::arrayValue);
  std::for_each(de->begin(),de->end(),DigitProcessor(&dv));
  m_jv->append(dv);

  return m_jv;
}


///////////////////////////////////////////////////////////////////////////

DigitProcessor::DigitProcessor(DigitsElement* de):m_de(de),m_jv(0){}
DigitProcessor::DigitProcessor(DigitsElement& de):m_de(&de),m_jv(0){}
DigitProcessor::DigitProcessor(Json::Value* jv):m_de(0),m_jv(jv){}
DigitProcessor::DigitProcessor(Json::Value& jv):m_de(0),m_jv(&jv){}
DigitsElement* DigitProcessor::operator()(const Json::Value& jv){
  if(jv.type()!=Json::objectValue){ 
    Squeak::mout(Squeak::error) <<"Expected type Json::objectValue"<<std::endl; 
    throw Squeal(Squeal::recoverable,"Json::Value input should be of type Json::objectValue","DigitProcessor::operator()(const Json::Value&)");
  }
  
  if(!jv.isMember("adc_counts")            ||
     !jv.isMember("tdc_counts")            ||
     !jv.isMember("time")                  ||
     !jv.isMember("number_photoelectrons") ||
     !jv.isMember("channel_id")            ||
     !jv.isMember("mc_mom")                ||
     !jv.isMember("mc_position")           ){
    Squeak::mout(Squeak::error) <<"Cannot find all expected content of \"digits\" branch"<<std::endl; 
    throw Squeal(Squeal::recoverable,"Not all \"digits\" content present","DigitProcessor::operator()(const Json::Value&)");
  }
  
  if(!m_de){m_de = new DigitsElement();}//lazy loading to guard against user doing wrong thing

  Digit* d = new Digit();
  m_de->push_back(d); 
  
  d->set_adc_counts(jv.get("adc_counts",Json::Value::null).asInt());
  d->set_tdc_counts(jv.get("tdc_counts",Json::Value::null).asInt());
  d->set_time(jv.get("time",Json::Value::null).asDouble());
  d->set_number_photoelectrons(jv.get("number_photoelectrons",Json::Value::null).asDouble());
  
  // ChannelID
  Json::Value ChannelIDNode = jv.get("channel_id",Json::Value::null);
  if(!ChannelIDNode.isMember("channel_number") ||
     !ChannelIDNode.isMember("fiber_number")   ||
     !ChannelIDNode.isMember("plane_number")   ||
     !ChannelIDNode.isMember("station_number") ||
     !ChannelIDNode.isMember("tracker_number") ||
     !ChannelIDNode.isMember("type")           ){
    Squeak::mout(Squeak::error) <<"Cannot find all expected content of \"digits.channel_id\" branch"<<std::endl; 
    throw Squeal(Squeal::recoverable,"Not all \"digits.channel_id\" content present","DigitProcessor::operator()(const Json::Value&)");
  }
  ChannelID c(ChannelIDNode.get("channel_number",Json::Value::null).asInt(),
	      ChannelIDNode.get("fiber_number",Json::Value::null).asInt(),
	      ChannelIDNode.get("plane_number",Json::Value::null).asInt(),
	      ChannelIDNode.get("station_number",Json::Value::null).asInt(),
	      ChannelIDNode.get("tracker_number",Json::Value::null).asInt(),
	      ChannelIDNode.get("type",Json::Value::null).asString());
  d->set_channel_id(c);
  
  // MCMomentum
  Json::Value MCMomentumNode = jv.get("mc_mom",Json::Value::null);
  if(!MCMomentumNode.isMember("x")   ||
     !MCMomentumNode.isMember("y")   ||
     !MCMomentumNode.isMember("z")   ){
    Squeak::mout(Squeak::error) <<"Cannot find all expected content of \"digits.mc_mom\" branch"<<std::endl; 
    throw Squeal(Squeal::recoverable,"Not all \"digits.mc_mom\" content present","DigitProcessor::operator()(const Json::Value&)");
  }
  MCMomentum m(MCMomentumNode.get("x",Json::Value::null).asDouble(),
	       MCMomentumNode.get("y",Json::Value::null).asDouble(),
	       MCMomentumNode.get("z",Json::Value::null).asDouble());
  d->set_mc_mom(m);
  
  // MCPosition
  Json::Value MCPositionNode = jv.get("mc_position",Json::Value::null);
  if(!MCPositionNode.isMember("x")   ||
     !MCPositionNode.isMember("y")   ||
     !MCPositionNode.isMember("z")   ){
    Squeak::mout(Squeak::error) <<"Cannot find all expected content of \"digits.mc_position\" branch"<<std::endl; 
    throw Squeal(Squeal::recoverable,"Not all \"digits.mc_position\" content present","DigitProcessor::operator()(const Json::Value&)");
  }
  MCPosition p(MCPositionNode.get("x",Json::Value::null).asDouble(),
	       MCPositionNode.get("y",Json::Value::null).asDouble(),
	       MCPositionNode.get("z",Json::Value::null).asDouble());
  d->set_mc_position(p);
 
  return m_de;
}

Json::Value* DigitProcessor::operator()(const Digit* d){
  if(!m_jv){m_jv=new Json::Value(Json::arrayValue); }//lazy loading to stop user messing up.
  
  Json::Value dv(Json::objectValue);
  
  dv["adc_counts"]=Json::Value(d->get_adc_counts());
  dv["tdc_counts"]=Json::Value(d->get_tdc_counts());
  dv["time"]=Json::Value(d->get_time());
  dv["number_photoelectrons"]=Json::Value(d->get_number_photoelectrons());


  Json::Value chid(Json::objectValue);
  chid["channel_number"] = Json::Value(d->get_channel_id().get_channel_number());
  chid["fiber_number"] = Json::Value(d->get_channel_id().get_fiber_number());
  chid["plane_number"] = Json::Value(d->get_channel_id().get_plane_number());
  chid["station_number"] = Json::Value(d->get_channel_id().get_station_number());
  chid["tracker_number"] = Json::Value(d->get_channel_id().get_tracker_number());
  chid["type"] = Json::Value(d->get_channel_id().get_type());
  dv["channel_id"] = chid;

  Json::Value mom(Json::objectValue);
  mom["x"] = Json::Value(d->get_mc_mom().get_x());
  mom["y"] = Json::Value(d->get_mc_mom().get_y());
  mom["z"] = Json::Value(d->get_mc_mom().get_z());
  dv["mc_mom"] = mom;

  Json::Value pos(Json::objectValue);
  pos["x"] = Json::Value(d->get_mc_position().get_x());
  pos["y"] = Json::Value(d->get_mc_position().get_y());
  pos["z"] = Json::Value(d->get_mc_position().get_z());
  dv["mc_position"] = pos;

  m_jv->append(dv);

  return m_jv;
}
