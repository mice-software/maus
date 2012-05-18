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

#include <vector>
#include <string>

#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"
#include "Interface/Squeak.hh"
#include "Interface/Squeal.hh"
#include "Interface/dataCards.hh"

#include "src/map/MapCppTOFSlabHits/MapCppTOFSlabHits.hh"

bool MapCppTOFSlabHits::birth(std::string argJsonConfigDocument) {
  // Check if the JSON document can be parsed, else return error only
  _classname = "MapCppTOFSlabHits";
  _stationKeys.push_back("tof0");
  _stationKeys.push_back("tof1");
  _stationKeys.push_back("tof2");

  // Check if the JSON document can be parsed, else return error only
  try {
    //  JsonCpp setup
    Json::Value configJSON;
    configJSON = JsonWrapper::StringToJson(argJsonConfigDocument);
    //  this will contain the configuration

    _tdcV1290_conversion_factor = JsonWrapper::GetProperty(configJSON,
                                                           "TOFtdcConversionFactor",
                                                           JsonWrapper::realValue).asDouble();

    return true;
  } catch(Squeal squee) {
    MAUS::CppErrorHandler::getInstance()->HandleSquealNoJson(squee, _classname);
  } catch(std::exception exc) {
    MAUS::CppErrorHandler::getInstance()->HandleStdExcNoJson(exc, _classname);
  }

  return false;
}

bool MapCppTOFSlabHits::death()  {return true;}

std::string MapCppTOFSlabHits::process(std::string document) {
  //  JsonCpp setup
  Json::FastWriter writer;
  Json::Value root;
  Json::Value xEventType;
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
    if (xEventType== "physics_event" || xEventType == "calibration_event") {
      Json::Value events = JsonWrapper::GetProperty(root, "recon_events", JsonWrapper::arrayValue);
      // Loop over each station.
      for (unsigned int n_event = 0; n_event < events.size(); n_event++) {
        Json::Value xDocTofEvent = JsonWrapper::GetItem(events,
                                                        n_event,
                                                        JsonWrapper::objectValue);
        xDocTofEvent = JsonWrapper::GetProperty(xDocTofEvent,
                                                "tof_event",
                                                JsonWrapper::objectValue);
        if (root["recon_events"][n_event]["tof_event"].isMember("tof_digits")) {
          root["recon_events"][n_event]["tof_event"]["tof_slab_hits"] =
                                                   Json::Value(Json::objectValue);
          for (unsigned int n_station = 0; n_station < _stationKeys.size(); n_station++) {
            Json::Value xDocPartEvent = JsonWrapper::GetProperty(xDocTofEvent,
                                                        "tof_digits",
                                                        JsonWrapper::objectValue);
            // Ack! sometimes tofn is a nullValue
            xDocPartEvent = JsonWrapper::GetProperty(xDocPartEvent,
                                                    _stationKeys[n_station],
                                                    JsonWrapper::anyValue);
            Json::Value xDocSlabHits = makeSlabHits(xDocPartEvent);
            root["recon_events"][n_event]["tof_event"]["tof_slab_hits"]
                                       [_stationKeys[n_station]] = xDocSlabHits;
          }
        }
      }
    }
  } catch(Squeal squee) {
    root = MAUS::CppErrorHandler::getInstance()
                                       ->HandleSqueal(root, squee, _classname);
  } catch(std::exception exc) {
    root = MAUS::CppErrorHandler::getInstance()
                                         ->HandleStdExc(root, exc, _classname);
  }
  // if (root.isMember("slab_hits")) std::cout<<root["slab_hits"]<<std::endl;
  return writer.write(root);
}

Json::Value MapCppTOFSlabHits::makeSlabHits(Json::Value xDocPartEvent) {

  Json::Value xDocSlabHits;
  if (xDocPartEvent.isArray()) {
    int n_digits = xDocPartEvent.size();
    // std::cout << xDocPartEvent << std::endl;

    // Create a map of all hited PMTs.
    std::map<string, int> xDigitPos;
    std::map<string, int>::iterator it;

    // Loop ovew the digits.
    for (int Digit = 0; Digit < n_digits; Digit++) {
      // Get the digit.
      Json::Value xThisDigit =
      JsonWrapper::GetItem(xDocPartEvent, Digit, JsonWrapper::objectValue);

      std::string xKeyStr =
      JsonWrapper::GetProperty(xThisDigit,
                               "tof_key",
                               JsonWrapper::stringValue).asString();

      TOFChannelKey xKey(xKeyStr);
      // Check if we already have this key in the map. This will mean that there are two
      // digits in the same PMT.
      it = xDigitPos.find(xKeyStr);
      if (it != xDigitPos.end()) {
        // We have two digits coming from one PMT.
        // Get the other digit and check which comes first in time.
        Json::Value xOtherDigit = JsonWrapper::GetItem(xDocPartEvent,
                                                       xDigitPos[xKeyStr],
                                                       JsonWrapper::objectValue);

        int time_thisDigit  = JsonWrapper::GetProperty(xThisDigit,
                                                       "leading_time",
                                                       JsonWrapper::intValue).asInt();

        int time_otherDigit  = JsonWrapper::GetProperty(xOtherDigit,
                                                        "leading_time",
                                                        JsonWrapper::intValue).asInt();

        // Only the digit that comes first in time will be used to create a slab hit.
        // Most probably the second one is created due to afterpulsing of the PMT.
        if (time_thisDigit < time_otherDigit)   // Else do nothing.
          xDigitPos[xKeyStr] = Digit;
      } else {
        // Add this PMT to the map.
        xDigitPos[xKeyStr] = Digit;
      }
    }

    // Now loop over the map of hited PMTs and create Slab hits.
    while ( xDigitPos.size() > 1 ) {
      // Get the first element of the map and check if we have a hit
      // at the opposite side of the slab.
      it = xDigitPos.begin();
      TOFChannelKey xKey(it->first);

      // Get the digit.
      Json::Value xThisDigit =
      JsonWrapper::GetItem(xDocPartEvent, it->second, JsonWrapper::objectValue);

      // Get the opposite PMT coded as string.
      std::string xOppositPmtKey_str = xKey.GetOppositeSidePMTStr();
      if (xDigitPos.find(xOppositPmtKey_str) != xDigitPos.end()) {
        Json::Value xDocTheSlabHit;
        Json::Value xOtherDigit = JsonWrapper::GetItem(xDocPartEvent,
                                                       xDigitPos[xOppositPmtKey_str],
                                                       JsonWrapper::objectValue);
        // Create Slab hit.
        if (xKey.pmt() == 0) {
          xDocTheSlabHit = fillSlabHit(xThisDigit, xOtherDigit);
        }
        if (xKey.pmt() == 1) {
          xDocTheSlabHit = fillSlabHit(xOtherDigit, xThisDigit);
        }
        xDocSlabHits.append(xDocTheSlabHit);
        // Erase both used hits from the map.
        xDigitPos.erase(it);
        xDigitPos.erase(xOppositPmtKey_str);
      } else {
        // Erese this hit from the map.
        xDigitPos.erase(it);
      }
    }
  }
  // std::cout << xDocSlabHits <<std::endl;
  return xDocSlabHits;
}

Json::Value MapCppTOFSlabHits::fillSlabHit(Json::Value xDocDigit0, Json::Value xDocDigit1) {
  Json::Value xDocSlabHit, xDocPMT0, xDocPMT1;

  // Use the information from the digits to fill the slab hit.
  std::string xKeyStr = JsonWrapper::GetProperty(xDocDigit0,
                                                 "tof_key",
                                                 JsonWrapper::stringValue).asString();
  TOFChannelKey xKey(xKeyStr);
  xDocPMT0["tof_key"] = xDocDigit0["tof_key"];
  xDocPMT1["tof_key"] = xDocDigit1["tof_key"];

  xDocSlabHit["station"]  = xKey.station();
  xDocSlabHit["plane"]    = xKey.plane();
  xDocSlabHit["slab"]     = xKey.slab();
  xDocSlabHit["detector"] = xKey.detector();

  xDocSlabHit["part_event_number"] = xDocDigit0["part_event_number"];
  xDocSlabHit["phys_event_number"] = xDocDigit0["phys_event_number"];

  int xTimeDigit0 = JsonWrapper::GetProperty(xDocDigit0,
                                             "leading_time",
                                             JsonWrapper::intValue).asInt();

  int xTriggerReqDigit0 = JsonWrapper::GetProperty(xDocDigit0,
                                                   "trigger_request_leading_time",
                                                   JsonWrapper::intValue).asInt();

  xDocPMT0["trigger_request_leading_time"] = xTriggerReqDigit0;
  xDocPMT0["leading_time"] = xTimeDigit0;

  int xTimeDigit1 = JsonWrapper::GetProperty(xDocDigit1,
                                             "leading_time",
                                             JsonWrapper::intValue).asInt();
  int xTriggerReqDigit1 = JsonWrapper::GetProperty(xDocDigit1,
                                                   "trigger_request_leading_time",
                                                   JsonWrapper::intValue).asInt();

  xDocPMT1["trigger_request_leading_time"] = xTriggerReqDigit1;
  xDocPMT1["leading_time"] = xTimeDigit1;

  // Charge of the digit can be unset because of the Zero suppresion of the fADCs.
  if (xDocDigit0.isMember("charge_mm") && xDocDigit1.isMember("charge_mm")) {
    int xChargeDigit0 = JsonWrapper::GetProperty(xDocDigit0,
                                                 "charge_mm",
                                                 JsonWrapper::intValue).asInt();

    int xChargeDigit1 = JsonWrapper::GetProperty(xDocDigit1,
                                                 "charge_mm",
                                                 JsonWrapper::intValue).asInt();

    xDocPMT0["charge"] = xChargeDigit0;
    xDocPMT1["charge"] = xChargeDigit1;
    xDocSlabHit["charge"] = xChargeDigit0 + xChargeDigit1;
    xDocSlabHit["charge_product"] = xChargeDigit0 * xChargeDigit1 / (xChargeDigit0 + xChargeDigit1);
  }

  // Calculate the measured value of the time in nanoseconds.
  double time_digit0 =
  _tdcV1290_conversion_factor*(static_cast<double>(xTimeDigit0 - xTriggerReqDigit0));
  double time_digit1 =
  _tdcV1290_conversion_factor*(static_cast<double>(xTimeDigit1 - xTriggerReqDigit1));

  xDocPMT0["raw_time"] = time_digit0;
  xDocPMT1["raw_time"] = time_digit1;
  xDocSlabHit["pmt0"] = xDocPMT0;
  xDocSlabHit["pmt1"] = xDocPMT1;
  double xRawTime = (time_digit0 + time_digit1)/2.;
  xDocSlabHit["raw_time"] = xRawTime;

  return xDocSlabHit;
}

