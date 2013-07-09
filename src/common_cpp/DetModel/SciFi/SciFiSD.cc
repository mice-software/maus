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

SciFiSD::SciFiSD(MiceModule* mod) : MAUSSD(mod) {
}

G4bool SciFiSD::ProcessHits(G4Step* aStep, G4TouchableHistory* ROhist) {

  G4double edep = aStep->GetTotalEnergyDeposit();

  int pid = aStep->GetTrack()->GetDefinition()->GetPDGEncoding();

  if ( pid == -13 ) {
    edep = 10.;
  } else {
    return false;
  }

  // if ( edep == 0. ) return false;

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
  double time = aStep->GetPreStepPoint()->GetGlobalTime();
  _hits["sci_fi_hits"][hit_i]["time"] = time;
  _hits["sci_fi_hits"][hit_i]["energy_deposited"] = edep;
  _hits["sci_fi_hits"][hit_i]["momentum"]["x"] = Mom.x();
  _hits["sci_fi_hits"][hit_i]["momentum"]["y"] = Mom.y();
  _hits["sci_fi_hits"][hit_i]["momentum"]["z"] = Mom.z();
  _hits["sci_fi_hits"][hit_i]["position"]["x"] = Pos.x();
  _hits["sci_fi_hits"][hit_i]["position"]["y"] = Pos.y();
  _hits["sci_fi_hits"][hit_i]["position"]["z"] = Pos.z();

  // this is the rotation of the fibre array
  /*
  // From tracker 0 to tracker 1, the modules global rotation IS different.
  // What DOESN'T change is the plane relative rotation wrt the tracker frame.
  std::cerr << "Module name: " << _module->fullName() << "\n"
            << "Module GLOBAL rotation: " << _module->globalRotation() << "\n"
            // << "Plane direction: " << dir << "\n"
            // << "Mother Logical Volume is: " << mlv->GetLogicalVolume()->GetName() << "\n"
            << "Rotation relatively to: \n"
            << "Station: " << _module->relativeRotation(_module->mother()) << "\n"
            << "Tracker: " << _module->relativeRotation(_module->mother()->mother()) << "\n"
            << "Solenoid: " << _module->relativeRotation(_module->mother()->mother()->mother()) << "\n"
  << "--------------------------------------------------------------------\n";
  */
  return true;
}

void SciFiSD::EndOfEvent(G4HCofThisEvent* HCE) {
  // do nothing
}
