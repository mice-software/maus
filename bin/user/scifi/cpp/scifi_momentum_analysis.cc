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
#include "TCut.h"
#include "TPaveText.h"

// MAUS headers
#include "src/common_cpp/Plotting/SciFi/TrackerDataAnalyserMomentum.hh"
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/DataStructure/Data.hh"
#include "src/common_cpp/JsonCppStreamer/IRStream.hh"

/** Analyse Tracker momentum data using ROOT */


int main(int argc, char *argv[]) {

  // First argument to code should be the input ROOT file name
  std::string filename = std::string(argv[1]);

  // Analysis parameters - some may be overidden with command line arguments. All momenta in MeV/c.
  int n_pt_bins = 0;           // No. of bins in histos used to find pt resols (0 = let ROOT decide)
  int n_pz_bins = 0;           // No. of bins in histos used to find pz resols (0 = let ROOT decide)
  int n_points = 9;            // No. of data points in each resolution plot
  double pt_fit_min = -10.0;   // Lower bound of the gaussian fit to histos used to find pt resols
  double pt_fit_max = 10.0;    // Upper bound of the gaussian fit to histos used to find pt resols
  double pz_fit_min = -50.0;   // Lower bound of the gaussian fit to histos used to find pz resols
  double pz_fit_max = 50.0;    // Upper bound of the gaussian fit to histos used to find pz resols
  double lower_bound = 0;      // Lower bound on pt_mc of resolution plots
  double upper_bound = 90;     // Upper bound on pt_mc of resolution plots
  double pz_rec_cut = 500.0;   // Cut on reconstruction pz applied to histos used to find pz resols

  // Parse any extra arguments supplied from the command line
  //   -p -> pause between events
  //   -g -> enables saving xyz plots and gives output graphics file type
  bool bool_pause = false;
  // bool bool_save = false;
  std::string save_type = "";

  for (int i = 2; i < argc; i++) {
    if ( std::strcmp(argv[i], "-p") == 0 ) {
      std::cout << "Will wait for user input between spills\n";
      bool_pause = true;
    } else if ( std::strcmp(argv[i], "-g") == 0 ) {
      if ( (i+1) < argc ) save_type = argv[i + 1];
      if ( (save_type == "eps") || (save_type == "pdf") || (save_type == "png") ) {
        std::cout << "Saving plots as " << save_type << " files.\n";
        // bool_save = true;
      } else if ( save_type != "" ) {
        std::cerr << "Invalid graphics output type given\n";
      }
    } else if ( std::strcmp(argv[i], "-n_pt_bins") == 0 ) {
      if ( (i+1) < argc ) {
        std::stringstream ss1(argv[i + 1]);
        ss1 >> n_pt_bins;
      }
    } else if ( std::strcmp(argv[i], "-n_pz_bins") == 0 ) {
        if ( (i+1) < argc ) {
          std::stringstream ss1(argv[i + 1]);
          ss1 >> n_pz_bins;
        }
    }
  }

  // Set up analysis class
  MAUS::TrackerDataAnalyserMomentum analyser;
  analyser.set_n_pt_bins(n_pt_bins);
  analyser.set_n_pz_bins(n_pz_bins);
  analyser.set_n_points(n_points);
  analyser.set_pt_fit_min(pt_fit_min);
  analyser.set_pt_fit_max(pt_fit_max);
  analyser.set_pz_fit_min(pz_fit_min);
  analyser.set_pz_fit_max(pz_fit_max);
  analyser.set_resol_lower_bound(lower_bound);
  analyser.set_resol_upper_bound(upper_bound);
  analyser.set_cut_pz_rec(pz_rec_cut);
  std::cout << "Pt resolution histogram number of bins: " << analyser.get_n_pt_bins() << "\n";
  std::cout << "Pt resolution histogram fit lower bound: " << analyser.get_pt_fit_min() << "\n";
  std::cout << "Pt resolution histogram fit upper bound: " << analyser.get_pt_fit_max() << "\n";
  std::cout << "Pz resolution histogram number of bins: " << analyser.get_n_pz_bins() << "\n";
  std::cout << "Pz resolution histogram fit lower bound: " << analyser.get_pz_fit_min() << "\n";
  std::cout << "Pz resolution histogram fit upper bound: " << analyser.get_pz_fit_max() << "\n";
  std::cout << "Resolution graphs number of points:  " << analyser.get_n_points() << "\n";
  std::cout << "Resolution graphs lower bound:  " << analyser.get_resol_lower_bound() << " MeV/c\n";
  std::cout << "Resolution graphs upper bound:  " << analyser.get_resol_upper_bound() << " MeV/c\n";
  std::cout << "Pz rec cut: " << analyser.get_cut_pz_rec() << " MeV/c\n";
  analyser.setUp();

  // Set up ROOT app, input file, and MAUS data class
  TApplication theApp("App", &argc, argv);
  std::cout << "Opening file " << filename << std::endl;
  irstream infile(filename.c_str(), "Spill");
  MAUS::Data data;

  // Loop over all spills
  int counter = 0;
  while ( infile >> readEvent != NULL ) {
    ++counter;
    infile >> branchName("data") >> data;
    MAUS::Spill* spill = data.GetSpill();
    if (spill != NULL && spill->GetDaqEventType() == "physics_event") {
      if ( fmod(counter, 1000) == 0 ) std::cout << "Total number of spills: " << counter << ".\n";
      analyser.accumulate(spill);
      if (bool_pause) {
        std::cout << "Press Enter to Continue";
        std::cin.ignore();
      }
    } else {
      std::cout << "Not a usable spill\n";
    }
  }

  // Make the final plots
  analyser.make_residual_histograms();
  analyser.make_residual_graphs();
  analyser.make_pt_resolutions();
  analyser.make_pz_resolutions();
  analyser.make_resolution_graphs();
  if (save_type != "") analyser.save_graphics(save_type);

  // Print some results
  double efficiency = static_cast<double>(analyser.get_n_rec_tracks_matched())
                      / static_cast<double>(analyser.get_n_mc_tracks_valid());
  double purity = static_cast<double>(analyser.get_n_rec_tracks_matched())
                  / static_cast<double>(analyser.get_n_rec_tracks_total());

  std::cerr << "\n-------------------------Results-------------------------\n";
  std::cerr << "n_mc_tracks_valid: " << analyser.get_n_mc_tracks_valid() << std::endl;
  std::cerr << "n_rec_tracks_invalid: " << analyser.get_n_mc_tracks_invalid() << std::endl;
  std::cerr << "n_rec_tracks_matched: " << analyser.get_n_rec_tracks_matched() << std::endl;
  std::cerr << "n_rec_tracks_unmatched: " << analyser.get_n_rec_tracks_unmatched() << std::endl;
  std::cerr << "n_rec_tracks_total: " << analyser.get_n_rec_tracks_total() << std::endl;
  std::cerr << "=> Efficiency: " << efficiency << std::endl;
  std::cerr << "=> Purity: " << purity << std::endl;

  TCanvas c1("c1", "Info", 300, 200);
  TPaveText tpt1(.05, .1, .95, .8);
  tpt1.SetFillColor(kWhite);
  tpt1.AddText("Run Results");
  tpt1.AddLine(.0, .66, 1., .66);
  std::stringstream ss1;
  ss1 << "Efficiency: " << efficiency;
  tpt1.AddText(ss1.str().c_str());
  ss1.clear();
  ss1.str("");
  ss1 << "Purity: " << purity;
  tpt1.AddText(ss1.str().c_str());
  c1.Update();
  tpt1.Draw();

  // Tidy up
  analyser.save_root();
  infile.close();
  theApp.Run();

  return 0;
}
