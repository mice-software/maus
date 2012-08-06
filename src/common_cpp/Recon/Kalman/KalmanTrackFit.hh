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
#include "src/common_cpp/Recon/Kalman/GlobalTrack.hh"
#include "src/common_cpp/Recon/Kalman/KalmanSite.hh"
#include "src/common_cpp/Recon/Kalman/KalmanMonitor.hh"

// namespace ublas = boost::numeric::ublas;

namespace MAUS {

class KalmanTrackFit {
 public:
  KalmanTrackFit();

  ~KalmanTrackFit();

  void process(std::vector<SciFiHelicalPRTrack> helical_tracks);

  void process(std::vector<SciFiStraightPRTrack> straight_tracks);

  void process(CLHEP::Hep3Vector &tof0, CLHEP::Hep3Vector &se, CLHEP::Hep3Vector &tof1);

  // This will: initialise the state vector;
  // Set covariance matrix;
  // Add plane measurents to all sites;
  void initialise(SciFiStraightPRTrack &evt, std::vector<KalmanSite> &sites);

  void initialise(SciFiHelicalPRTrack &evt, std::vector<KalmanSite> &sites);

  void initialise_global_track(CLHEP::Hep3Vector &tof0, CLHEP::Hep3Vector &se,
                               CLHEP::Hep3Vector &tof1, std::vector<KalmanSite> &sites);

  void process_clusters(std::vector<SciFiSpacePoint> &spacepoints,
                        std::vector<SciFiCluster*> &clusters);

  void extrapolate(std::vector<KalmanSite> &sites, KalmanTrack *track, int current_site);

  void filter(std::vector<KalmanSite> &sites, KalmanTrack *track, int current_site);

  void smooth(std::vector<KalmanSite> &sites, KalmanTrack *track, int current_site);

 private:
  static const bool _mc_run = 1;

  bool _helical_track, _straight_track;

// private:
};

} // ~namespace MAUS

#endif
