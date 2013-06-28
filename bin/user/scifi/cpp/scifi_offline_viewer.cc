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
#include <cmath>

// ROOT headers
#include "TCanvas.h"
#include "TF1.h"
#include "TApplication.h"
#include "TGClient.h"

// MAUS headers
#include "src/common_cpp/Recon/SciFi/TrackerData.hh"
#include "src/common_cpp/Recon/SciFi/TrackerDataManager.hh"
#include "src/common_cpp/Recon/SciFi/TrackerDataPlotterBase.hh"
#include "src/common_cpp/Recon/SciFi/TrackerDataPlotterSpoints.hh"
#include "src/common_cpp/Recon/SciFi/TrackerDataPlotterXYZ.hh"
#include "src/common_cpp/Recon/SciFi/TrackerDataPlotterInfoBox.hh"

#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/DataStructure/Data.hh"

#include "src/common_cpp/JsonCppStreamer/IRStream.hh"

/** Access Tracker data using ROOT */


int main(int argc, char *argv[]) {

  // First argument to code should be the input ROOT file name
  std::string filename = std::string(argv[1]);

  // Check if the user wants to pause between evts, indicated by making second argument to code "1"
  std::string str_pause;
  bool bool_pause = false;
  if (argc > 2) {
    std::string s1 = std::string(argv[2]);
    if (s1 == "1") bool_pause = true;
  }

  // Some set up
  TApplication theApp("App", &argc, argv);
  std::cout << "Opening file " << filename << std::endl;
  MAUS::Data data;
  irstream infile(filename.c_str(), "Spill");

  // Set up the data manager and plotters
  MAUS::TrackerDataManager tdm;
  MAUS::TrackerDataPlotterBase *xyzPlotter = new MAUS::TrackerDataPlotterXYZ();
  MAUS::TrackerDataPlotterBase *infoBoxPlotter
                                  = new MAUS::TrackerDataPlotterInfoBox(275, 600, 0.585, 0.93);
  std::vector<MAUS::TrackerDataPlotterBase*> plotters;
  plotters.push_back(xyzPlotter);
  plotters.push_back(infoBoxPlotter);

  // Loop over all events
  while (infile >> readEvent != NULL) {
    infile >> branchName("data") >> data;
    MAUS::Spill* spill = data.GetSpill();
    if (spill != NULL && spill->GetDaqEventType() == "physics_event") {
      tdm.process(spill);
      tdm.draw(plotters);
      if (bool_pause) {
        std::cout << "Press Enter to Continue";
        std::cin.ignore();
      }
    }
    tdm.clear_spill();
  }

  // Tidy up
  delete xyzPlotter;
  delete infoBoxPlotter;
  infile.close();
  theApp.Run();
}
