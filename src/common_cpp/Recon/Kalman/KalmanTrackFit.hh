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

#include "Utils/Exception.hh"
#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/Globals/GlobalsManager.hh"
#include "src/common_cpp/DataStructure/ThreeVector.hh"
#include "src/common_cpp/DataStructure/SciFiEvent.hh"
#include "src/common_cpp/DataStructure/SciFiTrack.hh"
#include "src/common_cpp/DataStructure/SciFiTrackPoint.hh"
#include "src/common_cpp/Recon/Kalman/KalmanFilter.hh"
#include "src/common_cpp/Recon/Kalman/KalmanStraightPropagator.hh"
#include "src/common_cpp/Recon/Kalman/KalmanHelicalPropagator.hh"
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

  void SaveGeometry(std::vector<ThreeVector> positions,
                    std::vector<HepRotation> rotations);

  /** @brief The main worker. All Kalman Filtering lives within.
   */
  void Process(std::vector<KalmanSeed*> seeds, SciFiEvent &event);

  /** @brief Loops over the track points in the finished track calculating the chi2.
   */
  void ComputeChi2(SciFiTrack *track, KalmanStatesPArray sites);

  /** @brief Saves the track into the SciFiEvent for data structure output.
   */
  void Save(SciFiEvent &event, SciFiTrack *track, KalmanStatesPArray sites);

  /** @brief Prints some info about the fitting evolution.
   */
  void DumpInfo(KalmanStatesPArray sites);

 private:
  bool _use_MCS;

  bool _use_Eloss;

  bool _verbose;

  std::vector<ThreeVector> _RefPos;

  std::vector<HepRotation> _Rot;

  KalmanPropagator *_propagator;

  KalmanFilter     *_filter;
};

} // ~namespace MAUS

#endif
