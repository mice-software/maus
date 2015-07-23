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
#include <string>

#include "Geant4/G4StateManager.hh"
#include "Geant4/G4ApplicationState.hh"

#include "Geant4/G4Region.hh"
#include "Geant4/G4RegionStore.hh"
#include "Geant4/G4UserLimits.hh"


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
    _keThreshold = JsonWrapper::GetProperty(*cards, "kinetic_energy_threshold",
					    JsonWrapper::realValue).asDouble();
    _trackMax = JsonWrapper::GetProperty(*cards, "max_track_length",
					 JsonWrapper::realValue).asDouble();
    _timeMax = JsonWrapper::GetProperty(*cards, "max_track_time",
					JsonWrapper::realValue).asDouble();
    _stepMax = JsonWrapper::GetProperty(*cards, "max_step_length",
					JsonWrapper::realValue).asDouble();

    // Create the gdml parser object


    std::string gdmlGeometry = "";
    bool usegdml = Globals::GetInstance()
      ->GetMonteCarloMiceModules()->propertyExistsThis("GDMLFile", "string");
    bool gdmldebug = JsonWrapper::GetProperty
       (*cards, "simulation_geometry_debug", JsonWrapper::booleanValue).asBool();
    // bool gdmldebug = false;

    if (usegdml && !gdmldebug) {

      gdmlGeometry =
	Globals::GetInstance()->GetMonteCarloMiceModules()->propertyString("GDMLFile");
      usegdml = true;

      Squeak::mout(Squeak::info) << "Using " << gdmlGeometry << std::endl;
      _parser.Read(gdmlGeometry.c_str());
      // Squeak::activateCout(true);
      // _paser.SetOverlapCheck(true);
      _detector = new Simulation::DetectorConstruction(_parser.GetWorldVolume(), *cards);
    } else {
      // just set up a dummy geometry
      _detector   = new Simulation::DetectorConstruction(*cards);
    }
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
    if (usegdml) {
      SetAuxInformation(*Globals::GetInstance()->GetMonteCarloMiceModules());
    }
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

void MAUSGeant4Manager::SetAuxInformation(MiceModule& module) {
  // Establish sensitive detectors using the SDmanager
  // Get the map of the auxiliary information from the parser
  const G4GDMLAuxMapType* auxmap = _parser.GetAuxMap();
  Squeak::mout(Squeak::info) << "Found " << auxmap->size()
			     << " volume(s) with auxiliary information.\n\n";

  double stepMax = _stepMax;
  double timeMax = _timeMax;
  double trackMax = _trackMax;
  double keThreshold = _keThreshold;
  double red = 1.;
  double green = 1.;
  double blue = 1.;
  bool vis = true;

  for (G4GDMLAuxMapType::const_iterator iter = auxmap->begin();
      iter != auxmap->end(); iter++) {
    // Construct a mice module containing the auxilliary information
    G4LogicalVolume* myvol = (*iter).first;
    Squeak::mout(Squeak::info) << "Checking aux map of volume "
			       << myvol->GetName() << std::endl;
    // Want to know, specifically if there is a sensitive detector in this object
    for (G4GDMLAuxListType::const_iterator vit = (*iter).second.begin();
	 vit != (*iter).second.end(); vit++) {
      if ((*vit).type.contains("SensitiveDetector")) {
	// Find the module corresponding to the volume name
	std::vector<const MiceModule*> mods =
	  module.findModulesByPropertyString((*vit).type, (*vit).value);
	//  Squeak::mout(Squeak::info)<<"Search for detector "<<(*vit).value
	// 			  <<" with name "<<myvol->GetName()<<": "
	// 			  <<mods.size()<<" candidates\n";
	for (unsigned i = 0; i < mods.size(); i++) {
	  // This is kind of a double check now to make
	  // sure that this is the same object.

	  if (mods.at(i)->name() == myvol->GetName()) {
	     // propertyExists("SensitiveDetector","PropertyString")){
	    // Make a copy of the module to remove the const cast
	    MiceModule* tempmod = MiceModule::deepCopy(*mods[i], false);
	    // tempmod->printThis(Squeak::mout(Squeak::info));
	    _detector->SetUserLimits(myvol, tempmod);
	    _detector->SetVisAttributes(myvol, tempmod);
	    _detector->BuildSensitiveDetector(myvol, tempmod);
	    _detector->AddToRegion(myvol, tempmod);
	    // Now loop over all daughters to add them to the sensitive volumes
	    if ((*vit).value == "SciFi" || (*vit).value == "KL") {
	      if (myvol->GetNoDaughters() > 0) {
		SetDaughterSensitiveDetectors(myvol);
	      }
	    }
	  }
	}
      } else if ((*vit).type.contains("G4StepMax")) {
	stepMax  = static_cast<double>(atof((*vit).value.c_str()));
	Squeak::mout(Squeak::info) << "Found " << (*vit).type << " with value "
				   << (*vit).value << " in object "
				   << myvol->GetName() << "\n";
      } else if ((*vit).type.contains("G4TrackMax")) {
	trackMax = static_cast<double>(atof((*vit).value.c_str()));
	Squeak::mout(Squeak::info) << "Found " << (*vit).type << " with value "
				   << (*vit).value << " in object "
				   << myvol->GetName() << "\n";
      } else if ((*vit).type.contains("G4TimeMax")) {
	timeMax  = static_cast<double>(atof((*vit).value.c_str()));
	Squeak::mout(Squeak::info) << "Found " << (*vit).type << " with value "
				   << (*vit).value << " in object "
				   << myvol->GetName() << "\n";
      } else if ((*vit).type.contains("G4KinMin")) {
	Squeak::mout(Squeak::info) << "Found " << (*vit).type << " with value "
				   << (*vit).value << " in object "
				   << myvol->GetName() << "\n";
	keThreshold = static_cast<double>(atof((*vit).value.c_str()));
      } else if ((*vit).type.contains("Region")) {
	std::string name = (*vit).value;
	G4RegionStore* store = G4RegionStore::GetInstance();
	if (store->GetRegion(name) == NULL) {
	  new G4Region(name);
	  _detector->GetRegions().push_back(name);
	}
	G4Region* region = store->GetRegion(name);
	if (region == NULL) {
	  throw MAUS::Exception(Exception::recoverable,
				"Failed to make region",
				"MAUSgeant4Manager::SetAuxInformation");
	}
	region->AddRootLogicalVolume(myvol);
      } else if ((*vit).type.contains("Invisible")) {
	vis = false;
      } else if ((*vit).type.contains("RedColour")) {
	red = static_cast<double>(atof((*vit).value.c_str()));
      } else if ((*vit).type.contains("GreenColour")) {
	green = static_cast<double>(atof((*vit).value.c_str()));
      } else if ((*vit).type.contains("BlueColour")) {
	blue = static_cast<double>(atof((*vit).value.c_str()));
      } else {
	Squeak::mout(Squeak::info) << "Found " << (*vit).type << " with value "
				  << (*vit).value << " in object "
				  << myvol->GetName()
				  << " with unknown purpose\n";
      }
    }

    _detector->GetUserLimits().push_back(new G4UserLimits(stepMax, trackMax,
							  timeMax, keThreshold));
    myvol->SetUserLimits(_detector->GetUserLimits().back());
    if (vis)
      _detector->GetVisAttributes().push_back(new G4VisAttributes(G4Color(red, green, blue)));
    else
      _detector->GetVisAttributes().push_back(new G4VisAttributes(false));
    myvol->SetVisAttributes(_detector->GetVisAttributes().back());
  }
}

void MAUSGeant4Manager::SetDaughterSensitiveDetectors(G4LogicalVolume* logic) {
  // std::cout << "Adding " << logic->GetNoDaughters()
  // << " to sensitive detector in " << logic->GetName() << std::endl;

  for (G4int i = 0; i < logic->GetNoDaughters(); i++) {
    logic->GetDaughter(i)->GetLogicalVolume()->
      SetSensitiveDetector(logic->GetSensitiveDetector());
    if (logic->GetDaughter(i)->GetLogicalVolume()->GetNoDaughters() > 0) {
      SetDaughterSensitiveDetectors(logic->GetDaughter(i)->GetLogicalVolume());
    }
  }
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

