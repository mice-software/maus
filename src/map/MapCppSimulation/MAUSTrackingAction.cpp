#include <iostream>

#include "MAUSTrackingAction.h"

namespace MAUS {
void MAUSTrackingAction::PreUserTrackingAction(const G4Track* aTrack) {
  MAUSSteppingAction* stepAct = MAUSSteppingAction::GetSteppingAction();
  if (!stepAct) return;
  if (!aTrack) return;

  Json::Value json_track = stepAct->GetTracks();

  // This gets the track name as indexed in the data structure
  std::stringstream ss;
  ss << "track";
  ss << aTrack->GetTrackID();

  json_track[ss.str()] = Json::Value();

  Json::Value position;
  position["x"] = aTrack->GetPosition().x();
  position["y"] = aTrack->GetPosition().y();
  position["z"] = aTrack->GetPosition().z();

  Json::Value momentum;
  momentum["x"] = aTrack->GetMomentum().x();
  momentum["y"] = aTrack->GetMomentum().y();
  momentum["z"] = aTrack->GetMomentum().z();

  json_track["initial_position"] = position;
  json_track[ss.str()]["initial_momentum"] = momentum;
  json_track[ss.str()]["particle_id"] = aTrack->GetDefinition()->GetPDGEncoding();
  json_track[ss.str()]["track_id"] = aTrack->GetTrackID();
  json_track[ss.str()]["parent_track_id"] = aTrack->GetParentID();
}

void MAUSTrackingAction::PostUserTrackingAction(const G4Track* aTrack) {
  MAUSSteppingAction* stepAct = MAUSSteppingAction::GetSteppingAction();
  if (!stepAct) return;
  if (!aTrack) return;

  // This gets the track name as indexed in the data structure
  std::stringstream ss;
  ss << "track";
  ss << aTrack->GetTrackID();
  Json::Value json_track = stepAct->GetTracks();

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
}

}  //  ends MAUS namespace
