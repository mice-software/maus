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

/* Analyze the output root file from reconstruction
 * and make plots or fill ntuples and write them to a root file
 */

#include <TFile.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TTree.h>
#include <TH1F.h>
#include <TH2F.h>
#include <iostream>
#include <vector>
#include "src/common_cpp/DataStructure/MAUSEvent.hh"
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/DataStructure/Data.hh"
#include "src/common_cpp/DataStructure/TOFEvent.hh"
#include "src/common_cpp/DataStructure/ReconEvent.hh"

char in_file[256], out_file[256];

void ana_mc(char* ifname, char* ofname) {

  TFile f(ifname);
  // setup tree, branches, pointers
  TTree *t1 = reinterpret_cast<TTree*>(f.Get("Spill"));
  MAUS::Data* mdata = new MAUS::Data();
  // set branch address
  t1->SetBranchAddress("data", &mdata);

  // maus classes
  // std::vector<VirtualHit>* vhitar;
  // std::vector<SpecialVirtualHit>* svhitar;
  // std::vector<TOFHit>* tofhitar;
  MAUS::ThreeVector hpos;

  int nmc;
  int ntof0_sp, ntof1_sp, ntof2_sp;
  TFile* fout = new TFile(ofname, "recreate");
  TH1F* tof01 = new TH1F("tof01", ";TOF0-1 time;", 120, 28., 36.);
  TH1F* tof12 = new TH1F("tof12", ";TOF1-2 time;", 600, 25., 60.);
  TH1F* tof01_2 = new TH1F("tof01_2", ";TOF0-1 time;", 65, 28., 32.);

  // get the number of entries in the tree
  int nspills = t1->GetEntries();
  // loop over spills
  int num_evt = 0;
  for (int n = 0; n < nspills; ++n) {
    // get entry
    t1->GetEntry(n);
    // load the spill
    MAUS::Spill* spill;
    spill = mdata->GetSpill();
    // skip if not a physics spill
    if (spill->GetDaqEventType() != "physics_event") continue;
    // get num events in this spill
    // num data events
    int nevts = spill->GetReconEventSize();
    // num mc events (should be same as reconstructed)
    nmc = spill->GetMCEventSize();

    // loop over events in the spill
    for (int evt = 0; evt < nevts; ++evt) {
      num_evt++;
      // get the tof event
      MAUS::ReconEvent revt = spill->GetAReconEvent(evt);
      MAUS::TOFEvent* tof_event = revt.GetTOFEvent();
      // get an MC event
      MAUS::MCEvent mevt = spill->GetAnMCEvent(evt);

      // get tof space points for this event
      MAUS::TOFEventSpacePoint tof_sp = tof_event->GetTOFEventSpacePoint();

      // # tof0 space points
      MAUS::TOF0SpacePointArray tof0_spar = tof_sp.GetTOF0SpacePointArray();
      ntof0_sp = tof0_spar.size();

      // # tof1 space points
      MAUS::TOF1SpacePointArray tof1_spar = tof_sp.GetTOF1SpacePointArray();
      ntof1_sp = tof1_spar.size();

      MAUS::TOF2SpacePointArray tof2_spar = tof_sp.GetTOF2SpacePointArray();
      ntof2_sp = tof2_spar.size();

      // get tof1-tof0 flight time
      // only select events with 1 spnt in tof1 and tof0
      if (tof0_spar.size() == 1 && tof1_spar.size() == 1) {
        MAUS::TOFSpacePoint sp0 = tof_sp.GetTOF0SpacePointArrayElement(0);
        MAUS::TOFSpacePoint sp1 = tof_sp.GetTOF1SpacePointArrayElement(0);
        tof01->Fill(sp1.GetTime()-sp0.GetTime());
        tof01_2->Fill(sp1.GetTime()-sp0.GetTime());
      }
      if (tof1_spar.size() == 1 && tof2_spar.size() == 1) {
        MAUS::TOFSpacePoint sp2 = tof_sp.GetTOF2SpacePointArrayElement(0);
        MAUS::TOFSpacePoint sp1 = tof_sp.GetTOF1SpacePointArrayElement(0);
        tof12->Fill(sp2.GetTime()-sp1.GetTime());
      }
    } // end loop over events
  } // end loop over spills
  fout->Write();
}

#ifndef __CINT__
int main(int argc, char *argv[]) {
  if (argc < 2) {
      std::cout << "Usage: " << argv[0];
      std::cout << " input-root-file  output-root-file" << std::endl;
      exit(1);
  }
  snprintf(in_file, sizeof(in_file), argv[1]);
  // strcpy(in_file, argv[1]);
  snprintf(out_file, sizeof(out_file), argv[2]);
  // strcpy(out_file, argv[2]);
  ana_mc(in_file, out_file);
  return 0;
}
#endif
