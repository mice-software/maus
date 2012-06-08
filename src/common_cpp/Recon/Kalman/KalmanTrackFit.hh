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
#include <assert.h>
#include <algorithm>

// C++ headers
#include <string>
#include <vector>
// #include "/home/edward/boost_1_49_0/boost/numeric/ublas/matrix.hpp"
// #include "/home/edward/boost_1_49_0/boost/numeric/ublas/lu.hpp"
#include "TMath.h"
#include "TMatrixD.h"

#include "src/common_cpp/Recon/SciFi/SciFiEvent.hh"
#include "src/common_cpp/Recon/Kalman/KalmanTrack.hh"
#include "src/common_cpp/Recon/Kalman/HelicalTrack.hh"
#include "src/common_cpp/Recon/Kalman/StraightTrack.hh"
#include "src/common_cpp/Recon/Kalman/KalmanSite.hh"
#include "src/common_cpp/Recon/Kalman/KalmanMonitor.hh"
#include "src/common_cpp/Recon/SciFi/SeedFinder.hh"

// namespace ublas = boost::numeric::ublas;

class KalmanTrackFit {
 public:
  KalmanTrackFit();

  ~KalmanTrackFit();

  void process(SciFiEvent &evt);

  void process(std::vector<SciFiSpacePoint> spacepoints,
               SeedFinder seed);

  void initialise(SciFiEvent &evt, std::vector<KalmanSite> &sites);

  void initialise_helix(std::vector<SciFiSpacePoint> &spacepoints,
                        std::vector<KalmanSite> &sites,
                        SeedFinder &seed);

  void process_clusters(std::vector<SciFiSpacePoint> &spacepoints,
                        std::vector<SciFiCluster*> &clusters);

  void extrapolate(std::vector<KalmanSite> &sites, KalmanTrack *track, int current_site);

  void filter(std::vector<KalmanSite> &sites, KalmanTrack *track, int current_site);

  void smooth(std::vector<KalmanSite> &sites, KalmanTrack *track, int current_site);

// private:
};

#endif
