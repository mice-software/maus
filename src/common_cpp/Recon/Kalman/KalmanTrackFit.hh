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

// C++ headers
#include <string>
#include <vector>
#include "TMath.h"
#include "TMatrixD.h"

#include "Interface/Squeal.hh"
#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/Globals/GlobalsManager.hh"
#include "src/common_cpp/DataStructure/ThreeVector.hh"
#include "src/common_cpp/DataStructure/SciFiEvent.hh"
#include "src/common_cpp/DataStructure/SciFiTrack.hh"
#include "src/common_cpp/DataStructure/SciFiTrackPoint.hh"
#include "src/common_cpp/Recon/Kalman/KalmanTrack.hh"
#include "src/common_cpp/Recon/Kalman/HelicalTrack.hh"
#include "src/common_cpp/Recon/Kalman/StraightTrack.hh"
#include "src/common_cpp/Recon/Kalman/KalmanSite.hh"
#include "src/common_cpp/Recon/Kalman/KalmanSeed.hh"

namespace MAUS {

/** @class KalmanTrackFit
 *
 *  @brief KalmanTrackFit manages the workflow of the fitting.
 *
 */
class KalmanTrackFit {
 public:
  KalmanTrackFit();

  virtual ~KalmanTrackFit();

  /** @brief The main worker. All Kalman Filtering lives within.
   */
  void Process(std::vector<KalmanSeed*> seeds, SciFiEvent &event);

  /** @brief Runs Filter routines.
   *
   *  Runs the Prediction->Filtering->Smoothing stages.
   *
   */
  void RunKalmanFilter(KalmanTrack *track,
                       KalmanSitesVector &sites);

  /** @brief  Manages all extrapolation steps.
   */
  void Extrapolate(KalmanTrack *track, KalmanSitesVector &sites, int current_site);

  /** @brief  Manages all filtering steps.
   */
  void Filter(KalmanTrack *track, KalmanSitesVector &sites, int current_site);

  void PrepareForSmoothing(KalmanSite *last_site);

  /** @brief  Manages all smoothing steps.
   */
  void Smooth(KalmanTrack *track, KalmanSitesVector &sites, int current_site);

  /** @brief
   *
   *  Saves the KalmanTrack to an output SciFiTrack.
   *
   */
  void Save(const KalmanTrack *kalman_track,
            KalmanSitesVector sites,
            SciFiEvent &event);

  /** @brief
   *
   *  Prints some info about the filtering status.
   *
   */
  void DumpInfo(KalmanSitesVector const &sites);

 private:
  bool _use_MCS;

  bool _use_Eloss;

  bool _verbose;
};

} // ~namespace MAUS

#endif
