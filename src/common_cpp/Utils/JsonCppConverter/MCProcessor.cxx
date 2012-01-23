#include "MCProcessor.h"
#include <algorithm>
#include "Interface/Squeak.hh"
#include "Interface/Squeal.hh"

MCProcessor::MCProcessor(MausData* md): m_md(md),m_jv(0){}
MCProcessor::MCProcessor(MausData& md): m_md(&md),m_jv(0){}
MCProcessor::MCProcessor(Json::Value* jv): m_md(0),m_jv(jv){}
MCProcessor::MCProcessor(Json::Value& jv): m_md(0),m_jv(&jv){}

MausData* MCProcessor::operator()(const Json::Value& jv){
  if (jv.type()!=Json::objectValue){
    Squeak::mout(Squeak::error) <<"Expected type Json::objectValue"<<std::endl; 
    throw Squeal(Squeal::recoverable,"Json::Value input should be of type Json::objectValue","MCProcessor::operator()(const Json::Value&)");
  }
  
  if(!jv.isMember("hits")       ||
     !jv.isMember("primary")    ){
    Squeak::mout(Squeak::warning) <<"Cannot find all expected content of \"mc\" branch"<<std::endl; 
    // throw Squeal(Squeal::recoverable,"Not all \"mc\" content present","MCProcessor::operator()(const Json::Value&)");
  }

  if(!m_md){m_md = new MausData(); }//lazy loading to stop user messing up

  MCHits*    mch = new MCHits();
  MCPrimary* mcp = new MCPrimary();
  MCElement* mce = new MCElement(mch,mcp);
  m_md->mc_push_back(mce);
  
  Json::Value HitsNode = jv.get("hits",Json::Value::null);
  if(HitsNode.type()==Json::arrayValue){
    std::for_each(HitsNode.begin(),HitsNode.end(),MCHitProcessor(mch));
  }
  else{ Squeak::mout(Squeak::warning) <<"Unable to extract \"mc.hits\" branch"<<std::endl; }
    
  Json::Value PrimaryNode = jv.get("primary",Json::Value::null);
  if(PrimaryNode.type()==Json::objectValue){
    MCPrimaryProcessor p(mcp);
    p(PrimaryNode);
  }
  else{ Squeak::mout(Squeak::warning) <<"Unable to extract \"mc.primary\" branch"<<std::endl; }
  
  return m_md;
}

Json::Value* MCProcessor::operator()(const MCElement* me){
  if(!m_jv){m_jv = new Json::Value(Json::arrayValue);}

  Json::Value jv(Json::objectValue);
  const MCHits& hits = me->get_hits();

  Json::Value jh(Json::arrayValue);
  std::for_each(hits.begin(),hits.end(),MCHitProcessor(&jh));
  jv["hits"]=jh;


  Json::Value jp(Json::objectValue);
  MCPrimaryProcessor p(&jp);
  p(&(me->get_primary()));
  jv["primary"]=jp;

  m_jv->append(jv);

  return m_jv;
}


/////////////////////////////////////////////////////////////////

MCHitProcessor::MCHitProcessor(MCHits* mch): m_mch(mch),m_jv(0){}
MCHitProcessor::MCHitProcessor(MCHits& mch): m_mch(&mch),m_jv(0){}
MCHitProcessor::MCHitProcessor(Json::Value* jv):m_mch(0),m_jv(jv){}
MCHitProcessor::MCHitProcessor(Json::Value& jv):m_mch(0),m_jv(&jv){}

MCHits* MCHitProcessor::operator()(const Json::Value& jv){
  if(jv.type()!=Json::objectValue){
    Squeak::mout(Squeak::error) <<"Expected type Json::objectValue"<<std::endl; 
    throw Squeal(Squeal::recoverable,"Json::Value input should be of type Json::objectValue","MCHitProcessor::operator()(const Json::Value&)");    
  }
  
  if(!jv.isMember("energy")            ||
     !jv.isMember("energy_deposited")  ||
     !jv.isMember("mass")              ||
     !jv.isMember("time")              ||
     !jv.isMember("channel_id")        ||
     !jv.isMember("particle_id")       ||
     !jv.isMember("charge")            ||
     !jv.isMember("track_id")          ){

    Squeak::mout(Squeak::error) <<"Cannot find all expected content of \"mc.hits\" branch"<<std::endl; 
    throw Squeal(Squeal::recoverable,"Not all \"mc.hits\" content present","MCHitProcessor::operator()(const Json::Value&)");
  }
  
  if(!m_mch){m_mch = new MCHits();}//lazy loading stops user messing up.
  
  MCHitsElement* mche = new MCHitsElement();
  m_mch->push_back(mche);
  
  
  mche->set_energy(jv.get("energy",Json::Value::null).asDouble());
  mche->set_energy_deposited(jv.get("energy_deposited",Json::Value::null).asDouble());
  mche->set_mass(jv.get("mass",Json::Value::null).asDouble());
  mche->set_time(jv.get("time",Json::Value::null).asDouble());
  mche->set_particle_id(jv.get("particle_id",Json::Value::null).asInt());
  mche->set_charge(jv.get("charge",Json::Value::null).asInt());
  mche->set_track_id(jv.get("track_id",Json::Value::null).asInt());
  
  
  Json::Value MCChannelIDNode = jv.get("channel_id",Json::Value::null);
  if(!MCChannelIDNode.isMember("station_number") ||
     !MCChannelIDNode.isMember("type")           ){
    Squeak::mout(Squeak::error) <<"Cannot find all expected content of \"mc.hits.channel_id\" branch"<<std::endl; 
    throw Squeal(Squeal::recoverable,"Not all \"mc.hits.channel_id\" content present","MCHitProcessor::operator()(const Json::Value&)");
  }
  MCChannelID c(MCChannelIDNode.get("station_number",Json::Value::null).asInt(),
		MCChannelIDNode.get("type",Json::Value::null).asString());

  mche->set_channel_id(c);

  return m_mch;

}

Json::Value* MCHitProcessor::operator()(const MCHitsElement* mche){
  if(!m_jv){m_jv = new Json::Value(Json::arrayValue);}//lazy loading stops user messing up

  Json::Value mv(Json::objectValue);

  mv["charge"]=Json::Value(mche->get_charge());
  mv["energy"]=Json::Value(mche->get_energy());
  mv["energy_deposited"]=Json::Value(mche->get_energy_deposited());
  mv["mass"]=Json::Value(mche->get_mass());
  mv["pid"]=Json::Value(mche->get_particle_id());
  mv["time"]=Json::Value(mche->get_time());
  mv["track_id"]=Json::Value(mche->get_track_id());

  Json::Value chid(Json::objectValue);
  chid["station_number"]=Json::Value(mche->get_channel_id().get_station_number());
  chid["type"]=Json::Value(mche->get_channel_id().get_type());
  mv["channel_id"]=chid;

  m_jv->append(mv);

  return m_jv;
}


////////////////////////////////////////////////////////////////

MCPrimaryProcessor::MCPrimaryProcessor(MCPrimary* mcp):m_mcp(mcp),m_jv(0){}
MCPrimaryProcessor::MCPrimaryProcessor(MCPrimary& mcp):m_mcp(&mcp),m_jv(0){}
MCPrimaryProcessor::MCPrimaryProcessor(Json::Value* jv): m_mcp(0),m_jv(jv){}
MCPrimaryProcessor::MCPrimaryProcessor(Json::Value& jv): m_mcp(0),m_jv(&jv){}

MCPrimary* MCPrimaryProcessor::operator()(const Json::Value& jv){
  if(jv.type()!=Json::objectValue){
    Squeak::mout(Squeak::error) <<"Expected type Json::objectValue"<<std::endl; 
    throw Squeal(Squeal::recoverable,"Json::Value input should be of type Json::objectValue","MCPrimaryProcessor::operator()(const Json::Value&)");
  }
  
  if(!jv.isMember("energy")       ||
     !jv.isMember("momentum")     ||
     !jv.isMember("position")     ||
     !jv.isMember("time")         ||
     !jv.isMember("random_seed")  ||
     !jv.isMember("particle_id")  ){
    Squeak::mout(Squeak::error) <<"Cannot find all expected content of \"mc.primary\" branch"<<std::endl; 
    throw Squeal(Squeal::recoverable,"Not all \"mc.primary\" content present","MCPrimaryProcessor::operator()(const Json::Value&)");
  }
  
  if(!m_mcp){m_mcp = new MCPrimary();}//lazy loading stops user messing up

  m_mcp->set_energy(jv.get("energy",Json::Value::null).asDouble());
  m_mcp->set_time(jv.get("time",Json::Value::null).asDouble());
  m_mcp->set_particle_id(jv.get("particle_id",Json::Value::null).asInt());
  m_mcp->set_random_seed(jv.get("random_seed",Json::Value::null).asInt());
  
  // Momentum
  Json::Value MCPrimaryMomentumNode = jv.get("momentum",Json::Value::null);
  if(!MCPrimaryMomentumNode.isMember("x")   ||
     !MCPrimaryMomentumNode.isMember("y")   ||
     !MCPrimaryMomentumNode.isMember("z")   ){
    Squeak::mout(Squeak::error) <<"Cannot find all expected content of \"mc.primary.momentum\" branch"<<std::endl; 
    throw Squeal(Squeal::recoverable,"Not all \"mc.primary.momentum\" content present","MCPrimaryProcessor::operator()(const Json::Value&)");
  }
  MCPrimaryMomentum m(MCPrimaryMomentumNode.get("x",Json::Value::null).asDouble(),
		      MCPrimaryMomentumNode.get("y",Json::Value::null).asDouble(),
		      MCPrimaryMomentumNode.get("z",Json::Value::null).asDouble());
  m_mcp->set_momentum(m);
  
  // Position
  Json::Value MCPrimaryPositionNode = jv.get("position",Json::Value::null);
  if(!MCPrimaryPositionNode.isMember("x")   ||
     !MCPrimaryPositionNode.isMember("y")   ||
     !MCPrimaryPositionNode.isMember("z")   ){
    Squeak::mout(Squeak::error) <<"Cannot find all expected content of \"mc.primary.position\" branch"<<std::endl; 
    throw Squeal(Squeal::recoverable,"Not all \"mc.primary.position\" content present","MCPrimaryProcessor::operator()(const Json::Value&)");
  }
  MCPrimaryPosition p(MCPrimaryPositionNode.get("x",Json::Value::null).asDouble(),
		      MCPrimaryPositionNode.get("y",Json::Value::null).asDouble(),
		      MCPrimaryPositionNode.get("z",Json::Value::null).asDouble());
  m_mcp->set_position(p);
  
  return m_mcp;
}


Json::Value* MCPrimaryProcessor::operator()(const MCPrimary* mp){
  if(!m_jv){m_jv = new Json::Value(Json::objectValue);}//lazy loading stops user messing up

  (*m_jv)["energy"]=Json::Value(mp->get_energy());
  (*m_jv)["particle_id"]=Json::Value(mp->get_particle_id());
  (*m_jv)["random_seen"]=Json::Value(mp->get_random_seed());
  (*m_jv)["time"]=Json::Value(mp->get_time());

  Json::Value mom(Json::objectValue);
  mom["x"] = Json::Value(mp->get_momentum().get_x());
  mom["y"] = Json::Value(mp->get_momentum().get_y());
  mom["z"] = Json::Value(mp->get_momentum().get_z());
  (*m_jv)["momentum"]=mom;

  Json::Value pos(Json::objectValue);
  pos["x"] = Json::Value(mp->get_position().get_x());
  pos["y"] = Json::Value(mp->get_position().get_y());
  pos["z"] = Json::Value(mp->get_position().get_z());
  (*m_jv)["momentum"]=pos;



  return m_jv;
}
