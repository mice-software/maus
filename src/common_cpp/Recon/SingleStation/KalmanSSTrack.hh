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


#ifndef KALMANSSTTRACK_HH
#define KALMANSSTTRACK_HH

// C headers
#include <assert.h>
#include <CLHEP/Vector/ThreeVector.h>
#include <math.h>
#include <iostream>
#include <fstream>

// C++ headers
#include <string>
#include <vector>

#include "TMath.h"
#include "TMatrixD.h"

#include "src/common_cpp/Recon/Kalman/KalmanTrack.hh"

namespace MAUS {

class KalmanSSTrack : public KalmanTrack {
 public:
  virtual ~KalmanSSTrack();

  //  Prediction
  void update_propagator(KalmanSite *old_site, KalmanSite *new_site);

  void magnet_drift();

  double compute_kappa(double current, double momentum);

  void straight_line(double deltaZ);

  // Acessors
  void compute_chi2(const std::vector<KalmanSite> &sites);
};

} // ~namespace MAUS

#endif
