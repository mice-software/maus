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

int main() {
    char* mrd = getenv("MAUS_ROOT_DIR");
    std::string filename = mrd+std::string("/tmp/example_load_root_file.root");
		MAUS::Data data;

    TH1D tof1_digits_0_hist("tof1 digits_0",
														"tof1 digits for plane 0;Slab number",
                            7, -0.5, 6.5);
    TH1D tof1_digits_1_hist("tof1 digits_1",
                            "tof1 digits for plane 1;Slab number",
                            7, -0.5, 6.5);


	  irstream infile(filename.c_str(), "Spill");
		while (infile >> readEvent != NULL) {
		    infile >> branchName("data") >> data;
				MAUS::Spill* spill = data.GetSpill();
				if (spill != NULL && spill->GetDaqEventType() == "physics_event") {
            for (size_t i = 0;  i < spill->GetReconEvents()->size(); ++i) {
                MAUS::TOFEvent* tof_event = (*spill->GetReconEvents())[i]->GetTOFEvent();
                MAUS::TOFEventDigit digits = tof_event->GetTOFEventDigit();
                for (size_t j = 0; j < digits.GetTOF1DigitArray().size(); ++j) {
                    MAUS::TOFDigit tof1_digit = digits.GetTOF1DigitArray()[j];	
                    if (tof1_digit.GetPlane() == 0) {
                        tof1_digits_0_hist.Fill(tof1_digit.GetSlab());
                    } else {
                        tof1_digits_1_hist.Fill(tof1_digit.GetSlab());
										}
								}
						}
				}
		}		

    TCanvas canvas_0("tof1_digits_0", "tof1_digits_0");
    tof1_digits_0_hist.Draw();
    canvas_0.Draw();
    canvas_0.Print("tof1_digits_0_load_root_file_cpp.png");
    TCanvas canvas_1("tof1_digits_1", "tof1_digits_1");
    tof1_digits_1_hist.Draw();
    canvas_1.Draw();
    canvas_1.Print("tof1_digits_1_load_root_file_cpp.png");
}
