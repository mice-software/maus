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

#ifndef KALMANSEED_HH
#define KALMANSEED_HH

#include <algorithm>
#include <vector>
#include <map>

#include "TMatrixD.h"
#include "TMath.h"
#include "TVirtualFitter.h"
#include "Math/Minimizer.h"
#include "Math/Factory.h"
#include "Math/Functor.h"

#include "src/common_cpp/DataStructure/ThreeVector.hh"
#include "src/common_cpp/Recon/SciFi/SciFiGeometryHelper.hh"
#include "src/common_cpp/DataStructure/SciFiEvent.hh"
#include "Interface/Squeak.hh"

#include "CLHEP/Units/PhysicalConstants.h"

namespace MAUS {

/** @class KalmanSeed
 *
 *  @brief A class to interface the Kalman Fitting with Pattern Recon.
 *
 */
class KalmanSeed {
 public:
  /** @brief Default constructor.
   */
  KalmanSeed();

  /** @brief Constructor accepting a Geometry map.
   */
  explicit KalmanSeed(SciFiGeometryMap map);

  /** @brief Destructor.
   */
  ~KalmanSeed();

  /** @brief Copy constructor.
   */
  KalmanSeed(const KalmanSeed &seed);

  /** @brief Equality operator.
   */
  KalmanSeed& operator=(const KalmanSeed& seed);

  /** @brief Builds a KalmanSeed from a pattern recon object.
   */
  template <class PRTrack>
  void Build(const PRTrack* pr_track);

  /** @brief Computes the initial state vector for a helical track.
   */
  TMatrixD ComputeInitialStateVector(const SciFiHelicalPRTrack* seed,
                                     const SciFiSpacePointPArray &spacepoints);

  /** @brief Computes the initial state vector for a straight track.
   */
  TMatrixD ComputeInitialStateVector(const SciFiStraightPRTrack* seed,
                                     const SciFiSpacePointPArray &spacepoints);

  /** @brief Computes the initial covariance matrix for helical track.
   */
  TMatrixD ComputeInitialCovariance(const SciFiHelicalPRTrack* seed);

  /** @brief Computes the initial covariance matrix for straight track.
   */
  TMatrixD ComputeInitialCovariance(const SciFiStraightPRTrack* seed);

  /** @brief Fills the _clusters member.
   */
  void RetrieveClusters(SciFiSpacePointPArray &spacepoints);

  /** @brief Getter for the initial state vector.
   */
  TMatrixD initial_state_vector() const { return _a0; }

  /** @brief Getter for the initial covariance matrix.
   */
  TMatrixD initial_covariance_matrix() const { return _full_covariance; }

  /** @brief Loops over the clusters in the event and makes a KalmanState for each.
   */
  void BuildKalmanStates();

  bool is_helical()  const { return _helical; }

  bool is_straight() const { return _straight; }

  SciFiClusterPArray GetClusters() const { return _clusters; }

  void SetClusters(SciFiClusterPArray clusters) { _clusters = clusters; }

  int n_parameters() const { return _n_parameters; }

  KalmanStatesPArray GetKalmanStates() const { return _kalman_sites; }

  void SetKalmanStates(KalmanStatesPArray sites) { _kalman_sites = sites; }

  void SetField(double bz) { _Bz = bz; }

  double GetField() const { return _Bz; }

  int tracker() const { return _tracker; }

  void set_tracker(int tracker) { _tracker = tracker; }

  int charge() const { return _particle_charge; }

  void set_charge(int charge) { _particle_charge = charge; }

  SciFiGeometryMap GetGeometryMap() { return _geometry_map; }

 private:
  /** @brief Initial 'guess' of the state vector [x, mx, y, my, (kappa)].
   */
  TMatrixD _a0;

  /** @brief The field in the tracker the seed belongs to.
   *
   * This is the field value wrt the global reference frame.
   */
  double _Bz;

  /** @brief Uncertainty on a0's elements - tunable parameter in the datacards.
   */
  double _seed_cov;

  bool _use_patrec_seed;

  TMatrixD _full_covariance;

  double _pos_resolution;

  SciFiGeometryMap _geometry_map;

  SciFiClusterPArray _clusters;

  KalmanStatesPArray _kalman_sites;

  int _tracker;

  bool _straight;

  bool _helical;

  int _n_parameters;

  int _particle_charge;
};

template <class PRTrack>
void KalmanSeed::Build(const PRTrack* pr_track) {
  if ( pr_track->get_type() ) {
    _helical = true;
    _n_parameters = 5;
  } else {
    _straight = true;
    _n_parameters = 4;
  }

  _tracker = pr_track->get_tracker();

  _a0.ResizeTo(_n_parameters, 1);
  _full_covariance.ResizeTo(_n_parameters, _n_parameters);

  SciFiSpacePointPArray spacepoints = pr_track->get_spacepoints_pointers();
  RetrieveClusters(spacepoints);
  _a0 = ComputeInitialStateVector(pr_track, spacepoints);
  _full_covariance = ComputeInitialCovariance(pr_track);
  BuildKalmanStates();
}

} // ~namespace MAUS

#endif
