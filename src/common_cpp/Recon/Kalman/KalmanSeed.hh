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

#include "TMatrixD.h"
#include "TMath.h"
#include "TVirtualFitter.h"
#include "Math/Minimizer.h"
#include "Math/Factory.h"
#include "Math/Functor.h"

#include "src/common_cpp/DataStructure/SciFiEvent.hh"
#include "Interface/Squeak.hh"

namespace MAUS {

class KalmanSeed {
 public:
  KalmanSeed();

  ~KalmanSeed();

  KalmanSeed(const KalmanSeed &seed);

  KalmanSeed& operator=(const KalmanSeed& seed);

  template <class PRTrack>
  void Build(const PRTrack* pr_track);

  TMatrixD ComputeInitialStateVector(const SciFiHelicalPRTrack* seed,
                                     const SciFiSpacePointPArray &spacepoints);

  TMatrixD ComputeInitialStateVector(const SciFiStraightPRTrack* seed,
                                     const SciFiSpacePointPArray &spacepoints);

  void RetrieveClusters(std::vector<SciFiSpacePoint*> &spacepoints);

  bool is_helical()  const { return _helical; }

  bool is_straight() const { return _straight; }

  SciFiClusterPArray clusters() const { return _clusters; }

  TMatrixD initial_state_vector() const { return _a0; }

  size_t is_usable() const { return _clusters.size(); }

  double momentum() const { return _momentum; }

  int n_parameters() const { return _n_parameters; }

 private:
  SciFiClusterPArray _clusters;

  TMatrixD _a0;

  double _momentum;

  bool _straight;

  bool _helical;

  int _n_parameters;

  int _tracker;
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

  SciFiSpacePointPArray spacepoints = pr_track->get_spacepoints();
  RetrieveClusters(spacepoints);

  _a0 = ComputeInitialStateVector(pr_track, spacepoints);
}

} // ~namespace MAUS

#endif
