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
#include <string>
#include <vector>
#include "CLHEP/Vector/ThreeVector.h"
// #include "/home/edward/boost_1_49_0/boost/numeric/ublas/matrix.hpp"
// #include "/home/edward/boost_1_49_0/boost/numeric/ublas/lu.hpp"
#include "TMath.h"
#include "TMatrixD.h"
#include "TFile.h"
#include "TGraph.h"
#include "TMultiGraph.h"

#include "src/common_cpp/Recon/Kalman/KalmanSite.hh"
// namespace ublas = boost::numeric::ublas;

class KalmanMonitor {
 public:
  KalmanMonitor();

  ~KalmanMonitor();

  void save(std::vector<KalmanSite> const &sites);

  void save_mc(std::vector<KalmanSite> const &sites);

  void print_info(std::vector<KalmanSite> const &sites);

 private:
  std::vector<double> _alpha_meas, _site, _alpha_projected;

 /* double _alpha_extrap[30], _alpha_std[30];

  double _alpha_smoothed[30], _alpha_std_smoothed[30];

  double _site[30], _err[30];
*/
};

#endif
