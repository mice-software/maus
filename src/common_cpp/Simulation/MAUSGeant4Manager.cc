// MAUS WARNING: THIS IS LEGACY CODE.
/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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

#include "src/common_cpp/Simulation/MAUSGeant4Manager.hh"

#include "src/legacy/Simulation/MICEPhysicsList.hh"

#include "src/common_cpp/Simulation/FieldPhaser.hh"

#include "src/legacy/Interface/Squeak.hh"

namespace MAUS {

MAUSGeant4Manager* MAUSGeant4Manager::GetInstance() {
  static MAUSGeant4Manager* _instance = new MAUSGeant4Manager();
  return _instance;
}

MAUSGeant4Manager::MAUSGeant4Manager() : _storeTracks(false) {
    _runManager = new G4RunManager;
    _detector   = new MICEDetectorConstruction(*MICERun::getInstance());
    _runManager->SetUserInitialization(_detector);

    _physList = MICEPhysicsList::GetMICEPhysicsList();
    _runManager->SetUserInitialization(_physList);

    _primary  = new MAUSPrimaryGeneratorAction();
    _stepAct  = new MAUSSteppingAction();
    _trackAct = new MAUSTrackingAction();
    _runManager->SetUserAction(_primary);
    _runManager->SetUserAction(_trackAct);
    _runManager->SetUserAction(_stepAct);
    _runManager->SetUserAction(new MICERunAction);

    _runManager->Initialize();

    G4UImanager* UI = G4UImanager::GetUIpointer();

    if (_storeTracks) {
      UI->ApplyCommand("/tracking/storeTrajectory 1");
    } else {
      UI->ApplyCommand("/tracking/storeTrajectory 0");
    }
}

MAUSGeant4Manager::~MAUSGeant4Manager() {
    delete _runManager;
}

void MAUSGeant4Manager::SetPhases() {
  FieldPhaser phaser;
  phaser.SetPhases();
}

MAUSPrimaryGeneratorAction::PGParticle MAUSGeant4Manager::GetReferenceParticle() {
    MAUSPrimaryGeneratorAction::PGParticle p;
    Json::Value* conf = MICERun::getInstance()->jsonConfiguration;
    Json::Value ref = JsonWrapper::GetProperty(*conf, "simulation_reference_particle", JsonWrapper::objectValue);
    p.ReadJson(ref);
    return p;
}

Json::Value MAUSGeant4Manager::RunParticle(MAUSPrimaryGeneratorAction::PGParticle p) {
    Squeak::mout(Squeak::debug) << "Firing particle with ";
    JsonWrapper::Print(Squeak::mout(Squeak::debug), p.WriteJson());
    Squeak::mout(Squeak::debug) << std::endl;
    MAUSGeant4Manager::GetInstance()->GetPrimaryGenerator()->Push(p);
    MAUSGeant4Manager::GetInstance()->GetRunManager()->BeamOn(1);
    Json::Value tracks = MAUSGeant4Manager::GetInstance()->GetStepping()->GetTracks();
    MAUSGeant4Manager::GetInstance()->GetStepping()->SetTracks(Json::Value(Json::objectValue));
    VirtualPlaneManager::StartOfEvent();  // Should be in EventAction
    return tracks;
}


} // namespace MAUS

