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


#ifndef KALMANTRACKFIT_HH
#define KALMANTRACKFIT_HH

// C headers
#include <CLHEP/Vector/ThreeVector.h>
#include <assert.h>
#include <algorithm>

// C++ headers
#include <string>
#include <vector>
#include "TMath.h"
#include "TMatrixD.h"

#include "src/common_cpp/DataStructure/SciFiEvent.hh"
#include "src/common_cpp/DataStructure/SciFiHelicalPRTrack.hh"
#include "src/common_cpp/DataStructure/SciFiStraightPRTrack.hh"
#include "src/common_cpp/Recon/Kalman/KalmanTrack.hh"
#include "src/common_cpp/Recon/Kalman/HelicalTrack.hh"
#include "src/common_cpp/Recon/Kalman/StraightTrack.hh"
#include "src/common_cpp/Recon/Kalman/KalmanSite.hh"
#include "src/common_cpp/Recon/Kalman/KalmanMonitor.hh"
#include "src/common_cpp/Recon/Kalman/KalmanSciFiAlignment.hh"
#include "src/common_cpp/Recon/Kalman/KalmanSeed.hh"

namespace MAUS {

class KalmanTrackFit {
 public:
  KalmanTrackFit();

  virtual ~KalmanTrackFit();

  void process(std::vector<KalmanSeed*> seeds);

  // This will: initialise the state vector;
  // Set covariance matrix;
  // Add plane measurents to all sites;
  void initialise(KalmanSeed *seed,
                  std::vector<KalmanSite> &sites,
                  KalmanSciFiAlignment &kalman_align);

  void extrapolate(std::vector<KalmanSite> &sites,
                   KalmanTrack *track,
                   int current_site);

  void filter(std::vector<KalmanSite> &sites,
              KalmanTrack *track,
              int current_site);

  void filter_updating_misalignments(std::vector<KalmanSite> &sites,
                                     KalmanTrack *track,
                                     int current_site);

  void smooth(std::vector<KalmanSite> &sites,
              KalmanTrack *track,
              int current_site);

  void update_alignment_parameters(std::vector<KalmanSite> &sites,
                                   KalmanTrack *track,
                                   KalmanSciFiAlignment &kalman_align);

 protected:
  double _seed_cov;
};

} // ~namespace MAUS

#endif
