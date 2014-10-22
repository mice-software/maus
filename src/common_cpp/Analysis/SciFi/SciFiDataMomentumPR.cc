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

// MAUS headers
#include "src/common_cpp/Analysis/SciFi/SciFiDataMomentumPR.hh"
#include "src/common_cpp/Plotting/SciFi/SciFiAnalysisTools.hh"
#include "src/common_cpp/Recon/SciFi/SciFiLookup.hh"
#include "src/common_cpp/DataStructure/MCEvent.hh"
#include "src/common_cpp/DataStructure/SciFiEvent.hh"
#include "src/common_cpp/DataStructure/SciFiHelicalPRTrack.hh"
#include "src/common_cpp/DataStructure/SciFiSpacePoint.hh"
#include "src/common_cpp/DataStructure/SciFiCluster.hh"
#include "src/common_cpp/DataStructure/SciFiDigit.hh"
#include "src/common_cpp/DataStructure/Hit.hh"
#include "src/common_cpp/DataStructure/ThreeVector.hh"

namespace MAUS {

SciFiDataMomentumPR::SciFiDataMomentumPR() : mNUnmatchedTracks(0),
                                             mNMatchedTracks(0) {
  // Do nothing
}

SciFiDataMomentumPR::~SciFiDataMomentumPR() {
  // Do nothing
}

void SciFiDataMomentumPR::Clear() {
  mNUnmatchedTracks = 0;
  mNMatchedTracks = 0;
  mData.clear();
}

void SciFiDataMomentumPR::ReduceData(MCEvent *aMcEvent, SciFiEvent* aSFEvent) {
  std::vector<SciFiHelicalPRTrack*> htrks = aSFEvent->helicalprtracks();

  // Create the lookup bridge between MC and Recon
  SciFiLookup lkup;
  lkup.make_hits_map(aMcEvent);

  // Reset tracks counters
  mNUnmatchedTracks = 0;

  // Loop over helical pattern recognition tracks in event
  for (size_t iTrk = 0; iTrk < htrks.size(); ++iTrk) {
    SciFiHelicalPRTrack* trk = htrks[iTrk];

    if ((trk->get_R() != 0.0) & (trk->get_dsdz() != 0.0)) {
      MomentumData lDataStruct;
      lDataStruct.TrackerNumber = trk->get_tracker();
      lDataStruct.Charge = trk->get_charge();
      lDataStruct.NumberOfPoints = trk->get_num_points();

      // Calc recon momentum
      lDataStruct.PtRec = 1.2 * trk->get_R();
      lDataStruct.PzRec = lDataStruct.PtRec / trk->get_dsdz();

      // Calc MC momentum
      std::vector< std::vector<int> > spoint_mc_tids;  // Vector of MC track ids for each spoint
      // Loop over seed spacepoints associated with the track
      std::vector<SciFiSpacePoint*> spnts = trk->get_spacepoints_pointers();
      for ( size_t k = 0; k < spnts.size(); ++k ) {
        std::map<int, int> track_id_counter;  // Map of track id to freq for each spoint
        std::vector<SciFiCluster*> clusters = spnts[k]->get_channels_pointers();

        // Loop over clusters
        for ( size_t l = 0; l < clusters.size(); ++l ) {
          std::vector<SciFiDigit*> digits = clusters[l]->get_digits_pointers();

          // Loop over digits
          for ( size_t m = 0; m < digits.size(); ++m ) {
            // Perform the digits to hits lookup
            std::vector<SciFiHit*> hits;
            if (!lkup.get_hits(digits[m], hits)) {
              std::cerr << "Lookup failed\n";
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
        } // ~// Loop over clusters

        // Find the track_ids for this spacepoint
        std::map<int, int>::iterator mit1;
        std::vector<int> mc_tracks_ids;
        for ( mit1 = track_id_counter.begin(); mit1 != track_id_counter.end(); ++mit1 ) {
          mc_tracks_ids.push_back(mit1->first);
        }
        spoint_mc_tids.push_back(mc_tracks_ids);
      } // ~Loop over seed spacepoints

      // Is there a track id associated with 3 or more spoints?
      int track_id = 0;
      int counter = 0;
      bool success = SciFiAnalysisTools::find_mc_tid(spoint_mc_tids, track_id, counter);
      // If we have not found a common track id, abort for this track
      if (!success) {
        std::cerr << "Malformed track, skipping\n";
        ++mNUnmatchedTracks;
        break;
      }
      ++mNMatchedTracks;

      // Calc the MC track momentum using hits only with this track id
      lDataStruct.PtMc = 0.0;
      lDataStruct.PzMc = 0.0;
      int counter2 = 0;
      for ( size_t k = 0; k < spnts.size(); ++k ) {
        ThreeVector mom_mc;
        bool success = SciFiAnalysisTools::find_mc_spoint_momentum(track_id, spnts[k],
                                                                   lkup, mom_mc);
        if (!success) {
          std::cerr << "Failed to find MC mom for track in tracker " << trk->get_tracker() << "\n";
          continue;
        }
        lDataStruct.PtMc += sqrt(mom_mc.x()*mom_mc.x() + mom_mc.y()*mom_mc.y());
        lDataStruct.PzMc += mom_mc.z();
        ++counter2;
      }
      lDataStruct.PtMc /= counter2;
      lDataStruct.PzMc /= counter2;

      mData.push_back(lDataStruct);
    } else {
      std::cout << "Bad track, skipping" << std::endl;
    }
  }  // ~ Loop over helical pattern recognition tracks in event
}

} // ~namespace MAUS
