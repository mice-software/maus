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
#include "Utils/Exception.hh"
#include "Interface/MICERun.hh"


#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/Simulation/MAUSSteppingAction.hh"
#include "src/common_cpp/Simulation/VirtualPlanes.hh"
#include "src/common_cpp/Simulation/MAUSGeant4Manager.hh"


namespace MAUS {

MAUSSteppingAction::MAUSSteppingAction() : _steps(NULL), _keepSteps(false),
    _maxNSteps(0) {
  Json::Value& conf = *MICERun::getInstance()->jsonConfiguration;
  _maxNSteps = JsonWrapper::GetProperty
               (conf, "maximum_number_of_steps", JsonWrapper::intValue).asInt();
  _keepSteps = JsonWrapper::GetProperty
               (conf, "keep_steps", JsonWrapper::booleanValue).asBool();
}

void MAUSSteppingAction::UserSteppingAction(const G4Step * aStep) {
    // record all step points if keep steps flag is set
    if (_keepSteps && _steps == NULL)
        throw Exception(Exception::recoverable,
                        "Attempt to do stepping action when steps were NULL",
                        "MAUSSteppingAction::UserSteppingAction");
    if (_keepSteps) {
        if (_steps->size() == 0) {
            _steps->push_back(StepToMaus(aStep, true));
        }
        _steps->push_back(StepToMaus(aStep, false));
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

std::vector<Step>* MAUSSteppingAction::TakeSteps() {
    std::vector<Step>* steps_tmp = _steps;
    _steps = NULL;
    return steps_tmp;
}


Step MAUSSteppingAction::StepToMaus(const G4Step* aStep, bool prestep) const {
    G4Track* aTrack = aStep->GetTrack();
    G4StepPoint* point = aStep->GetPostStepPoint();
    if (prestep) point = aStep->GetPreStepPoint();
    if (!aTrack || !point || !aStep)
        throw(Exception(Exception::recoverable,
                     "Failed to resolve step point",
                     "MAUSSteppingAction::StepPointToJson"));
    Step step;
    ThreeVector position(point->GetPosition().x(),
                         point->GetPosition().y(),
                         point->GetPosition().z());
    step.SetPosition(position);

    ThreeVector momentum(point->GetMomentum().x(),
                         point->GetMomentum().y(),
                         point->GetMomentum().z());
    step.SetMomentum(momentum);

    ThreeVector spin(point->GetPolarization().x(),
                     point->GetPolarization().y(),
                     point->GetPolarization().z());
    step.SetSpin(spin);

    double f_pos[4] = {point->GetPosition().x(), point->GetPosition().y(),
                       point->GetPosition().z(), point->GetGlobalTime()};
    double field[6] = {0., 0., 0., 0., 0., 0.};
    MAUSGeant4Manager::GetInstance()->GetField()->GetFieldValue(f_pos, field);
    ThreeVector bfield(field[0], field[1], field[2]);
    step.SetBField(bfield);
    ThreeVector efield(field[3], field[4], field[5]);
    step.SetEField(efield);

    step.SetEnergy(point->GetTotalEnergy());
    step.SetProperTime(point->GetProperTime());
    step.SetPathLength(aTrack->GetTrackLength());
    step.SetEnergyDeposited(aStep->GetTotalEnergyDeposit());
    step.SetTime(point->GetGlobalTime());
    // G4StepPoint GetPhysicalVolume is inlined
    // point->GetTouchable()->GetPhysicalVolume() with no NULL check so we do it
    // here
    if (point->GetTouchable() != NULL && point->GetPhysicalVolume() != NULL) {
        step.SetVolume(point->GetPhysicalVolume()->GetName());
    } else {
        step.SetVolume("");
    }
    if (point->GetMaterial() != NULL) {
        step.SetMaterial(point->GetMaterial()->GetName());
    } else {
        step.SetMaterial("");
    }
    return step;
}

void  MAUSSteppingAction::SetSteps(std::vector<Step>* steps) {
    if (_steps != NULL) {
        delete _steps;
    }
    _steps = steps;
}


}  //  ends MAUS namespace
