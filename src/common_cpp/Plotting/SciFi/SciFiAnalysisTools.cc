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

// C++ headers
#include <map>

// MAUS headers
#include "src/common_cpp/Plotting/SciFi/SciFiAnalysisTools.hh"

namespace SciFiAnalysisTools {

bool find_mc_cluster_momentum(const int track_id, const MAUS::SciFiCluster* clus,
                              MAUS::SciFiLookup &lkup, MAUS::ThreeVector &mom) {

  // Loop over digits within the cluster
  std::vector<MAUS::SciFiDigit*> digits = clus->get_digits_pointers();
  std::vector<MAUS::SciFiDigit*>::iterator dig_it;
  for (dig_it = digits.begin(); dig_it != digits.end(); ++dig_it) {
    std::vector<MAUS::SciFiHit*> hits;
    // Make the lookup table to access the MC hits associated with this digit
    if (!lkup.get_hits((*dig_it), hits)) {
      std::cerr << "Lookup failed\n";
      continue;
    }
    // Loop over the hits until find one matching the track ID, then use to set the cluster mom
    std::vector<MAUS::SciFiHit*>::iterator hit_it;
    for (hit_it = hits.begin(); hit_it != hits.end(); ++hit_it) {
      if (track_id == (*hit_it)->GetTrackId()) {
        mom = (*hit_it)->GetMomentum();
        return true;
      }
    }
  }
  return false;
};

bool find_mc_spoint_momentum(const int track_id, const MAUS::SciFiSpacePoint* sp,
                             MAUS::SciFiLookup &lkup, MAUS::ThreeVector &mom) {

  std::vector<MAUS::SciFiCluster*> clusters = sp->get_channels_pointers();
  mom = MAUS::ThreeVector(0.0, 0.0, 0.0);
  int counter = 0;

  // Calculate the momentum for each cluster then take the average
  for (size_t i = 0; i < clusters.size(); ++i) {
    MAUS::ThreeVector clus_mom(0.0, 0.0, 0.0);
    if ( find_mc_cluster_momentum(track_id, clusters[i], lkup, clus_mom) ) {
      mom += clus_mom;
      ++counter;
    }
  }
  if ( counter > 0 ) {
    mom /= static_cast<double>(counter);
    return true;
  }
  return false;
};


bool find_mc_tid(const std::vector< std::vector<int> > &spoint_mc_tids, int &tid, int &counter) {

  std::map<int, int> track_id_counter;  // Map of track id to freq for each spoint
  for ( size_t i = 0; i < spoint_mc_tids.size(); ++i ) {
    for ( size_t j = 0; j < spoint_mc_tids[i].size(); ++j ) {
      int current_tid = spoint_mc_tids[i][j];
      if (track_id_counter.count(current_tid))
        track_id_counter[current_tid] = track_id_counter[current_tid] + 1;
      else
        track_id_counter[current_tid] = 1;
    }
  }

  std::map<int, int>::iterator mit1;
  tid = 0;
  for ( mit1 = track_id_counter.begin(); mit1 != track_id_counter.end(); ++mit1 ) {
    if ( mit1->second > 2 && tid == 0 ) {
      tid = mit1->first;
      counter = mit1->second;
    } else if ( mit1->second > 2 && tid != 0 ) {
      tid = -1;  // A malformed track, cannot associate with an mc track id
      counter = 0;
      return false;
    }
  }
  return true;
};

} // ~namespace SciFiAnalysisTools

