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

#include "src/common_cpp/Recon/Kalman/KalmanSeed.hh"

namespace MAUS {

// Ascending site number.
bool SortByID(const SciFiCluster *a, const SciFiCluster *b) {
  return ( a->get_id() < b->get_id() );
}

// Ascending station number.
bool SortByStation(const SciFiSpacePoint *a, const SciFiSpacePoint *b) {
  return ( a->get_station() < b->get_station() );
}

KalmanSeed::KalmanSeed(): _straight(false), _helical(false) {}

KalmanSeed::~KalmanSeed() {}

KalmanSeed& KalmanSeed::operator=(const KalmanSeed &rhs) {
  if ( this == &rhs ) {
    return *this;
  }
  _clusters = rhs._clusters;
  _a0       = rhs._a0;
  _momentum = rhs._momentum;
  _straight = rhs._straight;
  _helical  = rhs._helical;
  _tracker  = rhs._tracker;
  _n_parameters = rhs._n_parameters;

  return *this;
}

KalmanSeed::KalmanSeed(const KalmanSeed &seed) {
  _clusters = seed._clusters;
  _a0       = seed._a0;
  _momentum = seed._momentum;
  _straight = seed._straight;
  _helical  = seed._helical;
  _tracker  = seed._tracker;
  _n_parameters = seed._n_parameters;
}

TMatrixD KalmanSeed::ComputeInitialStateVector(const SciFiHelicalPRTrack* seed,
                                               const SciFiSpacePointPArray &spacepoints) {
  // Get seed values.
  double r  = seed->get_R();
  double B  = -4.;
  double pt = -0.3*B*r;
  // double pt = _particle_charge*(CLHEP::c_light*1.e-9)*B*r;

  double dsdz  = seed->get_dsdz();
  double tan_lambda = 1./dsdz;

  double pz = pt*tan_lambda;

  double kappa = fabs(1./pz);

  double phi_0 = seed->get_phi0();
  double phi = phi_0 + TMath::PiOver2();
  double px  = pt*cos(phi);
  double py  = pt*sin(phi);

  double x, y;
  if ( _tracker == 0 ) {
    x = spacepoints.back()->get_position().x();
    y = spacepoints.back()->get_position().y();
  } else if ( _tracker == 1 ) {
    x = spacepoints.front()->get_position().x();
    y = spacepoints.front()->get_position().y();
  } else {
    x = y = -666; // removes a compiler warning.
    throw(Squeal(Squeal::recoverable,
                 "Pattern Recon has bad tracker number.",
                 "KalmanSeed::ComputeInitialStateVector"));
  }

  TMatrixD a(_n_parameters, 1);
  a(0, 0) = x;
  a(1, 0) = px*kappa;
  a(2, 0) = y;
  a(3, 0) = py*kappa;
  a(4, 0) = kappa;

  _momentum = TMath::Sqrt(px*px+py*py+pz*pz);

  return a;
}

TMatrixD KalmanSeed::ComputeInitialStateVector(const SciFiStraightPRTrack* seed,
                                               const SciFiSpacePointPArray &spacepoints) {
  double x, y;
  if ( _tracker == 0 ) {
    x = spacepoints.back()->get_position().x();
    y = spacepoints.back()->get_position().y();
  } else if ( _tracker == 1 ) {
    x = spacepoints.front()->get_position().x();
    y = spacepoints.front()->get_position().y();
  } else {
    x = y = -666; // removes a compiler warning.
    throw(Squeal(Squeal::recoverable,
      "Pattern Recon has bad tracker number.",
      "KalmanSeed::ComputeInitialStateVector"));
  }

  double mx = seed->get_mx();
  double my = seed->get_my();

  // Straight tracks don't have Pz. Assume a reasonable number
  // just so that we can allow for some more trajectory kink correction.
  double seed_pz = 226.;

  TMatrixD a(_n_parameters, 1);
  a(0, 0) = x;
  a(1, 0) = mx;
  a(2, 0) = y;
  a(3, 0) = my;

  _momentum = seed_pz;

  return a;
}

void KalmanSeed::RetrieveClusters(SciFiSpacePointPArray &spacepoints) {
  size_t numb_spacepoints = spacepoints.size();

  for ( size_t i = 0; i < numb_spacepoints; ++i ) {
    SciFiSpacePoint *spacepoint = spacepoints[i];
    size_t numb_clusters = spacepoint->get_channels().size();
    for ( size_t j = 0; j < numb_clusters; ++j ) {
      SciFiCluster *cluster = spacepoint->get_channels()[j];
      _clusters.push_back(cluster);
    }
  }

  std::sort(_clusters.begin(), _clusters.end(), SortByID);
  std::sort(spacepoints.begin(), spacepoints.end(), SortByStation);
}

} // ~namespace MAUS
