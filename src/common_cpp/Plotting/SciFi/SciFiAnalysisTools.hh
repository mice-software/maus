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
#include "src/common_cpp/DataStructure/ThreeVector.hh"
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
bool find_mc_tid(const std::vector< std::vector<int> > &spoint_mc_tids, int &tid, int &counter);

} // ~namespace SciFiTools
