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

#ifndef MAUS_TRACK_WRAPPER_HH
#define MAUS_TRACK_WRAPPER_HH

#include "src/common_cpp/Recon/SciFi/SciFiGeometryHelper.hh"

#include "src/common_cpp/Recon/Kalman/KalmanTrack.hh"
#include "src/common_cpp/Recon/Kalman/KalmanTrackFit.hh"
#include "src/common_cpp/DataStructure/SciFiEvent.hh" // Includes everything!
#include "src/common_cpp/DataStructure/SciFiTrack.hh"
#include "src/common_cpp/DataStructure/SciFiHelicalPRTrack.hh"
#include "src/common_cpp/DataStructure/SciFiStraightPRTrack.hh"

/** 
 *  This file contains useful functions for interfacing the Kalman filter
 *  routines with the rest of MAUS. 
 *
 *  Specifically this involves converting Kalman Tracks to Scifi Tracks, or
 *  similar; or the creation of the seed state.
 */
namespace MAUS {

  /** @brief Create a seed from a helical track
   */
  Kalman::State ComputeSeed(SciFiHelicalPRTrack* h_track, const SciFiGeometryHelper* geom,
                                          bool correct_energy_loss = true, double seed_cov = -1.0);

  /** @brief Create a seed from a straight track
   */
  Kalman::State ComputeSeed(SciFiStraightPRTrack* s_track, const SciFiGeometryHelper* geom,
                                                                           double seed_cov = -1.0);

  /** @brief Convert a KalmanTrack to a SciFiTrack for the data structure
   */
  SciFiTrack* ConvertToSciFiTrack(const Kalman::TrackFit* fitter, const SciFiGeometryHelper* geom);

  /** @brief Builds a data track using a PR track
   */
  template<class PR_TYPE> Kalman::Track BuildTrack(PR_TYPE* pr_track,
                                                                  const SciFiGeometryHelper* geom);

  /** @brief Builds a data track using an array of spacepoints
   */
  Kalman::Track BuildSpacepointTrack(SciFiSpacePointPArray spacepoints,
                           const SciFiGeometryHelper* geom, int plane_num = 0, double smear = 0.2);




////////////////////////////////////////////////////////////////////////////////
  // Template Definitions
////////////////////////////////////////////////////////////////////////////////

  template<class PR_TYPE>
  Kalman::Track BuildTrack(PR_TYPE* pr_track, const SciFiGeometryHelper* geom) {

    SciFiSpacePointPArray spacepoints = pr_track->get_spacepoints_pointers();

    Kalman::Track new_track(1);

    const SciFiPlaneMap& geom_map = geom->GeometryMap().find(
                                                           pr_track->get_tracker())->second.Planes;
    int tracker_const = (pr_track->get_tracker() == 0 ? -1 : 1);

    for (SciFiPlaneMap::const_iterator iter = geom_map.begin(); iter != geom_map.end(); ++iter) {
      int id = iter->first * tracker_const;
      Kalman::State new_state = Kalman::State(1, iter->second.Position.z());
      new_state.SetId(id);
      new_track.Append(new_state);
    }

    size_t numb_spacepoints = spacepoints.size();
    for (size_t i = 0; i < numb_spacepoints; ++i) {
      SciFiSpacePoint *spacepoint = spacepoints[i];
      size_t numbclusters = spacepoint->get_channels()->GetLast() + 1;
      for (size_t j = 0; j < numbclusters; ++j) {
        SciFiCluster* cluster = static_cast<SciFiCluster*>(spacepoint->get_channels()->At(j));

        int id = (cluster->get_station() - 1)*3 + cluster->get_plane(); // Actually (id - 1)

        // TODO :
        // - APPLY GEOMETRY CORRECTIONS!
        // - Fill covariance matrix correctly!
        TMatrixD state_vector(1, 1);
        TMatrixD covariance(1, 1);

        state_vector(0, 0) = cluster->get_alpha();
//        covariance(0, 0) = 0.427*0.427 / 12.0;
//        covariance(0, 0) = 1.4925*1.4945 / 12.0;
        covariance(0, 0) = 0.0;

        new_track[id].SetVector(state_vector);
        new_track[id].SetCovariance(covariance);
      }
    }
    return new_track;
  }

  Kalman::Track BuildTrack(SciFiClusterPArray pr_track, const SciFiGeometryHelper* geom);
} // namespace MAUS

#endif // MAUS_TRACK_WRAPPER_HH

