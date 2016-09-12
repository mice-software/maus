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

/** @class MapCppTrackerRecon 
 *  Digitize events by running Tracker electronics simulation.
 *
 */

#ifndef _SRC_MAP_MAPCPPTrackerDigits_H_
#define _SRC_MAP_MAPCPPTrackerDigits_H_

// C headers
#include <assert.h>
#include <json/json.h>
#include <CLHEP/Units/PhysicalConstants.h>

// C++ headers
#include <cmath>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

// Other headers
#include "Utils/Squeak.hh"
#include "Config/MiceModule.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"

#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/DataStructure/SciFiEvent.hh"
#include "src/common_cpp/Recon/SciFi/RealDataDigitization.hh"
#include "src/common_cpp/DataStructure/SciFiDigit.hh"

#include "src/common_cpp/API/MapBase.hh"

namespace MAUS {

class MapCppTrackerDigits : public MapBase<Data> {
 public:
  MapCppTrackerDigits();

  ~MapCppTrackerDigits();

 private:
  /** Sets up the worker
   *
   *  \param argJsonConfigDocument a JSON document with
   *         the configuration.
   */
  void _birth(const std::string& argJsonConfigDocument);

  /** Shutdowns the worker
   *
   *  This takes no arguments and does nothing
   */
  void _death();

  /** process MAUS spill object
   *
   *  Receive a document with MC hits and return
   *  a document with digits
   * \param document a MAUS data object for a MICE spill
   */
  void _process(Data* document) const;

 private:
  /// This will contain the configuration
  Json::Value _configJSON;
  /// This will contain the root value after parsing
  Spill* _spill_cpp;
  ///  JsonCpp setup
  Json::Reader reader;
  ///  Cut value for npe.
  double minPE;
  ///  Vector with the MICE SciFi Modules.
  std::vector<const MiceModule*> modules;

  int SciFiRunRecon;

  RealDataDigitization *real;
}; // Don't forget this trailing colon!!!!
#endif

} // ~namespace MAUS
