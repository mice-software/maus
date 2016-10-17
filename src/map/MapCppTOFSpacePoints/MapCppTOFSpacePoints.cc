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
#include "Utils/Squeak.hh"
#include "Utils/Exception.hh"
#include "Interface/dataCards.hh"
#include "API/PyWrapMapBase.hh"
#include "Config/MiceModule.hh"
#include "src/common_cpp/DataStructure/RunHeaderData.hh"
#include "src/common_cpp/DataStructure/RunHeader.hh"

#include "src/map/MapCppTOFSpacePoints/MapCppTOFSpacePoints.hh"

namespace MAUS {
PyMODINIT_FUNC init_MapCppTOFSpacePoints(void) {
  PyWrapMapBase<MAUS::MapCppTOFSpacePoints>::PyWrapMapBaseModInit
                                      ("MapCppTOFSpacePoints", "", "", "", "");
}

////////////////////////////////////////////////////////////
MapCppTOFSpacePoints::MapCppTOFSpacePoints()
    : MapBase<MAUS::Data>("MapCppTOFSpacePoints") {
    _map_init = false;
    runNumberSave = -1;
}

////////////////////////////////////////////////////////////
void MapCppTOFSpacePoints::_birth(const std::string& argJsonConfigDocument) {
  // Check if the JSON document can be parsed, else return error only
  // JsonCpp setup
  configJSON = JsonWrapper::StringToJson(argJsonConfigDocument);

  _makeSpacePointCut =
  JsonWrapper::GetProperty(configJSON,
                           "TOF_makeSpacePointCut",
                           JsonWrapper::realValue).asDouble(); // nanoseconds
  _findTriggerPixelCut =
  JsonWrapper::GetProperty(configJSON,
                           "TOF_findTriggerPixelCut",
                           JsonWrapper::realValue).asDouble(); // nanoseconds

  _triggerStation
    = JsonWrapper::GetProperty(configJSON,
                               "TOF_trigger_station",
                               JsonWrapper::stringValue).asString();

  // set the station numbering
  _trigStn = -1;
  if (_triggerStation == "tof0") {
      _trigStn = 0;
  } else if (_triggerStation == "tof1") {
      _trigStn = 1;
  } else if (_triggerStation == "tof2") {
      _trigStn = 2;
  } else {
    throw MAUS::Exceptions::Exception(Exceptions::recoverable,
                          "TOF trigger station invalid. Must be tof0/tof1/tof2",
                          "MapCppTOFSpacePoints::_birth");
  }
  _geo_filename = configJSON["reconstruction_geometry_filename"].asString();
  build_geom_map();
}

////////////////////////////////////////////////////////////
void MapCppTOFSpacePoints::_death() {}

////////////////////////////////////////////////////////////
void MapCppTOFSpacePoints::_process(MAUS::Data* data) const {
  Spill *spill = data->GetSpill();

  if (spill->GetReconEvents() == NULL)
    return;

  if (spill->GetDaqEventType() != "physics_event")
    return;

  int runNumber = 0;
  runNumber = spill->GetRunNumber();
  // std::cerr << "RunNum = " << runNumber << std::endl;

  if (!_map_init || runNumber != runNumberSave) {
      const_cast<MapCppTOFSpacePoints*>(this)->getTofCalib(runNumber);
  }
  std::map<int, std::string> triggerhit_pixels;

  ReconEventPArray *events = spill->GetReconEvents();
  int nPartEvents = events->size();
  // std::cerr << "evsize = " << nPartEvents << std::endl;

  for (int n_event = 0; n_event < nPartEvents; n_event++) {
      // std::cerr << "=== spill " << spill->GetSpillNumber() << " evt: " << n_event << std::endl;
      TOFEventSlabHit* tSlabHits = (*events)[n_event]->GetTOFEvent()->GetTOFEventSlabHitPtr();
        // NOTE: DR March15
        // Cheating -- until I figure out how to handle trig-req-time in MC:
        //   I have to change the triggerpixelcut for MC in order for the
        //   calib corrections to be applied correctly.
        //   2 options --
        //   a) change the cut in ConfigDefaults
        //      but this <may> mess up data -- though I did run on real data
        //      with this modified cut and things (resol,time) look OK @ 1st
        //      glance
        //   b) use a different cut if it's MC
        //      this breaks the agreement that we'll treat real data/MC same
        //      way but for now it at least lets MC get reconstructed without
        //      clobbering real data
        // For now I have chosen option b)
      if (spill->GetMCEventSize() > 0)
          const_cast<MapCppTOFSpacePoints*>(this)->_findTriggerPixelCut = 50.0;

      // std::cerr << tSlabHits->GetTOF0SlabHitArraySize() << " "
      //           << tSlabHits->GetTOF1SlabHitArraySize() << " "
      //           << tSlabHits->GetTOF2SlabHitArraySize() << std::endl;
      TOF0SpacePointArray* tof0_spoints  =
        (*events)[n_event]->GetTOFEvent()->GetTOFEventSpacePointPtr()->GetTOF0SpacePointArrayPtr();

      TOF1SpacePointArray* tof1_spoints  =
        (*events)[n_event]->GetTOFEvent()->GetTOFEventSpacePointPtr()->GetTOF1SpacePointArrayPtr();

      TOF2SpacePointArray* tof2_spoints  =
        (*events)[n_event]->GetTOFEvent()->GetTOFEventSpacePointPtr()->GetTOF2SpacePointArrayPtr();

      TOF0SlabHitArray tof0_slHits = tSlabHits->GetTOF0SlabHitArray();
      TOF1SlabHitArray tof1_slHits = tSlabHits->GetTOF1SlabHitArray();
      TOF2SlabHitArray tof2_slHits = tSlabHits->GetTOF2SlabHitArray();

      keysVec_t station_vec(0);

      // store the pointers to slabhits, spacepoints, and the detector name
      station_vec.push_back(make_pair(make_pair(tof0_slHits, tof0_spoints),
                                      "tof0"));
      station_vec.push_back(make_pair(make_pair(tof1_slHits, tof1_spoints),
                                      "tof1"));
      station_vec.push_back(make_pair(make_pair(tof2_slHits, tof2_spoints),
                                      "tof2"));

      // IMPORTANT: the trigger station MUST be processed first
      // this is to enable the trigger-pixel-finding necessary for calibrating
      // -- check the trigger station and reorder the vector of stationKeys
      //    so that trigger station hits go first
      switch ( _trigStn ) {
          case 1:
             std::swap(station_vec[0], station_vec[1]);
             break;
          case 2:
             std::swap(station_vec[0], station_vec[2]);
             break;
      }

      // loop over the stations and process them
      for (keysVec_t::iterator it = station_vec.begin();
              it != station_vec.end();
              ++it) {
          // std::cerr << "DEBUG: Processing " << it->second << std::endl;
          processTOFStation((it->first).first, (it->first).second, it->second,
                            n_event, triggerhit_pixels);
          std::string det = it->second;
          if (det == "tof0")
              tof0_slHits = (it->first).first;
          else if (det == "tof1")
              tof1_slHits = (it->first).first;
          else if (det == "tof2")
              tof2_slHits = (it->first).first;
          for (unsigned int j = 0; j < ((it->first).first).size(); ++j) {
            TOFSlabHit tof_slh = ((it->first).first)[j];
          }
      }
      tSlabHits->SetTOF0SlabHitArray(tof0_slHits);
      tSlabHits->SetTOF1SlabHitArray(tof1_slHits);
      tSlabHits->SetTOF2SlabHitArray(tof2_slHits);
      (*events)[n_event]->GetTOFEvent()->SetTOFEventSlabHit(*tSlabHits);
  }
}
////////////////////////////////////////////////////////////
void MapCppTOFSpacePoints::processTOFStation(
                          TOF1SlabHitArray &slHits,
                          TOF1SpacePointArray* spPoints,
                          std::string detector,
                          unsigned int part_event,
                          std::map<int, std::string>& triggerhit_pixels) const {
  // std::cout << "DEBUG MapCppTOFSpacePoints::processTOFStation| "
  //          << "Entry Checkpoint" << std::endl;
  // Get the slab hits document for this TOF station.
  std::vector<int> xPlane0Hits;
  std::vector<int> xPlane1Hits;
  if (slHits.size() != 0) {
      for (unsigned int nslh = 0; nslh < slHits.size(); ++nslh) {
          TOFSlabHit tof1_slh = slHits[nslh];
          int xPlane = tof1_slh.GetPlane();
          switch (xPlane) {
            case 0 :
              xPlane0Hits.push_back(nslh);
              break;
            case 1 :
              xPlane1Hits.push_back(nslh);
              break;
          }
      }
  }
  if (_triggerStation == detector) {
      triggerhit_pixels[part_event] = findTriggerPixel(slHits,
                                                       xPlane0Hits,
                                                       xPlane1Hits);
  }
  // std::cout << "DEBUG MapCppTOFSpacePoints::processTOFStation| "
  //           << "Trigger Pixel: " << triggerhit_pixels[part_event]
  //           << std::endl;

  // If trigger pixel is unknown there is no way to reconstruct time.
  if (triggerhit_pixels[part_event] != "unknown") {
      // Create the space point. Add the calibrated value of the time to the
      // slab hits.
      makeSpacePoints(slHits, spPoints, xPlane0Hits, xPlane1Hits, triggerhit_pixels);
  }
}

////////////////////////////////////////////////////////////
std::string MapCppTOFSpacePoints::findTriggerPixel(
                                       TOF1SlabHitArray &slHits,
                                       std::vector<int> xPlane0Hits,
                                       std::vector<int> xPlane1Hits) const {

  // Loop over all possible combinations of slab hits in the trigger station.
  for (unsigned int nX = 0; nX < xPlane0Hits.size(); nX++) {
    for (unsigned int nY = 0; nY < xPlane1Hits.size(); nY++) {
      // Get the two slab hits.
      TOFSlabHit tof_slh_X = slHits[xPlane0Hits[nX]];
      TOFSlabHit tof_slh_Y = slHits[xPlane1Hits[nY]];
      int slabX = tof_slh_X.GetSlab();
      int slabY = tof_slh_Y.GetSlab();
      TOFPixelKey xTriggerPixelKey(_trigStn, slabX, slabY, _triggerStation);

      // Apply the calibration corrections assuming that this pixel gives the
      // trigger. If this assumption is correct the value of the time after the
      // corrections has to be approximately 0.
      double t_x, t_y;
      if (calibrateSlabHit(xTriggerPixelKey, tof_slh_X, t_x) &&
          calibrateSlabHit(xTriggerPixelKey, tof_slh_Y, t_y)) {
         // std::cerr << "DEBUG MapCppTOFSpacePoints::findTriggerPixel| "
         //          << "t_x: " << t_x << "\tt_y: " << t_y
         //          << "\t_findTriggerPixelCut: " << _findTriggerPixelCut
         //          << std::endl;
        if (fabs(t_x/2. + t_y/2.) < _findTriggerPixelCut) {
          // The trigger pixel has been found.
          // std::cout << xTriggerPixelKey << std::endl;
          return xTriggerPixelKey.str();
        }
      }
    }
  }
  return "unknown";
}

////////////////////////////////////////////////////////////
void MapCppTOFSpacePoints::makeSpacePoints(
              TOF1SlabHitArray &slHits,
              TOF1SpacePointArray* spPoints,
              std::vector<int> xPlane0Hits,
              std::vector<int> xPlane1Hits,
              std::map<int, std::string>& triggerhit_pixels) const {
  // Loop over all possible combinations of slab hits in the trigger station.
  for (unsigned int nX = 0; nX < xPlane0Hits.size(); nX++) {
    for (unsigned int nY = 0; nY < xPlane1Hits.size(); nY++) {
      TOFSpacePoint xTheSpacePoint;
      int xPartEvent = (slHits[(xPlane0Hits[0])]).GetPartEventNumber();
      TOFSlabHit slh_x = slHits[(xPlane0Hits[nX])];
      TOFSlabHit slh_y = slHits[(xPlane1Hits[nY])];
      TOFPixelKey xTriggerPixelKey(triggerhit_pixels[xPartEvent]);
      double t_x, t_y;
      if (calibrateSlabHit(xTriggerPixelKey,
                           slh_x,
                           t_x) &&
          calibrateSlabHit(xTriggerPixelKey,
                           slh_y,
                           t_y)) {
        // The first argument should be the hit in the horizontal slab and the
        // second should be the hit in the vertical slab. This is mandatory!!!
        // std::cerr << "filling sp" << std::endl;
        slHits[(xPlane0Hits[nX])] = slh_x;
        slHits[(xPlane1Hits[nY])] = slh_y;
        fillSpacePoint(xTheSpacePoint, slh_x, slh_y);
        double deltaT = xTheSpacePoint.GetDt();
        if (fabs(deltaT) < _makeSpacePointCut) {
          spPoints->push_back(xTheSpacePoint);
        }
      }
    }
  }
}

////////////////////////////////////////////////////////////
void MapCppTOFSpacePoints::fillSpacePoint(TOFSpacePoint &xSpacePoint, TOFSlabHit &xDocSlabHit_X,
                                                 TOFSlabHit &xDocSlabHit_Y) const {

  // First get the two channel keys and make the pixel key.
  std::string keyStr_SlabX_digit0 = (xDocSlabHit_X.GetPmt0Ptr())->GetTofKey();
  TOFChannelKey xKey_SlabX_digit0(keyStr_SlabX_digit0);

  std::string keyStr_SlabY_digit0 = (xDocSlabHit_Y.GetPmt0Ptr())->GetTofKey();
  TOFChannelKey xKey_SlabY_digit0(keyStr_SlabY_digit0);

///////////////////
  // ATTENTION : according to the convention used in the cabling file the
  // horizontal slabs are always in plane 0 and the vertical slabs are always in
  // plane 1.
  // The second argument in the constructor has to be the number of the
  // horizontal slab, and the third argument has to be the vertical slab.
  // This is mandatory!!!
  TOFPixelKey xSPKey(xKey_SlabX_digit0.station(),
                     xKey_SlabX_digit0.slab(),
                     xKey_SlabY_digit0.slab(),
                     xKey_SlabY_digit0.detector());
  double time_SlabX = xDocSlabHit_X.GetTime();
  double time_SlabY = xDocSlabHit_Y.GetTime();
  double charge_SlabX = xDocSlabHit_X.GetCharge();
  double charge_SlabY = xDocSlabHit_Y.GetCharge();
  double chargeProduct_SlabX = xDocSlabHit_X.GetChargeProduct();
  double chargeProduct_SlabY = xDocSlabHit_Y.GetChargeProduct();
  double time = (time_SlabX + time_SlabY)/2.;
  double dt = time_SlabX - time_SlabY;
  xSpacePoint.SetTime(time);
  xSpacePoint.SetDt(dt);
  xSpacePoint.SetPartEventNumber(xDocSlabHit_X.GetPartEventNumber());
  xSpacePoint.SetPhysEventNumber(xDocSlabHit_X.GetPhysEventNumber());
  xSpacePoint.SetSlabx(xKey_SlabX_digit0.slab());
  xSpacePoint.SetHorizSlab(xKey_SlabX_digit0.slab());
  xSpacePoint.SetStation(xKey_SlabX_digit0.station());
  xSpacePoint.SetDetector(xKey_SlabX_digit0.detector());
  xSpacePoint.SetSlaby(xKey_SlabY_digit0.slab());
  xSpacePoint.SetVertSlab(xKey_SlabY_digit0.slab());
  xSpacePoint.SetPixelKey(xSPKey.str());
  xSpacePoint.SetCharge(charge_SlabX + charge_SlabY);
  xSpacePoint.SetChargeProduct(chargeProduct_SlabX + chargeProduct_SlabY);
  // std::cout << xSPKey << "  t = " << time << " dt = " << dt << std::endl;

  // get the global position of the space point and store it

  // encode the Station+SlabX+SlabY in a key string
  std::stringstream ss;
  ss << xKey_SlabX_digit0.station() << xKey_SlabX_digit0.slab() << xKey_SlabY_digit0.slab();

  // get the position of the station/slab/plane from the geometry
  std::map<std::string, TOFModuleGeo>::const_iterator _geoIt = _geom_map.find(ss.str());

  // std::cout << "DEBUG: MapCppTOFSpacePoints: Station: "
  //           << _geoIt->second.station << " SlabX: " << _geoIt->second.slabX
  //           << " SlabY: " << _geoIt->second.slabY << " " << std::endl;

  xSpacePoint.SetGlobalPosX(_geoIt->second.posX);
  xSpacePoint.SetGlobalPosY(_geoIt->second.posY);
  xSpacePoint.SetGlobalPosZ(_geoIt->second.posZ);
  xSpacePoint.SetGlobalPosXErr(_geoIt->second.posXErr);
  xSpacePoint.SetGlobalPosYErr(_geoIt->second.posYErr);
  xSpacePoint.SetGlobalPosZErr(_geoIt->second.posZErr);
}

////////////////////////////////////////////////////////////
template<typename T>
bool MapCppTOFSpacePoints::calibratePmtHit(TOFPixelKey xTriggerPixelKey,
                                           T xPmtHit,
                                           double &time) const {
  int charge;
  // Charge of the digit can be unset because of the Zero suppresion of the
  // fADCs.
  charge = xPmtHit->GetCharge();
  std::string keyStr = xPmtHit->GetTofKey();
  TOFChannelKey xChannelKey(keyStr);

  double raw_time = xPmtHit->GetRawTime();
  // Get the calibration correction.
  double dT = _map.dT(xChannelKey, xTriggerPixelKey, charge);
  // std::cout << "dt= " << dT << std::endl;
  if (dT == TOFCalibrationMap::NOCALIB)
    return  false;

  time = raw_time - dT;
  xPmtHit->SetTime(time);
  // std::cout << "calibratePmtHit " << xChannelKey << " " << xTriggerPixelKey
  //           << " t = " << raw_time << " - " << dT << " = " << time << std::endl;
  return true;
}

////////////////////////////////////////////////////////////
bool MapCppTOFSpacePoints::calibrateSlabHit(TOFPixelKey xTriggerPixelKey,
                                            TOFSlabHit &xSlabHit,
                                            double &time) const {

  double time_digit0, time_digit1;

  // Calibrate the digit measurements.
  if (calibratePmtHit(xTriggerPixelKey, xSlabHit.GetPmt0Ptr(), time_digit0) &&
      calibratePmtHit(xTriggerPixelKey, xSlabHit.GetPmt1Ptr(), time_digit1)) {
    time = (time_digit0 + time_digit1)/2.;
    xSlabHit.SetTime(time);
    return true;
  }
  return false;
}

////////////////////////////////////////////////////////////
void MapCppTOFSpacePoints::getTofCalib(int runNumber) {
  // Load the calibration.
  runNumberSave = runNumber;
  _map_init = _map.InitializeFromCards(configJSON, runNumber);
  if (!_map_init) {
    throw MAUS::Exceptions::Exception(Exceptions::recoverable,
                          "Failed to initialize calibration map",
                          "MapCppTOFSpacePoints::_process");
  }
  // _map.Print();
}

///////////////////////////////////////////////////////////
void MapCppTOFSpacePoints::build_geom_map() {
  // get the tof geometry modules
  geo_module = new MiceModule(_geo_filename);
  tof_modules = geo_module->findModulesByPropertyString("SensitiveDetector", "TOF");
  for (unsigned int sx = 0; sx < tof_modules.size(); ++sx) {
    for (unsigned int sy = 0; sy < tof_modules.size(); ++sy) {
      std::stringstream ss;
      int stnX = tof_modules[sx]->propertyInt("Station");
      int stnY = tof_modules[sy]->propertyInt("Station");
      if (stnX != stnY) continue;

      int plnX = tof_modules[sx]->propertyInt("Plane");
      int plnY = tof_modules[sy]->propertyInt("Plane");
      if (plnX != 0 || plnY != 1) continue;

      int slbX = tof_modules[sx]->propertyInt("Slab");
      int slbY = tof_modules[sy]->propertyInt("Slab");

      ss << stnX << slbX << slbY;

      // std::cout << "DEBUG: MapCppTOFSpacePoints: Global Position: "
      //           << tof_modules[sx]->globalPosition()
      //           << " " << tof_modules[sy]->globalPosition() << std::endl;

      TOFModuleGeo _mod_geo;

      _mod_geo.station = stnX;
      _mod_geo.slabX = slbX;
      _mod_geo.slabY = slbY;

      _mod_geo.posX = tof_modules[sy]->globalPosition().x();
      _mod_geo.posXErr = tof_modules[sy]->dimensions().x()/sqrt(12.);
      _mod_geo.posY = tof_modules[sx]->globalPosition().y();
      _mod_geo.posYErr = tof_modules[sx]->dimensions().y()/sqrt(12.);

      double zx = tof_modules[sx]->globalPosition().z();
      double zy = tof_modules[sy]->globalPosition().z();
      _mod_geo.posZ = (zx + zy)/2.0;
      _mod_geo.posZErr = tof_modules[sx]->dimensions().z()/sqrt(12.);

      _geom_map[ss.str()] = _mod_geo;
    }
  }
  delete geo_module;
}
///////////////////////////////////////////////////////////
} // end namespace MAUS
