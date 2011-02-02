#include "MapCppSimulation.h"

//  This stuff is *still* needed until persistency move is done
dataCards MyDataCards("Simulation");
MICEEvent simEvent;

bool MapCppSimulation::Birth(std::string argJsonConfigDocument) {
  //  JsonCpp setup
  Json::Value configJSON;   //  this will contain the configuration
  Json::Reader reader;

  // Check if the JSON document can be parsed, else return error only
  bool parsingSuccessful = reader.parse(argJsonConfigDocument, configJSON);
  if (!parsingSuccessful) {
    Squeak::mout(Squeak::fatal) << "MapCppSimulation: could not parse argJsonConfigDocument!\n";
    return false;
  }
  
  try {

    MICERun& simRun = *MICERun::getInstance();

    //if (MyDataCards.readKeys(inputFileDataCard) == 0) {
    //Squeak::mout(Squeak::fatal) << " *** DataCard File " << inputFileDataCard
    //<< " not found ***" << endl;
    //exit(2);
    //}

    simRun.DataCards = & MyDataCards;
    // Next function disables std::cout, std::clog,
    // std::cerr depending on VerboseLevel
    Squeak::setStandardOutputs();

    // Materials
    simRun.miceMaterials = new MiceMaterials();

    // MICE Model setup
    MiceModule* module;
    if (!configJSON.isMember("simulation_geometry_filename")) {
      Squeak::mout(Squeak::fatal) << "MapCppSimulation: could not find 'simulation_geometry_filename'!\n";
      return false;
    }
    module = new MiceModule(configJSON["simulation_geometry_filename"].asString());

    if (module == NULL) {
      Squeak::mout(Squeak::fatal) << "MapCppSimulation: could not create MiceModule!\n";
      return false;
    }
    
    simRun.miceModule = module;

    // G4 Materials
    fillMaterials(simRun);

    _runManager = new G4RunManager;
    _detector = new MICEDetectorConstruction(simRun);
    _runManager->SetUserInitialization(_detector);

    _physList = MICEPhysicsList::GetMICEPhysicsList();
    _runManager->SetUserInitialization(_physList);

    _primary  = new MAUSPrimaryGeneratorAction;
    _stepAct = new MAUSSteppingAction;
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

    // Normal session, no visualization
  } catch(Squeal squee) {
    squee.Print();
    Squeak::mout(Squeak::fatal) <<
        "Attempting to rescue simulated data" << std::endl;
    Squeak::mout(Squeak::fatal) << "Exiting" << std::endl;
    return false;
  } catch(std::exception exc) {
    Squeak::mout(Squeak::fatal) << "std::exception in G4MICE - " <<
        "probably while trying to do some file" <<
        "IO or string manipulation - reported as:" << std::endl;
    Squeak::mout(Squeak::fatal) << std::string(exc.what()) << std::endl;
    Squeak::mout(Squeak::fatal)
        << "Attempting to rescue simulated data" << std::endl;
    Squeak::mout(Squeak::fatal) << "Exiting" << std::endl;
    return false;
  } catch(...) {
    Squeak::mout(Squeak::fatal) << "Unknown error in G4MICE" << std::endl;
    Squeak::mout(Squeak::fatal) <<
        "Attempting to rescue simulated data" << std::endl;
    Squeak::mout(Squeak::fatal) << "Exiting" << std::endl;
    return false;
  }

  return true;  // Sucessful completion
}

std::string MapCppSimulation::Process(std::string document) {
  //  JsonCpp setup
  Json::Value root;   // will contains the root value after parsing.
  Json::Reader reader;
  Json::FastWriter writer;

  // Check if the JSON document can be parsed, else return error only
  bool parsingSuccessful = reader.parse(document, root);
  if (!parsingSuccessful) {
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says:" << reader.getFormatedErrorMessages();
    errors["bad_json_document"] = ss.str();
    root["errors"] = errors;
    return writer.write(root);
  }

  // Check if the JSON document has a 'mc' branch, else return error
  if (!root.isMember("mc")) {
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says:" << "I need an MC branch to simulate.";
    errors["missing_branch"] = ss.str();
    root["errors"] = errors;
    return writer.write(root);
  }

  Json::Value mc = root.get("mc", 0);

  // Check if JSON document is of the right type, else return error
  if (!mc.isArray()) {
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says:" << "MC branch isn't an array";
    errors["bad_type"] = ss.str();
    root["errors"] = errors;
    return writer.write(root);
  }

  for (int mc_particle_i = 0; mc_particle_i < mc.size(); mc_particle_i++) {
    Json::Value particle = mc[mc_particle_i];

    // geant4 pid
    _primary->SetNextParticleID(particle.get("particle_id", -13).asInt());

    //  If the position isn't set, make a fake one since defaults will get found
    if (!particle.isMember("position")) {
      particle["position"] = Json::Value();
    }

    // Units is 'mm'
    _primary->SetNextPositionVector(
        particle["position"].get("x", 0.0).asDouble(),
        particle["position"].get("y", 0.0).asDouble(),
        particle["position"].get("z", -5000.0).asDouble());

    //  If the momentum isn't set, make a fake one
    // since defaults will get found.
    if (!particle.isMember("unit_momentum")) {
      particle["unit_momentum"] = Json::Value();
    }

    _primary->SetNextMomentumUnitVector(
        particle["unit_momentum"].get("x", 0.0).asDouble(),   // unit vector
        particle["unit_momentum"].get("y", 0.0).asDouble(),   // unit vector
        particle["unit_momentum"].get("z", 1.0).asDouble());  // unit vector

    _primary->SetNextEnergy(particle.get("energy", 200.0).asDouble());

    //stepAct->_track.clear();

    CLHEP::HepRandom::setTheSeed(particle.get("random_seed", 0.0).asUInt());
    
    // Fire single muon.  There is about a 0.5 s slowdown here since
    // geant4 will have to open and close the geometry. 
    _runManager->BeamOn(1);

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
      particle["tracks"] =  _stepAct->_track;
    }

    root["mc"][mc_particle_i] = particle;
  }

  return writer.write(root);
}

bool MapCppSimulation::Death() {
  // User actions, physics_list, the detector are all owned
  // and deleted by the run manager and should not be deleted
  // here!!!  -With love, Tunnell 2010
  delete _runManager;
  return true;  // successful
}

