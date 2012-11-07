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

#include "Geant4/G4Track.hh"

#include "json/json.h"

#include "src/common_cpp/Utils/Globals.hh"
#include "src/legacy/Interface/STLUtils.hh"
#include "src/common_cpp/Simulation/MAUSGeant4Manager.hh"
#include "src/common_cpp/Simulation/MAUSStackingAction.hh"

namespace MAUS {

MAUSStackingAction::MAUSStackingAction()
           : G4UserStackingAction(), _default(true), _pdg_pid_keep_kill(),
             _kinetic_energy_threshold(0.) {
    Json::Value& cards = (*Globals::GetInstance()->GetConfigurationCards());
    _default = cards["default_keep_or_kill"].asBool();
    _kinetic_energy_threshold = cards["kinetic_energy_threshold"].asDouble();
    Json::Value keep_flags = cards["keep_or_kill_particles"];
    for (Json::Value::iterator it = keep_flags.begin();
                                                 it != keep_flags.end(); ++it) {
        _pdg_pid_keep_kill[it.key().asString()] =
                                       keep_flags[it.key().asString()].asBool();
    }
}

G4ClassificationOfNewTrack MAUSStackingAction::ClassifyNewTrack
                                                      (const G4Track * aTrack) {
    if (aTrack->GetKineticEnergy() < _kinetic_energy_threshold) {
        std::string ke = STLUtils::ToString(aTrack->GetKineticEnergy());
        std::string thresh = STLUtils::ToString(_kinetic_energy_threshold);
        MAUSGeant4Manager::GetInstance()->GetTracking()->SetKillReason
                (aTrack, "Kinetic Energy "+ke+" less than threshold "+thresh);
        return fKill;
    }
    std::string name = aTrack->GetDefinition()->GetParticleName();
    // if name is not in the keep_kill map
    if (_pdg_pid_keep_kill.find(name) == _pdg_pid_keep_kill.end()) {
        if (!_default) { // false means we kill by default
            MAUSGeant4Manager::GetInstance()->GetTracking()
                                    ->SetKillReason(aTrack, "PID "+name+
                                   " not in keep list and default set to kill");
            return fKill;
        }
    } else { // name is in keep_kill map
        if (!_pdg_pid_keep_kill[name]) {
            MAUSGeant4Manager::GetInstance()->GetTracking()
                           ->SetKillReason(aTrack, "PID "+name+" in kill list");
            return fKill;
        }
    }
    return fWaiting;
}
}


