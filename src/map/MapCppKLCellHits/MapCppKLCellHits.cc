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
#include "Interface/Squeak.hh"
#include "Utils/Exception.hh"
#include "Interface/dataCards.hh"
#include "src/common_cpp/API/PyWrapMapBase.hh"
#include "Config/MiceModule.hh"

#include "src/map/MapCppKLCellHits/MapCppKLCellHits.hh"

namespace MAUS {

PyMODINIT_FUNC init_MapCppKLCellHits(void) {
  PyWrapMapBase<MAUS::MapCppKLCellHits>::PyWrapMapBaseModInit
                                          ("MapCppKLCellHits", "", "", "", "");
}

MapCppKLCellHits::MapCppKLCellHits() : MapBase<Json::Value>("MapCppKLCellHits") {
}

void MapCppKLCellHits::_birth(const std::string& argJsonConfigDocument) {
  // Check if the JSON document can be parsed, else return error only
  _stationKeys.push_back("kl");

  //  JsonCpp setup
  Json::Value configJSON;
  configJSON = JsonWrapper::StringToJson(argJsonConfigDocument);

  // get the geometry
  if (!configJSON.isMember("reconstruction_geometry_filename"))
      throw(Exception(Exception::recoverable,
                   "Could not find geometry file",
                   "MapCppKLCellHits::birth"));
  std::string filename;
  filename = configJSON["reconstruction_geometry_filename"].asString();
  // get the kl geometry modules
  geo_module = new MiceModule(filename);
  kl_modules = geo_module->findModulesByPropertyString("SensitiveDetector", "KL");
  kl_mother_modules = geo_module->findModulesByPropertyString("Region", "KLregion");
}

void MapCppKLCellHits::_death()  {}

void MapCppKLCellHits::_process(Json::Value* data) const {
  if (!data) {
      throw MAUS::Exception(Exception::recoverable,
                            "data was NULL", "MapCppKLCellHits::_process");
  }
  Json::Value& root = *data;
  //  JsonCpp setup
  Json::Value xEventType = JsonWrapper::GetProperty(root,
                                        "daq_event_type",
  JsonWrapper::stringValue);
  if (xEventType== "physics_event" || xEventType == "calibration_event") {
    Json::Value events = JsonWrapper::GetProperty(root,
                                                  "recon_events",
                                                  JsonWrapper::arrayValue);
    for (unsigned int n_event = 0; n_event < events.size(); n_event++) {
      Json::Value xDocKlEvent = JsonWrapper::GetItem(events,
                                                      n_event,
                                                      JsonWrapper::objectValue);
      xDocKlEvent = JsonWrapper::GetProperty(xDocKlEvent,
                                             "kl_event",
                                             JsonWrapper::objectValue);
      if (root["recon_events"][n_event]["kl_event"].isMember("kl_digits")) {

          root["recon_events"][n_event]["kl_event"]["kl_cell_hits"] =
                                                 Json::Value(Json::objectValue);

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
}

Json::Value MapCppKLCellHits::makeCellHits(Json::Value xDocPartEvent) const {

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

Json::Value MapCppKLCellHits::fillCellHit(Json::Value xDocDigit0, Json::Value xDocDigit1) const {
  Json::Value xDocCellHit, xDocPMT0, xDocPMT1;

  // std::cout << "xDocPMT0 " << xDocDigit0["kl_key"] << std::endl;
  // std::cout << "xDocPMT1 " << xDocDigit1["kl_key"] << std::endl;
  xDocPMT1["kl_key"] = xDocDigit1["kl_key"];
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

  // cell global position
  // find the geo module corresponding to this hit
  const MiceModule* hit_module = NULL;
  Hep3Vector cellGlobalPos;
  Hep3Vector cellErrorPos;

  for ( unsigned int jj = 0; !hit_module && jj < kl_modules.size(); ++jj ) {
      if ( kl_modules[jj]->propertyExists("Cell", "int") &&
         kl_modules[jj]->propertyInt("Cell") == xKey.cell()) {
             // got it
             hit_module = kl_modules[jj];
      } // end check on module
  } // end loop over kl_modules

  if (hit_module) {
      cellGlobalPos = hit_module->globalPosition();
      cellErrorPos = hit_module->dimensions()/sqrt(12);
  } else {
      cellGlobalPos.setX(-9999999.);
      cellGlobalPos.setY(-9999999.);
      cellGlobalPos.setZ(-9999999.);
      cellErrorPos.setX(-9999999.);
      cellErrorPos.setY(-9999999.);
      cellErrorPos.setZ(-9999999.);
  }

  // get the local (relative to KL) cell positions from the geometry
  const MiceModule* mother_module = NULL;
  Hep3Vector cellLocalPos;

  if (kl_mother_modules.size()) {
    for ( unsigned int jj = 0; !mother_module &&  jj < kl_mother_modules.size(); ++jj ) {
      mother_module= kl_mother_modules[jj];
    }
    cellLocalPos = hit_module->relativePosition(mother_module);
  } else {
    cellLocalPos.setX(-9999999.);
    cellLocalPos.setY(-9999999.);
    cellLocalPos.setZ(-9999999.);
  }

  xDocCellHit["global_pos_x"] = cellGlobalPos.x();
  xDocCellHit["global_pos_y"] = cellGlobalPos.y();
  xDocCellHit["global_pos_z"] = cellGlobalPos.z();
  xDocCellHit["local_pos_x"] = cellLocalPos.x();
  xDocCellHit["local_pos_y"] = cellLocalPos.y();
  xDocCellHit["local_pos_z"] = cellLocalPos.z();
  xDocCellHit["err_x"] = cellErrorPos.x();
  xDocCellHit["err_y"] = cellErrorPos.y();
  xDocCellHit["err_z"] = cellErrorPos.z();

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
    xDocCellHit["charge"] = (xChargeDigit0 + xChargeDigit1)/2;
    if (xDocCellHit["charge"] == 0) {
        xDocCellHit["charge_product"] = 0;
        xDocCellHit["flag"] = false;
    } else {
        xDocCellHit["charge_product"] = 2 * xChargeDigit0 * xChargeDigit1 /
                                        (xChargeDigit0 + xChargeDigit1);
        xDocCellHit["flag"] = true;
    }
  }

//  xDocCellHit["pmt0"] = xDocPMT0;
//  xDocCellHit["pmt1"] = xDocPMT1;


  return xDocCellHit;
}
}
