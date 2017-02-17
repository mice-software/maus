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

#ifndef _SRC_MAP_MAPCPPTrackerTrackFit_H_
#define _SRC_MAP_MAPCPPTrackerTrackFit_H_

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
#include "src/common_cpp/DataStructure/SciFiSeed.hh"
#include "src/common_cpp/Recon/SciFi/PatternRecognition.hh"
#include "src/common_cpp/Recon/Kalman/KalmanTrackFit.hh"
#include "src/common_cpp/Recon/Kalman/KalmanTrack.hh"
#include "src/common_cpp/Recon/Kalman/MAUSSciFiPropagators.hh"
#include "src/common_cpp/Recon/Kalman/MAUSSciFiMeasurements.hh"
#include "src/common_cpp/API/MapBase.hh"

namespace MAUS {

struct SciFiPlaneGeometry;

/** @class MapCppTrackerTrackFit
 *  Perform the final tracker track fit using the Kalman filter
 *
 */
class MapCppTrackerTrackFit : public MapBase<Data> {

 public:
  /** Constructor - initialises pointers to NULL */
  MapCppTrackerTrackFit();

  /** Constructor - deletes any allocated memory */
  ~MapCppTrackerTrackFit();

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
  SciFiTrack* track_fit_helix(MAUS::SciFiSeed* seed) const;

  SciFiTrack* track_fit_straight(MAUS::SciFiSeed* seed) const;

  /** 
    * @brief Rates the track based on the outcome of the reconstruction
    */
  void calculate_track_rating(SciFiTrack* track) const;

 private:

  void _set_field_values(SciFiEvent* event) const;

  /// This will contain the configuration
  Json::Value _configJSON;

  // Various options and parameters
  bool _kalman_on;
  bool _use_mcs;
  bool _use_eloss;
  bool _use_patrec_seed;
  bool _correct_pz;
  int _excellent_num_trackpoints;
  int _good_num_trackpoints;
  int _poor_num_trackpoints;
  double _excellent_p_value;
  double _good_p_value;
  double _poor_p_value;
  int _excellent_num_spacepoints;
  int _good_num_spacepoints;
  int _poor_num_spacepoints;
  double _seed_value;

  /// Kalman Track Fitter Object
  Kalman::TrackFit* _helical_track_fitter;
  Kalman::TrackFit* _straight_track_fitter;

  SciFiHelicalMeasurements* _helical_measurement;
  SciFiStraightMeasurements* _straight_measurement;

  ///  Map of the planes geometry.
  SciFiGeometryHelper _geometry_helper;

  int SciFiRunRecon;
}; // Don't forget this trailing colon!!!!

} // ~namespace MAUS

#endif
