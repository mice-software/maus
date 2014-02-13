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

#include "Utils/Exception.hh"

#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"
#include "src/common_cpp/Simulation/MAUSVisManager.hh"
#include "src/common_cpp/Globals/GlobalsManager.hh"

#include "src/map/MapCppSimulation/MapCppSimulation.hh"

namespace MAUS {

bool MapCppSimulation::birth(std::string argJsonConfigDocument) {
  // Check if the JSON document can be parsed, else return error only
  try {
    if (!Globals::HasInstance()) {
        GlobalsManager::InitialiseGlobals(argJsonConfigDocument);
    }
    _doVis = MAUSGeant4Manager::GetInstance()->GetVisManager() != NULL;
    return true;  // Sucessful completion
  // Normal session, no visualization
  } catch (Exception& exception) {
    MAUS::CppErrorHandler::getInstance()->HandleExceptionNoJson(exception, _classname);
  } catch (std::exception& exc) {
    MAUS::CppErrorHandler::getInstance()->HandleStdExcNoJson(exc, _classname);
  }
  return false;
}

std::string MapCppSimulation::process(std::string document) {
  Json::Value spill;
  try {spill = JsonWrapper::StringToJson(document);}
  catch (...) {
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
  catch (Exception& exception) {
    spill = MAUS::CppErrorHandler::getInstance()
                                       ->HandleException(spill, exception, _classname);
  } catch (std::exception& exc) {
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
}
