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

SciFiSD::SciFiSD(MiceModule* mod) : MAUSSD(mod), _hits(NULL) {
}

G4bool SciFiSD::ProcessHits(G4Step* aStep, G4TouchableHistory* ROhist) {
  G4double edep = aStep->GetTotalEnergyDeposit();
  if ( edep == 0. ) return false;

  MAUS::SciFiHit hit;
  G4TouchableHandle theTouchable = aStep->GetPreStepPoint()->GetTouchableHandle();
  G4int fiberNumber = theTouchable->GetCopyNumber();  // get the fiber copy number
  G4ThreeVector Pos = aStep->GetPreStepPoint()->GetPosition();  // true MC position
  G4ThreeVector Mom = aStep->GetTrack()->GetMomentum();  // true momentum

  MAUS::SciFiChannelId* channel_id = new MAUS::SciFiChannelId();
  channel_id->SetFibreNumber(fiberNumber);
  channel_id->SetTrackerNumber(_module->propertyInt("Tracker"));
  channel_id->SetStationNumber(_module->propertyInt("Station"));
  channel_id->SetPlaneNumber(_module->propertyInt("Plane"));

  hit.SetChannelId(channel_id);
  hit.SetTrackId(aStep->GetTrack()->GetTrackID());
  hit.SetEnergy(aStep->GetTrack()->GetTotalEnergy());
  hit.SetCharge(aStep->GetTrack()->GetDefinition()->GetPDGCharge());
  hit.SetParticleId(aStep->GetTrack()->GetDefinition()->GetPDGEncoding());
  hit.SetTime(aStep->GetPreStepPoint()->GetGlobalTime());
  hit.SetEnergyDeposited(edep);
  hit.SetPosition(MAUS::ThreeVector(Pos.x(), Pos.y(), Pos.z()));
  hit.SetMomentum(MAUS::ThreeVector(Mom.x(), Mom.y(), Mom.z()));

  _hits->push_back(hit);
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

int SciFiSD::GetNHits() {
    if (_hits == NULL)
        return 0;
    return _hits->size();
}

void SciFiSD::ClearHits() {
    if (_hits != NULL) {
        delete _hits;
    }
    _hits = new std::vector<MAUS::SciFiHit>();
}

void SciFiSD::GetHits(MAUS::MCEvent* event) {
    event->SetSciFiHits(_hits);
    ClearHits(); 
}

void SciFiSD::EndOfEvent(G4HCofThisEvent* HCE) {
  // do nothing
}
