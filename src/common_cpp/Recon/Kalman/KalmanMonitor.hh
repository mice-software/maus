/* This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
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


#ifndef KALMANMONITOR_HH
#define KALMANMONITOR_HH

// C headers
#include <assert.h>

// C++ headers
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "CLHEP/Vector/ThreeVector.h"
#include "TMath.h"
#include "TMatrixD.h"
#include "TFile.h"
#include "TGraph.h"
#include "TMultiGraph.h"

#include "src/common_cpp/Recon/Kalman/KalmanSite.hh"

namespace MAUS {

class KalmanMonitor {
 public:
  KalmanMonitor();

  ~KalmanMonitor();

  void save(std::vector<KalmanSite> const &sites);

  void print_info(std::vector<KalmanSite> const &sites);

  double get_smoothed_measurement(KalmanSite &a_site);

 private:
  std::vector<double> _alpha_meas, _site, _alpha_projected;
};

} // ~namespace MAUS

#endif
