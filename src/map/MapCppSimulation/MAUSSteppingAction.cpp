#include <iostream>

#include "Interface/Squeak.hh"
#include "Interface/Squeal.hh"

#include "MAUSSteppingAction.h"

namespace MAUS {

MAUSSteppingAction*
MAUSSteppingAction::_self = 0;

MAUSSteppingAction::MAUSSteppingAction() {
  _maxNSteps = 1000000; // BUG - how to access _maxNSteps?
  if (_self == 0) {
    _self = this;
  } else {
    throw Squeal(Squeal::recoverable,
                 "Error.  You may only have one MAUSSteppingAction",
                 "MAUSSteppingAction()");
  }
}

void MAUSSteppingAction::UserSteppingAction(const G4Step * aStep) {
  G4Track* aTrack = aStep->GetTrack();
  G4StepPoint* postStep = aStep->GetPostStepPoint();
  if (!aTrack) return;
  if (!postStep) return;

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

  if (!_tracks.isMember(ss.str())) {  //  new track, then
    _tracks[ss.str()] = Json::Value();
    _tracks[ss.str()]["steps"] = Json::Value(Json::arrayValue);
  }

  _tracks[ss.str()]["steps"].append(step);


  if (_tracks[ss.str()]["steps"].size() > _maxNSteps) {
    std::stringstream ss2;
    ss2 << _tracks[ss.str()]["steps"].size() <<  " steps greater than maximum "
        << _maxNSteps << " - suspected of looping";
    _tracks[ss.str()]["KillReason"] = Json::Value(ss2.str());
    aTrack->SetTrackStatus(fStopAndKill);
  }
}

}  //  ends MAUS namespace
