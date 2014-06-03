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

 /** @class MapCppTrackerMCNoise
 *
 *  Simulate noise in the tracker electronics.
 */

#ifndef _COMPONENTS_MAP_MAPCPPTRACKERMCNOISE_H_
#define _COMPONENTS_MAP_MAPCPPTRACKERMCNOISE_H_

// C headers
#include <json/json.h>
#include <CLHEP/Random/RandPoisson.h>

// C++ headers
#include <cmath>
#include <vector>
#include <string>

// MAUS headers
#include "Config/MiceModule.hh"
#include "Utils/Exception.hh"
#include "Interface/Squeak.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"
#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/Globals/GlobalsManager.hh"
#include "src/common_cpp/DataStructure/SciFiNoiseHit.hh"
#include "src/common_cpp/DataStructure/MCEvent.hh"
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/API/MapBase.hh"
#include "src/common_cpp/JsonCppProcessors/SpillProcessor.hh"

namespace MAUS {

class MapCppTrackerMCNoise : public MapBase<Data> {
  public:
  /** Constructor - initialises pointers to NULL */
  MapCppTrackerMCNoise();

  /** Constructor - deletes any allocated memory */
  ~MapCppTrackerMCNoise();

  private:
  /** Sets up the worker */
  void _birth(const std::string& argJsonConfigDocument);

  /** Shuts down the worker */
  void _death();

  /** @brief Process Data object
   *
   *  Receive a structure with SciFi digits and returns
   *  a structure with simulated noise digits added
   */
  void _process(Data* data) const;

  /** @brief Simulates dark count
   *
   *  Check each SciFi channel for Dark PEs
   *  adds results to SciFiDigits
   */
  void dark_count(MAUS::Spill &spill) const;

  private:
  // MapCppMCNoise setup containers
  /// This will contain the configuration
  Json::Value* _configJSON;
  /// This will contain the root value before parsing
  std::string argJsonConfigDocument;
  /// This will contain all SciFi elements in MICE
  std::vector<const MiceModule*> SF_modules;
  /// Mean number of dark count PE
  double poisson_mean;
};
} // end namespace

#endif
