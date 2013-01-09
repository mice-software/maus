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

#include "src/common_cpp/DataStructure/SciFiDigit.hh"
#include "src/common_cpp/DataStructure/SciFiCluster.hh"
#include "src/common_cpp/DataStructure/SciFiSpacePoint.hh"
#include "src/common_cpp/DataStructure/SciFiEvent.hh"
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/Recon/SciFi/RealDataDigitization.hh"
#include "src/common_cpp/Recon/SciFi/SciFiClusterRec.hh"
#include "src/common_cpp/Recon/SciFi/SciFiSpacePointRec.hh"
#include "src/common_cpp/Recon/SciFi/PatternRecognition.hh"
#include "src/common_cpp/Recon/Kalman/KalmanTrackFit.hh"
#include "src/common_cpp/Recon/Kalman/KalmanSeed.hh"

namespace MAUS {

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

  /** Process JSON document
   *
   *  Receive a document with digits (either MC or real) and then call the higher level
   *  reconstruction algorithms
   * 
   * \param document a line/spill from the JSON input
   */
  std::string process(std::string document);

  /** Performs the cluster reconstruction
   *
   *  \param evt the current SciFiEvent
   */
  void cluster_recon(MAUS::SciFiEvent &evt);

  /** Performs the spacepoint reconstruction
   *
   *  \param evt the current SciFiEvent
   */
  void spacepoint_recon(MAUS::SciFiEvent &evt);

  /** Performs the pattern recogniton
   *
   *  Pattern Recogntion identifies which spacepoints are associate with particle tracks,
   *  then fits functions to the tracks using simple least squared fitting 
   *
   *  \param evt the current SciFiEvent
   */
  void pattern_recognition(const bool helical_pr_on, const bool straight_pr_on,
                           MAUS::SciFiEvent &evt);

  /** Performs the final track fit
   *
   *  Track fit takes the spacepoints from Pattern Recognition and, going back to the clusters
   *  which formed the spacepoints, fits the tracks more acurately using a Kalman filter
   *
   *  \param evt the current SciFiEvent
   */
  void track_fit(MAUS::SciFiEvent &evt);

  /** Takes json data and returns a Sc
   *
   *  Track fit takes the spacepoints from Pattern Recognition and, going back to the clusters
   *  which formed the spacepoints, fits the tracks more acurately using a Kalman filter
   *
   *  \param evt the current SciFiEvent
   */
  bool read_in_json(std::string json_data, MAUS::Spill &spill);

  void save_to_json(MAUS::Spill &spill);

  void print_event_info(MAUS::SciFiEvent &event);

  /** @brief Set helical PR on flag */
  void set_helical_pr_on(const bool helical_pr_on) { _helical_pr_on = helical_pr_on; }

  /** @brief Set straight PR on flag */
  void set_straight_pr_on(const bool straight_pr_on) { _straight_pr_on = straight_pr_on; }

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
  /// Pattern recognition flags
  bool _helical_pr_on;
  bool _straight_pr_on;
  ///  Vector with the MICE SciFi Modules.
  std::vector<const MiceModule*> modules;

  int SciFiRunRecon;
}; // Don't forget this trailing colon!!!!

} // ~namespace MAUS

#endif
