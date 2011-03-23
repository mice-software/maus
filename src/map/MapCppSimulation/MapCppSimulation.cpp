#include "Interface/CppErrorHandler.hh"
#include "Interface/Squeal.hh"
#include "Interface/JsonWrapper.hh"

#include "MapCppSimulation.h"

//  This stuff is *still* needed until persistency move is done
dataCards MyDataCards("Simulation");
MICEEvent simEvent;

bool MapCppSimulation::Birth(std::string configuration) {
  // Check if the JSON document can be parsed, else return error only
  try {
    SetConfiguration(configuration);
    SetGeant4();
    return true;  // Sucessful completion
    // Normal session, no visualization
  } catch(Squeal squee) {
    CppErrorHandler::HandleSquealNoJson(squee, _classname);
  } catch(std::exception exc) {
    CppErrorHandler::HandleStdExcNoJson(exc, _classname);
  }
  return false;
}

std::string MapCppSimulation::Process(std::string document) {
  std::string output = 
    "{[\"errors\"][\"bad_json_document\"][\"Failed to parse input document\"]}";
  try {
    MAUSSteppingAction* stepAct = MAUSSteppingAction::GetSteppingAction();
    stepAct->SetTracks( Json::Value() );

    Json::Value spill = JsonWrapper::StringToJson(document);
    Json::Value mc   = JsonWrapper::GetProperty
                                         (spill, "mc", JsonWrapper::arrayValue);
    for (int mc_particle_i = 0; mc_particle_i < mc.size(); ++mc_particle_i) {
      Json::Value particle = JsonWrapper::GetItem
                                  (mc, mc_particle_i, JsonWrapper::objectValue);
      SetNextParticle(particle);
      _runManager->BeamOn(1);
      StoreTracking(particle);
      spill["mc"][mc_particle_i] = particle;
    }
    Json::FastWriter writer;
    output = writer.write(spill);
  }
  catch(Squeal squee) {
    CppErrorHandler::HandleSquealNoJson(squee, _classname);
  } catch(std::exception exc) {
    CppErrorHandler::HandleStdExcNoJson(exc, _classname);
  }
  return output;
}

bool MapCppSimulation::Death() {
  // User actions, physics_list, the detector are all owned
  // and deleted by the run manager and should not be deleted
  // here!!!  -With love, Tunnell 2010
  delete _runManager;
  return true;  // successful
}

void MapCppSimulation::SetNextParticle(Json::Value particle) {
  Json::Value pos = JsonWrapper::GetProperty
                             (particle, "position", JsonWrapper::objectValue);
  Json::Value mom = JsonWrapper::GetProperty
                             (particle, "momentum", JsonWrapper::objectValue);
  MAUSPrimaryGeneratorAction::PGParticle p;
  p.pid = JsonWrapper::GetProperty
                       (particle, "particle_id", JsonWrapper::intValue).asInt();
  p.seed = JsonWrapper::GetProperty
                       (particle, "random_seed", JsonWrapper::intValue).asInt();
  p.x = JsonWrapper::GetProperty(pos, "x", JsonWrapper::realValue).asDouble();
  p.y = JsonWrapper::GetProperty(pos, "y", JsonWrapper::realValue).asDouble();
  p.z = JsonWrapper::GetProperty(pos, "z", JsonWrapper::realValue).asDouble();
  p.px = JsonWrapper::GetProperty(mom, "x", JsonWrapper::realValue).asDouble();
  p.py = JsonWrapper::GetProperty(mom, "y", JsonWrapper::realValue).asDouble();
  p.pz = JsonWrapper::GetProperty(mom, "z", JsonWrapper::realValue).asDouble();
  p.energy = JsonWrapper::GetProperty
                        (particle, "energy", JsonWrapper::realValue).asDouble();
  p.time = JsonWrapper::GetProperty
                        (particle, "time", JsonWrapper::realValue).asDouble();
  _primary->Push(p);
}

void MapCppSimulation::SetGeant4() {
    _runManager = new G4RunManager;
    _detector   = new MICEDetectorConstruction(*MICERun::getInstance());
    _runManager->SetUserInitialization(_detector);

    _physList = MICEPhysicsList::GetMICEPhysicsList();
    _runManager->SetUserInitialization(_physList);

    _primary  = new MAUSPrimaryGeneratorAction;
    _stepAct  = new MAUSSteppingAction;
    _eventAct = new MAUSEventAction;
    _trackAct = new MAUSTrackingAction;
    _runManager->SetUserAction(_primary);
    _runManager->SetUserAction(_eventAct);
    _runManager->SetUserAction(_trackAct);
    _runManager->SetUserAction(_stepAct);
    _runManager->SetUserAction(new MICERunAction);

    _runManager->Initialize();

    G4UImanager* UI = G4UImanager::GetUIpointer();

    if (_storeTracks) {
      UI->ApplyCommand("/tracking/storeTrajectory 1");
    }
    else {
      UI->ApplyCommand("/tracking/storeTrajectory 0");
    }
}

void MapCppSimulation::SetConfiguration(std::string json_configuration) {
  MICERun& simRun = *MICERun::getInstance();
  simRun.jsonConfiguration = new Json::Value
                                (JsonWrapper::StringToJson(json_configuration));
  Json::Value& config = *simRun.jsonConfiguration;
  simRun.DataCards = &MyDataCards;
  // Next function disables std::cout, std::clog,
  // std::cerr depending on VerboseLevel
  Squeak::setStandardOutputs();
  // Materials
  simRun.miceMaterials = new MiceMaterials();
  // MICE Model setup
  Json::Value modname = JsonWrapper::GetProperty
             (config, "simulation_geometry_filename", JsonWrapper::stringValue);
  simRun.miceModule = new MiceModule(modname.asString());
  // G4 Materials
  fillMaterials(simRun);
}

void MapCppSimulation::StoreTracking(Json::Value particle) {
  //  Loop over detectors to fetch hits
  if (_detector->GetSDSize() > 0) {
    //  For each detector i
    for (int i = 0; i < _detector->GetSDSize(); i++) {
      //  Retrieve detector i's hits
      vector<Json::Value> hits = _detector->GetSDHits(i);
      // Ensure there are hits in this detector
      if (hits.size() == 0) {
        continue;
      }
      // If there are hits, loop over and append to particle
      for (int j = 0; j < hits.size(); j++) {
        if (!hits[j].isNull()) {
          particle["hits"].append(hits[j]);
        }
      }
    }
  }

  if (_storeTracks) {
    particle["tracks"] =  _stepAct->GetTracks();
  }

}
