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

#include "src/common_cpp/DataStructure/SciFiEvent.hh"
#include "Interface/Squeak.hh"

namespace MAUS {

class KalmanSeed {
 public:
  KalmanSeed();

  ~KalmanSeed();

  template <class PRTrack>
  void Build(const PRTrack* pr_track);

  void Build(const SciFiEvent &evt, bool field_on);

  template <class PRTrack>
  void ProcessMeasurements(const PRTrack *track);

  TMatrixD ComputeInitialStateVector(const SciFiHelicalPRTrack* seed);

  TMatrixD ComputeInitialStateVector(const SciFiStraightPRTrack* seed);

  void RetrieveClusters(std::vector<SciFiSpacePoint*> &spacepoints, double &seed_pz);

  bool is_helical()  const { return _helical; }

  bool is_straight() const { return _straight; }

  std::vector<SciFiCluster*> clusters() const { return _clusters; }

  TMatrixD initial_state_vector() const { return _a0; }

  size_t is_usable() const { return _clusters.size(); }

  double momentum() const { return _momentum; }

  std::vector<SciFiSpacePoint*> spacepoints() { return _spacepoints; }

  int n_parameters() const { return _n_parameters; }

  void get_gradients(double &mx, double &my);

  void get_tan_lambda(double &tanlambda);

  void get_circle_param(double &radius, double &x0, double &y0);

  double tan_lambda_fit(const double *par);

  double circle_fit(const double *par);

  double mx_fit(const double *par);

  double my_fit(const double *par);

 private:
  double _x[5], _y[5], _z[5], _phi[5], _s[5];

  std::vector<SciFiCluster*> _clusters;

  std::vector<SciFiSpacePoint*> _spacepoints;

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

  ProcessMeasurements<PRTrack>(pr_track);

  _a0 = ComputeInitialStateVector(pr_track);
}

template <class PRTrack>
void KalmanSeed::ProcessMeasurements(const PRTrack *pr_track) {
  for ( size_t i = 0; i < pr_track->get_spacepoints().size(); ++i ) {
    SciFiSpacePoint *sp = pr_track->get_spacepoints()[i];
    _spacepoints.push_back(sp);
  }
  double pz_from_timing;

  RetrieveClusters(_spacepoints, pz_from_timing);
}

} // ~namespace MAUS

#endif
