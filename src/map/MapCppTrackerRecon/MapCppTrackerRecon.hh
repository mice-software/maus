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

#ifndef _SRC_MAP_MAPCPPTrackerRecon_H_
#define _SRC_MAP_MAPCPPTrackerRecon_H_
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

#include "src/common_cpp/Reconstruction/SciFi/SciFiSpill.hh"
#include "src/common_cpp/Reconstruction/SciFi/SciFiEvent.hh"
#include "src/common_cpp/Reconstruction/SciFi/RealDataDigitization.hh"
#include "src/common_cpp/Reconstruction/SciFi/SciFiClusterRec.hh"
#include "src/common_cpp/Reconstruction/SciFi/SciFiSpacePointRec.hh"
#include "src/common_cpp/Reconstruction/SciFi/SciFiDigit.hh"
#include "src/common_cpp/Reconstruction/SciFi/SciFiCluster.hh"
#include "src/common_cpp/Reconstruction/SciFi/SciFiSpacePoint.hh"
#include "src/common_cpp/Reconstruction/SciFi/PatternRecognition.hh"

class MapCppTrackerRecon {
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

  /** makes digits
   *
   *  Digits are made by either picking up
   *  daq or MC.
   *  \param spill a SciFiSpill object
   *  \param root the parsed JSON input
   */
  void digitization(SciFiSpill &spill, Json::Value &root);

  /** fills digits from MC digitization
   *
   *  \param digits the MC digits
   *  \param a_spill the SciFiSpill we are processing
   */
  void fill_digits_vector(Json::Value &digits, SciFiSpill &a_spill);

  /** performs the cluster reconstruction
   *
   *  \param evt the current SciFiEvent
   */
  void cluster_recon(SciFiEvent &evt);

  /** performs the spacepoint reconstruction
   *
   *  \param evt the current SciFiEvent
   */
  void spacepoint_recon(SciFiEvent &evt);

  void pattern_recognition(SciFiEvent &evt);

  void save_to_json(SciFiEvent &evt);

  void print_event_info(SciFiEvent &event);

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
}; // Don't forget this trailing colon!!!!
#endif
