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
#include "src/common_cpp/Analysis/SciFi/SciFiDataMomentum.hh"
#include "src/common_cpp/Plotting/SciFi/SciFiAnalysisTools.hh"
#include "src/common_cpp/Recon/SciFi/SciFiLookup.hh"
#include "src/common_cpp/DataStructure/Data.hh"
#include "src/common_cpp/DataStructure/MCEvent.hh"
#include "src/common_cpp/DataStructure/SciFiEvent.hh"
#include "src/common_cpp/DataStructure/SciFiHelicalPRTrack.hh"
#include "src/common_cpp/DataStructure/SciFiSpacePoint.hh"
#include "src/common_cpp/DataStructure/SciFiCluster.hh"
#include "src/common_cpp/DataStructure/SciFiDigit.hh"
#include "src/common_cpp/DataStructure/Hit.hh"
#include "src/common_cpp/DataStructure/ThreeVector.hh"

namespace MAUS {

SciFiDataMomentum::SciFiDataMomentum() : mTree(NULL) {
  mTree = new TTree("mTree", "SciFi Momentum Data");
  mTree->Branch("SpillNumber", &mDataStruct.SpillNumber, "SpillNumber/I");
  mTree->Branch("TrackerNumber", &mDataStruct.TrackerNumber, "TrackerNumber/I");
  mTree->Branch("NumberOfPoints", &mDataStruct.NumberOfPoints, "NumberOfPoints/I");
  mTree->Branch("Charge", &mDataStruct.Charge, "Charge/I");
  mTree->Branch("PtMc", &mDataStruct.PtMc, "PtMc/D");
  mTree->Branch("PzMc", &mDataStruct.PzMc, "PtMz/D");
  mTree->Branch("PtRec", &mDataStruct.PtRec, "PtRec/D");
  mTree->Branch("PzRec", &mDataStruct.PzRec, "PtRec/D");
}

SciFiDataMomentum::~SciFiDataMomentum() {
  delete mTree;
}

bool SciFiDataMomentum::Accumulate(Spill* aSpill) {
  if (aSpill != NULL && aSpill->GetDaqEventType() == "physics_event") {
    mDataStruct.SpillNumber = aSpill->GetSpillNumber();

    // Loop over MC events in the aSpill
    for (size_t iMaSFEvent = 0; iMaSFEvent < aSpill->GetMCEvents()->size(); ++iMaSFEvent) {
      MCEvent *aMcEvent = (*aSpill->GetMCEvents())[iMaSFEvent];
      SciFiEvent *aSFEvent = (*aSpill->GetReconEvents())[iMaSFEvent]->GetSciFiEvent();
      ReduceData(aMcEvent, aSFEvent);
    }
  } else {
    std::cerr << "SciFiDataMomentum: Not a usable spill" << std::endl;
    return false;
  }
  return true;
}


void SciFiDataMomentum::ReduceData(MCEvent *aMcEvent, SciFiEvent* aSFEvent) {
  std::vector<SciFiHelicalPRTrack*> htrks = aSFEvent->helicalprtracks();

  // Create the lookup bridge between MC and Recon
  SciFiLookup lkup;
  lkup.make_hits_map(aMcEvent);

  // Reset tracks counters
  mNBadTracks = 0;

  // Loop over helical pattern recognition tracks in event
  for (size_t iTrk = 0; iTrk < htrks.size(); ++iTrk) {
    SciFiHelicalPRTrack* trk = htrks[iTrk];
    mDataStruct.PtRec = 0.0;
    mDataStruct.PzRec = 0.0;
    if ((trk->get_R() != 0.0) & (trk->get_dsdz() != 0.0)) {
      mDataStruct.NumberOfPoints = trk->get_num_points();
      std::vector< std::vector<int> > spoint_mc_tids;  // Vector of MC track ids for each spoint

      // Calc recon momentum
      mDataStruct.PtRec = 1.2 * trk->get_R();
      mDataStruct.PzRec = mDataStruct.PtRec / trk->get_dsdz();

      // Calc MC momentum
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
        ++mNBadTracks;
        break;
      }
      // If we have found a common track id amoung the spoints, fill the tree
      mMcTrackId = track_id;
      mNMatchedTracks = counter;
      mNMismatchedTracks = spoint_mc_tids.size() - counter;
      mNMissedTracks = 5 - counter; // TODO: improve

      // Calc the MC track momentum using hits only with this track id
      mDataStruct.PtMc = 0.0;
      mDataStruct.PzMc = 0.0;
      int counter2 = 0;
      for ( size_t k = 0; k < spnts.size(); ++k ) {
        ThreeVector mom_mc;
        bool success = SciFiAnalysisTools::find_mc_spoint_momentum(track_id, spnts[k],
                                                                   lkup, mom_mc);
        if (!success) continue;
        mDataStruct.PtMc += sqrt(mom_mc.x()*mom_mc.x() + mom_mc.y()*mom_mc.y());
        mDataStruct.PzMc += mom_mc.z();
        ++counter2;
      }
      mDataStruct.PtMc /= counter2;
      mDataStruct.PzMc /= counter2;

      mDataStruct.TrackerNumber = trk->get_tracker();
      mDataStruct.Charge = trk->get_charge();
      mTree->Fill();
    } else {
      std::cout << "Bad track, skipping" << std::endl;
    }
  }
}
}
