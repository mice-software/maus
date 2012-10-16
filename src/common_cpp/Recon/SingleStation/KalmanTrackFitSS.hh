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


#ifndef KalmanTrackFitSS_HH
#define KalmanTrackFitSS_HH

// C headers
#include <CLHEP/Vector/ThreeVector.h>
#include <assert.h>
#include <math.h>

// C++ headers
#include <iostream>
#include <fstream>
#include <algorithm>

#include <string>
#include <vector>
#include "TMath.h"
#include "TMatrixD.h"

#include "src/common_cpp/Recon/Kalman/KalmanTrackFit.hh"
#include "src/common_cpp/Recon/Kalman/KalmanSite.hh"
#include "src/common_cpp/Recon/SingleStation/KalmanSSTrack.hh"
#include "src/common_cpp/Recon/Kalman/KalmanMonitor.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"

namespace MAUS {

class KalmanTrackFitSS : public KalmanTrackFit {
 public:
  virtual ~KalmanTrackFitSS();

  void process(Json::Value event);

  // This will: initialise the state vector;
  // Set covariance matrix;
  // Add plane measurents to all sites;
  void initialise_global_track(Json::Value event, std::vector<KalmanSite> &sites,
                               double &particle_mass, double &particle_momentum);

  void filter_virtual(std::vector<KalmanSite> &sites,
                      int current_site);

  void perform_elementar_pattern_recon(Json::Value event,
                                       double &x_pr, double &y_pr,
                                       double &mx_pr, double &my_pr, double &p_z, double &mass);


  void initialise_prove_track(std::vector<KalmanSite> &old_sites, std::vector<KalmanSite> &new_sites);

  void prove_origin_large_residuals(std::vector<KalmanSite> old_sites, double particle_momentum, double particle_mass);

  void save(std::vector<KalmanSite> const &sites); 

 private:
  static const int tof0_num_slabs = 10;
  static const int tof1_num_slabs = 7;
  static const double tof0_a = 40.; // mm
  static const double tof1_a = 60.; // mm
};

} // ~namespace MAUS

#endif
