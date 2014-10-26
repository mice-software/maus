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

/** @namespace SciFiAnalysisTools
 *
 * Various tools encapsulated in a class for use with SciFi data
 * (in particular during Pattern Recognition).
 *
 */

// C++ headers
#include <vector>

// ROOT headers

// MAUS headers
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/DataStructure/MCEvent.hh"
#include "src/common_cpp/DataStructure/Track.hh"
#include "src/common_cpp/DataStructure/ThreeVector.hh"
#include "src/common_cpp/DataStructure/Hit.hh"
#include "src/common_cpp/Recon/SciFi/SciFiLookup.hh"
#include "src/common_cpp/DataStructure/SciFiSpacePoint.hh"
#include "src/common_cpp/DataStructure/SciFiCluster.hh"

namespace SciFiAnalysisTools {

/** Find the MC momentum of a track cluster */
bool find_mc_cluster_momentum(const int track_id, const MAUS::SciFiCluster* clus,
                              MAUS::SciFiLookup &lkup, MAUS::ThreeVector &mom);

/** Find the MC momentum of a track spacepoint */
bool find_mc_spoint_momentum(const int track_id, const MAUS::SciFiSpacePoint* sp,
                             MAUS::SciFiLookup &lkup, MAUS::ThreeVector &mom);

/** Find the MC track ID number given a vector of spacepoint numbers and their MC track IDs */
bool find_mc_tid(const std::vector< std::vector<int> > &spoint_mc_tids, const int min_matches,
                 int &tid, int &counter);

/** @brief Find the number of MC tracks which intercept 4 or more stations in each tracker
 *  @param pdg_id The particle data group ID. If set to zero, all particles are counted
 *  @param mc_evt The MC event pointer
 *  @param n_tracks_t1 The number of valid MC tracks in T1
 *  @param n_tracks_t2 The number of valid MC tracks in T2
 * */
bool find_n_valid_mc_track(int pdg_id, const MAUS::MCEvent* mc_evt,
                           int &n_tracks_t1, int &n_tracks_t2);

/** Test to see whether a MC track is good for scifi analysis, defined according to whether a hit */
bool is_valid_mc_track(int trker_num, const MAUS::Track* mc_trk,
                       const std::vector<MAUS::SciFiHit*> hits);

} // ~namespace SciFiTools
