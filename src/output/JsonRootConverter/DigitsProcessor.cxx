#include "DigitsProcessor.h"

DigitsProcessor::DigitsProcessor(MsgStream::LEVEL loglevel,
				       TTree* tree):
  ProcessorBase("NEWDigitsProcessor", loglevel, tree){
  initialise("digits", m_digits);
}

bool DigitsProcessor::processEvent(Json::Value& treeRootNode){

  if(!treeRootNode.isArray()){
    m_log << MsgStream::ERROR << "Expected to find an array as the object associated with digits"<<endl;
    return false;
  }


  Json::ValueIterator rootItr = treeRootNode.begin();
  for(;rootItr!=treeRootNode.end();++rootItr){
    if((*rootItr).isNull()){
      m_digits.elements.push_back(0);
      m_log << MsgStream::VERBOSE <<"Found a null entry."<<endl;
      continue;
    }
    else if(!(*rootItr).isArray()){
      m_log << MsgStream::ERROR << "Expected array objects under digits."<<std::endl;
      return false;
    }

    
    DigitsElement *de = new DigitsElement();
    m_digits.elements.push_back(de);

    
    Json::ValueIterator vItr = (*rootItr).begin();
    for(;vItr!=(*rootItr).end();++vItr){
      Digit* d = new Digit();
      if(!(*vItr).isObject()){
	m_log << MsgStream::ERROR <<"Elements of the digits arrays should be objects."<<std::endl;
	return false;
      }
      if(!(*vItr).isMember("adc_counts")            ||
	 !(*vItr).isMember("channel_id")            ||
	 !(*vItr).isMember("mc_mom")                ||
	 !(*vItr).isMember("mc_position")           ||
	 !(*vItr).isMember("time")                  ||
	 !(*vItr).isMember("number_photoelectrons") ||
	 !(*vItr).isMember("tdc_counts")){
	m_log << MsgStream::ERROR << "One or more of the expected contents of the digits object is missing." <<std::endl;
	m_log << MsgStream::ERROR << "Expected contents is:" <<std::endl;
	m_log << MsgStream::ERROR << "\tadc_counts" <<std::endl;
	m_log << MsgStream::ERROR << "\tchannel_id" <<std::endl;
	m_log << MsgStream::ERROR << "\tmc_mom" <<std::endl;
	m_log << MsgStream::ERROR << "\tmc_position" <<std::endl;
	m_log << MsgStream::ERROR << "\tnumber_photoelectrons" <<std::endl;
	m_log << MsgStream::ERROR << "\ttdc_counts" <<std::endl;
	m_log << MsgStream::ERROR << "\ttime" <<std::endl;
	return false;
      }

      
      Json::Value node = (*vItr).get("adc_counts",Json::Value::null);
      if(node.isNull()){
	m_log << MsgStream::ERROR << "Could not extract 'adc_counts'"<<std::endl;
	return false;
      }
      if(node.isInt()) d->adc_counts=node.asInt();//m_adc_counts.push_back(node.asInt());

      node = (*vItr).get("tdc_counts",Json::Value::null);
      if(node.isNull()){
	m_log << MsgStream::ERROR << "Could not extract 'tdc_counts'"<<std::endl;
	return false;
      }
      if(node.isInt()) d->tdc_counts=node.asInt();//m_tdc_counts.push_back( node.asInt());

      node = (*vItr).get("number_photoelectrons",Json::Value::null);
      if(node.isNull()){
	m_log << MsgStream::ERROR << "Could not extract 'number_photoelectrons'"<<std::endl;
	return false;
      }
      if(node.isDouble()) d->number_photoelectrons=node.asDouble();//m_number_photoelectrons.push_back( node.asDouble());

      node = (*vItr).get("time",Json::Value::null);
      if(node.isNull()){
	m_log << MsgStream::ERROR << "Could not extract 'time'"<<std::endl;
	return false;
      }  
      if(node.isDouble()) d->time=node.asDouble();//m_time.push_back( node.asDouble());


      
      

      node = (*vItr).get("channel_id",Json::Value::null);
      if(node.isNull()){
	m_log << MsgStream::ERROR << "Could not extract 'channel_id'"<<std::endl;
	return false;
      }
      if(node.isObject()){
	if(!node.isMember("fiber_number") ||
	   !node.isMember("plane_number") ||
	   !node.isMember("station_number") ||
	   !node.isMember("tracker_number") ||
	   !node.isMember("type")){
	  m_log << MsgStream::ERROR << "One or more of the expected contents of the channel_id object is missing." <<std::endl;
	  m_log << MsgStream::ERROR << "Expected contents is:" <<std::endl;
	  m_log << MsgStream::ERROR << "\tchannel_number" <<std::endl;
	  m_log << MsgStream::ERROR << "\tfiber_number" <<std::endl;
	  m_log << MsgStream::ERROR << "\tplane_number" <<std::endl;
	  m_log << MsgStream::ERROR << "\tstation_number" <<std::endl;
	  m_log << MsgStream::ERROR << "\ttracker_number" <<std::endl;
	  m_log << MsgStream::ERROR << "\ttype" <<std::endl;
	  return false;
	}
	
	Json::Value sub_node = node.get("channel_number",Json::Value::null);
	if(sub_node.isNull()){
	  m_log << MsgStream::ERROR << "Could not extract 'channel_id/channel_number'"<<std::endl;
	  return false;
	}
	if(sub_node.isInt()) d->channel_id.channel_number =sub_node.asInt();

	sub_node = node.get("fiber_number",Json::Value::null);
	if(sub_node.isNull()){
	   m_log << MsgStream::ERROR << "Could not extract 'channel_id/fiber_number'"<<std::endl;
	  return false;
	}
	if(sub_node.isInt()) d->channel_id.fiber_number = sub_node.asInt();
	
	sub_node = node.get("plane_number",Json::Value::null);
	if(sub_node.isNull()){
	   m_log << MsgStream::ERROR << "Could not extract 'channel_id/plane_number'"<<std::endl;
	  return false;
	}
	if(sub_node.isInt()) d->channel_id.plane_number =sub_node.asInt();

	sub_node = node.get("station_number",Json::Value::null);
	if(sub_node.isNull()){
	  m_log << MsgStream::ERROR << "Could not extract 'channel_id/station_number'"<<std::endl;	 
	  return false;
	}
	if(sub_node.isInt()) d->channel_id.station_number = sub_node.asInt();

	sub_node = node.get("tracker_number",Json::Value::null);
	if(sub_node.isNull()){
	  m_log << MsgStream::ERROR << "Could not extract 'channel_id/tracker_number'"<<std::endl;	 
	  return false;
	}
	if(sub_node.isInt()) d->channel_id.tracker_number = sub_node.asInt();

	sub_node = node.get("type",Json::Value::null);
	if(sub_node.isNull()){
	  m_log << MsgStream::ERROR << "Could not extract 'channel_id/type'"<<std::endl;	 
	  return false;
	}
	if(sub_node.isString()) strcpy(d->channel_id.type,sub_node.asCString());//d->channel_id.type.assign(sub_node.asString());
      }

      node = (*vItr).get("mc_mom",Json::Value::null);
      if(node.isNull()){
	m_log << MsgStream::ERROR << "Could not extract 'mc_mom'"<<std::endl;
	return false;
      }
      if(node.isObject()){
	if(!node.isMember("x") ||
	   !node.isMember("y") ||
	   !node.isMember("z")) {
	  m_log << MsgStream::ERROR << "One or more of the expected contents of the mc_mom object is missing." <<std::endl;
	  m_log << MsgStream::ERROR << "Expected contents is:" <<std::endl;
	  m_log << MsgStream::ERROR << "\tx" <<std::endl;
	  m_log << MsgStream::ERROR << "\ty" <<std::endl;
	  m_log << MsgStream::ERROR << "\tz" <<std::endl;
	  return false;
	}
	
	Json::Value sub_node = node.get("x",Json::Value::null);
	if(sub_node.isNull()){
	  m_log << MsgStream::ERROR << "Could not extract 'mc_mom/x'"<<std::endl;
	  return false;
	}
	if(sub_node.isDouble()) d->mc_mom.x = sub_node.asDouble();

	sub_node = node.get("y",Json::Value::null);
	if(sub_node.isNull()){
	   m_log << MsgStream::ERROR << "Could not extract 'mc_mom/y'"<<std::endl;
	  return false;
	}
	if(sub_node.isDouble()) d->mc_mom.y = sub_node.asDouble();
	
	sub_node = node.get("z",Json::Value::null);
	if(sub_node.isNull()){
	   m_log << MsgStream::ERROR << "Could not extract 'mc_mom/z'"<<std::endl;
	  return false;
	}
	if(sub_node.isDouble()) d->mc_mom.z = sub_node.asDouble();
      }


      node = (*vItr).get("mc_position",Json::Value::null);
      if(node.isNull()){
	m_log << MsgStream::ERROR << "Could not extract 'mc_position'"<<std::endl;
	return false;
      }
      if(node.isObject()){
	if(!node.isMember("x") ||
	   !node.isMember("y") ||
	   !node.isMember("z")) {
	  m_log << MsgStream::ERROR << "One or more of the expected contents of the mc_position object is missing." <<std::endl;
	  m_log << MsgStream::ERROR << "Expected contents is:" <<std::endl;
	  m_log << MsgStream::ERROR << "\tx" <<std::endl;
	  m_log << MsgStream::ERROR << "\ty" <<std::endl;
	  m_log << MsgStream::ERROR << "\tz" <<std::endl;
	  return false;
	}
	
	Json::Value sub_node = node.get("x",Json::Value::null);
	if(sub_node.isNull()){
	  m_log << MsgStream::ERROR << "Could not extract 'mc_position/x'"<<std::endl;
	  return false;
	}
	if(sub_node.isDouble()) d->mc_position.x = sub_node.asDouble();

	sub_node = node.get("y",Json::Value::null);
	if(sub_node.isNull()){
	   m_log << MsgStream::ERROR << "Could not extract 'mc_position/y'"<<std::endl;
	  return false;
	}
	if(sub_node.isDouble()) d->mc_position.y = sub_node.asDouble();
	
	sub_node = node.get("z",Json::Value::null);
	if(sub_node.isNull()){
	   m_log << MsgStream::ERROR << "Could not extract 'mc_position/z'"<<std::endl;
	  return false;
	}
	if(sub_node.isDouble()) d->mc_position.z =sub_node.asDouble();
      }


      de->digits.push_back(d);
    }
  }

  return true;
}

bool DigitsProcessor::clear(){

  m_digits.clear();
  return true;
}
