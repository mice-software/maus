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

// MAUS headers
#include "src/common_cpp/Analysis/SciFi/SciFiAnalysis.hh"
#include "src/common_cpp/Analysis/SciFi/SciFiDisplayMomentumResidualsPR.hh"
#include "src/common_cpp/Analysis/SciFi/SciFiDisplayPositionResidualsKF.hh"
#include "src/common_cpp/Analysis/SciFi/SciFiDisplayMomentumResidualsKF.hh"
#include "src/common_cpp/Analysis/SciFi/SciFiDisplayMomentumResolutionsPR.hh"
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/DataStructure/Data.hh"
#include "src/common_cpp/JsonCppStreamer/IRStream.hh"

/** Analyse Tracker momentum data using ROOT */


int main(int argc, char *argv[]) {

  // First argument to code should be the input ROOT file name
  std::string filename = std::string(argv[1]);

  // Analysis parameters - some may be overridden with command line arguments. All momenta in MeV/c.
  int n_pt_bins = 100;        // No. of bins in histos used to find pt resols (0 = let ROOT decide)
  int n_pz_bins = 100;        // No. of bins in histos used to find pz resols (0 = let ROOT decide)
  int n_points = 9;           // No. of data points in each resolution plot
  double pt_fit_min = -10.0;  // Lower bound of the gaussian fit to histos used to find pt resols
  double pt_fit_max = 10.0;   // Upper bound of the gaussian fit to histos used to find pt resols
  double pz_fit_min = -50.0;  // Lower bound of the gaussian fit to histos used to find pz resols
  double pz_fit_max = 50.0;   // Upper bound of the gaussian fit to histos used to find pz resols
  double lower_bound = 0;     // Lower bound on pt_mc of resolution plots
  double upper_bound = 90;    // Upper bound on pt_mc of resolution plots
  double pz_rec_cut = 500.0;  // Cut on reconstruction pz applied to histos used to find pz resols

  // Parse any extra arguments supplied from the command line
  //   -p -> pause between events
  //   -g -> enables saving xyz plots and gives output graphics file type
  bool bool_pause = false;
  std::string save_type = "";

  for (int i = 2; i < argc; i++) {
    if ( std::strcmp(argv[i], "-p") == 0 ) {
      std::cout << "Will wait for user input between spills\n";
      bool_pause = true;
    } else if ( std::strcmp(argv[i], "-g") == 0 ) {
      if ( (i+1) < argc ) save_type = argv[i + 1];
      if ( (save_type == "eps") || (save_type == "pdf") || (save_type == "png") ) {
        std::cout << "Saving plots as " << save_type << " files.\n";
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

  // Set up analysis and display classes
  MAUS::SciFiAnalysis analyser;

  // Create Pat Rec momentum residual display
  MAUS::SciFiDisplayMomentumResidualsPR* pr_mom_residuals
    = new MAUS::SciFiDisplayMomentumResidualsPR();

  // Create a Kalman fit position residual display
  MAUS::SciFiDisplayPositionResidualsKF* kf_pos_residuals
    = new MAUS::SciFiDisplayPositionResidualsKF();

  // Create a Kalman fit momentum residual display
  MAUS::SciFiDisplayMomentumResidualsKF* kf_mom_residuals
    = new MAUS::SciFiDisplayMomentumResidualsKF();

  // Create Pat Rec momentum resolutions display
  MAUS::SciFiDisplayMomentumResolutionsPR* resolutions =
    new MAUS::SciFiDisplayMomentumResolutionsPR();
  resolutions->SetNBinsPt(n_pt_bins);
  resolutions->SetNBinsPz(n_pz_bins);
  resolutions->SetNPoints(n_points);
  resolutions->SetPtFitMin(pt_fit_min);
  resolutions->SetPtFitMax(pt_fit_max);
  resolutions->SetPzFitMin(pz_fit_min);
  resolutions->SetPzFitMax(pz_fit_max);
  resolutions->SetLowerBoundPzMC(150.0);
  resolutions->SetUpperBoundPzMC(250.0);
  resolutions->SetResolLowerBound(lower_bound);
  resolutions->SetResolUpperBound(upper_bound);
  resolutions->SetCutPzRec(pz_rec_cut);
  std::cout << "Pt resol histogram number of bins: " << resolutions->GetNBbinsPt() << "\n";
  std::cout << "Pt resol histogram fit lower bound: " << resolutions->GetPtFitMin() << "\n";
  std::cout << "Pt resol histogram fit upper bound: " << resolutions->GetPtFitMax() << "\n";
  std::cout << "Pz resol histogram number of bins: " << resolutions->GetNBinsPz() << "\n";
  std::cout << "Pz resol histogram fit lower bound: " << resolutions->GetPzFitMin() << "\n";
  std::cout << "Pz resol histogram fit upper bound: " << resolutions->GetPzFitMax() << "\n";
  std::cout << "Resol graphs number of points: " << resolutions->GetNPoints() << "\n";
  std::cout << "Resol graphs pzmc lower bound: " << resolutions->GetLowerBoundPzMC() << " MeV/c\n";
  std::cout << "Resol graphs pzmc upper bound: " << resolutions->GetUpperBoundPzMC() << " MeV/c\n";
  std::cout << "Resol graphs ptmc lower bound: " << resolutions->GetResolLowerBound() << " MeV/c\n";
  std::cout << "Resol graphs ptmc upper bound: " << resolutions->GetResolUpperBound() << " MeV/c\n";
  std::cout << "Pz rec cut: " << resolutions->GetCutPzRec() << " MeV/c\n";

  // Set up the displays
  analyser.AddDisplay(pr_mom_residuals);
  analyser.AddDisplay(kf_pos_residuals);
  analyser.AddDisplay(kf_mom_residuals);
  analyser.AddDisplay(resolutions);
  analyser.SetUpDisplays();

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
      analyser.Process(spill);
      if (bool_pause) {
        std::cout << "Press Enter to Continue";
        std::cin.ignore();
      }
    } else {
      std::cout << "Not a usable spill\n";
    }
  }

  // Tidy up
  infile.close();

  // Make the final plots and save
  analyser.Plot();
  analyser.Save();
  theApp.Run();

  return 0;
}
