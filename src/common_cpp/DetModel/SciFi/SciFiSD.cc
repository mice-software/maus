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
#include "src/common_cpp/DetModel/SciFi/SciFiSD.hh"

#include "Geant4/G4TransportationManager.hh"
#include "Geant4/G4FieldManager.hh"
#include "Geant4/G4Field.hh"
#include "Geant4/G4HCofThisEvent.hh"
#include "Geant4/G4Step.hh"
#include "Geant4/G4ThreeVector.hh"
#include "Geant4/G4SDManager.hh"
#include "Geant4/G4ios.hh"

#include <iostream>
#include <fstream>
#include <cmath>
#include "Interface/MICEEvent.hh"
#include "src/legacy/Config/MiceModule.hh"

#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TTree.h"

SciFiSD::SciFiSD(MiceModule* mod) : MAUSSD(mod) {
}


G4bool SciFiSD::ProcessHits(G4Step* aStep, G4TouchableHistory* ROhist) {

  G4double edep = aStep->GetTotalEnergyDeposit();

  int pid = aStep->GetTrack()->GetDefinition()->GetPDGEncoding();

  if ( edep == 0. ) return false;
  if ( fabs(pid) != 13 ) return false;
  // the old chanNo, held for comparison
  int old_chanNo = legacy_chanNo(aStep);

  if (!_hits.isMember("sci_fi_hits")) {
    _hits["sci_fi_hits"] = Json::Value(Json::arrayValue);
  }
  int hit_i = _hits["sci_fi_hits"].size();
  _hits["sci_fi_hits"].append(Json::Value());

  G4TouchableHandle theTouchable = aStep->GetPreStepPoint()->GetTouchableHandle();
  G4int fiberNumber = theTouchable->GetCopyNumber();  // get the fiber copy number
  G4ThreeVector Pos = aStep->GetPreStepPoint()->GetPosition();  // true MC position
  G4ThreeVector Mom = aStep->GetTrack()->GetMomentum();  // true momentum

  Json::Value channel_id;
  channel_id["fibre_number"] = fiberNumber;
  channel_id["tracker_number"] = _module->propertyInt("Tracker");
  channel_id["station_number"] = _module->propertyInt("Station");
  channel_id["plane_number"] = _module->propertyInt("Plane");

  _hits["sci_fi_hits"][hit_i]["channel_id"] = channel_id;
  _hits["sci_fi_hits"][hit_i]["track_id"] = aStep->GetTrack()->GetTrackID();
  _hits["sci_fi_hits"][hit_i]["energy"] = aStep->GetTrack()->GetTotalEnergy();
  _hits["sci_fi_hits"][hit_i]["charge"] =
                             aStep->GetTrack()->GetDefinition()->GetPDGCharge();
  _hits["sci_fi_hits"][hit_i]["particle_id"] = pid;
  _hits["sci_fi_hits"][hit_i]["time"] =
                                      aStep->GetPreStepPoint()->GetGlobalTime();
  _hits["sci_fi_hits"][hit_i]["energy_deposited"] = edep;
  _hits["sci_fi_hits"][hit_i]["momentum"]["x"] = Mom.x();
  _hits["sci_fi_hits"][hit_i]["momentum"]["y"] = Mom.y();
  _hits["sci_fi_hits"][hit_i]["momentum"]["z"] = Mom.z();
  _hits["sci_fi_hits"][hit_i]["position"]["x"] = Pos.x();
  _hits["sci_fi_hits"][hit_i]["position"]["y"] = Pos.y();
  _hits["sci_fi_hits"][hit_i]["position"]["z"] = Pos.z();

  int chanNo;
  int numbFibres = static_cast<int> (7*2*(_module->propertyDouble("CentralFibre")+0.5));
  if ( _module->propertyInt("Tracker") == 0 ) {
    chanNo = static_cast<int> (floor((numbFibres-fiberNumber)/7));
  } else {
    chanNo = static_cast<int> (floor(fiberNumber/7));
  }
  // assert agreement on chanNo with legacy calculation
  assert(abs(chanNo-old_chanNo) < 2);

  return true;
}

void SciFiSD::EndOfEvent(G4HCofThisEvent* HCE) {
  // do nothing
}

int SciFiSD::legacy_chanNo(G4Step* aStep) {
  Hep3Vector pos = _module->globalPosition();
  Hep3Vector perp(-1., 0., 0.);
  double chanWidth = 1.4945; // effective channel width without overlap
  perp *= _module->globalRotation();
  Hep3Vector delta = aStep->GetPreStepPoint()->GetPosition() - pos;
  double dist = delta.x() * perp.x() + delta.y() * perp.y() + delta.z() * perp.z();
  double fibre = _module->propertyDouble("CentralFibre") + dist * 2.0 /
                ( _module->propertyDouble("Pitch") * 7.0 );

  double centFibre = _module->propertyDouble("CentralFibre");
  int  firstChan = static_cast<int> (fibre + 0.5);
  int secondChan(-1);
  double overlap = chanWidth - (0.1365/2.0);
  double underlap = (0.1365/2.0);
  // int nChans(1);
  nChans = 1;
  double distInChan = ((firstChan - centFibre)*chanWidth) - dist;
  // distance in channel greater than section which does not overlap
  if ((sqrt(distInChan*distInChan) > overlap) || (sqrt(distInChan*distInChan) < underlap)) {
    nChans = 2;
    if (distInChan < 0) {
      secondChan = firstChan - 1;
    } else {
      secondChan = firstChan + 1;
    }
  }
  return firstChan;
}
