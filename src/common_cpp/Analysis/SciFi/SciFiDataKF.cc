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
#include <vector>
#include <map>
#include <cmath>

// MAUS headers
#include "src/common_cpp/Analysis/SciFi/SciFiDataKF.hh"
#include "src/common_cpp/Plotting/SciFi/SciFiAnalysisTools.hh"
#include "src/common_cpp/Recon/SciFi/SciFiLookup.hh"
#include "src/common_cpp/DataStructure/MCEvent.hh"
#include "src/common_cpp/DataStructure/SciFiEvent.hh"
#include "src/common_cpp/DataStructure/SciFiTrack.hh"
#include "src/common_cpp/DataStructure/SciFiTrackPoint.hh"
#include "src/common_cpp/DataStructure/SciFiCluster.hh"
#include "src/common_cpp/DataStructure/SciFiDigit.hh"
#include "src/common_cpp/DataStructure/Hit.hh"
#include "src/common_cpp/DataStructure/ThreeVector.hh"

namespace MAUS {

SciFiDataKF::SciFiDataKF() {
  // Do nothing
}

SciFiDataKF::~SciFiDataKF() {
  // Do nothing
}

void SciFiDataKF::Clear() {
  mDataKF.clear();
}

void SciFiDataKF::ReduceData(MCEvent *aMcEvent, SciFiEvent* aSFEvent) {
  std::vector<SciFiTrack*> trks = aSFEvent->scifitracks();

  // Create the lookup bridge between MC and Recon
  SciFiLookup lkup;
  lkup.make_hits_map(aMcEvent);

  // Loop over tracks in event
  for (size_t iTrk = 0; iTrk < trks.size(); ++iTrk) {
    SciFiTrack* trk = trks[iTrk];

    DataKF lDataStruct;
    lDataStruct.TrackerNumber = trk->tracker();
    lDataStruct.Charge = trk->charge();
    lDataStruct.NDF = trk->ndf();

    // Calc recon momentum
    //////////////////////

    // Loop over trackpoints and find those in station 1
    std::vector<SciFiTrackPoint*> tps_s1;
    for (size_t iTP = 0; iTP < trk->scifitrackpoints().size(); ++iTP) {
      SciFiTrackPoint* tp_temp;
      tp_temp = trk->scifitrackpoints()[iTP];
      if ( tp_temp->station() == 1 ) {
        tps_s1.push_back(tp_temp);
      } else  {
        continue;
      }
    }

    // If we found no track points in station, then give up on this track
    if (tps_s1.size() == 0) continue;

    // Now, specify that the trackpoint must come from the reference plane (i.e. plane 0)
    SciFiTrackPoint* tp = NULL;
    for (size_t iTP = 0; iTP < tps_s1.size(); ++iTP) {
      if (tps_s1[iTP]->plane() == 0) {
        tp = tps_s1[iTP];
      }
    }

    // If found not track point in plane 0, give up for this track
    if (!tp) break;

    lDataStruct.xRec = tp->pos().x();
    lDataStruct.yRec = tp->pos().y();
    lDataStruct.zRec = tp->pos().z();
    lDataStruct.PxRec = tp->mom().x();
    lDataStruct.PyRec = tp->mom().y();
    lDataStruct.PtRec = std::sqrt(tp->mom().x()*tp->mom().x() + tp->mom().y()*tp->mom().y());
    lDataStruct.PzRec = tp->mom().z();

    // Calc MC momentum
    ///////////////////

    std::vector< std::vector<int> > mc_tids;  // Vector of MC track ids for each spoint
    std::vector<SciFiTrackPoint*> tps = trk->scifitrackpoints();
    // Loop over trackpoints associated with the track
    for ( size_t iTP = 0; iTP < tps.size(); ++iTP ) {
      std::map<int, int> track_id_counter;  // Map of track id to freq for each spoint
      SciFiCluster* clus = tps[iTP]->get_cluster_pointer();
      std::vector<SciFiDigit*> digits = clus->get_digits_pointers();

      // Loop over digits
      for ( size_t m = 0; m < digits.size(); ++m ) {
        // Perform the digits to hits lookup
        std::vector<SciFiHit*> hits;
        if (!lkup.get_hits(digits[m], hits)) {
          std::cerr << "SciFiDataKF::Lookup failed\n";
          continue;
        }

        // Loop over MC hits
        for ( size_t n = 0; n < hits.size(); ++n ) {
          int track_id = hits[n]->GetTrackId();
          if (track_id_counter.count(track_id))
            track_id_counter[track_id] = track_id_counter[track_id] + 1;
          else
            track_id_counter[track_id] = 1;
        } // ~Loop over MC hits
      } // ~Loop over digits

      // Find the track_ids for this spacepoint
      std::map<int, int>::iterator mit1;
      std::vector<int> mc_tracks_ids;
      for ( mit1 = track_id_counter.begin(); mit1 != track_id_counter.end(); ++mit1 ) {
        mc_tracks_ids.push_back(mit1->first);
      }
      mc_tids.push_back(mc_tracks_ids);
    } // ~Loop over track points

    // Is there a track id associated with 3 or more track points?
    int track_id = 0;
    int counter = 0;
    bool success = SciFiAnalysisTools::find_mc_tid(mc_tids, 7, track_id, counter);
    // If we have not found a common track id, abort for this track
    if (!success) {
      std::cerr << "SciFiDataKF::Malformed track, skipping\n";
      break;
    }

    // Calc the MC track info using hits only with this track id
    ThreeVector mom;
    ThreeVector pos;
    SciFiCluster* clus = tp->get_cluster_pointer();
    success = SciFiAnalysisTools::find_mc_cluster_data(track_id, clus, lkup, mom, pos);
    if (!success) {
      std::cerr << "SciFiDataKF::Failed to find Kalman MC info with cluster at address "
    		    << clus <<"\n";
      break;
    }

/*
    SciFiDigit* dig = clus->get_digits_pointers().at(0);
    std::vector<SciFiHit*> hits;
    lkup.get_hits(dig, hits);
    mom = hits[0]->GetMomentum();
*/

    lDataStruct.xMc = pos.x();
    lDataStruct.yMc = pos.y();
    lDataStruct.zMc = pos.z();
    lDataStruct.PxMc = mom.x();
    lDataStruct.PyMc = mom.y();
    lDataStruct.PzMc = mom.z();
    lDataStruct.PtMc = std::sqrt(mom.x()*mom.x() + mom.y()*mom.y());

    mDataKF.push_back(lDataStruct);
  }  // ~ Loop over helical pattern recognition tracks in event
}

} // ~namespace MAUS
