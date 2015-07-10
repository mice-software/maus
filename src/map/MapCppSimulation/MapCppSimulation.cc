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

#include <string>
#include <vector>

#include "src/common_cpp/Simulation/MAUSGeant4Manager.hh"
#include "src/common_cpp/API/PyWrapMapBase.hh"
#include "src/common_cpp/Simulation/MAUSVisManager.hh"
#include "src/map/MapCppSimulation/MapCppSimulation.hh"

namespace MAUS {
std::string class_docstring =
std::string("MapCppSimulation is the interface from the MAUS module\n")+
std::string("framework to the Geant4 simulation modules. MapCppSimulation\n")+
std::string("tracks particles and fills the MC branch of the MAUS event.\n");

std::string birth_docstring =
std::string("Checks whether the visualisation manager is initialised.\n\n")+
std::string("  datacards: ignored.\n");

std::string process_docstring =
std::string("Run tracks through the simulation.\n\n")+
std::string("  data: data from a single spill. Tracks primary of each MC\n")+
std::string("        event in the spill data.\n");

std::string death_docstring = std::string("Does nothing.\n");

PyMODINIT_FUNC init_MapCppSimulation(void) {
  PyWrapMapBase<MAUS::MapCppSimulation>::PyWrapMapBaseModInit
                              ("MapCppSimulation",
                               class_docstring,
                               birth_docstring,
                               process_docstring,
                               death_docstring);
}

void MapCppSimulation::_birth(const std::string& argJsonConfigDocument) {
  // Check if the JSON document can be parsed, else return error only
  _doVis = MAUSGeant4Manager::GetInstance()->GetVisManager() != NULL;
}

void MapCppSimulation::_process(MAUS::Data* data) const {
  if (_doVis) {
      MAUS::MAUSGeant4Manager::GetInstance()->GetVisManager()->SetupRun();
  }
  Spill* spill = data->GetSpill();
  std::vector<MCEvent*>* mc = spill->GetMCEvents();
  // note that MAUS::Data still owns all this memory...
  MAUS::MAUSGeant4Manager::GetInstance()->RunManyParticles(mc);
  if (_doVis)
      MAUS::MAUSGeant4Manager::GetInstance()->GetVisManager()->TearDownRun();
}
}
