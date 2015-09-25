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
#include "Config/MiceModule.hh"
#include "Interface/Squeak.hh"
#include "Utils/Exception.hh"
#include "Interface/dataCards.hh"
#include "API/PyWrapMapBase.hh"

#include "src/map/MapCppTOFSlabHits/MapCppTOFSlabHits.hh"

namespace MAUS {
PyMODINIT_FUNC init_MapCppTOFSlabHits(void) {
  PyWrapMapBase<MAUS::MapCppTOFSlabHits>::PyWrapMapBaseModInit
                                          ("MapCppTOFSlabHits", "", "", "", "");
}

MapCppTOFSlabHits::MapCppTOFSlabHits()
    : MapBase<MAUS::Data>("MapCppTOFSlabHits") {
}

void MapCppTOFSlabHits::_birth(const std::string& argJsonConfigDocument) {
  // Check if the JSON document can be parsed, else return error only
  //  JsonCpp setup
  Json::Value configJSON;
  configJSON = JsonWrapper::StringToJson(argJsonConfigDocument);
  //  this will contain the configuration

  _tdcV1290_conversion_factor = JsonWrapper::GetProperty(configJSON,
                                                         "TOFtdcConversionFactor",
                                                         JsonWrapper::realValue).asDouble();

  // get the tof geometry modules
  _geo_filename = configJSON["reconstruction_geometry_filename"].asString();
  build_geom_map();
}

void MapCppTOFSlabHits::_death()  {}

void MapCppTOFSlabHits::_process(MAUS::Data *data) const {
  // Get spill, break if there's no DAQ data
  Spill *spill = data->GetSpill();

  if (spill->GetReconEvents() == NULL)
    return;

  if (spill->GetDaqEventType() != "physics_event")
    return;

  ReconEventPArray *events = spill->GetReconEvents();
  int nPartEvents = events->size();

  for (int xPE = 0; xPE < nPartEvents; xPE++) {
    TOF0DigitArray *tof0_digits = (*events)[xPE]->GetTOFEvent()
                                   ->GetTOFEventDigitPtr()->GetTOF0DigitArrayPtr();

    TOF1DigitArray *tof1_digits = (*events)[xPE]->GetTOFEvent()
                                   ->GetTOFEventDigitPtr()->GetTOF1DigitArrayPtr();

    TOF2DigitArray *tof2_digits = (*events)[xPE]->GetTOFEvent()
                                   ->GetTOFEventDigitPtr()->GetTOF2DigitArrayPtr();


    TOF0SlabHitArray *tof0_slhits = (*events)[xPE]->GetTOFEvent()
                                     ->GetTOFEventSlabHitPtr()->GetTOF0SlabHitArrayPtr();

    TOF1SlabHitArray *tof1_slhits = (*events)[xPE]->GetTOFEvent()
                                     ->GetTOFEventSlabHitPtr()->GetTOF1SlabHitArrayPtr();

    TOF2SlabHitArray *tof2_slhits = (*events)[xPE]->GetTOFEvent()
                                     ->GetTOFEventSlabHitPtr()->GetTOF2SlabHitArrayPtr();

    this->makeSlabHits(tof0_slhits, tof0_digits);
    this->makeSlabHits(tof1_slhits, tof1_digits);
    this->makeSlabHits(tof2_slhits, tof2_digits);
  }
}

void MapCppTOFSlabHits::makeSlabHits(TOF0SlabHitArray *slhits,
                                     TOF0DigitArray *digits) const {

  int n_digits = digits->size();
  if (n_digits == 0)
    return;
  // std::cout << xDocPartEvent << std::endl;

  // Create a map of all hited PMTs.
  std::map<string, int> xDigitPos;
  std::map<string, int>::iterator it;

  // Loop ovew the digits.
  for (int xDigit = 0; xDigit < n_digits; xDigit++) {
    // Get the digit channel key.
    std::string xKeyStr = (*digits)[xDigit].GetTofKey();
//     TOFChannelKey xKey(xKeyStr);
    // Check if we already have this key in the map. This will mean that there are two
    // digits in the same PMT.
    it = xDigitPos.find(xKeyStr);
    if (it != xDigitPos.end()) {
      int time_thisDigit  = (*digits)[xDigit].GetLeadingTime();
      int time_otherDigit = (*digits)[xDigitPos[xKeyStr]].GetLeadingTime();

      // Only the digit that comes first in time will be used to create a slab hit.
      // Most probably the second one is created due to afterpulsing of the PMT.
      if (time_thisDigit < time_otherDigit)   // Else do nothing.
      xDigitPos[xKeyStr] = xDigit;
    } else {
      // Add this PMT to the map.
      xDigitPos[xKeyStr] = xDigit;
    }
  }

  // Now loop over the map of hited PMTs and create Slab hits.
  while ( xDigitPos.size() > 1 ) {
    // Get the first element of the map and check if we have a hit
    // at the opposite side of the slab.
    it = xDigitPos.begin();
    TOFChannelKey xKey(it->first);

    // Get the opposite PMT coded as string.
    std::string xOppositPmtKey_str = xKey.GetOppositeSidePMTStr();
    if (xDigitPos.find(xOppositPmtKey_str) != xDigitPos.end()) {
      // Create Slab hit.
      TOFSlabHit xTheSlabHit;
      if (xKey.pmt() == 0) {
        this->fillSlabHit(xTheSlabHit, (*digits)[it->second],
                                       (*digits)[xDigitPos[xOppositPmtKey_str]]);
      }

      if (xKey.pmt() == 1) {
        this->fillSlabHit(xTheSlabHit, (*digits)[xDigitPos[xOppositPmtKey_str]],
                                       (*digits)[it->second]);
      }

      slhits->push_back(xTheSlabHit);
      // Erase both used hits from the map.
      xDigitPos.erase(it);
      xDigitPos.erase(xOppositPmtKey_str);
    } else {
      // Erese this hit from the map.
      xDigitPos.erase(it);
    }
  }
}

void MapCppTOFSlabHits::fillSlabHit(MAUS::TOFSlabHit &slHit,
                                    MAUS::TOFDigit &xDigit0,
                                    MAUS::TOFDigit &xDigit1) const {

  TOFChannelKey xKey(xDigit0.GetTofKey());

  slHit.SetSlab(xKey.slab());
  slHit.SetPlane(xKey.plane());
  if (xKey.plane() == 0) {
    slHit.SetHorizontal(true);
  } else if (xKey.plane() == 1) {
    slHit.SetVertical(true);
  }
  slHit.SetStation(xKey.station());
  slHit.SetDetector(xKey.detector());

  slHit.SetPartEventNumber(xDigit0.GetPartEventNumber());
  slHit.SetPhysEventNumber(xDigit0.GetPhysEventNumber());

  int xTimeDigit0 = xDigit0.GetLeadingTime();
  int xTriggerReqDigit0 = xDigit0.GetTriggerRequestLeadingTime();

  int xTimeDigit1 = xDigit1.GetLeadingTime();
  int xTriggerReqDigit1 = xDigit1.GetTriggerRequestLeadingTime();

  // Calculate the measured value of the time in nanoseconds.
  double time_digit0 =
  _tdcV1290_conversion_factor*(static_cast<double>(xTimeDigit0 - xTriggerReqDigit0));
  double time_digit1 =
  _tdcV1290_conversion_factor*(static_cast<double>(xTimeDigit1 - xTriggerReqDigit1));

  double xRawTime = (time_digit0 + time_digit1)/2.;
  slHit.SetRawTime(xRawTime);

  int xChargeDigit0 = xDigit0.GetChargeMm();
  int xChargeDigit1 = xDigit1.GetChargeMm();
  slHit.SetCharge(xChargeDigit0 + xChargeDigit1);

  int xChargeProd;
  if (xChargeDigit0 == 0 && xChargeDigit1 == 0)
    xChargeProd = 0;
  else
    xChargeProd = xChargeDigit0 * xChargeDigit1 / (xChargeDigit0 + xChargeDigit1);

  slHit.SetChargeProduct(xChargeProd);

  slHit.GetPmt0Ptr()->SetRawTime(time_digit0);
  slHit.GetPmt0Ptr()->SetCharge(xChargeDigit0);
  slHit.GetPmt0Ptr()->SetLeadingTime(xTimeDigit0);
  slHit.GetPmt0Ptr()->SetTriggerRequestLeadingTime(xTriggerReqDigit0);
  slHit.GetPmt0Ptr()->SetTofKey(xDigit0.GetTofKey());

  slHit.GetPmt1Ptr()->SetRawTime(time_digit1);
  slHit.GetPmt1Ptr()->SetCharge(xChargeDigit1);
  slHit.GetPmt1Ptr()->SetLeadingTime(xTimeDigit1);
  slHit.GetPmt1Ptr()->SetTriggerRequestLeadingTime(xTriggerReqDigit1);
  slHit.GetPmt1Ptr()->SetTofKey(xDigit1.GetTofKey());

  // get the global position of the slab hit and store it
  Hep3Vector SlabGlobalPos, SlabErrorPos;
  double globX = -99., globY = -99., globZx = -99., globZy = -99., globZ = -99.;
  double globXErr = -99., globYErr = -99., globZErr = -99.;

  std::stringstream ss;
  ss << xKey.station() << xKey.slab() << xKey.plane();

  // get the position of the station/slab/plane from the geometry
  std::map<std::string, TOFModuleGeo>::const_iterator _geoIt = _geom_map.find(ss.str());
  // std::cout << "DEBUG: MapCppTOFSlabHits: Global Position: "
  //           << _geoIt->second.position << " Station: "
  //           << _geoIt->second.station << " Slab: " << _geoIt->second.slab
  //           << " Plane: " << _geoIt->second.plane << std::endl;
  SlabGlobalPos = _geoIt->second.position;
  globX = SlabGlobalPos.x();
  globY = SlabGlobalPos.y();
  globZ = SlabGlobalPos.z();
  SlabErrorPos = _geoIt->second.dimensions;
  globXErr = SlabErrorPos.x()/sqrt(12);
  globYErr = SlabErrorPos.y()/sqrt(12);
  globZErr = SlabErrorPos.z()/sqrt(12);

  slHit.SetGlobalPosX(globX);
  slHit.SetGlobalPosY(globY);
  slHit.SetGlobalPosZ(globZ);
  slHit.SetGlobalPosXErr(globXErr);
  slHit.SetGlobalPosYErr(globYErr);
  slHit.SetGlobalPosZErr(globZErr);
}

///////////////////////////////////////////////////////////
void MapCppTOFSlabHits::build_geom_map() {
  geo_module = new MiceModule(_geo_filename);
  tof_modules = geo_module->findModulesByPropertyString("SensitiveDetector", "TOF");
  for (unsigned int m = 0; m < tof_modules.size(); ++m) {
      std::stringstream ss;
      int stn = tof_modules[m]->propertyInt("Station");
      int slb = tof_modules[m]->propertyInt("Slab");
      int pln = tof_modules[m]->propertyInt("Plane");
      ss << stn << slb << pln;
      TOFModuleGeo _mod_geo;
      _mod_geo.station = stn;
      _mod_geo.slab = slb;
      _mod_geo.plane = pln;
      _mod_geo.position = tof_modules[m]->globalPosition();
      _mod_geo.dimensions = tof_modules[m]->dimensions();
      _geom_map[ss.str()] = _mod_geo;
  }
  delete geo_module;
}
///////////////////////////////////////////////////////////
} // end namespace MAUS
