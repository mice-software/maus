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
#include "Geant4/G4Navigator.hh"

#include "src/legacy/Interface/Squeak.hh"

#include "src/common_cpp/DataStructure/MCEvent.hh"
#include "src/common_cpp/JsonCppProcessors/ArrayProcessors.hh"
#include "src/common_cpp/JsonCppProcessors/MCEventProcessor.hh"
#include "src/common_cpp/Utils/Globals.hh"

#include "src/common_cpp/Simulation/MAUSGeant4Manager.hh"
#include "src/common_cpp/Simulation/FieldPhaser.hh"
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
      SetVolumeInformation(*Globals::GetInstance()->GetMonteCarloMiceModules(),
			   _detector->GetWorldVolume()->GetLogicalVolume());
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
    PointerArrayProcessor<MCEvent> proc(new MCEventProcessor());
    std::vector<MCEvent*>* events = proc.JsonToCpp(particle_array);
    events = RunManyParticles(events);
    Json::Value* events_json = proc.CppToJson(*events);
    Json::Value events_json_val(*events_json);
    delete events_json;
    for (size_t i = 0; i < events->size(); ++i)
        delete events->at(i);
    delete events;
    return events_json_val;
}

std::vector<MCEvent*>* MAUSGeant4Manager::RunManyParticles(std::vector<MCEvent*>* event) {
    _eventAct->SetEvents(event);
    for (size_t i = 0; i < event->size(); ++i) {
        MAUSPrimaryGeneratorAction::PGParticle primary;
        primary.ReadCpp(event->at(i)->GetPrimary());
        GetPrimaryGenerator()->Push(primary);
    }
    BeamOn(event->size());
    return _eventAct->TakeEvents();
}


MCEvent* MAUSGeant4Manager::RunParticle(MAUS::Primary particle) {
    MAUSPrimaryGeneratorAction::PGParticle p;
    p.ReadCpp(&particle);
    return Tracking(p);
}

MCEvent* MAUSGeant4Manager::RunParticle
                                    (MAUSPrimaryGeneratorAction::PGParticle p) {
    return Tracking(p);
}


MCEvent* MAUSGeant4Manager::Tracking
                                    (MAUSPrimaryGeneratorAction::PGParticle p) {
    Squeak::mout(Squeak::debug) << "Firing particle with ";
    JsonWrapper::Print(Squeak::mout(Squeak::debug), p.WriteJson());
    Squeak::mout(Squeak::debug) << std::endl;

    GetPrimaryGenerator()->Push(p);

    std::vector<MCEvent*>* event_vector = new std::vector<MCEvent*>();
    event_vector->push_back(new MCEvent());
    event_vector->at(0)->SetPrimary(p.WriteCpp());
    _eventAct->SetEvents(event_vector); // EventAction now owns this memory
    Squeak::mout(Squeak::debug) << "Beam On" << std::endl;
    GetField()->Print(Squeak::mout(Squeak::debug));
    BeamOn(1);
    Squeak::mout(Squeak::debug) << "Beam Off" << std::endl;
    event_vector = _eventAct->TakeEvents(); // EventAction still owns this memory
    if (event_vector->size() > 1) {
        for (size_t i = 0; i < event_vector->size(); ++i)
            delete event_vector->at(i);
        delete event_vector;
        throw(Exception(Exception::recoverable,
                        "More than one event in return",
                        "MAUSGeant4Manager::Tracking"));
    }
    MCEvent* ev_return = event_vector->at(0);
    delete event_vector;
    return ev_return; // this is a deep copy
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

void MAUSGeant4Manager::SetVolumeInformation(MiceModule& module, G4LogicalVolume* base) {

  // G4VPhysicalVolume* world = _parser.GetWorldVolume();
  G4int nDaughters = base->GetNoDaughters();
  for ( int i = 0; i < nDaughters; i++ ) {
    G4VPhysicalVolume* daughter = base->GetDaughter(i);
    G4LogicalVolume* myvol = daughter->GetLogicalVolume();
    // first check if there are auxiliary objects
    const G4GDMLAuxListType auxlist = _parser.GetVolumeAuxiliaryInformation(myvol);

    // Squeak::mout(Squeak::info) << "Found volume " << myvol->GetName()
    // << " with "<< auxlist.size() <<" auxiliary elements." << std::endl;
    if (auxlist.size() > 0) {
      // Set auxiliary information
      SetAuxInformation(module, myvol, auxlist);
    }
    // 	 else {
    // 	 _detector->GetUserLimits().push_back(new G4UserLimits(_stepMax, _trackMax,
    // 	 _timeMax, _keThreshold));
    // 	 myvol->SetUserLimits(_detector->GetUserLimits().back());
    // 	 }
    if ( myvol->GetNoDaughters() > 0 ) {
      // Consider adding information to the daughter volumes
      SetVolumeInformation(module, myvol);
    }
  }
}

void MAUSGeant4Manager::SetSteppingLimits(G4LogicalVolume* myvol) {

  double stepMax = _stepMax;
  double timeMax = _timeMax;
  double trackMax = _trackMax;
  double keThreshold = _keThreshold;
  double red = 1.;
  double green = 1.;
  double blue = 1.;
  bool vis = true;

  // Squeak::mout(Squeak::info) << "Defining user limits for " << myvol->GetName() << std::endl;
  _detector->GetUserLimits().push_back(new G4UserLimits(stepMax, trackMax,
							timeMax, keThreshold));
  // Squeak::mout(Squeak::info) << "Setting user limits for " << myvol->GetName() << std::endl;
  myvol->SetUserLimits(_detector->GetUserLimits().back());
  if (vis)
    _detector->GetVisAttributes().push_back(new G4VisAttributes(G4Color(red, green, blue)));
  else
    _detector->GetVisAttributes().push_back(new G4VisAttributes(false));
  myvol->SetVisAttributes(_detector->GetVisAttributes().back());
  // Squeak::mout(Squeak::info) << "Limits set for " << myvol->GetName() << std::endl;
}

void MAUSGeant4Manager::SetAuxInformation(MiceModule& module, G4LogicalVolume* myvol,
					  const G4GDMLAuxListType auxlist) {
  // Establish sensitive detectors using the SDmanager
  // Get the map of the auxiliary information from the parser
  /*
  const G4GDMLAuxMapType* auxmap = _parser.GetAuxMap();
    Squeak::mout(Squeak::info) << "Found " << auxmap->size()
			     << " volume(s) with auxiliary information.\n\n";
  */
  double stepMax = _stepMax;
  double timeMax = _timeMax;
  double trackMax = _trackMax;
  double keThreshold = _keThreshold;
  double red = 1.;
  double green = 1.;
  double blue = 1.;
  bool vis = true;
  /*
  for (G4GDMLAuxMapType::const_iterator iter = auxmap->begin();
      iter != auxmap->end(); iter++) {
    // Construct a mice module containing the auxilliary information
    G4LogicalVolume* myvol = (*iter).first;
    Squeak::mout(Squeak::info) << "Checking aux map of volume "
			       << myvol->GetName() << std::endl;
  */
  // Define sensitive detectors etc.
  bool sensdet = false;
  std::string sensdetname = "";
  G4GDMLAuxListType::const_iterator vit = auxlist.begin();
  // Want to know, specifically, if there is a sensitive detector in this object
  // but there will be other information of interest
  // for (int i=0; i < auxlist.size(); i++){
  do {
    try {
      if ((*vit).type.contains("SensitiveDetector")) {
	sensdet = true;
	sensdetname = (*vit).value;
      } else if ((*vit).type.contains("G4StepMax")) {
	stepMax  = atof((*vit).value.c_str());
	//
      } else if ((*vit).type.contains("G4TrackMax")) {
	trackMax = atof((*vit).value.c_str());
      } else if ((*vit).type.contains("G4TimeMax")) {
	timeMax  = atof((*vit).value.c_str());
      } else if ((*vit).type.contains("G4KinMin")) {
	keThreshold = atof((*vit).value.c_str());
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
      } else if ((*vit).type.contains("RedColor")) {
	red = atof((*vit).value.c_str());
      } else if ((*vit).type.contains("GreenColor")) {
	green = atof((*vit).value.c_str());
      } else if ((*vit).type.contains("BlueColor")) {
	blue = atof((*vit).value.c_str());
      } else {
	// Don't really know what to do with this.
	// Do nothing if selection is not otherwise known.
      }
    } catch (...) {
      continue;
    }
    // Squeak::mout(Squeak::info) << "Found " << (*vit).type << " with value "
    // 		       << (*vit).value << " in object "
    // 			       << myvol->GetName() << "\n";
    vit++;
  } while (vit != auxlist.end());

  if (sensdet) {
    DefineSensitiveDetector(module, myvol, sensdetname);
  }
  _detector->GetUserLimits().push_back(new G4UserLimits(stepMax, trackMax,
							timeMax, keThreshold));
  // Squeak::mout(Squeak::info) << "User limits set for " << myvol->GetName()
  // 			     << " with max step " << stepMax << std::endl;
  myvol->SetUserLimits(_detector->GetUserLimits().back());
  // if (myvol->GetNoDaughters() > 0) {
  //   SetDaughterUserLimits(myvol);
  // }

  if (vis)
    _detector->GetVisAttributes().push_back(new G4VisAttributes(G4Color(red, green, blue)));
  else
    _detector->GetVisAttributes().push_back(new G4VisAttributes(false));

  myvol->SetVisAttributes(_detector->GetVisAttributes().back());

  // Squeak::mout(Squeak::info) << "Attributes set for volume "
  // << myvol->GetName() << std::endl;
}


void MAUSGeant4Manager::DefineSensitiveDetector(MiceModule& module, G4LogicalVolume* myvol,
						std::string sensdetname) {
  // Find the module corresponding to the volume name
  std::vector<const MiceModule*> mods =
    module.findModulesByPropertyString("SensitiveDetector", sensdetname);
  // Squeak::mout(Squeak::info) << "Search for detector " << sensdetname
  // 			     << " with name " << myvol->GetName() << ": "
  // 		     << mods.size() << " candidates\n";
  bool moduleFound = false;
  for ( unsigned i = 0; i < mods.size(); i++ ) {
    // This is kind of a double check now to make
    // sure that this is the same object.
    // Squeak::mout(Squeak::info) << "Check module " << i << " named "
    // << mods.at(i)->name() << std::endl;
    if (mods.at(i)->name() == myvol->GetName()) {
      moduleFound = true;
      // propertyExists("SensitiveDetector","PropertyString")){
      // Make a copy of the module to remove the const cast
      MiceModule* tempmod = MiceModule::deepCopy(*mods[i], false);
      // tempmod->printThis(Squeak::mout(Squeak::info));
      _detector->SetUserLimits(myvol, tempmod);
      _detector->SetVisAttributes(myvol, tempmod);
      _detector->BuildSensitiveDetector(myvol, tempmod);
      _detector->AddToRegion(myvol, tempmod);
      // Now loop over all daughters to add them to the sensitive volumes
      if (sensdetname == "SciFi" || sensdetname == "KL" ||
	  sensdetname == "EMR" ) {
	if (myvol->GetNoDaughters() > 0) {
	  SetDaughterSensitiveDetectors(myvol);
	}
      }
    }
    // if (moduleFound)
    // Squeak::mout(Squeak::info) << "Module for " << myvol->GetName()
    // 			 << " found." << std::endl;
  }
}
void MAUSGeant4Manager::SetDaughterSensitiveDetectors(G4LogicalVolume* logic) {
  // std::cout << "Adding " << logic->GetNoDaughters()
  // 	    << " to sensitive detector in " << logic->GetName() << std::endl;

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

