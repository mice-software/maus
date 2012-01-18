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

#include "src/common_cpp/Recon/TrackerSpill.hh"
#include "src/common_cpp/Recon/TrackerEvent.hh"
#include "src/map/MapCppTrackerRecon/RealDataDigitization.hh"
#include "src/common_cpp/Recon/SciFiDigit.hh"
#include "src/common_cpp/Recon/SciFiCluster.hh"
#include "src/common_cpp/Recon/SciFiSpacePoint.hh"

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
   */
  std::string process(std::string document);

  /** sanity check
   *
   *  Checks the sanity of the JSON input,
   */

  bool check_sanity_daq(Json::Value daq);

  bool check_sanity_digits(Json::Value digits);

  void cluster_recon(TrackerEvent &evt);

  void spacepoint_recon(TrackerEvent &evt);

  bool clusters_are_not_used(SciFiCluster* candidate_A, SciFiCluster* candidate_B);

  bool clusters_are_not_used(SciFiCluster* candidate_A, SciFiCluster* candidate_B,
                             SciFiCluster* candidate_C);

  bool kuno_accepts(SciFiCluster* cluster1, SciFiCluster* cluster2, SciFiCluster* cluster3);

  void save_to_json(TrackerEvent &evt);

  void fill_digits_vector(Json::Value digits, TrackerSpill &a_spill);

  void print_event_info(TrackerEvent &event);

  void dump_info(SciFiCluster* candidate_A, SciFiCluster* candidate_B, SciFiCluster* candidate_C);

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
  ///  Vector with the MICE SciFi Modules.
  std::vector<const MiceModule*> modules;
}; // Don't forget this trailing colon!!!!

#endif
