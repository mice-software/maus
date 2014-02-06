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
#include "src/common_cpp/Recon/SciFi/SciFiLookup.hh"

/** Access Tracker data using ROOT */


int main(int argc, char *argv[]) {
  // First argument to code should be the input ROOT file name
  std::string filename = std::string(argv[1]);

  // Set up ROOT app, input file, and MAUS data class
  TApplication theApp("App", &argc, argv);
  std::cout << "Opening file " << filename << std::endl;
  irstream infile(filename.c_str(), "Spill");
  MAUS::Data data;

  // Loop over all spills
  while ( infile >> readEvent != NULL ) {
    infile >> branchName("data") >> data;
    MAUS::Spill* spill = data.GetSpill();
    if (spill != NULL && spill->GetDaqEventType() == "physics_event") {
      // Analysis code
      std::cout << "Spill: " << spill->GetSpillNumber() << "\n";
      std::vector<MAUS::ReconEvent*>* revts = spill->GetReconEvents();
      std::vector<MAUS::MCEvent*>* mevts = spill->GetMCEvents();
      for ( size_t i = 0; i < mevts->size(); ++i ) {
        MAUS::SciFiLookup lkup;
        lkup.make_hits_map(mevts->at(i));
        MAUS::SciFiEvent* sfevt = revts->at(i)->GetSciFiEvent();
        std::vector<MAUS::SciFiHelicalPRTrack*> htrks = sfevt->helicalprtracks();
        std::cout << " Helical PR tracks: " << htrks.size() << std::endl;
        for ( size_t j = 0; j < htrks.size(); ++j ) {
          std::vector<MAUS::SciFiSpacePoint*> spnts = htrks[j]->get_spacepoints();
          std::cout << "  Seed spacepoiints: " << spnts.size() << std::endl;
          for ( size_t k = 0; k < spnts.size(); ++k ) {
            std::vector<MAUS::SciFiCluster*> clusters = spnts[k]->get_channels();
            std::cout << "   Clusters: " << clusters.size() << std::endl;
            for ( size_t l = 0; l < clusters.size(); ++l ) {
              std::vector<MAUS::SciFiDigit*> digits = clusters[l]->get_digits();
              std::cout << "    Digits: " << digits.size() << std::endl;
              for ( size_t m = 0; m < digits.size(); ++m ) {
                std::vector<MAUS::SciFiHit*> hits;
                bool success = lkup.get_hits(digits[m], hits);
                if (!success) {
                  std::cerr << "     Lookup failed\n";
                  continue;
                }
                std::cout << "     Hits: " << hits.size() << "; ";
                for ( size_t n = 0; n < hits.size(); ++n ) {
                  std::cout << hits[n]->GetTrackId()
                            << " " << hits[n]->GetParticleId() << "; ";
                }
                std::cout << std::endl;
              }
            }
          }
        }
      }
    } else {
      std::cout << "Not a usable spill\n";
    }
  }

  // Tidy up
  infile.close();
  theApp.Run();
}

