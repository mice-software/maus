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

// ROOT headers
#include "TCanvas.h"
#include "TF1.h"
#include "TApplication.h"
#include "TGClient.h"

// MAUS headers
#include "src/common_cpp/Plotting/SciFi/TrackerData.hh"
#include "src/common_cpp/Plotting/SciFi/TrackerDataManager.hh"
#include "src/common_cpp/Plotting/SciFi/TrackerDataPlotterBase.hh"
#include "src/common_cpp/Plotting/SciFi/TrackerDataPlotterSpoints.hh"
#include "src/common_cpp/Plotting/SciFi/TrackerDataPlotterXYZ.hh"
#include "src/common_cpp/Plotting/SciFi/TrackerDataPlotterSZ.hh"
#include "src/common_cpp/Plotting/SciFi/TrackerDataPlotterInfoBox.hh"

#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/DataStructure/Data.hh"

#include "src/common_cpp/JsonCppStreamer/IRStream.hh"

/** Access Tracker data using ROOT */


int main(int argc, char *argv[]) {

  // First argument to code should be the input ROOT file name
  std::string filename = std::string(argv[1]);

  // Parse any extra arguments supplied
  //   -p -> pause between events
  //   -g -> enables saving xyz plots and gives output graphics file type
  bool bool_pause = false;
  bool bool_save = false;
  std::string save_type = "";

  for (int i = 2; i < argc; i++) {
    if ( std::strcmp(argv[i], "-p") == 0 ) {
      std::cout << "Will wait for user input between spills\n";
      bool_pause = true;
    } else if ( std::strcmp(argv[i], "-g") == 0 ) {
      if ( (i+1) < argc ) save_type = argv[i + 1];
      if ( (save_type == "eps") || (save_type == "pdf") || (save_type == "png") ) {
        std::cout << "Saving plots as " << save_type << " files.\n";
        bool_save = true;
      } else {
        std::cerr << "Invalid graphics output type given\n";
      }
    }
  }

  // Set up the data manager and plotters
  MAUS::TrackerDataManager tdm;
  tdm.set_print_tracks(true);
  tdm.set_print_seeds(true);
  MAUS::TrackerDataPlotterBase *xyzPlotter = new MAUS::TrackerDataPlotterXYZ();
  if (bool_save) {
    xyzPlotter->SetSaveOutput(true);
    xyzPlotter->SetOutputName("xyzPlotterOutput." + save_type);
  }
  MAUS::TrackerDataPlotterBase *szPlotter = new MAUS::TrackerDataPlotterSZ();
  MAUS::TrackerDataPlotterBase *infoBoxPlotter
                                  = new MAUS::TrackerDataPlotterInfoBox(300, 750, 0.585, 0.945);
  std::vector<MAUS::TrackerDataPlotterBase*> plotters;
  plotters.push_back(xyzPlotter);
  plotters.push_back(szPlotter);
  plotters.push_back(infoBoxPlotter);

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
      tdm.process(spill);
      tdm.draw(plotters);
      if (bool_pause) {
        std::cout << "Press Enter to Continue";
        std::cin.ignore();
      }
      tdm.clear_spill();
    } else {
      std::cout << "Not a usable spill\n";
    }
  }

  // Tidy up
  delete xyzPlotter;
  delete infoBoxPlotter;
  delete szPlotter;
  infile.close();
  theApp.Run();
}

