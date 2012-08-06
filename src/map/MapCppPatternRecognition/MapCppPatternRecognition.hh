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

/** @class MapCppPatternRecognition
 *  Digitize events by running Tracker electronics simulation.
 *
 */

#ifndef _SRC_MAP_MAPCPPPatternRecognition_H_
#define _SRC_MAP_MAPCPPPatternRecognition_H_

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
#include "Interface/Squeak.hh"
#include "Config/MiceModule.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"

#include "src/common_cpp/DataStructure/SciFiDigit.hh"
#include "src/common_cpp/DataStructure/SciFiCluster.hh"
#include "src/common_cpp/DataStructure/SciFiSpacePoint.hh"
#include "src/common_cpp/DataStructure/SciFiEvent.hh"
#include "src/common_cpp/DataStructure/SciFiSpill.hh"
#include "src/common_cpp/DataStructure/ThreeVector.hh"
#include "src/common_cpp/Recon/SciFi/SciFiClusterRec.hh"
#include "src/common_cpp/Recon/SciFi/SciFiSpacePointRec.hh"
#include "src/common_cpp/Recon/SciFi/PatternRecognition.hh"
// #include "src/common_cpp/Recon/SciFi/KalmanTrackFit.hh"

namespace MAUS {

class MapCppPatternRecognition {
 public:
  /** Sets up the worker
   *
   *  \param argJsonConfigDocument a JSON document with
   *         the configuration.
   */
  bool birth(std::string argJsonConfigDocument);

  /** Shutdowns the worker
   *
   *  This takes no arguments and does nothing
   */
  bool death();

  /** process JSON document
   *
   *  Receive a document with MC hits and return
   *  a document with digits
   * \param document a line/spill from the JSON input
   */
  std::string process(std::string document);

  /** Populates SciFiSpill with events that have spacepoint info ONLY
   *
   * @param spacepoint_array Array that contains all of the events in the spill
   * @param spill SciFiSpill that we are filling
   *
   */
  void make_SciFiSpill(Json::Value spacepoint_array, MAUS::SciFiSpill &spill);

  void pattern_recognition(MAUS::SciFiEvent &evt);

  // void track_fit(MAUS::SciFiEvent &evt);

  void save_to_json(MAUS::SciFiEvent &evt);

  void print_event_info(MAUS::SciFiEvent &event);

  Json::Value ConvertToJson(std::string jsonString);

  std::string JsonToString(Json::Value json_in);

 private:
  /// This should be the classname
  std::string _classname;
  /// This will contain the configuration
  Json::Value _configJSON;
  /// This will contain the root value after parsing
  Json::Value root;
  ///  JsonCpp setup
  Json::Reader reader;
  ///  Cut value for npe.
  double minPE;
  /// Value above which reconstruction is aborted.
  int ClustException;
  ///  Vector with the MICE SciFi Modules.
  std::vector<const MiceModule*> modules;

  int SciFiRunRecon;
}; // Don't forget this trailing colon!!!!

} // ~namespace MAUS

#endif
