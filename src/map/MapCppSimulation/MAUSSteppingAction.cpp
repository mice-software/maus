#include "MAUSSteppingAction.h"
#include <iostream>
namespace MAUS {

MAUSSteppingAction*
MAUSSteppingAction::_self = 0;

MAUSSteppingAction::MAUSSteppingAction() {
  if (_self == 0) {
    _self = this;
  } else {
    G4Exception("Error.  You may only have one MAUSSteppinAction");
  }
}

void MAUSSteppingAction::UserSteppingAction(const G4Step * aStep) {
  G4Track* aTrack = aStep->GetTrack();
  if (!aTrack)
    return;

  G4StepPoint* postStep = aStep->GetPostStepPoint();
  if (!postStep)
    return;

  if (abs(aTrack->GetDefinition()->GetPDGEncoding()) != 13){
    aTrack->SetTrackStatus(fStopAndKill);
    return;
  }

  Json::Value step;

  Json::Value position;
  position["x"] = postStep->GetPosition().x();
  position["y"] = postStep->GetPosition().y();
  position["z"] = postStep->GetPosition().z();

  Json::Value momentum;
  momentum["x"] = postStep->GetMomentum().x();
  momentum["y"] = postStep->GetMomentum().y();
  momentum["z"] = postStep->GetMomentum().z();

  step["position"] = position;
  step["momentum"] = momentum;
  step["energy_deposited"] = aStep->GetTotalEnergyDeposit();

  std::stringstream ss;
  ss << "track";
  ss << aTrack->GetTrackID();

  if (!_track.isMember(ss.str())) {  //  new track, then
    _track[ss.str()] = Json::Value();
    _track[ss.str()]["steps"] = Json::Value(Json::arrayValue);
  }

  _track[ss.str()]["steps"].append(step);


  std::cout<<"hi "<<postStep->GetPosition().z()<<" "<<_track[ss.str()]["steps"].size()<<std::endl;
  
  if (_track[ss.str()]["steps"].size() > 100000) {
    aTrack->SetTrackStatus(fStopAndKill);
    std::cout<<"killing"<<std::endl;
  }
}

}  //  ends MAUS namespace
