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

/** @class MapCppSimulation
 *  Simulate the JSON input and return JSON input
 *
 */

// TODO(Rogers): Error is that we use wrong geant4 devices for this stuff.
//               Really we should use EventAction to control per spill stuff and
//               stick multiple tracks from the spill into the same primary


#ifndef _SRC_MAP_MAPCPPSIMULATION_HH_
#define _SRC_MAP_MAPCPPSIMULATION_HH_

// C headers
#include <stdlib.h>

// C++ headers
#include <string>

// external libraries
#include "json/json.h"

//  MAUS code
#include "src/common_cpp/API/MapBase.hh"

namespace MAUS {

class MapCppSimulation : public MapBase<MAUS::Data> {
 public:
  /** @brief Sets up the worker
   */
  MapCppSimulation()
    : MapBase<MAUS::Data>("MapCppSimulation") {
  }

  ~MapCppSimulation() {}

 private:
  /** @brief Begin the startup procedure for Simulation
   *
   *  @param config a JSON document with the configuration.
   */
  void _birth(const std::string& argJsonConfigDocument);

  /** @brief NULL op - nothing to death
   *
   *  This takes no arguments
   */
  void _death() {}

  /** @brief Track JSON input and return new document
   *
   *  This function will simulate a single spill defined
   *  in JSON format.
   *
   * @param document a JSON document for a spill
   */
  void _process(MAUS::Data* data) const;

  bool _doVis;
};  // Don't forget this trailing colon!!!!
}

#endif  // _SRC_MAP_MAPCPPSIMULATION_HH_
