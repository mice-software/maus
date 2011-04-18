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

#include "Interface/Squeak.hh"
#include "Interface/Squeal.hh"
#include "Interface/MICERun.hh"
#include "Interface/JsonWrapper.hh"

#include "MAUSSteppingAction.hh"

namespace MAUS {

MAUSSteppingAction::MAUSSteppingAction() : _keepTracks(false) {
  Json::Value& conf = *MICERun::getInstance()->jsonConfiguration;
  _maxNSteps = JsonWrapper::GetProperty
               (conf, "maximum_number_of_steps", JsonWrapper::intValue).asInt();
}

void MAUSSteppingAction::UserSteppingAction(const G4Step * aStep) {

    G4Track* aTrack = aStep->GetTrack();
    if (!aTrack) return;
    std::stringstream ss;
    ss << "track";
    ss << aTrack->GetTrackID();

    if ( _keepTracks ) {
        if (!_tracks.isMember(ss.str())) {  //  new track, then
            _tracks[ss.str()] = Json::Value();
            _tracks[ss.str()]["steps"] = Json::Value(Json::arrayValue);
            _tracks[ss.str()]["steps"].append(StepPointToJson
                                                   (aStep, false));
        }
        _tracks[ss.str()]["steps"].append(StepPointToJson
                                                   (aStep, true));
    }

    if (aTrack->GetCurrentStepNumber() > _maxNSteps) {
        std::stringstream ss2;
        ss2 << aTrack->GetCurrentStepNumber() <<  " steps greater than maximum "
            << _maxNSteps << " - suspected of looping";
        _tracks[ss.str()]["KillReason"] = Json::Value(ss2.str());
        aTrack->SetTrackStatus(fStopAndKill);
    }
}

Json::Value MAUSSteppingAction::StepPointToJson
                                     (const G4Step* aStep, bool prestep) const {
    G4Track* aTrack = aStep->GetTrack();
    G4StepPoint* point = aStep->GetPostStepPoint();
    if (prestep) point = aStep->GetPreStepPoint();
    if (!aTrack || !point || !aStep)
        throw(Squeal(Squeal::recoverable, 
                     "Failed to resolve step point",
                     "MAUSSteppingAction::StepPointToJson"));
    Json::Value step;
    Json::Value position;
    position["x"] = point->GetPosition().x();
    position["y"] = point->GetPosition().y();
    position["z"] = point->GetPosition().z();
    step["position"] = position;

    Json::Value momentum;
    momentum["x"] = point->GetMomentum().x();
    momentum["y"] = point->GetMomentum().y();
    momentum["z"] = point->GetMomentum().z();
    step["momentum"] = momentum;

    step["energy"] = point->GetTotalEnergy();
    step["proper_time"] = point->GetProperTime();
    step["path_length"] = aTrack->GetTrackLength();
    step["energy_deposited"] = aStep->GetTotalEnergyDeposit();
    step["time"] = point->GetGlobalTime();

    return step;
}

}  //  ends MAUS namespace
