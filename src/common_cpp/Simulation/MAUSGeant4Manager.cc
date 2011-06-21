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

MAUSGeant4Manager::MAUSGeant4Manager() {
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
    //  _runManager->SetUserAction(new MAUSStackingActionKillNonMuons);
    _runManager->SetUserAction(new MICERunAction);
    _virtPlanes = new VirtualPlaneManager;
    _virtPlanes->ConstructVirtualPlanes(
      MICERun::getInstance()->btFieldConstructor, 
      MICERun::getInstance()->miceModule
    );
    _runManager->Initialize();
}

MAUSGeant4Manager::~MAUSGeant4Manager() {
    delete _runManager;
}

void MAUSGeant4Manager::SetPhases() {
  FieldPhaser phaser;
  phaser.SetPhases();
}

MAUSPrimaryGeneratorAction::PGParticle 
                                     MAUSGeant4Manager::GetReferenceParticle() {
    MAUSPrimaryGeneratorAction::PGParticle p;
    Json::Value* conf = MICERun::getInstance()->jsonConfiguration;
    Json::Value ref = JsonWrapper::GetProperty
              (*conf, "simulation_reference_particle", JsonWrapper::objectValue);
    p.ReadJson(ref);
    return p;
}

Json::Value MAUSGeant4Manager::RunParticle(Json::Value particle) {
    SetEvent(particle);
    MAUSPrimaryGeneratorAction::PGParticle p;
    p.ReadJson(particle["primary"]);
    return Tracking(p);
}

Json::Value MAUSGeant4Manager::RunParticle
                                    (MAUSPrimaryGeneratorAction::PGParticle p) {
    SetEvent(Json::Value(Json::objectValue));
    return Tracking(p);
}

Json::Value MAUSGeant4Manager::Tracking
                                    (MAUSPrimaryGeneratorAction::PGParticle p) {
    // Some stuff should be in EventAction
    Squeak::mout(Squeak::debug) << "Firing particle with ";
    JsonWrapper::Print(Squeak::mout(Squeak::debug), p.WriteJson());
    Squeak::mout(Squeak::debug) << std::endl;
    GetPrimaryGenerator()->Push(p);
    BeginOfEventAction(NULL);
    GetRunManager()->BeamOn(1);
    EndOfEventAction(NULL);
    return GetEvent();

}

void MAUSGeant4Manager::BeginOfEventAction(const G4Event *anEvent) {
    _virtPlanes->StartOfEvent();
    if (GetTracking()->GetWillKeepTracks())
        GetTracking()->SetTracks(Json::Value(Json::objectValue));
    if (GetStepping()->GetWillKeepSteps())
        GetStepping()->SetSteps(Json::Value(Json::arrayValue));
}

void MAUSGeant4Manager::EndOfEventAction(const G4Event *anEvent) {
    //  For each detector i
    for (unsigned int i = 0; i < GetGeometry()->GetSDSize(); i++) {
      //  Retrieve detector i's hits
      std::vector<Json::Value> hits = GetGeometry()->GetSDHits(i);
      for (size_t j = 0; j < hits.size(); ++j) {
        if (!hits[j].isNull()) {
          _event["hits"].append(hits[j]);  // Json cpp just makes a new array?
        }
      }
    }
    if (GetTracking()->GetWillKeepTracks())
        _event["tracks"] = GetTracking()->GetTracks();
    if (GetVirtualPlanes()->GetWillUseVirtualPlanes())
        _event["virtual_hits"] = GetVirtualPlanes()->GetVirtualHits();
}

void MAUSGeant4Manager::SetEvent(Json::Value event) {
    if (!event.isObject())
        throw(Squeal(Squeal::recoverable, "Particle must be an object value",
                     "MAUSEventAction::SetEvent"));
    _event = event;
}

} // namespace MAUS

