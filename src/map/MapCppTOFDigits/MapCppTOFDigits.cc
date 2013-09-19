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
#include "Utils/Exception.hh"
#include "Interface/dataCards.hh"

#include "src/map/MapCppTOFDigits/MapCppTOFDigits.hh"

namespace MAUS {

bool MapCppTOFDigits::birth(std::string argJsonConfigDocument) {

  _classname = "MapCppTOFDigits";
  _stationKeys.push_back("tof0");
  _stationKeys.push_back("tof1");
  _stationKeys.push_back("tof2");

  char* pMAUS_ROOT_DIR = getenv("MAUS_ROOT_DIR");

  if (!pMAUS_ROOT_DIR) {
    Squeak::mout(Squeak::error)
    << "Could not find the $MAUS_ROOT_DIR environmental variable." << std::endl;
    Squeak::mout(Squeak::error) << "Did you try running: source env.sh ?" << std::endl;
    return false;
  }

  // Check if the JSON document can be parsed, else return error only
  try {
    //  JsonCpp setup
    Json::Value configJSON;
    Json::Value map_file_name;
    Json::Value xEnable_V1290_Unpacking;
    Json::Value xEnable_V1724_Unpacking;
    configJSON = JsonWrapper::StringToJson(argJsonConfigDocument);
    //  this will contain the configuration

/*
    map_file_name = JsonWrapper::GetProperty(configJSON,
                                             "TOF_cabling_file",
                                             JsonWrapper::stringValue);

    std::string xMapFile = std::string(pMAUS_ROOT_DIR) + map_file_name.asString();
    bool loaded = _map.InitFromFile(xMapFile);
*/
    map_init = true;
    bool loaded = _map.InitializeCards(configJSON);
    if (!loaded)
      map_init = false;

    xEnable_V1290_Unpacking = JsonWrapper::GetProperty(configJSON,
                                                       "Enable_V1290_Unpacking",
                                                       JsonWrapper::booleanValue);
    xEnable_V1724_Unpacking = JsonWrapper::GetProperty(configJSON,
                                                       "Enable_V1724_Unpacking",
                                                       JsonWrapper::booleanValue);

    if (!xEnable_V1290_Unpacking.asBool()) {
      Squeak::mout(Squeak::warning)
      << "WARNING in MapCppTOFDigits::birth. The unpacking of the TDC V1290 is disabled!!!"
      << " Are you shure you want this?"
      << std::endl;
    }
    if (!xEnable_V1724_Unpacking.asBool()) {
      Squeak::mout(Squeak::warning)
      << "WARNING in MapCppTOFDigits::birth. The unpacking of the flashADC V1724 is disabled!!!"
      << " Are you shure you want this?"
      << std::endl;
    }

  return true;
  } catch(Exception exception) {
    MAUS::CppErrorHandler::getInstance()->HandleExceptionNoJson(exception, _classname);
  } catch(std::exception exc) {
    MAUS::CppErrorHandler::getInstance()->HandleStdExcNoJson(exc, _classname);
  }

  return false;
}


bool MapCppTOFDigits::death()  {return true;}

std::string MapCppTOFDigits::process(std::string document) {
  //  JsonCpp setup
  Json::FastWriter writer;
  Json::Value root;
  Json::Value xEventType;
  // don't try to process if we have not initialized the channel map
  // this could be because of an invalid map
  // or because the CDB is down
  if (!map_init) {
      Json::Value errors;
      std::stringstream ss;
      ss << _classname << " says: Failed to intialize channel map";
      errors["no_channel_map"] = ss.str();
      root["errors"] = errors;
      return writer.write(root);
  }
  // Check if the JSON document can be parsed, else return error only
  try {root = JsonWrapper::StringToJson(document);}
  catch(...) {
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says: Failed to parse input document";
    errors["bad_json_document"] = ss.str();
    root["errors"] = errors;
    return writer.write(root);
  }

  try {
    xEventType = JsonWrapper::GetProperty(root,
                                          "daq_event_type",
                                          JsonWrapper::stringValue);
    if (xEventType == "physics_event" || xEventType == "calibration_event") {
      Json::Value xDaqData = JsonWrapper::GetProperty(root, "daq_data", JsonWrapper::objectValue);
      Json::Value xDocTrig, xDocTrigReq;
      if (xDaqData.isMember("trigger") &&
          xDaqData.isMember("trigger_request")) {

        xDocTrigReq = JsonWrapper::GetProperty(xDaqData,
                                               "trigger_request",
                                               JsonWrapper::arrayValue);

        xDocTrig = JsonWrapper::GetProperty(xDaqData,
                                            "trigger",
                                            JsonWrapper::arrayValue);

        Json::Value xDocAllDigits; // list by station then by event
        unsigned int n_events = 0;
        for (unsigned int n_station = 0; n_station < _stationKeys.size(); n_station++) {
          if (xDaqData.isMember(_stationKeys[n_station])) {
            Json::Value xDocDetectorData = JsonWrapper::GetProperty(xDaqData,
                                                                    _stationKeys[n_station],
                                                                    JsonWrapper::arrayValue);
            // list of events for station n_station
            xDocAllDigits[n_station] = makeDigits
                                    (xDocDetectorData, xDocTrigReq, xDocTrig);
            n_events = xDocAllDigits[n_station].size();
          }
        }
        for (unsigned int ev = 0; ev < n_events; ev++) {
          Json::Value xDocTofDigits(Json::objectValue);
          for (unsigned int stat = 0; stat < _stationKeys.size(); stat++) {
            if (xDocAllDigits[stat][ev].type() == Json::arrayValue) {
              xDocTofDigits[_stationKeys[stat]] = xDocAllDigits[stat][ev];
            } else {
              xDocTofDigits[_stationKeys[stat]] = Json::Value(Json::arrayValue);
            }
          }
          root["recon_events"][ev]["tof_event"]["tof_digits"] = xDocTofDigits;
        }
      }
    }
  } catch(Exception exception) {
    root = MAUS::CppErrorHandler::getInstance()
                                       ->HandleException(root, exception, _classname);
  } catch(std::exception exc) {
    root = MAUS::CppErrorHandler::getInstance()
                                         ->HandleStdExc(root, exc, _classname);
  }
  // if (root.isMember("digits")) std::cout<<root["digits"]<<std::endl;
  return writer.write(root);
}

bool MapCppTOFDigits::SetConfiguration(std::string json_configuration) {

  return true;
}

Json::Value MapCppTOFDigits::makeDigits(Json::Value xDocDetData,
                                        Json::Value xDocTrigReq,
                                        Json::Value xDocTrig) {
  Json::Value xDocDigits;
  // Get number of Particle trigger.
  int n_part_event_triggers = xDocTrig.size();
  xDocDigits.resize(n_part_event_triggers);

  if ( xDocDetData.isArray() ) {
    // Get number of digits in the detector record. It can be different from
    // the number of Particle triggers because of the Zero Suppression.
    int n_part_events = xDocDetData.size();
    for ( int PartEvent = 0; PartEvent < n_part_events; PartEvent++ ) {
      // Get the data, trigger and trigger request for this particle event.
      Json::Value xDocPartEvent_data = JsonWrapper::GetItem(xDocDetData,
                                                            PartEvent,
                                                            JsonWrapper::anyValue);

      Json::Value xDocPartEvent_trigger = JsonWrapper::GetItem(xDocTrig,
                                                               PartEvent,
                                                               JsonWrapper::anyValue );

      Json::Value xDocPartEvent_triggerReq = JsonWrapper::GetItem(xDocTrigReq,
                                                                  PartEvent,
                                                                  JsonWrapper::anyValue);

      if (xDocPartEvent_data.isMember("V1290") && xDocPartEvent_data.isMember("V1724")) {
        Json::Value xDocTdc = JsonWrapper::GetProperty(xDocPartEvent_data,
                                                       "V1290",
                                                       JsonWrapper::arrayValue);

        Json::Value xDocfAdc = JsonWrapper::GetProperty(xDocPartEvent_data,
                                                        "V1724",
                                                        JsonWrapper::arrayValue);

        int n_tdc_hits = xDocTdc.size();
        Json::Value xDocPmtHits;
        for ( int TdcHitCount = 0; TdcHitCount < n_tdc_hits; TdcHitCount++ ) {
          // Get the Tdc info from the particle event. This must be done before getting
          // info from any other equipment.
          Json::Value xDocTheDigit = getTdc(xDocTdc[TdcHitCount]);

          // Get flash ADC info and appent it to the digit. The info from the Tdc is
          // passed as well in order to find the ascosiated adc values.
          if (!getAdc(xDocfAdc, xDocTdc[TdcHitCount], xDocTheDigit)) {
             xDocTheDigit["charge_mm"] = 0;
             xDocTheDigit["charge_pm"] = 0;
          };

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

Json::Value MapCppTOFDigits::getTdc(Json::Value xDocTdcHit) {
  std::stringstream xConv;
  Json::Value xDocInfo;

  DAQChannelKey xTdcDaqKey;

  std::string xDaqKey_tdc_str = JsonWrapper::GetProperty(xDocTdcHit,
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

    xDocInfo["part_event_number"] = xDocTdcHit["part_event_number"];
    xDocInfo["phys_event_number"] = xDocTdcHit["phys_event_number"];
    xDocInfo["leading_time"]      = xDocTdcHit["leading_time"];
    xDocInfo["trailing_time"]     = xDocTdcHit["trailing_time"];
    xDocInfo["trigger_time_tag"]  = xDocTdcHit["trigger_time_tag"];
    xDocInfo["time_stamp"]        = xDocTdcHit["time_stamp"];
  }

  return xDocInfo;
}

bool MapCppTOFDigits::getAdc(Json::Value xDocfAdc,
                             Json::Value xDocTdcHit,
                             Json::Value &xDocDigit) throw(Exception) {

  int n_Adc_hits = xDocfAdc.size();
  std::string xTofKey_str = JsonWrapper::GetProperty(xDocDigit,
                                                     "tof_key",
                                                     JsonWrapper::stringValue).asString();

  TOFChannelKey xTdcTofKey(xTofKey_str);

  for (int AdcHitCount = 0; AdcHitCount < n_Adc_hits; AdcHitCount++) {
    std::string xDaqKey_adc_str = JsonWrapper::GetProperty(xDocfAdc[AdcHitCount],
                                                           "channel_key",
                                                           JsonWrapper::stringValue).asString();

    DAQChannelKey xAdcDaqKey(xDaqKey_adc_str);
    TOFChannelKey* xAdcTofKey = _map.find(&xAdcDaqKey);

    if (xTdcTofKey == *xAdcTofKey) {
      xDocDigit["charge_mm"] = JsonWrapper::GetProperty(xDocfAdc[AdcHitCount],
                                                        "charge_mm",
                                                        JsonWrapper::intValue );
      xDocDigit["charge_pm"] = JsonWrapper::GetProperty(xDocfAdc[AdcHitCount],
                                                        "charge_pm",
                                                        JsonWrapper::intValue );

      if (!xDocfAdc[AdcHitCount].isMember("charge_mm"))
          xDocDigit["charge_mm"] = 0;
      if (!xDocfAdc[AdcHitCount].isMember("charge_pm"))
          xDocDigit["charge_pm"] = 0;
      if (xDocDigit["part_event_number"] != xDocfAdc[AdcHitCount]["part_event_number"]) {
        throw(Exception(Exception::recoverable,
              std::string("Wrong part_event_number!"),
              "MapCppTOFDigits::getAdc"));
      }
      if (xDocDigit["phys_event_number"] != xDocfAdc[AdcHitCount]["phys_event_number"]) {
        throw(Exception(Exception::recoverable,
              std::string("Wrong phys_event_number!"),
              "MapCppTOFDigits::getAdc"));
      }

      return true;
    }
  }

  return false;
}

bool MapCppTOFDigits::getTrig(Json::Value xDocTrig,
                              Json::Value xDocTdcHit,
                              Json::Value &xDocDigit ) throw(Exception) {
  Json::Value xDocT = JsonWrapper::GetProperty(xDocTrig, "V1290", JsonWrapper::arrayValue);
  int HitGeo = JsonWrapper::GetProperty(xDocTdcHit , "geo", JsonWrapper::intValue).asInt();
  int n_count = xDocT.size();
  // Loop over all of the triggers for this particle event.
  for (int TrigCount = 0; TrigCount < n_count; TrigCount++) {
    Json::Value Trig = JsonWrapper::GetItem(xDocT, TrigCount, JsonWrapper::objectValue);

    int TrGeo = JsonWrapper::GetProperty(Trig, "geo", JsonWrapper::intValue).asInt();
    // If the Geo number of the trigger request matches the Geo number for the TDC then add the
    // trigger information to the digit.
    if ( TrGeo == HitGeo ) {
      xDocDigit["trigger_leading_time"]  = JsonWrapper::GetProperty(Trig,
                                                                    "leading_time",
                                                                    JsonWrapper::intValue);
      xDocDigit["trigger_trailing_time"] = JsonWrapper::GetProperty(Trig,
                                                                    "trailing_time",
                                                                    JsonWrapper::intValue);

      if (xDocDigit["part_event_number"] != Trig["part_event_number"]) {
        throw(Exception(Exception::recoverable,
              std::string("Wrong part_event_number!"),
              "MapCppTOFDigits::getTrig"));
      }
      if (xDocDigit["phys_event_number"] != Trig["phys_event_number"]) {
        throw(Exception(Exception::recoverable,
              std::string("Wrong phys_event_number!"),
              "MapCppTOFDigits::getTrig"));
      }

      return true;  // will break the loop and return true it finds the first trigger that matches.
    }
  }

  return false;
}

bool MapCppTOFDigits::getTrigReq(Json::Value xDocTrigReq,
                                 Json::Value xDocTdcHit,
                                 Json::Value &xDocDigit ) throw(Exception) {
  Json::Value xDocTR = JsonWrapper::GetProperty(xDocTrigReq, "V1290", JsonWrapper::arrayValue);
  int HitGeo = JsonWrapper::GetProperty(xDocTdcHit, "geo", JsonWrapper::intValue).asInt();
  int n_req_count = xDocTR.size();

  // Loop over all of the trigger requests for this particle event.
  for (int TrigReqCount = 0; TrigReqCount < n_req_count; TrigReqCount++) {
    Json::Value TrigReq = JsonWrapper::GetItem(xDocTR, TrigReqCount, JsonWrapper::objectValue);

    int TrReqGeo = JsonWrapper::GetProperty(TrigReq, "geo", JsonWrapper::intValue).asInt();
    // If the Geo number of the trigger request matches the Geo number for the TDC then add the
    // trigger request information to the digit.
    if (TrReqGeo == HitGeo) {
      xDocDigit["trigger_request_leading_time"]  = JsonWrapper::GetProperty(TrigReq,
                                                                            "leading_time",
                                                                            JsonWrapper::intValue);

      xDocDigit["trigger_request_trailing_time"] = JsonWrapper::GetProperty(TrigReq,
                                                                            "trailing_time",
                                                                            JsonWrapper::intValue);

      if (xDocDigit["part_event_number"] != TrigReq["part_event_number"]) {
        throw(Exception(Exception::recoverable,
              std::string("Wrong part_event_number!"),
              "MapCppTOFDigits::getTrigReq"));
      }
      if (xDocDigit["phys_event_number"] != TrigReq["phys_event_number"]) {
        throw(Exception(Exception::recoverable,
              std::string("Wrong phys_event_number!"),
              "MapCppTOFDigits::getTrigReq"));
      }
      return true;  // will break the loop when it finds the first request that matches.
      // There may be multiple requests per board but for now we only get the first one.
    }
  }

  return false;
}
}
