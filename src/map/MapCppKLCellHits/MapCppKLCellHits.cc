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

#include "src/map/MapCppKLCellHits/MapCppKLCellHits.hh"

namespace MAUS {
bool MapCppKLCellHits::birth(std::string argJsonConfigDocument) {
  // Check if the JSON document can be parsed, else return error only
  _classname = "MapCppKLCellHits";
  _stationKeys.push_back("kl");

  // Check if the JSON document can be parsed, else return error only
  try {
    //  JsonCpp setup
    Json::Value configJSON;
    configJSON = JsonWrapper::StringToJson(argJsonConfigDocument);
    //  this will contain the configuration

    return true;
  } catch(Squeal squee) {
    MAUS::CppErrorHandler::getInstance()->HandleSquealNoJson(squee, _classname);
  } catch(std::exception exc) {
    MAUS::CppErrorHandler::getInstance()->HandleStdExcNoJson(exc, _classname);
  }

  return false;
}

bool MapCppKLCellHits::death()  {return true;}

std::string MapCppKLCellHits::process(std::string document) {
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
      for (unsigned int n_event = 0; n_event < events.size(); n_event++) {
        Json::Value xDocKlEvent = JsonWrapper::GetItem(events,
                                                        n_event,
                                                        JsonWrapper::objectValue);
        xDocKlEvent = JsonWrapper::GetProperty(xDocKlEvent,
                                               "kl_event",
                                               JsonWrapper::objectValue);
        if (root["recon_events"][n_event]["kl_event"].isMember("kl_digits")) {

	 root["recon_events"][n_event]["kl_event"]["kl_cell_hits"] = Json::Value(Json::objectValue);

	 Json::Value xDocPartEvent = JsonWrapper::GetProperty(xDocKlEvent,
                                                        "kl_digits",
                                                        JsonWrapper::objectValue);

	xDocPartEvent = JsonWrapper::GetProperty(xDocPartEvent,
                                                    "kl",
                                                    JsonWrapper::anyValue);


	 Json::Value xDocCellHits = makeCellHits(xDocPartEvent);

	 root["recon_events"][n_event]["kl_event"]["kl_cell_hits"]["kl"] = xDocCellHits;
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

  return writer.write(root);
}

Json::Value MapCppKLCellHits::makeCellHits(Json::Value xDocPartEvent) {

  Json::Value xDocCellHits;
  if (xDocPartEvent.isArray()) {
    int n_digits = xDocPartEvent.size();
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
                               "kl_key",
                               JsonWrapper::stringValue).asString();

      KLChannelKey xKey(xKeyStr);

      // Add this PMT to the map.
      xDigitPos[xKeyStr] = Digit;
    }

    // Now loop over the map of hited PMTs and create Cell hits.
    while ( xDigitPos.size() > 1 ) {
      // Get the first element of the map and check if we have a hit
      // at the opposite side of the cell.
      it = xDigitPos.begin();
      KLChannelKey xKey(it->first);

      // Get the digit.
      Json::Value xThisDigit =
      JsonWrapper::GetItem(xDocPartEvent, it->second, JsonWrapper::objectValue);

      // Get the opposite PMT coded as string.
      std::string xOppositPmtKey_str = xKey.GetOppositeSidePMTStr();
      if (xDigitPos.find(xOppositPmtKey_str) != xDigitPos.end()) {
        Json::Value xDocTheCellHit;
        Json::Value xOtherDigit = JsonWrapper::GetItem(xDocPartEvent,
                                                       xDigitPos[xOppositPmtKey_str],
                                                       JsonWrapper::objectValue);
        // Create Cell hit.
        if (xKey.pmt() == 0) {
          xDocTheCellHit = fillCellHit(xThisDigit, xOtherDigit);
        }
        if (xKey.pmt() == 1) {
          xDocTheCellHit = fillCellHit(xOtherDigit, xThisDigit);
        }
        xDocCellHits.append(xDocTheCellHit);
        // Erase both used hits from the map.
        xDigitPos.erase(it);
        xDigitPos.erase(xOppositPmtKey_str);
      } else {
        // Erese this hit from the map.
        xDigitPos.erase(it);
      }
    }
  }
  // std::cout << xDocCellHits <<std::endl;
  return xDocCellHits;
}

Json::Value MapCppKLCellHits::fillCellHit(Json::Value xDocDigit0, Json::Value xDocDigit1) {
  Json::Value xDocCellHit, xDocPMT0, xDocPMT1;

  // Use the information from the digits to fill the cell hit.
  std::string xKeyStr = JsonWrapper::GetProperty(xDocDigit0,
                                                 "kl_key",
                                                 JsonWrapper::stringValue).asString();
  KLChannelKey xKey(xKeyStr);
  xDocPMT0["kl_key"] = xDocDigit0["kl_key"];
  xDocPMT1["kl_key"] = xDocDigit1["kl_key"];

  xDocCellHit["cell"]     = xKey.cell();
  xDocCellHit["detector"] = xKey.detector();

  xDocCellHit["part_event_number"] = xDocDigit0["part_event_number"];
  xDocCellHit["phys_event_number"] = xDocDigit0["phys_event_number"];



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
    xDocCellHit["charge"] = xChargeDigit0 + xChargeDigit1;
    xDocCellHit["charge_product"] = 2 * xChargeDigit0 * xChargeDigit1 /
				    (xChargeDigit0 + xChargeDigit1);
  }

//  xDocCellHit["pmt0"] = xDocPMT0;
//  xDocCellHit["pmt1"] = xDocPMT1;


  return xDocCellHit;
}
}
