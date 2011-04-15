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

#include <iostream>

#include "MAUSGeant4Manager.hh"
#include "MAUSTrackingAction.hh"

namespace MAUS {

void MAUSTrackingAction::PreUserTrackingAction(const G4Track* aTrack) {
  MAUSSteppingAction* stepAct = MAUSGeant4Manager::GetInstance()->GetStepping();
  if (!stepAct || !aTrack) 
    throw(Squeal(Squeal::nonRecoverable,
                 "Failed to get tracking",
                 "MAUSTrackingAction::PreUserTrackingAction"));

  Json::Value json_track = stepAct->GetTracks();

  // This gets the track name as indexed in the data structure
  std::stringstream ss;
  ss << "track";
  ss << aTrack->GetTrackID();

  if (json_track[ss.str()].type() != Json::nullValue)
    throw(Squeal(Squeal::recoverable,
                 "Json "+ss.str()+" already exists",
                 "MAUSTrackingAction::PreUserTrackingAction"));

  json_track[ss.str()] = Json::Value();

  Json::Value position;
  position["x"] = aTrack->GetPosition().x();
  position["y"] = aTrack->GetPosition().y();
  position["z"] = aTrack->GetPosition().z();

  Json::Value momentum;
  momentum["x"] = aTrack->GetMomentum().x();
  momentum["y"] = aTrack->GetMomentum().y();
  momentum["z"] = aTrack->GetMomentum().z();

  json_track[ss.str()]["initial_position"] = position;
  json_track[ss.str()]["initial_momentum"] = momentum;
  json_track[ss.str()]["particle_id"] = aTrack->GetDefinition()->GetPDGEncoding();
  json_track[ss.str()]["track_id"] = aTrack->GetTrackID();
  json_track[ss.str()]["parent_track_id"] = aTrack->GetParentID();
  stepAct->SetTracks(json_track);
}

void MAUSTrackingAction::PostUserTrackingAction(const G4Track* aTrack) {
  MAUSSteppingAction* stepAct = MAUSGeant4Manager::GetInstance()->GetStepping();
  if (!stepAct || !aTrack) 
    throw(Squeal(Squeal::nonRecoverable,
                 "Failed to get tracking",
                 "MAUSTrackingAction::PostUserTrackingAction"));

  // This gets the track name as indexed in the data structure
  std::stringstream ss;
  ss << "track";
  ss << aTrack->GetTrackID();
  Json::Value json_track = stepAct->GetTracks();
  if (json_track[ss.str()].type() == Json::nullValue)
    throw(Squeal(Squeal::recoverable,
                 "Failed to find Json "+ss.str(),
                 "MAUSTrackingAction::PostUserTrackingAction"));

  Json::Value position;
  position["x"] = aTrack->GetPosition().x();
  position["y"] = aTrack->GetPosition().y();
  position["z"] = aTrack->GetPosition().z();

  Json::Value momentum;
  momentum["x"] = aTrack->GetMomentum().x();
  momentum["y"] = aTrack->GetMomentum().y();
  momentum["z"] = aTrack->GetMomentum().z();

  json_track[ss.str()]["final_position"] = position;
  json_track[ss.str()]["final_momentum"] = momentum;

  stepAct->SetTracks(json_track);
}

}  //  ends MAUS namespace
