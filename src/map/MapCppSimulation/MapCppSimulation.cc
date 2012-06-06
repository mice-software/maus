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
#include "src/common_cpp/Simulation/MAUSVisManager.hh"

#include "Interface/Squeal.hh"
#include "Interface/dataCards.hh"

#include "src/map/MapCppSimulation/MapCppSimulation.hh"

namespace MAUS {

bool MapCppSimulation::birth(std::string argJsonConfigDocument) {
  // Check if the JSON document can be parsed, else return error only
  try {
    SetConfiguration(argJsonConfigDocument);
    return true;  // Sucessful completion
  // Normal session, no visualization
  } catch(Squeal& squee) {
    MAUS::CppErrorHandler::getInstance()->HandleSquealNoJson(squee, _classname);
  } catch(std::exception& exc) {
    MAUS::CppErrorHandler::getInstance()->HandleStdExcNoJson(exc, _classname);
  }
  return false;
}

std::string MapCppSimulation::process(std::string document) {
  Json::Value spill;
  try {spill = JsonWrapper::StringToJson(document);}
  catch(...) {
    return std::string("{\"errors\":{\"bad_json_document\":")+
           std::string("\"Failed to parse input document\"}}");
  }
  try {
    if (_doVis) {
        MAUS::MAUSGeant4Manager::GetInstance()->GetVisManager()->SetupRun();
    }
    Json::Value mc = JsonWrapper::GetProperty
                                  (spill, "mc_events", JsonWrapper::arrayValue);
    spill["mc_events"] =
                   MAUS::MAUSGeant4Manager::GetInstance()->RunManyParticles(mc);
    if (_doVis)
        MAUS::MAUSGeant4Manager::GetInstance()->GetVisManager()->TearDownRun();
  }
  catch(Squeal& squee) {
    spill = MAUS::CppErrorHandler::getInstance()
                                       ->HandleSqueal(spill, squee, _classname);
  } catch(std::exception& exc) {
    spill = MAUS::CppErrorHandler::getInstance()
                                         ->HandleStdExc(spill, exc, _classname);
  }
  Json::FastWriter writer;
  std::string output = writer.write(spill);
  return output;
}

bool MapCppSimulation::death() {
  return true;  // successful
}

void MapCppSimulation::SetConfiguration(std::string json_configuration) {
  MICERun& simRun = *MICERun::getInstance();
  simRun.jsonConfiguration = new Json::Value
                                (JsonWrapper::StringToJson(json_configuration));
  Json::Value& config = *simRun.jsonConfiguration;
  simRun.DataCards = new dataCards("Simulation");
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
  _g4manager = MAUS::MAUSGeant4Manager::GetInstance();
  // RF cavity phases
  _g4manager->SetPhases();
  Squeak::mout(Squeak::info) << "Fields:" << std::endl;
  simRun.btFieldConstructor->Print(Squeak::mout(Squeak::info));
  _doVis = JsonWrapper::GetProperty
           (config, "geant4_visualisation", JsonWrapper::booleanValue).asBool();
}
}
