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
}

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
}

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
}

bool find_n_valid_mc_track(int pdg_id, const MAUS::MCEvent* mc_evt,
                           int &n_tracks_t1, int &n_tracks_t2) {

  // Check the MC evt pointer is not NULL
  if (!mc_evt) {
    std::cerr << "Empty MC event pointer passed, aborting." << std::endl;
    return false;
  }

  std::vector<MAUS::SciFiHit>* hits = mc_evt->GetSciFiHits();
  std::vector<MAUS::Track>* tracks = mc_evt->GetTracks();

  // Loop over all MC tracks in the MC event
  for ( size_t iTrk= 0; iTrk < tracks->size(); ++iTrk ) {
    int station_counter_t1 = 0;   // # of stations in T1 found so far with hits from the MC track
    int station_counter_t2 = 0;   // # of stations in T1 found so far with hits from the MC track
    bool found_valid_t1 = false;  // Have enough stations been hit that know track is valid in T1
    bool found_valid_t2 = false;  // Have enough stations been hit that know track is valid in T2

    // Setup a vector, index is station number - 1, content is bool of whether station has been hit
    int n_stations = 5;
    std::vector<bool> stations_hit_t1(n_stations);
    std::vector<bool> stations_hit_t2(n_stations);
    for ( int i = 0; i < n_stations; ++i ) {
      stations_hit_t1[i] = false;
      stations_hit_t2[i] = false;
    }

    // Loop over all hits in MC event
    for ( size_t iHit= 0; iHit < hits->size(); ++iHit ) {
      // Check if the current hit caused by the current MC track
      if ( hits->at(iHit).GetTrackId() == tracks->at(iTrk).GetTrackId() ) {
        // Check that if the pdg_id is set, the current track matches the specified particle type
        if ( pdg_id == 0 || pdg_id == tracks->at(iTrk).GetParticleId() ) {
          int tracker_num = hits->at(iHit).GetChannelId()->GetTrackerNumber();
          int station_num = hits->at(iHit).GetChannelId()->GetStationNumber();
          if (tracker_num == 0) {
            // Check if the current station has been NOT been hit (set false), and if not set to true
            if ( !stations_hit_t1[station_num - 1] ) {
            stations_hit_t1[station_num - 1] = true;
            station_counter_t1++;
            }
          }
          if (tracker_num == 1) {
            if ( !stations_hit_t2[station_num - 1] ) {
              stations_hit_t2[station_num - 1] = true;
              station_counter_t2++;
            }
          }
        }
      }
      // If have found that the MC track is valid in T1 for the first time for this track...
      if ( station_counter_t1 >= 4 ) {
        if (!found_valid_t1) ++n_tracks_t1;
        found_valid_t1 = true;
      }
      // If have found that the MC track is valid in T2 for the first time for this track...
      if ( station_counter_t2 >= 4 ) {
        if (!found_valid_t2) ++n_tracks_t2;
        found_valid_t2 = true;
      }
      // If have found that MC track is valid in both trackers, move on to next track
      if ( found_valid_t1 && found_valid_t2 ) break;
    }
  }
  return true;
}

bool is_valid_mc_track(int trker_num, const MAUS::Track* mc_trk,
                       const std::vector<MAUS::SciFiHit*> hits) {

  if (!mc_trk) {
    std::cerr << "Empty MC track pointer passed, aborting." << std::endl;
    return false;
  }

  int station_counter(0);
  std::vector<bool> stations_hit(5);
  for ( size_t i = 0; i < stations_hit.size(); ++i ) {
    stations_hit[i] = false;
  }

  for ( size_t i = 0; i < hits.size(); ++i ) {
    if (!hits[i]) {
      std::cerr << "Empty SciFiHit pointer found, skipping." << std::endl;
      continue;
    } else if ( !(hits[i]->GetChannelId()) ) {
      std::cerr << "Empty SciFiChannelId pointer found, skipping." << std::endl;
      continue;
    }
    if ( hits[i]->GetTrackId() == mc_trk->GetTrackId() ) {
      if ( hits[i]->GetChannelId()->GetTrackerNumber() == trker_num )  {
        int station_num = hits[i]->GetChannelId()->GetStationNumber();
        if ( station_num < 1 || station_num > 5 ) {
          std::cerr << "Bad station number found (" << station_num << ") skipping." << std::endl;
          continue;
        }
        if ( !stations_hit[station_num - 1] ) {
          stations_hit[station_num - 1] = true;
          station_counter++;
        }
        if (station_counter >= 4) return true;
      }
    }
  }
  return false;
}

} // ~namespace SciFiAnalysisTools

