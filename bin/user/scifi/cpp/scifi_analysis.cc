/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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

// C / C++ headers
#include <stdlib.h>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <vector>
#include <map>
#include <algorithm>

// ROOT headers
#include "TCanvas.h"
#include "TF1.h"
#include "TApplication.h"
#include "TGClient.h"

#include "src/common_cpp/JsonCppStreamer/IRStream.hh"
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/DataStructure/Data.hh"
#include "src/common_cpp/DataStructure/ReconEvent.hh"
#include "src/common_cpp/DataStructure/MCEvent.hh"
#include "src/common_cpp/DataStructure/SciFiEvent.hh"
#include "src/common_cpp/DataStructure/SciFiHelicalPRTrack.hh"
#include "src/common_cpp/DataStructure/SciFiSpacePoint.hh"
#include "src/common_cpp/DataStructure/SciFiCluster.hh"
#include "src/common_cpp/DataStructure/SciFiDigit.hh"
#include "src/common_cpp/DataStructure/SciFiChannelId.hh"
#include "src/common_cpp/DataStructure/Hit.hh"
#include "src/common_cpp/DataStructure/Track.hh"
#include "src/common_cpp/DataStructure/ThreeVector.hh"
#include "src/common_cpp/Recon/SciFi/SciFiLookup.hh"

/** Access Tracker data using ROOT */

struct ReducedTrackData {
  int spill;
  int tracker;
  int n_points;
  int charge;
  int mc_track_id;
  int mc_pid;
  int n_matched;
  int n_mismatched;
  int n_missed;
};

struct ReducedTrackMomentum {
  double pt_mc;
  double pz_mc;
  double pt_rec;
  double pz_rec;
};

int find_mc_track_id(std::map<int, int> &track_id_counter) {
  std::map<int, int>::iterator mit1;
  int most_freq_track_id = 0;
  int counter = 0;
  for ( mit1 = track_id_counter.begin(); mit1 != track_id_counter.end(); ++mit1 ) {
    if ( (*mit1).second > counter ) {
      counter = (*mit1).second;
      most_freq_track_id = (*mit1).first;
    }
  }
  return most_freq_track_id;
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
      tid = -1;  // A malformed track, cannot asscoiate with an mc track id
      counter = 0;
      return false;
    }
  }
  return true;
};

bool find_mc_spoint_momentum(const int track_id, const MAUS::SciFiSpacePoint* sp,
                             MAUS::SciFiLookup &lkup, MAUS::ThreeVector &mom) {

  std::vector<MAUS::SciFiCluster*> clusters = sp->get_channels();
  std::vector<MAUS::SciFiCluster*>::iterator clus_it;
  for (clus_it = clusters.begin(); clus_it != clusters.end(); ++clus_it) {
    std::vector<MAUS::SciFiDigit*> digits = (*clus_it)->get_digits();
    std::vector<MAUS::SciFiDigit*>::iterator dig_it;
    for (dig_it = digits.begin(); dig_it != digits.end(); ++dig_it) {
      std::vector<MAUS::SciFiHit*> hits;
      if (!lkup.get_hits((*dig_it), hits)) {
        std::cerr << "Lookup failed\n";
        continue;
      }
      std::vector<MAUS::SciFiHit*>::iterator hit_it;
      for (hit_it = hits.begin(); hit_it != hits.end(); ++hit_it) {
        if (track_id == (*hit_it)->GetTrackId()) {
          mom = (*hit_it)->GetMomentum();
          return true;
        }
      }
    }
  }
  return false;
};

int main(int argc, char *argv[]) {
  // First argument to code should be the input ROOT file name
  std::string filename = std::string(argv[1]);

  // Set up ROOT app, input file, and MAUS data class
  TApplication theApp("App", &argc, argv);
  std::cout << "Opening file " << filename << std::endl;
  irstream infile(filename.c_str(), "Spill");
  MAUS::Data data;

  // Set up an output ROOT tree
  TTree t1("t1", "scifi analysis data");
  ReducedTrackData rtd1;
  ReducedTrackMomentum rtm1;
  std::string sData = "spill/I:tracker:n_points:charge:mc_track_id:mc_pid:n_matched:n_mismatched:";
  sData += "n_missed";
  t1.Branch("track_data", &rtd1.spill, sData.c_str());
  t1.Branch("track_momentum", &rtm1.pt_mc, "pt_mc/D:pz_mc:pt_rec:pz_rec");

  // Loop over all spills
  while ( infile >> readEvent != NULL ) {
    infile >> branchName("data") >> data;
    MAUS::Spill* spill = data.GetSpill();
    if (spill == NULL || spill->GetDaqEventType() != "physics_event") {
      std::cout << "Not a usable spill\n";
      continue;
    }

    std::cout << "Spill: " << spill->GetSpillNumber() << "\n";
    rtd1.spill = spill->GetSpillNumber();
    std::vector<MAUS::ReconEvent*>* revts = spill->GetReconEvents();
    std::vector<MAUS::MCEvent*>* mevts = spill->GetMCEvents();

    // Loop over MC events
    for ( size_t i = 0; i < mevts->size(); ++i ) {
      if ( !mevts->at(i) || !revts->at(i) ) {
        std::cerr << "Bad event, skipping\n";
        continue;
      }

      // Create the lookup providing a link between recon digits and MC hits
      MAUS::SciFiLookup lkup;
      lkup.make_hits_map(mevts->at(i));

      // Pull out the scifi event and helical pat rec tracks
      MAUS::SciFiEvent* sfevt = revts->at(i)->GetSciFiEvent();
      std::vector<MAUS::SciFiHelicalPRTrack*> htrks = sfevt->helicalprtracks();
      std::cout << " Helical PR tracks: " << htrks.size() << std::endl;

      // Loop over helical PR tracks
      for ( size_t j = 0; j < htrks.size(); ++j ) {
        rtd1.tracker = htrks[j]->get_tracker();
        rtd1.n_points = htrks[j]->get_num_points();
        rtd1.charge = htrks[j]->get_charge();
        std::vector< std::vector<int> > spoint_mc_tids;  // Vector of MC track ids for each spoint
        std::vector<MAUS::SciFiSpacePoint*> spnts = htrks[j]->get_spacepoints();

        // Loop over seed spacepoints
        for ( size_t k = 0; k < spnts.size(); ++k ) {
          std::map<int, int> track_id_counter;  // Map of track id to freq for each spoint
          std::vector<MAUS::SciFiCluster*> clusters = spnts[k]->get_channels();

          // Loop over clusters
          for ( size_t l = 0; l < clusters.size(); ++l ) {
            std::vector<MAUS::SciFiDigit*> digits = clusters[l]->get_digits();

            // Loop over digits
            for ( size_t m = 0; m < digits.size(); ++m ) {
              // Perform the digits to hits lookup
              std::vector<MAUS::SciFiHit*> hits;
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
        bool success = find_mc_tid(spoint_mc_tids, track_id, counter);
        // If we have not found a common track id, abort for this track
        if (!success) {
          std::cerr << "Malformed track, skipping\n";
          break;
        }
        // If we have found a common track id amoung the spoints, fill the tree
        rtd1.mc_track_id = track_id;
        rtd1.n_matched = counter;
        rtd1.n_mismatched = spoint_mc_tids.size() - counter;
        rtd1.n_missed = 5 - counter; // TODO: improve

        // Calc the MC track momentum using hits only with this track id
        rtm1.pt_mc = 0.0;
        rtm1.pz_mc = 0.0;
        rtm1.pt_rec = 0.0;
        rtm1.pz_rec = 0.0;
        int counter2 = 0;
        for ( size_t k = 0; k < spnts.size(); ++k ) {
          MAUS::ThreeVector mom_mc;
          bool success = find_mc_spoint_momentum(track_id, spnts[k], lkup, mom_mc);
          if (!success) continue;
          rtm1.pt_mc += sqrt(mom_mc.x()*mom_mc.x() + mom_mc.y()*mom_mc.y());
          rtm1.pz_mc += mom_mc.z();
          ++counter2;
        }
        rtm1.pt_mc /= counter2;
        rtm1.pz_mc /= counter2;
        // ... and now the recon track momentum
        rtm1.pt_rec = 1.2 * htrks[j]->get_R();
        rtm1.pz_rec = rtm1.pt_rec / htrks[j]->get_dsdz();

        // Fill the tree for this track
        t1.Fill();
      } // ~Loop over helical PR tracks
    } // ~Loop over MC events
  } // ~Loop over all spills

  // Save output data and tidy up
  infile.close();
  TFile of1("scifi_analysis_output.root", "RECREATE");
  of1.cd();
  t1.Write();
  of1.Close();
  theApp.Run();
}

