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
#include "TObject.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TApplication.h"
#include "TGClient.h"

#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/DataStructure/Data.hh"
#include "src/common_cpp/DataStructure/ReconEvent.hh"
#include "src/common_cpp/DataStructure/MCEvent.hh"
#include "src/common_cpp/DataStructure/SciFiEvent.hh"
#include "src/common_cpp/DataStructure/SciFiTrack.hh"
#include "src/common_cpp/DataStructure/SciFiSeed.hh"
#include "src/common_cpp/DataStructure/SciFiTrackPoint.hh"
#include "src/common_cpp/DataStructure/SciFiBasePRTrack.hh"
#include "src/common_cpp/DataStructure/SciFiHelicalPRTrack.hh"
#include "src/common_cpp/DataStructure/SciFiSpacePoint.hh"
#include "src/common_cpp/DataStructure/SciFiCluster.hh"
#include "src/common_cpp/DataStructure/SciFiDigit.hh"
#include "src/common_cpp/DataStructure/SciFiChannelId.hh"
#include "src/common_cpp/DataStructure/Hit.hh"
#include "src/common_cpp/DataStructure/Track.hh"
#include "src/common_cpp/DataStructure/ThreeVector.hh"
#include "src/common_cpp/Recon/SciFi/SciFiLookup.hh"
#include "src/common_cpp/Recon/SciFi/SciFiTools.hh"

/** Access Tracker data using ROOT */

MAUS::SciFiSeed* ExtractSeed(MAUS::SciFiTrack* aTrack) {
  MAUS::SciFiSeed* seed = aTrack->scifi_seed();
  if (!seed) {
    // std::cerr << "Empty seed pointer at: " << seed << std::endl;
  }
  TObject* seed_obj = aTrack->scifi_seed_tobject();
  if (!seed_obj) {
    // std::cerr << "Empty seed TObject pointer" << std::endl;
  }
  if (!seed && !seed_obj) {
    // std::cerr << "No seed pointer at all" << std::endl;
    return NULL;
  }
  if (!seed && seed_obj) {
    seed = dynamic_cast<MAUS::SciFiSeed*>(seed_obj); // NOLINT(runtime/rtti)
    if (!seed) {
      std::cerr << "Dynamic cast from SciFiSeed TObject failed" << std::endl;
      return NULL;
    }
  }
  return seed;
}

int main(int argc, char *argv[]) {
  // First argument to code should be the input ROOT file name
  std::string filename = std::string(argv[1]);

  // Set up ROOT app, input file, and MAUS data class
  // TApplication theApp("App", &argc, argv);
  std::cout << "Opening file " << filename << std::endl;
  TFile f1(filename.c_str());
  TTree* T = static_cast<TTree*>(f1.Get("Spill"));
  MAUS::Data* data = NULL;  // Don't forget = NULL or you get a seg fault
  T->SetBranchAddress("data", &data); // Yes, this is the *address* of a *pointer*
  int nentries = T->GetEntries();
  std::cerr << "Found " << nentries << " spills\n";

  // Loop over all spills
  for (int i = 0; i < nentries; ++i) {
    T->GetEntry(i);
    if (!data) {
      std::cout << "Data is NULL\n";
      continue;
    }
    MAUS::Spill* spill = data->GetSpill();
    if (spill == NULL) {
      std::cout << "Spill is NULL\n";
      continue;
    }
    if (spill->GetDaqEventType() != "physics_event") {
      std::cout << "Spill is of type " << spill->GetDaqEventType() << ", not a usable spill\n";
      continue;
    }

    std::cout << "Spill: " << spill->GetSpillNumber() << "\n";
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

      // Pull out the scifi event
      MAUS::SciFiEvent* sfevt = revts->at(i)->GetSciFiEvent();

      // Perform your analysis here, e.g.

      // Loop over pattern recognition helical tracks
      std::vector<MAUS::SciFiHelicalPRTrack*> htrks = sfevt->helicalprtracks();
      std::vector<MAUS::SciFiHelicalPRTrack*>::iterator htrk;
      for ( htrk = htrks.begin(); htrk != htrks.end(); ++htrk ) {
        std::vector<MAUS::SciFiSpacePoint*> spnts = (*htrk)->get_spacepoints_pointers();
        std::vector<MAUS::SciFiSpacePoint*>::iterator spnt;
        // Loop over associated spacepoints
        for ( spnt = spnts.begin(); spnt != spnts.end(); ++spnt ) {
          // Loop over associated clusters
          std::vector<MAUS::SciFiCluster*> clusters = (*spnt)->get_channels_pointers();
          std::vector<MAUS::SciFiCluster*>::iterator clus;
          for ( clus = clusters.begin(); clus != clusters.end(); ++clus ) {
            // Loop over associated digits
            std::vector<MAUS::SciFiDigit*> digits = (*clus)->get_digits_pointers();
            std::vector<MAUS::SciFiDigit*>::iterator dig;
            for ( dig = digits.begin(); dig != digits.end(); ++dig ) {
              // Perform the digits to hits lookup
              std::vector<MAUS::SciFiHit*> hits;
              std::vector<MAUS::SciFiHit*>::iterator hit;
              if (!lkup.get_hits((*dig), hits)) {
                std::cerr << "Lookup failed\n";
                continue;
              }
              // Loop over associated MC hits
              for ( hit = hits.begin(); hit != hits.end(); ++hit ) {
                // Print the MC track ID
                int track_id = (*hit)->GetTrackId();
                // std::cout << "track_id: " << track_id << std::endl;
              } // ~Loop over associated MC hits
            } // ~Loop over associated digits
          } // ~Loop over associated clusters
        } // ~Loop over associated spacepoints
      } // Loop over pattern recognition helical tracks

      // Loop over kalman tracks
      std::vector<MAUS::SciFiTrack*> trks = sfevt->scifitracks();
      for (auto trk : sfevt->scifitracks())  {
        std::cout << "  SciFi Track chi_sq: " << trk->chi2() << std::endl;

        // Access the SciFiSeed associate with this track
        MAUS::SciFiSeed* seed = ExtractSeed(trk);
        if (!seed) {
          std::cerr << "Failed to extract SciFiSeed\n";
          continue;
        }

        // Access the PR track from the SciFiSeed in order to get at the spacepoints and clusters
        TObject* pr_track_obj = seed->getPRTrackTobject();
        if (!pr_track_obj) {
          std::cerr << "Empty PR track TObject pointer, skipping track" << std::endl;
          continue;
        }
        MAUS::SciFiBasePRTrack* prtrk = static_cast<MAUS::SciFiBasePRTrack*>(pr_track_obj);
        auto spnts = prtrk->get_spacepoints_pointers();

        // Loop over associated trackpoints
        for (auto tpnt : trk->scifitrackpoints()) {
          std::cout << "    SciFi TrackPoint chi_sq: " << tpnt->chi2() << std::endl;

          // Pull out the associated cluster
          MAUS::SciFiCluster* clus = nullptr;
          for (auto sp : spnts) {
            if (sp->get_station() == tpnt->station()) {
              for (auto lclus : sp->get_channels_pointers()) {
                if (clus->get_plane() == tpnt->plane()) {
                  clus = lclus;
                }
              }
            }
          }
          if (!clus) {
            std::cerr << "Failed to find associated cluster\n";
            continue;
          }

          // Loop over associated digits
          std::vector<MAUS::SciFiDigit*> digits = clus->get_digits_pointers();
          std::vector<MAUS::SciFiDigit*>::iterator dig;
          for ( dig = digits.begin(); dig != digits.end(); ++dig ) {
            // Perform the digits to hits lookup
            std::vector<MAUS::SciFiHit*> hits;
            std::vector<MAUS::SciFiHit*>::iterator hit;
            if (!lkup.get_hits((*dig), hits)) {
              std::cerr << "Lookup failed\n";
              continue;
            }
            // Loop over associated MC hits
            for ( hit = hits.begin(); hit != hits.end(); ++hit ) {
              int track_id = (*hit)->GetTrackId();
              // Print the MC track ID
              std::cout << "track_id: " << track_id << std::endl;
            } // ~Loop over MC hits
          } // ~Loop over digits
        } // ~Loop over track points
      } // ~Loop over kalman tracks
    } // ~Loop over MC events
  } // ~Loop over all spills
  // theApp.Run();
}
