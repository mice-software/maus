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
#include <string>
#include <sstream>
#include <vector>
#include <map>

// Other headers
#include "Utils/Exception.hh"
#include "Interface/Squeak.hh"
#include "Config/MiceModule.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/Globals/GlobalsManager.hh"
#include "src/common_cpp/JsonCppProcessors/SpillProcessor.hh"
#include "src/common_cpp/DataStructure/ReconEvent.hh"

#include "src/common_cpp/DataStructure/SciFiEvent.hh"
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/Recon/SciFi/RealDataDigitization.hh"
#include "src/common_cpp/Recon/SciFi/SciFiClusterRec.hh"
#include "src/common_cpp/Recon/SciFi/SciFiSpacePointRec.hh"
#include "src/common_cpp/Recon/SciFi/PatternRecognition.hh"
#include "src/common_cpp/Recon/Kalman/KalmanTrackFit.hh"
#include "src/common_cpp/Recon/Kalman/KalmanTrack.hh"
#include "src/common_cpp/Recon/Kalman/MAUSSciFiPropagators.hh"
#include "src/common_cpp/Recon/Kalman/MAUSSciFiMeasurements.hh"
#include "src/common_cpp/API/MapBase.hh"

namespace MAUS {

struct SciFiPlaneGeometry;

class MapCppTrackerRecon : public MapBase<Data> {

 public:
  /** Constructor - initialises pointers to NULL */
  MapCppTrackerRecon();

  /** Constructor - deletes any allocated memory */
  ~MapCppTrackerRecon();

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

  /** Process MAUS data object
   *
   *  Receive a data object with digits (either MC or real) and then call the higher level
   *  reconstruction algorithms
   * 
   * \param document a line/spill from the JSON input
   */
  void _process(Data* data) const;

  /** Performs the cluster reconstruction
   *
   *  \param evt the current SciFiEvent
   */
  void cluster_recon(MAUS::SciFiEvent &evt) const;

  /** Performs the spacepoint reconstruction
   *
   *  \param evt the current SciFiEvent
   */
  void spacepoint_recon(MAUS::SciFiEvent &evt) const;

  /** Performs the pattern recogniton
   *
   *  Pattern Recogntion identifies which spacepoints are associate with particle tracks,
   *  then fits functions to the tracks using simple least squared fitting 
   *
   *  \param evt the current SciFiEvent
   */
  void pattern_recognition(MAUS::SciFiEvent &evt) const;

  /** Performs the final track fit
   *
   *  Track fit takes the spacepoints from Pattern Recognition and, going back to the clusters
   *  which formed the spacepoints, fits the tracks more acurately using a Kalman filter
   *
   *  \param evt the current SciFiEvent
   */
  void track_fit(MAUS::SciFiEvent &evt) const;

  void print_event_info(MAUS::SciFiEvent &event) const;

 private:
  /// This will contain the configuration
  Json::Value _configJSON;
  ///  Cut value for npe.
  double  _min_npe;
  /// Value above which reconstruction is aborted.
  int _size_exception;
  /// Pattern recognition flags
  bool _straight_pr_on;
  bool _helical_pr_on;
  bool _kalman_on;

  /// Kalman Track Fitter Object
  Kalman::TrackFit* _helical_track_fitter;
  Kalman::TrackFit* _straight_track_fitter;

  ///  Map of the planes geometry.
  SciFiGeometryHelper _geometry_helper;

  int SciFiRunRecon;
}; // Don't forget this trailing colon!!!!

} // ~namespace MAUS

#endif
