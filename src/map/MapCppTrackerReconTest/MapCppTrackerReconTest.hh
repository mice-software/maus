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

/** @class MapCppTrackerReconTest
 *  Digitize events by running Tracker electronics simulation.
 *
 */

#ifndef _SRC_MAP_MAPCPPTrackerRecon_H_
#define _SRC_MAP_MAPCPPTrackerRecon_H_

// #define KALMAN_TEST

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
#include "Utils/Squeak.hh"
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

class MapCppTrackerReconTest : public MapBase<Data> {

 public:
  /** Constructor - initialises pointers to NULL */
  MapCppTrackerReconTest();

  /** Constructor - deletes any allocated memory */
  ~MapCppTrackerReconTest();

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

  /** Performs the final track fit
   *
   *  Track fit takes the spacepoints from Pattern Recognition and, going back to the clusters
   *  which formed the spacepoints, fits the tracks more acurately using a Kalman filter
   *
   *  \param evt the current SciFiEvent
   */
  void track_fit(MAUS::SciFiEvent &evt) const;

  /** Deduce and fill the reference plane position and momentum for
    * helical tracks
    */
  void extrapolate_helical_reference(SciFiEvent& event) const;

  /** Deduce and fill the reference plane position and momentum for
    * straight tracks
    */
  void extrapolate_straight_reference(SciFiEvent& event) const;


  void print_event_info(MAUS::SciFiEvent &event) const;

  /**
   * @brief Populate global position output for the spacepoints
   */
  void set_spacepoint_global_output(SciFiSpacePointPArray spoints) const;

 private:
  /// This will contain the configuration
  Json::Value _configJSON;
  ///  Cut value for npe.
  double  _min_npe;
  /// Value above which reconstruction is aborted.
  int _size_exception;
  /// Spacepoint reconstruction parameters
  double _acceptable_radius;
  double _kuno_sum_T1_S5;
  double _kuno_sum;
  double _kuno_tolerance;
  /// Pattern recognition flags
  bool _spacepoints_on;
  bool _up_straight_pr_on;
  bool _down_straight_pr_on;
  bool _up_helical_pr_on;
  bool _down_helical_pr_on;
  bool _kalman_on;
  bool _patrec_on;

  bool _use_mcs;
  bool _use_eloss;
  bool _use_patrec_seed;
  bool _correct_pz;

  double _seed_value;

  /// Reconstruction Classes
  SciFiClusterRec _cluster_recon;
  SciFiSpacePointRec _spacepoint_recon;

  /// Pattern Recognitin Class
  PatternRecognition _pattern_recognition;

  /// Kalman Track Fitter Object
  Kalman::TrackFit* _spacepoint_helical_track_fitter;
  Kalman::TrackFit* _spacepoint_straight_track_fitter;

  int _spacepoint_recon_plane;

  ///  Map of the planes geometry.
  SciFiGeometryHelper _geometry_helper;

  int SciFiRunRecon;
}; // Don't forget this trailing colon!!!!

} // ~namespace MAUS

#endif
