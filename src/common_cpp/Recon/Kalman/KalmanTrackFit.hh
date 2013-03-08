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

// C++ headers
#include <string>
#include <vector>
#include "TMath.h"
#include "TMatrixD.h"

#include "Interface/Squeal.hh"
#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/Globals/GlobalsManager.hh"
#include "src/common_cpp/DataStructure/SciFiEvent.hh"
#include "src/common_cpp/DataStructure/SciFiHelicalPRTrack.hh"
#include "src/common_cpp/DataStructure/SciFiStraightPRTrack.hh"
#include "src/common_cpp/DataStructure/SciFiTrack.hh"
#include "src/common_cpp/Recon/Kalman/KalmanTrack.hh"
#include "src/common_cpp/Recon/Kalman/HelicalTrack.hh"
#include "src/common_cpp/Recon/Kalman/StraightTrack.hh"
#include "src/common_cpp/Recon/Kalman/KalmanSite.hh"
#include "src/common_cpp/Recon/Kalman/KalmanMonitor.hh"
#include "src/common_cpp/Recon/Kalman/KalmanSciFiAlignment.hh"
#include "src/common_cpp/Recon/Kalman/KalmanSeed.hh"

#include "src/common_cpp/Recon/Kalman/Particle.hh"

namespace MAUS {

class KalmanTrackFit {
 public:
  KalmanTrackFit();

  virtual ~KalmanTrackFit();

  void process(std::vector<KalmanSeed*> seeds, SciFiEvent &event);

  /// This will: initialise the state vector;
  /// Set covariance matrix;
  /// Add plane measurents to all sites;
  void initialise(KalmanSeed *seed,
                  std::vector<KalmanSite> &sites,
                  KalmanSciFiAlignment &kalman_align);

  void run_filter(KalmanTrack *track,
                  std::vector<KalmanSite> &sites);

  void run_filter(KalmanTrack *track,
                  std::vector<KalmanSite> &sites,
                  int ignore_i);

  void filter_virtual(KalmanSite &a_site);

  void launch_misaligment_search(KalmanTrack *track,
                                 std::vector<KalmanSite> &sites,
                                 KalmanSciFiAlignment &kalman_align);

  void save(const KalmanTrack *kalman_track,
            std::vector<KalmanSite> sites,
            SciFiEvent &event);

 private:
  double _seed_cov;

  bool _use_MCS;

  bool _use_Eloss;

  bool _update_misalignments;

  std::string _type_of_dataflow;
};

} // ~namespace MAUS

#endif
