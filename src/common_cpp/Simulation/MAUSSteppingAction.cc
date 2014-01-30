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

#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/Simulation/MAUSSteppingAction.hh"
#include "src/common_cpp/Simulation/VirtualPlanes.hh"
#include "src/common_cpp/Simulation/MAUSGeant4Manager.hh"


namespace MAUS {

MAUSSteppingAction::MAUSSteppingAction() : _steps(), _keepSteps(false),
    _maxNSteps(0) {
  Json::Value& conf = *MICERun::getInstance()->jsonConfiguration;
  _maxNSteps = JsonWrapper::GetProperty
               (conf, "maximum_number_of_steps", JsonWrapper::intValue).asInt();
  _keepSteps = JsonWrapper::GetProperty
               (conf, "keep_steps", JsonWrapper::booleanValue).asBool();
}

void MAUSSteppingAction::UserSteppingAction(const G4Step * aStep) {
    // record all step points if keep steps flag is set
    if (_keepSteps) {
        if (_steps.size() == 0) {
            _steps.append(StepToJson(aStep, true));
        }
        _steps.append(StepToJson(aStep, false));
    }

    // kill particles that are looping
    G4Track* aTrack = aStep->GetTrack();
    if (aTrack->GetCurrentStepNumber() > _maxNSteps) {
        std::stringstream ss;
        ss << aTrack->GetCurrentStepNumber() <<  " steps greater than maximum "
           << _maxNSteps << " - suspected of looping";
        MAUSGeant4Manager::GetInstance()->GetTracking()
                  ->SetKillReason(aTrack, ss.str());
        aTrack->SetTrackStatus(fStopAndKill);
    }

    // find and append any virtual planes
    MAUSGeant4Manager::GetInstance()->
                         GetVirtualPlanes()->VirtualPlanesSteppingAction(aStep);
}

Json::Value MAUSSteppingAction::StepToJson
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
    
    

//################################################//

    Json::Value Polarization;
    Polarization["x"] = point->GetPolarization().x();
    Polarization["y"] = point->GetPolarization().y();
    Polarization["z"] = point->GetPolarization().z();
    step["Polarization"] = Polarization;



//################################################//




    step["energy"] = point->GetTotalEnergy();
    step["proper_time"] = point->GetProperTime();
    step["path_length"] = aTrack->GetTrackLength();
    step["energy_deposited"] = aStep->GetTotalEnergyDeposit();
    step["time"] = point->GetGlobalTime();

    return step;
}

void  MAUSSteppingAction::SetSteps(Json::Value steps) {
    if (!steps.isArray())
        throw(Squeal(Squeal::recoverable,
              "Attempting to set steps to non-array type",
              "MAUSSteppingAction::SetSteps()"));
    _steps = steps;
}


}  //  ends MAUS namespace
