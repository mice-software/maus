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

#include <Python.h>

#include <vector>
#include <string>

#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"

#include "Interface/Squeal.hh"
#include "Interface/dataCards.hh"

#include "src/map/MapCppSimulation/MapCppSimulation.hh"

//  This stuff is *still* needed until persistency move is done
dataCards MyDataCards("Simulation");
MICEEvent simEvent;

bool MapCppSimulation::birth(std::string argJsonConfigDocument) {
  // Check if the JSON document can be parsed, else return error only
  try {
    SetConfiguration(argJsonConfigDocument);
    return true;  // Sucessful completion
  // Normal session, no visualization
  } catch(Squeal& squee) {
    CppErrorHandler::getInstance()->HandleSquealNoJson(squee, _classname);
  } catch(std::exception& exc) {
    CppErrorHandler::getInstance()->HandleStdExcNoJson(exc, _classname);
  }
  return false;
}

std::string MapCppSimulation::process(std::string document) {
  Json::Value spill;
  try {spill = JsonWrapper::StringToJson(document);}
  catch(...) {
    return "{\"errors\":{\"bad_json_document\":\"Failed to parse input document\"}}";
  }
  try {
    MAUSSteppingAction* stepAct = MAUSGeant4Manager::GetInstance()->
                                                                  GetStepping();
    Json::Value mc   = JsonWrapper::GetProperty
                                        (spill, "mc", JsonWrapper::arrayValue);
    Squeak::mout(Squeak::info) << "Spill with " << mc.size() << " primaries" << std::endl;
    for (int mc_particle_i = 0; mc_particle_i < mc.size(); ++mc_particle_i) {
      stepAct->SetTracks(Json::Value());
      Json::Value particle = JsonWrapper::GetItem
                                  (mc, mc_particle_i, JsonWrapper::objectValue);
      SetNextParticle(particle);
      _g4manager->GetRunManager()->BeamOn(1);
      particle = StoreTracking(particle);
      spill["mc"][mc_particle_i] = particle;
    }
  }
  catch(Squeal squee) {
    spill = CppErrorHandler::getInstance()
                                       ->HandleSqueal(spill, squee, _classname);
  } catch(std::exception exc) {
    spill = CppErrorHandler::getInstance()
                                         ->HandleStdExc(spill, exc, _classname);
  }
  Json::FastWriter writer;
  std::string output = writer.write(spill);
  return output;
}

bool MapCppSimulation::death() {
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
  _g4manager->GetPrimaryGenerator()->Push(p);
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
  _g4manager = MAUSGeant4Manager::GetInstance();
  // RF cavity phases
  _g4manager->SetPhases();
  Squeak::mout(Squeak::info) << "Fields:" << std::endl;
  simRun.btFieldConstructor->Print(Squeak::mout(Squeak::info));
}

Json::Value MapCppSimulation::StoreTracking(Json::Value particle) {
    //  For each detector i
    for (unsigned int i = 0; i < _g4manager->GetGeometry()->GetSDSize(); i++) {
      //  Retrieve detector i's hits
      vector<Json::Value> hits = _g4manager->GetGeometry()->GetSDHits(i);

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
    particle["tracks"] = _g4manager->GetStepping()->GetTracks();
    return particle;
}
