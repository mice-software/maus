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
#include <iostream>
#include <fstream>

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
#include "src/common_cpp/DataStructure/SciFiTrackPoint.hh"
#include "src/common_cpp/Recon/Kalman/KalmanTrack.hh"
#include "src/common_cpp/Recon/Kalman/HelicalTrack.hh"
#include "src/common_cpp/Recon/Kalman/StraightTrack.hh"
#include "src/common_cpp/Recon/Kalman/KalmanSite.hh"
#include "src/common_cpp/Recon/Kalman/KalmanSciFiAlignment.hh"
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

  /** @brief Sets up the KalmanSites.
   *
   *  This will: initialise the state vector;
   *  Set the covariance matrix;
   *  Add plane measurents to all sites;
   *
   */
  void Initialise(KalmanSeed *seed,
                  std::vector<KalmanSite> &sites,
                  KalmanSciFiAlignment &kalman_align);

  /** @brief Runs Filter routines.
   *
   *  Runs the Prediction->Filtering->Smoothing stages.
   *
   */
  void RunFilter(KalmanTrack *track,
                 std::vector<KalmanSite> &sites);

  /** @brief Runs Filter routines ignoring a station.
   *
   *  This removes one station from the fit and runs the normal filter.
   *  Useful for misalignment studies, for ex.
   *
   */
  void RunFilter(KalmanTrack *track,
                 std::vector<KalmanSite> &sites,
                 int ignore_i);

  /** @brief "Filter" a station ignoring the measurement.
   *
   *  In other words, it makes the filtered states equal to the projected ones.
   *
   */
  void FilterVirtual(KalmanSite &a_site);

  /** @brief
   *
   *  Performs a misalignment search.
   *
   */
  void LaunchMisaligmentSearch(KalmanTrack *track,
                               std::vector<KalmanSite> &sites,
                               KalmanSciFiAlignment &kalman_align);

  /** @brief
   *
   *  Saves the KalmanTrack to an output SciFiTrack.
   *
   */
  void Save(const KalmanTrack *kalman_track,
            std::vector<KalmanSite> sites,
            SciFiEvent &event);

  /** @brief
   *
   *  Prints some info about the filtering status.
   *
   */
  void DumpInfo(std::vector<KalmanSite> const &sites);

 private:
  double _seed_cov;

  bool _use_MCS;

  bool _use_Eloss;

  bool _verbose;

  bool _update_misalignments;

  std::string _type_of_dataflow;
};

} // ~namespace MAUS

#endif
