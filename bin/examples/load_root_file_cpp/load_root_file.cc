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

#include <stdlib.h>

#include <string>

#include "TCanvas.h"
#include "TH1D.h"
#include "TFile.h"
#include "TTree.h"

#include "src/common_cpp/DataStructure/TOFEvent.hh"
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/DataStructure/Data.hh"
#include "src/common_cpp/JsonCppStreamer/IRStream.hh"

/** Perform a toy analysis using ROOT
 *
 *  Plot digits for TOF1 plane 0 and plane 1. See documentation in
 *  maus_user_guide "Accessing Data"
 */
int main() {
    char* mrd = getenv("MAUS_ROOT_DIR");
    std::string filename = mrd+std::string("/tmp/example_load_root_file.root");
    MAUS::Data data;

    // Make histograms; to fill later on
    TH1D tof1_digits_0_hist("tof1 digits_0",
                            "tof1 digits for plane 0;Slab number",
                            7, -0.5, 6.5);
    TH1D tof1_digits_1_hist("tof1 digits_1",
                            "tof1 digits for plane 1;Slab number",
                            7, -0.5, 6.5);

    // Use MAUS internal routines to generate a ROOT streamer. We are here
    // accessing the Spill tree which contains DAQ output data
    // Other trees are e.g. JobHeader (contains Job information), RunHeader, etc
    irstream infile(filename.c_str(), "Spill");
    // Iterate over all events
    while (infile >> readEvent != NULL) {
        infile >> branchName("data") >> data;
        MAUS::Spill* spill = data.GetSpill();
        // Iterate over all events; top level event in the "Spill" tree
        // corresponds to a daq_event; e.g. "start_of_burst" (spill start
        //  signal) or "physics_event" (spill data)
        if (spill != NULL && spill->GetDaqEventType() == "physics_event") {
            // Each recon event corresponds to a particle trigger; data in the
            // recon event should all have the same trigger
            for (size_t i = 0;  i < spill->GetReconEvents()->size(); ++i) {
                // TOF event holds TOF information for the particle trigger
                MAUS::TOFEvent* tof_event =
                                   (*spill->GetReconEvents())[i]->GetTOFEvent();
                MAUS::TOFEventDigit digits = tof_event->GetTOFEventDigit();
                // Iterate over TOF digits
                for (size_t j = 0; j < digits.GetTOF1DigitArray().size(); ++j) {
                    MAUS::TOFDigit tof1_digit = digits.GetTOF1DigitArray()[j];
                    // get the slab where the digit was registered
                    if (tof1_digit.GetPlane() == 0) {
                        tof1_digits_0_hist.Fill(tof1_digit.GetSlab());
                    } else {
                        tof1_digits_1_hist.Fill(tof1_digit.GetSlab());
                    }
                }
            }
        }
    }

    // Now plot the histograms
    TCanvas canvas_0("tof1_digits_0", "tof1_digits_0");
    tof1_digits_0_hist.Draw();
    canvas_0.Draw();
    canvas_0.Print("tof1_digits_0_load_root_file_cpp.png");
    TCanvas canvas_1("tof1_digits_1", "tof1_digits_1");
    tof1_digits_1_hist.Draw();
    canvas_1.Draw();
    canvas_1.Print("tof1_digits_1_load_root_file_cpp.png");
}
