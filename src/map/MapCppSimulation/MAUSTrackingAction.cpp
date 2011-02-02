#include "MAUSTrackingAction.h"
#include <iostream>
namespace MAUS {

void MAUSTrackingAction::PreUserTrackingAction(const G4Track* aTrack) {
  MAUSSteppingAction* stepAct = MAUSSteppingAction::GetSteppingAction();
  if (!stepAct)
    return;

  if (!aTrack)
    return;

  std::cout<<"YO!"<<std::endl;

  
  // This gets the track name as indexed in the data structure
  std::stringstream ss;
  ss << "track";
  ss << aTrack->GetTrackID();

  stepAct->_track[ss.str()] = Json::Value();

  Json::Value position;
  position["x"] = aTrack->GetPosition().x();
  position["y"] = aTrack->GetPosition().y();
  position["z"] = aTrack->GetPosition().z();

  Json::Value momentum;
  momentum["x"] = aTrack->GetMomentum().x();
  momentum["y"] = aTrack->GetMomentum().y();
  momentum["z"] = aTrack->GetMomentum().z();

  stepAct->_track[ss.str()]["initial_position"] = position;
  stepAct->_track[ss.str()]["initial_momentum"] = momentum;
  stepAct->_track[ss.str()]["particle_id"] = aTrack->GetDefinition()->GetPDGEncoding();
  stepAct->_track[ss.str()]["track_id"] = aTrack->GetTrackID();
  stepAct->_track[ss.str()]["parent_track_id"] = aTrack->GetParentID();
  std::cout<<"Tsize "<< MAUSSteppingAction::GetSteppingAction()->_track.size()<<std::endl;
}

void MAUSTrackingAction::PostUserTrackingAction(const G4Track* aTrack) {
  MAUSSteppingAction* stepAct = MAUSSteppingAction::GetSteppingAction();
  if (!stepAct)
    return;

  if (!aTrack)
    return;

  // This gets the track name as indexed in the data structure
  std::stringstream ss;
  ss << "track";
  ss << aTrack->GetTrackID();

  Json::Value position;
  position["x"] = aTrack->GetPosition().x();
  position["y"] = aTrack->GetPosition().y();
  position["z"] = aTrack->GetPosition().z();

  Json::Value momentum;
  momentum["x"] = aTrack->GetMomentum().x();
  momentum["y"] = aTrack->GetMomentum().y();
  momentum["z"] = aTrack->GetMomentum().z();

  stepAct->_track[ss.str()]["final_position"] = position;
  stepAct->_track[ss.str()]["final_momentum"] = momentum;
  std::cout<<"Tsize "<< MAUSSteppingAction::GetSteppingAction()->_track.size()<<std::endl;
}

}  //  ends MAUS namespace
