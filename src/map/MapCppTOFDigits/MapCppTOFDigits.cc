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

#include "Utils/CppErrorHandler.hh"
#include "Utils/JsonWrapper.hh"
#include "Utils/TOFChannelMap.hh"
#include "Utils/DAQChannelMap.hh"
#include "Interface/Squeal.hh"
#include "Interface/dataCards.hh"

#include "src/map/MapCppTOFDigits/MapCppTOFDigits.hh"

bool MapCppTOFDigits::birth(std::string argJsonConfigDocument) {
  // Check if the JSON document can be parsed, else return error only
  _classname = "MapCppTOFDigits";
  StationKeys.push_back("tof0");
  StationKeys.push_back("tof1");
  StationKeys.push_back("tof2");

  if( SetConfiguration(argJsonConfigDocument) )
    return true;  // Sucessful completion

  return false;
}


bool MapCppTOFDigits::death()  { return true;}

std::string MapCppTOFDigits::process( std::string document ) {

  //  JsonCpp setup
  Json::FastWriter writer;

  // Check if the JSON document can be parsed, else return error only
  Json::Value root = JsonWrapper::StringToJson( document );
  Json::Value xEventType = JsonWrapper::GetProperty( root, "daq_event_type", JsonWrapper::stringValue );

  if(xEventType=="physics_event" || xEventType=="calibration_event" ) {
    Json::Value xDaqData = JsonWrapper::GetProperty( root, "daq_data", JsonWrapper::objectValue );  

    Json::Value xDocTrigReq      = JsonWrapper::GetProperty( xDaqData,
                                                             "trigger_request",
                                                             JsonWrapper::arrayValue);
    
    Json::Value xDocTrig         = JsonWrapper::GetProperty( xDaqData,
                                                             "trigger",   // CHECK THIS
                                                             JsonWrapper::arrayValue);

    for(unsigned int n_station = 0; n_station < StationKeys.size(); n_station++ ) {
      Json::Value xDocDetectorData = JsonWrapper::GetProperty( xDaqData,
                                                               StationKeys[n_station],
                                                               JsonWrapper::arrayValue);

      Json::Value xDocDigits = makeDigits( xDocDetectorData, xDocTrigReq, xDocTrig );
      root["digits"][StationKeys[n_station]] = xDocDigits;
    }
  }
  
  //std::cout<<root["digits"]<<std::endl;
  return writer.write(root);
}

bool MapCppTOFDigits::SetConfiguration(std::string json_configuration) {
  //  JsonCpp setup
  Json::Value configJSON = JsonWrapper::StringToJson(json_configuration);   //  this will contain the configuration

  Json::Value map_file_name = JsonWrapper::GetProperty( configJSON, "TOF_cabling_file", JsonWrapper::stringValue);
  char* pMAUS_ROOT_DIR = getenv("MAUS_ROOT_DIR");

  if(!pMAUS_ROOT_DIR) {
    Squeak::mout(Squeak::error) << "Could not find the $MAUS_ROOT_DIR environmental variable." << std::endl;
    Squeak::mout(Squeak::error) << "Did you try running: source env.sh ?" << std::endl;
    return false;
  }
  std::string xMapFile = std::string(pMAUS_ROOT_DIR) + map_file_name.asString();
  bool loaded = _map.InitFromFile(xMapFile);
  if (!loaded)
    return false;

  return true;
}

Json::Value MapCppTOFDigits::makeDigits( Json::Value xDocDetData, 
                                         Json::Value xDocTrigReq, 
                                         Json::Value xDocTrig) {
  Json::Value xDocDigits;

  if( xDocDetData.isArray() ) {
    int n_part_events = xDocDetData.size();
    for ( int PartEvent = 0; PartEvent < n_part_events; PartEvent++ ) {
      // Get the data, trigger and trigger request for this particle event.
      Json::Value xDocPartEvent_data = JsonWrapper::GetItem( xDocDetData, PartEvent, JsonWrapper::anyValue );
      Json::Value xDocPartEvent_trigger = JsonWrapper::GetItem( xDocTrig, PartEvent, JsonWrapper::anyValue );
      Json::Value xDocPartEvent_triggerReq = JsonWrapper::GetItem( xDocTrigReq, PartEvent, JsonWrapper::anyValue );

      if (xDocPartEvent_data.isMember("V1290") && xDocPartEvent_data.isMember("V1724")) {
        Json::Value xDocTdc = JsonWrapper::GetProperty(xDocPartEvent_data, 
                                                       "V1290", 
                                                       JsonWrapper::arrayValue );

        Json::Value xDocfAdc = JsonWrapper::GetProperty(xDocPartEvent_data,
                                                        "V1724",
                                                        JsonWrapper::arrayValue );

        int n_tdc_hits = xDocTdc.size();
        Json::Value xDocPmtHits;
        for ( int TdcHitCount = 0; TdcHitCount < n_tdc_hits; TdcHitCount++ ) {
          // Get the Tdc info from the particle event. This must be done before getting
          // info from any other equipment.
          Json::Value xDocTheDigit = getTdc(xDocTdc[TdcHitCount]);

          // Get flash ADC info and appent it to the digit. The info from the Tdc is 
          // passed as well in order to find the ascosiated adc values.
          getAdc(xDocfAdc, xDocTdc[TdcHitCount], xDocTheDigit);

          // Get the trigger leading and trailing times and add them to the digit.
          getTrig(xDocPartEvent_trigger, xDocTdc[TdcHitCount], xDocTheDigit);

          // Get the trigger request leading and trailing times and add them to the digit.
          getTrigReq(xDocPartEvent_triggerReq, xDocTdc[TdcHitCount], xDocTheDigit);

          xDocPmtHits.append(xDocTheDigit);
        }
        xDocDigits[PartEvent] = xDocPmtHits;
      }
    }
  }
  return xDocDigits;
}

Json::Value MapCppTOFDigits::getTdc(Json::Value xDocTdcHit){
  std::stringstream xConv;
  Json::Value xDocInfo;

  DAQChannelKey xTdcDaqKey;
  
  std::string xDaqKey_tdc_str = JsonWrapper::GetProperty( xDocTdcHit,
                                                          "channel_key",
                                                          JsonWrapper::stringValue).asString();

  xConv << xDaqKey_tdc_str;
  xConv >> xTdcDaqKey;

  TOFChannelKey* xTofTdcKey = _map.find(&xTdcDaqKey);
  
  if (xTofTdcKey) {
    xDocInfo["tof_key"]           = xTofTdcKey->str();
    xDocInfo["station"]           = xTofTdcKey->station();
    xDocInfo["plane"]             = xTofTdcKey->plane();
    xDocInfo["slab"]              = xTofTdcKey->slab();
    xDocInfo["pmt"]               = xTofTdcKey->pmt();

    xDocInfo["part_event_number"] = JsonWrapper::GetProperty( xDocTdcHit,
                                                              "part_event_number",
                                                              JsonWrapper::intValue );

    xDocInfo["phys_event_number"] = JsonWrapper::GetProperty( xDocTdcHit,
                                                              "phys_event_number",
                                                              JsonWrapper::intValue );

    xDocInfo["leading_time"]      = JsonWrapper::GetProperty( xDocTdcHit,
                                                              "leading_time",
                                                              JsonWrapper::intValue );
    
    xDocInfo["trailing_time"]     = JsonWrapper::GetProperty( xDocTdcHit,
                                                              "trailing_time",
                                                              JsonWrapper::intValue );
    
    xDocInfo["trigger_time_tag"]  = JsonWrapper::GetProperty( xDocTdcHit,
                                                              "trigger_time_tag",
                                                              JsonWrapper::intValue );

    xDocInfo["time_stamp"]        = JsonWrapper::GetProperty( xDocTdcHit,
                                                              "time_stamp",
                                                              JsonWrapper::intValue );
  }
  //std::cout << xDocInfo << std::endl;    
  return xDocInfo;
}

bool MapCppTOFDigits::getAdc(Json::Value xDocfAdc, Json::Value xDocTdcHit, Json::Value &xDocDigit) {

  int n_Adc_hits = xDocfAdc.size();
  std::string xTofKey_str = JsonWrapper::GetProperty( xDocDigit, 
                                                          "tof_key", 
                                                          JsonWrapper::stringValue).asString();

  TOFChannelKey xTdcTofKey(xTofKey_str);

  for ( int AdcHitCount = 0; AdcHitCount < n_Adc_hits; AdcHitCount++ ) {
    std::string xDaqKey_adc_str = JsonWrapper::GetProperty( xDocfAdc[AdcHitCount],
                                                       "channel_key",
                                                       JsonWrapper::stringValue).asString();

    DAQChannelKey xAdcDaqKey(xDaqKey_adc_str);
    TOFChannelKey* xAdcTofKey = _map.find(&xAdcDaqKey);
    
    if ( xTdcTofKey == *xAdcTofKey ){
      xDocDigit["charge_mm"] = JsonWrapper::GetProperty( xDocfAdc[AdcHitCount],
                                                        "charge_mm",
                                                        JsonWrapper::intValue );
      xDocDigit["charge_pm"] = JsonWrapper::GetProperty( xDocfAdc[AdcHitCount],
                                                        "charge_pm",
                                                        JsonWrapper::intValue );
      return true;
    }
  }

  return false;
}

bool MapCppTOFDigits::getTrig( Json::Value xDocTrig, Json::Value xDocTdcHit, Json::Value &xDocDigit ) {
  Json::Value xDocT = JsonWrapper::GetProperty( xDocTrig, "V1290", JsonWrapper::arrayValue);
  int HitGeo = JsonWrapper::GetProperty(xDocTdcHit , "geo", JsonWrapper::intValue).asInt();
  int n_count = xDocT.size();
  //loop over all of the triggers for this particle event.
  for ( int TrigCount = 0; TrigCount < n_count; TrigCount++ ){
    Json::Value Trig = JsonWrapper::GetItem( xDocT, TrigCount, JsonWrapper::objectValue);

    int TrGeo = JsonWrapper::GetProperty(Trig, "geo", JsonWrapper::intValue).asInt();
    // If the Geo number of the trigger request matches the Geo number for the TDC then add the 
    // trigger information to the digit.
    if ( TrGeo == HitGeo ){
      xDocDigit["trigger_leading_time"]  = JsonWrapper::GetProperty( Trig,
                                                                     "leading_time",
                                                                     JsonWrapper::intValue);
      xDocDigit["trigger_trailing_time"] = JsonWrapper::GetProperty( Trig,
                                                                     "trailing_time",
                                                                     JsonWrapper::intValue);

      return true;  // will break the loop  and return truewhen it finds the first trigger that matches.
    }
  }

  return false;	
}

bool MapCppTOFDigits::getTrigReq( Json::Value xDocTrigReq, Json::Value xDocTdcHit, Json::Value &xDocDigit ) {
  Json::Value xDocTR = JsonWrapper::GetProperty( xDocTrigReq, "V1290", JsonWrapper::arrayValue);
  int HitGeo = JsonWrapper::GetProperty(xDocTdcHit , "geo", JsonWrapper::intValue).asInt();
  int n_req_count = xDocTR.size();

  //loop over all of the trigger requests for this particle event.
  for ( int TrigReqCount = 0; TrigReqCount < n_req_count; TrigReqCount++ ){
    Json::Value TrigReq = JsonWrapper::GetItem( xDocTR, TrigReqCount, JsonWrapper::objectValue);

    int TrReqGeo = JsonWrapper::GetProperty(TrigReq, "geo", JsonWrapper::intValue).asInt();
    // If the Geo number of the trigger request matches the Geo number for the TDC then add the 
    // trigger request information to the digit.
    if ( TrReqGeo == HitGeo ){
      xDocDigit["trigger_request_leading_time"]  = JsonWrapper::GetProperty( TrigReq,
                                                                             "leading_time",
                                                                             JsonWrapper::intValue);

      xDocDigit["trigger_request_trailing_time"] = JsonWrapper::GetProperty( TrigReq,
                                                                             "trailing_time",
                                                                             JsonWrapper::intValue);
      return true;  // will break the loop when it finds the first request that matches. there may be
      // multiple requests per board but for now we only get the first one.
    }
  }

  return false;
}
