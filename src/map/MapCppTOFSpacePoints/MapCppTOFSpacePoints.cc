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

#include "src/map/MapCppTOFSpacePoints/MapCppTOFSpacePoints.hh"

bool MapCppTOFSpacePoints::birth(std::string argJsonConfigDocument) {
  // Check if the JSON document can be parsed, else return error only
  _classname = "MapCppTOFSpacePoints";

  if (SetConfiguration(argJsonConfigDocument))
    return true;  // Sucessful completion
  else
    return false;
}

bool MapCppTOFSpacePoints::death()  {return true;}

std::string MapCppTOFSpacePoints::process(std::string document) {
  //  JsonCpp setup
  Json::FastWriter writer;
  Json::Value root;
  Json::Value xEventType;
  // Check if the JSON document can be parsed, else return error only
  try {
    root = JsonWrapper::StringToJson(document);
    xEventType = JsonWrapper::GetProperty(root,
                                          "daq_event_type",
                                          JsonWrapper::stringValue);
  }catch(Squeal e) {
    Squeak::mout(Squeak::error)
    << "Error in  MapCppTOFSlabHits::process. Bad json document."
    << std::endl;
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says:" << e.GetMessage();
    errors["bad_json_document"] = ss.str();
    root["errors"] = errors;
    return writer.write(root);
  }

  if (xEventType== "physics_event" || xEventType == "calibration_event") {
    Json::Value xSlabHits = JsonWrapper::GetProperty(root, "slab_hits", JsonWrapper::objectValue);
    StationKeys.push_back("tof1");
    StationKeys.push_back("tof0");
    StationKeys.push_back("tof2");
    for (unsigned int i=0; i<_trigger_pixels.size(); i++)
      delete _trigger_pixels[i];

   _trigger_pixels.resize(0);
    // Loop over each station.
    for (unsigned int n_station = 0; n_station < StationKeys.size(); n_station++) {

      Json::Value xDocDetectorSlabHits = JsonWrapper::GetProperty(xSlabHits,
                                                                StationKeys[n_station],
                                                                JsonWrapper::arrayValue);

      if (xDocDetectorSlabHits.isArray()) {
        int n_part_events = xDocDetectorSlabHits.size();
        _trigger_pixels.resize(n_part_events);

        // Loop over the particle events.
        for (int PartEvent = 0; PartEvent < n_part_events; PartEvent++) {
          Json::Value xDocPartEvent = JsonWrapper::GetItem(xDocDetectorSlabHits,
                                                           PartEvent,
                                                           JsonWrapper::anyValue);

          Json::Value xDocSpacePoints = makeSpacePoints(xDocPartEvent);
          root["space_points"][StationKeys[n_station]][PartEvent] = xDocSpacePoints;
        }
      }
    }
  }
  // std::cout << root["space_points"] << std::endl;
  return writer.write(root);
}

bool MapCppTOFSpacePoints::SetConfiguration(std::string json_configuration) {
  //  JsonCpp setup
  Json::Value configJSON = JsonWrapper::StringToJson(json_configuration);
  // this will contain the configuration

  _map.SetTriggerStation(1);
  bool loaded = _map.InitializeFromCards(json_configuration);
  if (!loaded)
    return false;

  return true;
}

Json::Value MapCppTOFSpacePoints::makeSpacePoints(Json::Value xDocPartEvent) {

  Json::Value xDocSpacePoints;

  if (xDocPartEvent.isArray()) {
    int n_slab_hits = xDocPartEvent.size();
     std::cout << xDocPartEvent << std::endl;
 
    // Create a map of all slab hits.
    std::vector<int> xPlane0Hits;
    std::vector<int> xPlane1Hits;

    // Loop ovew the slab hits.
    for (int SlHit = 0; SlHit < n_slab_hits; SlHit++) {
      // Get the slab hit.
      Json::Value xThisSlabHit =
      JsonWrapper::GetItem(xDocPartEvent, SlHit, JsonWrapper::objectValue);

      int xPlane  = JsonWrapper::GetProperty(xThisSlabHit,
                                             "plane",
                                             JsonWrapper::intValue).asInt();

      switch (xPlane) {
        case 0 :
          xPlane0Hits.push_back(SlHit);
          break;
        case 1 :
          xPlane1Hits.push_back(SlHit);
          break;
      }

      Json::Value xDocSpacePoint;
      if (xPlane0Hits.size() == 1 && xPlane1Hits.size() == 1) {

        Json::Value xDocSlabHit0 = JsonWrapper::GetItem(xDocPartEvent,
                                                       xPlane0Hits[0],
                                                       JsonWrapper::objectValue);

        Json::Value xDocSlabHit1 = JsonWrapper::GetItem(xDocPartEvent,
                                                       xPlane1Hits[0],
                                                       JsonWrapper::objectValue);

        xDocSpacePoint = fillSpacePoint(xDocSlabHit0, xDocSlabHit1);
      }
      unsigned int n = 0;
      xDocSpacePoints[n] = xDocSpacePoint;
      //xDocSpacePoints.append(xDocSpacePoint);
    }
  }
  
  return xDocSpacePoints;
}

Json::Value MapCppTOFSpacePoints::fillSpacePoint(Json::Value xDocSlabHit0,
                                                 Json::Value xDocSlabHit1) {

  Json::Value xDocSpacePoint;

  xDocSpacePoint["part_event_number"] = xDocSlabHit0["part_event_number"];
  xDocSpacePoint["phys_event_number"] = xDocSlabHit0["phys_event_number"];

  // Get the two channel keys and make the pixel key.
  std::string keyStr_Slab0_digit0 = JsonWrapper::GetProperty(xDocSlabHit0["pmt0"],
                                                             "tof_key",
                                                             JsonWrapper::stringValue).asString();
  TOFChannelKey xKey_Slab0_digit0(keyStr_Slab0_digit0);


  std::string keyStr_Slab1_digit0 = JsonWrapper::GetProperty(xDocSlabHit1["pmt0"],
                                                             "tof_key",
                                                             JsonWrapper::stringValue).asString();
  TOFChannelKey xKey_Slab1_digit0(keyStr_Slab1_digit0);

  TOFPixelKey *xSPKey = new TOFPixelKey(xKey_Slab0_digit0.station(),
                                        xKey_Slab0_digit0.slab(),
                                        xKey_Slab1_digit0.slab(),
                                        xKey_Slab1_digit0.detector());

  int nPartEvent = xDocSlabHit0["part_event_number"].asInt();
  if (xSPKey->station() == 1) _trigger_pixels[nPartEvent] = xSPKey;
  std::cout << nPartEvent << std::endl;
  // Fill the information in the Space point.
  xDocSpacePoint["slab0"]     = xKey_Slab0_digit0.slab();
  xDocSpacePoint["station"]   = xKey_Slab0_digit0.station();
  xDocSpacePoint["detector"]  = xKey_Slab0_digit0.detector();
  xDocSpacePoint["slab1"]     = xKey_Slab1_digit0.slab();
  xDocSpacePoint["pixel_key"] = xSPKey->str();

  if (_trigger_pixels[nPartEvent]) {
  double time_Slab0_digit0, time_Slab0_digit1, time_Slab1_digit0, time_Slab1_digit1;
  // Process Digit0 of Slab0
  calibratePmtHit(*(_trigger_pixels[nPartEvent]), xDocSlabHit0["pmt0"], time_Slab0_digit0);
  
 // Process Digit1 of Slab0
  calibratePmtHit(*(_trigger_pixels[nPartEvent]), xDocSlabHit0["pmt1"], time_Slab0_digit1);
  
// Process Digit0 of Slab1
  calibratePmtHit(*(_trigger_pixels[nPartEvent]), xDocSlabHit1["pmt0"], time_Slab1_digit0);
    
 // Process Digit1 of Slab1
  calibratePmtHit(*(_trigger_pixels[nPartEvent]), xDocSlabHit1["pmt1"], time_Slab1_digit1);
  
  // Now calculate the time.
  double time = 
  (time_Slab0_digit0 + time_Slab0_digit1 + time_Slab1_digit0 + time_Slab1_digit1)/4.;

  xDocSpacePoint["time"] = time;
   std::cout << *xSPKey << " t = " << time << std::endl;
  }
  // std::cout << xDocSpacePoint << std::endl;
  return xDocSpacePoint;
}

bool MapCppTOFSpacePoints::calibratePmtHit(TOFPixelKey xTriggerPixelKey,
                                           Json::Value &xPmtHit, 
                                           double &time) {

  std::string keyStr = JsonWrapper::GetProperty(xPmtHit,
                                                "tof_key",
                                                JsonWrapper::stringValue).asString();
  TOFChannelKey xChannelKey(keyStr);
  
  double raw_time  = JsonWrapper::GetProperty(xPmtHit,
                                              "raw_time",
                                              JsonWrapper::realValue).asDouble();

  int charge = JsonWrapper::GetProperty(xPmtHit,
                                        "charge",
                                        JsonWrapper::intValue).asInt();

  double dT = _map.dT(xChannelKey, xTriggerPixelKey, charge);
  time = raw_time - dT;
  xPmtHit["time"] = time;
  // std::cout << xChannelKey << " t = " << raw_time << " - " << dT << " = " << time << std::endl;

  return true;
}




