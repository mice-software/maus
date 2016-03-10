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

#include <string>

#include "src/common_cpp/API/MapBase.hh"
#include "src/common_cpp/DataStructure/Data.hh"
#include "src/common_cpp/Recon/Kalman/KalmanTrack.hh"



namespace MAUS {

class Data;
namespace Kalman {
  class GlobalMeasurement;
  class GlobalPropagator;
}

class MapCppGlobalTrackFit : public MapBase<Data> {

 public:
  /** Constructor - initialises pointers to NULL */
  MapCppGlobalTrackFit();

  /** Constructor - deletes any allocated memory */
  ~MapCppGlobalTrackFit();

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
   * \param data data corresponding to the spill
   */
  void _process(Data* data) const;

  /** Performs the final track fit
   *
   *  Track fit takes the spacepoints from Pattern Recognition and, going back to the clusters
   *  which formed the spacepoints, fits the tracks more acurately using a Kalman filter
   *
   *  \param evt the current SciFiEvent
   */
  void track_fit(ReconEvent &event) const;

 private:

  void append_to_data(GlobalEvent& event, Kalman::Track fit) const;
  Kalman::Track build_data(ReconEvent& event) const;
  Kalman::State build_seed(ReconEvent& event) const;

  Kalman::GlobalMeasurement* _measurement = NULL;
  Kalman::GlobalPropagator* _propagator = NULL;
  Kalman::TrackFit* _kalman_fit = NULL;
  std::vector<DataStructure::Global::DetectorPoint> _active_detectors;
  DataStructure::Global::DetectorPoint _position_seed_1;
  DataStructure::Global::DetectorPoint _position_seed_2;
  DataStructure::Global::DetectorPoint _time_seed_1;
  DataStructure::Global::DetectorPoint _time_seed_2;
  bool _will_require_triplets;

  static double mu_mass;
  static double c_light;

};

} // ~namespace MAUS

#endif
