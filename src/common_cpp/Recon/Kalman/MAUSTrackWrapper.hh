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
#include "src/common_cpp/DataStructure/ScFiTrack.hh"
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
  Kalman::State ComputeSeed(SciFiHelicalPRTrack* h_track, SciFiGeometryHelper* geom);

  /** @brief Create a seed from a straight track
   */
  Kalman::State ComputeSeed(SciFiStraightPRTrack* s_track, SciFiGeometryHelper* geom);

  /** @brief Convert a KalmanTrack to a SciFiTrack for the data structure
   */
  SciFiTrack* CovertToSciFiTrack(Kalman::Track& k_track, SciFiGeometryHelper* geom);

  /** @brief Builds a data track using a PR track
   */
  template<class PR_TYPE> Kalman::Track BuildTrack(PR_TYPE* pr_track, SciFiGeometryHelper* geom);






////////////////////////////////////////////////////////////////////////////////
  // Template Definitions
////////////////////////////////////////////////////////////////////////////////

  template<class PR_TYPE>
  Kalman::Track BuildTrack(PR_TYPE* pr_track, SciFiGeometryHelper* geom) {

    SciFiSpacePointPArray spacepoints = pr_track->get_spacepoints_pointers();

    Kalman::Track new_track(1);

    if ( pr_track->tracker() == 0 ) {
      for ( unsigned int i = 0; i < 15; ++i ) {
        new_track.Append( Kalman::State(1, geom->GeometryMap()[-1*i].Position));
      }
    } else {
      for ( unsigned int i = 0; i < 15; ++i ) {
        new_track.Append( Kalman::State(1, geom->GeometryMap()[i].Position));
      }
    }

    size_t numb_spacepoints = spacepoints.size();
    for ( size_t i = 0; i < numb_spacepoints; ++i ) {
      SciFiSpacePoint *spacepoint = spacepoints[i];
      size_t numbclusters = spacepoint->get_channels()->GetLast() + 1;
      for ( size_t j = 0; j < numbclusters; ++j ) {
        SciFiCluster* cluster = static_cast<SciFiCluster*>(spacepoint->get_channels()->At(j));
        
        int id = cluster->get_station()*3 + cluster->get_plane();
//        SciFiPlaneGeometry = geom->GeometryMap()[id];

        // TODO : 
        // - APPLY GEOMETRY CORRECTIONS!
        // - Fill covariance matrix correctly!
        TMatrixD state_vector(1, 1);
        TMatrixD covariance(1, 1);

        state_vector(0, 0) = cluster->get_alpha();
        covariance(0, 0) = 0.427 / sqrt(12.0);

        new_track[id].SetVector(state_vector);
        new_track[id].SetCovariance(covariance);
      }
    }
    return new_track;
  }
} // namespace MAUS

#endif // MAUS_TRACK_WRAPPER_HH

