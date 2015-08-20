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
#include "Config/MiceModule.hh"
#include "src/common_cpp/API/PyWrapMapBase.hh"

#include "src/map/MapCppKLCellHits/MapCppKLCellHits.hh"

namespace MAUS {

PyMODINIT_FUNC init_MapCppKLCellHits(void) {
  PyWrapMapBase<MAUS::MapCppKLCellHits>::PyWrapMapBaseModInit
                                          ("MapCppKLCellHits", "", "", "", "");
}

MapCppKLCellHits::MapCppKLCellHits() : MapBase<MAUS::Data>("MapCppKLCellHits") {
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

void MapCppKLCellHits::_process(MAUS::Data* data) const {

  // Get spill, break if there's no DAQ data
  Spill *spill = data->GetSpill();

  if (spill->GetReconEvents() == NULL)
    return;

  if (spill->GetDaqEventType() != "physics_event")
    return;

  ReconEventPArray *events = spill->GetReconEvents();
  int nPartEvents = events->size();

  for (int xPE = 0; xPE < nPartEvents; xPE++) {
    KLDigitArray *kl_digits = (events->at(xPE))->GetKLEvent()
                                   ->GetKLEventDigitPtr()->GetKLDigitArrayPtr();
    KLCellHitArray *kl_cellHits = (events->at(xPE))->GetKLEvent()
                                     ->GetKLEventCellHitPtr()->GetKLCellHitArrayPtr();
    this->makeCellHits(kl_cellHits, kl_digits);
  }
}

void MapCppKLCellHits::makeCellHits(KLCellHitArray* kl_CellHits, KLDigitArray* kl_digits) const {

  int n_digits = kl_digits->size();
  if (n_digits == 0)
    return;

  // Create a map of all hited PMTs.
  std::map<string, int> xDigitPos;
  std::map<string, int>::iterator it;

  for (int xDigit = 0; xDigit < n_digits; xDigit++) {
    std::string xKeyStr = (*kl_digits)[xDigit].GetKlKey();
    KLChannelKey xKey(xKeyStr);
    // Add this PMT to the map.
    xDigitPos[xKeyStr] = xDigit;
  }
  while ( xDigitPos.size() > 1 ) {
    // Get the first element of the map and check if we have a hit
    // at the opposite side of the slab.
    it = xDigitPos.begin();
    KLChannelKey xKey(it->first);

    // Get the opposite PMT coded as string.
    std::string xOppositPmtKey_str = xKey.GetOppositeSidePMTStr();
    if (xDigitPos.find(xOppositPmtKey_str) != xDigitPos.end()) {
      // Create Cell hit.
      KLCellHit xTheCellHit;
      if (xKey.pmt() == 0) {
        this->fillCellHit(xTheCellHit, (*kl_digits)[it->second],
                                       (*kl_digits)[xDigitPos[xOppositPmtKey_str]]);
      }

      if (xKey.pmt() == 1) {
        this->fillCellHit(xTheCellHit, (*kl_digits)[xDigitPos[xOppositPmtKey_str]],
                                       (*kl_digits)[it->second]);
      }

      kl_CellHits->push_back(xTheCellHit);
      // Erase both used hits from the map.
      xDigitPos.erase(it);
      xDigitPos.erase(xOppositPmtKey_str);
    } else {
      // Erese this hit from the map.
      xDigitPos.erase(it);
    }
  }
}

void MapCppKLCellHits::fillCellHit(KLCellHit &cellHit, KLDigit &xDigit0, KLDigit &xDigit1) const {

  std::string xKeyStr = xDigit0.GetKlKey();
  KLChannelKey xKey(xKeyStr);
  cellHit.SetCell(xKey.cell());
  cellHit.SetDetector(xKey.detector());
  cellHit.SetPartEventNumber(xDigit0.GetPartEventNumber());
  cellHit.SetPhysEventNumber(xDigit0.GetPhysEventNumber());


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


  cellHit.SetGlobalPosX(cellGlobalPos.x());
  cellHit.SetGlobalPosY(cellGlobalPos.y());
  cellHit.SetGlobalPosZ(cellGlobalPos.z());
  cellHit.SetLocalPosX(cellLocalPos.x());
  cellHit.SetLocalPosY(cellLocalPos.y());
  cellHit.SetLocalPosZ(cellLocalPos.z());
  cellHit.SetErrorX(cellErrorPos.x());
  cellHit.SetErrorY(cellErrorPos.y());
  cellHit.SetErrorZ(cellErrorPos.z());

  // Charge of the digit can be unset because of the Zero suppresion of the fADCs.


  int xChargeDigit0 = xDigit0.GetChargeMm();
  int xChargeDigit1 = xDigit1.GetChargeMm();
  cellHit.SetCharge((xChargeDigit0 + xChargeDigit1)/2);
  if ((xChargeDigit0 + xChargeDigit1) == 0) {
     cellHit.SetChargeProduct(0);
     cellHit.SetFlag(false);
  } else {
     cellHit.SetChargeProduct(2 * xChargeDigit0 * xChargeDigit1 /
                                        (xChargeDigit0 + xChargeDigit1));
     cellHit.SetFlag(true);
  }
}
}
