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

#include <vector>

#include "Geant4/G4StateManager.hh"
#include "Geant4/G4ApplicationState.hh"

#include "src/common_cpp/Simulation/MAUSGeant4Manager.hh"

#include "src/common_cpp/Simulation/FieldPhaser.hh"

#include "src/legacy/Interface/Squeak.hh"
#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/Simulation/DetectorConstruction.hh"
#include "src/common_cpp/Simulation/MAUSStackingAction.hh"
#include "src/common_cpp/Simulation/MAUSPhysicsList.hh"
#include "src/common_cpp/Simulation/MAUSVisManager.hh"
#include "src/common_cpp/Simulation/MAUSRunAction.hh"


namespace MAUS {

MAUSGeant4Manager* MAUSGeant4Manager::_instance = NULL;
bool MAUSGeant4Manager::_isClosed = false;

bool MAUSGeant4Manager::HasInstance() {
  if (_instance == NULL && !_isClosed) {
    return false;
  }
  return true;
}

MAUSGeant4Manager* MAUSGeant4Manager::GetInstance() {
  if (_instance == NULL && !_isClosed) {
      _instance = new MAUSGeant4Manager();
  }
  return _instance;
}

MAUSGeant4Manager::MAUSGeant4Manager() : _virtPlanes(NULL) {
    if (_instance != NULL)
        throw(Exception(
              Exception::recoverable,
              "Attempt to initialise MAUSGeant4Manager twice",
              "MAUSGeant4Manager::MAUSGeant4Manager"));
    _instance = this;
    SetVirtualPlanes(new VirtualPlaneManager);
    _visManager = NULL;  // set by GetVisManager
    SetVisManager();
    _runManager = new G4RunManager;
    Json::Value* cards = Globals::GetInstance()->GetConfigurationCards();
    // just set up a dummy geometry
    _detector   = new Simulation::DetectorConstruction(*cards);
    _runManager->SetUserInitialization(_detector);

    _physList = MAUSPhysicsList::GetMAUSPhysicsList();
    _runManager->SetUserInitialization(_physList);

    _primary  = new MAUSPrimaryGeneratorAction();
    _stepAct  = new MAUSSteppingAction();
    _trackAct = new MAUSTrackingAction();
    _eventAct = new MAUSEventAction();
    _runManager->SetUserAction(_primary);
    _runManager->SetUserAction(_trackAct);
    _runManager->SetUserAction(_stepAct);
    _runManager->SetUserAction(_eventAct);
    _runManager->SetUserAction(new MAUSStackingAction);
    _runManager->SetUserAction(new MAUSRunAction);
    _runManager->Initialize();
    // now set up full geometry
    SetMiceModules(*Globals::GetInstance()->GetMonteCarloMiceModules());
}

MAUSGeant4Manager::~MAUSGeant4Manager() {
    if (_visManager != NULL) {
       delete _visManager;
    }
    delete _runManager;
    _isClosed = true;
    if (_virtPlanes != NULL)
        delete _virtPlanes;
}

void MAUSGeant4Manager::SetPhases() {
  FieldPhaser phaser;
  phaser.SetPhases();
}

MAUSPrimaryGeneratorAction::PGParticle
                                     MAUSGeant4Manager::GetReferenceParticle() {
    MAUSPrimaryGeneratorAction::PGParticle p;
    Json::Value* conf = Globals::GetInstance()->GetConfigurationCards();
    Json::Value ref = JsonWrapper::GetProperty
             (*conf, "simulation_reference_particle", JsonWrapper::objectValue);
    p.ReadJson(ref);
    p.MassShellCondition();
    return p;
}

Json::Value MAUSGeant4Manager::RunManyParticles(Json::Value particle_array) {
    _eventAct->SetEvents(particle_array);  // checks type
    for (size_t i = 0; i < particle_array.size(); ++i) {
        MAUSPrimaryGeneratorAction::PGParticle primary;
        Json::Value event = JsonWrapper::GetItem
                                  (particle_array, i, JsonWrapper::objectValue);
        Json::Value primary_json = JsonWrapper::GetProperty
                                  (event, "primary", JsonWrapper::objectValue);
        primary.ReadJson(primary_json);
        GetPrimaryGenerator()->Push(primary);
    }
    BeamOn(particle_array.size());
    return _eventAct->GetEvents();
}

Json::Value MAUSGeant4Manager::RunParticle(Json::Value particle) {
    MAUSPrimaryGeneratorAction::PGParticle p;
    p.ReadJson(particle["primary"]);
    return Tracking(p);
}

Json::Value MAUSGeant4Manager::RunParticle
                                    (MAUSPrimaryGeneratorAction::PGParticle p) {
    return Tracking(p);
}


Json::Value MAUSGeant4Manager::Tracking
                                    (MAUSPrimaryGeneratorAction::PGParticle p) {
    Squeak::mout(Squeak::debug) << "Firing particle with ";
    JsonWrapper::Print(Squeak::mout(Squeak::debug), p.WriteJson());
    Squeak::mout(Squeak::debug) << std::endl;

    GetPrimaryGenerator()->Push(p);
    Json::Value event_array = Json::Value(Json::arrayValue);
    Json::Value event(Json::objectValue);
    event["primary"] = p.WriteJson();
    event_array.append(event);
    _eventAct->SetEvents(event_array);
    Squeak::mout(Squeak::debug) << "Beam On" << std::endl;
    GetField()->Print(Squeak::mout(Squeak::debug));
    BeamOn(1);
    Squeak::mout(Squeak::debug) << "Beam Off" << std::endl;
    return _eventAct->GetEvents()[Json::Value::UInt(0)];
}

void MAUSGeant4Manager::SetVisManager() {
  if (_visManager != NULL) delete _visManager;
  _visManager = NULL;
  // if _visManager == NULL, attempt to build it
  Json::Value& conf = *Globals::GetInstance()->GetConfigurationCards();
  if (JsonWrapper::GetProperty
           (conf, "geant4_visualisation", JsonWrapper::booleanValue).asBool()) {
      _visManager = new MAUSVisManager;
      _visManager->Initialize();
  }
}

BTFieldConstructor* MAUSGeant4Manager::GetField() {
  return _detector->GetField();
}

// should be const MiceModule
void MAUSGeant4Manager::SetMiceModules(MiceModule& module) {
    SetVirtualPlanes(new VirtualPlaneManager);
    _virtPlanes->ConstructVirtualPlanes(&module);
    _detector->SetMiceModules(module);
    if (Globals::GetMCFieldConstructor() != NULL)
        SetPhases();
}

void MAUSGeant4Manager::BeamOn(int number_of_particles) {
    G4StateManager* stateManager = G4StateManager::GetStateManager();
    G4ApplicationState currentState = stateManager->GetCurrentState();
    if (currentState != G4State_PreInit && currentState != G4State_Idle)
        throw(MAUS::Exception(Exception::recoverable,
              "Geant4 is not ready to run, aborting run",
              "MAUSGeant4Manager::BeamOn"));
    GetRunManager()->BeamOn(number_of_particles);
}
}  // namespace MAUS

