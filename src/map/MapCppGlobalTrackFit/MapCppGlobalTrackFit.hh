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

#include <vector>
#include <string>

#include "src/common_cpp/DataStructure/Global/ReconEnums.hh"
#include "src/common_cpp/DataStructure/Data.hh"
#include "src/common_cpp/DataStructure/ReconEvent.hh"
#include "src/common_cpp/DataStructure/GlobalEvent.hh"
#include "src/common_cpp/Recon/Kalman/KalmanTrack.hh"
#include "src/common_cpp/API/MapBase.hh"
#include "src/common_cpp/Recon/Kalman/Global/ErrorTracking.hh"

namespace MAUS {


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

  void append_to_data(GlobalEvent& event, Kalman::Track fit, double mass) const;

  SeedManager _seed_manager;

  static double c_light;
  MAUS::Kalman::Global::ErrorTracking::MCSModel _scat_model = MAUS::Kalman::Global::ErrorTracking::moliere_forwards;
  MAUS::Kalman::Global::ErrorTracking::ELossModel _eloss_model = MAUS::Kalman::Global::ErrorTracking::bethe_bloch_forwards;
  MAUS::Kalman::Global::ErrorTracking::GeometryModel _geom_model = MAUS::Kalman::Global::ErrorTracking::geant4;

  class FitInfo {
    public:
      FitInfo(Json::Value args, SeedManager& seeds);
      double mass_hypothesis;
      double charge_hypothesis;
      double min_step;
      double max_step;
      double min_z;
      double max_z;
      size_t max_iteration;
      bool will_smooth;
      bool will_require_triplet_space_points;
      SeedBase* seed_algorithm;
      std::vector<DataStructure::Global::DetectorPoint> active_detectors;
  };
  std::vector<FitInfo> _fit_info;
};


} // ~namespace MAUS

#endif

