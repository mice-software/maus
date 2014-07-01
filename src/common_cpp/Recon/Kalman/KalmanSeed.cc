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

// TODO: use Pattern Recognition to set up _particle_charge

#include "src/common_cpp/Recon/Kalman/KalmanSeed.hh"

namespace MAUS {

// Ascending z.
template <class element>
bool SortByZ(const element *a, const element *b) {
  return ( a->get_position().z() < b->get_position().z() );
}

KalmanSeed::KalmanSeed() : _Bz(0.),
                           _straight(false),
                           _helical(false),
                           _n_parameters(-1),
                           _particle_charge(1) {}

KalmanSeed::KalmanSeed(SciFiGeometryMap map): _Bz(0.),
                                              _geometry_map(map),
                                              _straight(false),
                                              _helical(false),
                                              _n_parameters(-1),
                                              _particle_charge(1) {
  Json::Value *json = Globals::GetConfigurationCards();
  _seed_cov    = (*json)["SciFiSeedCovariance"].asDouble();
  _plane_width = (*json)["SciFiParams_Plane_Width"].asDouble();
}

KalmanSeed::~KalmanSeed() {}

KalmanSeed& KalmanSeed::operator=(const KalmanSeed &rhs) {
  if ( this == &rhs ) {
    return *this;
  }
  _clusters = rhs._clusters;
  _a0       = rhs._a0;
  _straight = rhs._straight;
  _helical  = rhs._helical;
  _tracker  = rhs._tracker;
  _n_parameters = rhs._n_parameters;

  _clusters.resize(rhs._clusters.size());
  for (size_t i = 0; i < rhs._clusters.size(); ++i) {
    // _clusters[i] = new SciFiCluster(*rhs._clusters[i]);
    _clusters[i] = rhs._clusters[i];
  }

  _kalman_sites.resize(rhs._kalman_sites.size());
  for (size_t i = 0; i < rhs._kalman_sites.size(); ++i) {
    _kalman_sites[i] = new KalmanState(*rhs._kalman_sites[i]);
  }
  return *this;
}

KalmanSeed::KalmanSeed(const KalmanSeed &seed) {
  _clusters = seed._clusters;
  _a0       = seed._a0;
  _straight = seed._straight;
  _helical  = seed._helical;
  _tracker  = seed._tracker;
  _n_parameters = seed._n_parameters;

  _clusters.resize(seed._clusters.size());
  for (size_t i = 0; i < seed._clusters.size(); ++i) {
    // _clusters[i] = new SciFiCluster(*seed._clusters[i]);
     _clusters[i] = seed._clusters[i];
  }


  _kalman_sites.resize(seed._kalman_sites.size());
  for (size_t i = 0; i < seed._kalman_sites.size(); ++i) {
    _kalman_sites[i] = new KalmanState(*seed._kalman_sites[i]);
  }
}

void KalmanSeed::BuildKalmanStates() {
  size_t numb_sites = _clusters.size();
  for ( size_t j = 0; j < numb_sites; ++j ) {
    // SciFiCluster& cluster = (*_clusters[j]);

    KalmanState* a_site = new KalmanState();
    a_site->Initialise(_n_parameters);

    int id = _clusters[j]->get_id();
    a_site->set_spill(_clusters[j]->get_spill());
    a_site->set_event(_clusters[j]->get_event());
    a_site->set_id(id);
    a_site->set_measurement(_clusters[j]->get_alpha());
    a_site->set_direction(_clusters[j]->get_direction());
    a_site->set_z(_clusters[j]->get_position().z());
    a_site->set_cluster(_clusters[j]);

    std::map<int, SciFiPlaneGeometry>::iterator iterator;
    iterator = _geometry_map.find(id);
    SciFiPlaneGeometry this_plane = (*iterator).second;
    ThreeVector plane_position  = this_plane.Position;

    TMatrixD shift(3, 1);
    shift(0, 0) = plane_position.x();
    shift(1, 0) = plane_position.y();
    shift(2, 0) = 0.0;
    a_site->set_input_shift(shift);

    _kalman_sites.push_back(a_site);
  }
  //
  // Set up first state and its covariance.
  TMatrixD C(_n_parameters, _n_parameters);
  C.Zero();
  for ( int i = 0; i < _n_parameters; ++i ) {
    C(i, i) = _seed_cov;
  }
  C(0, 0) = _plane_width*_plane_width/12.;
  C(2, 2) = _plane_width*_plane_width/12.;
  _kalman_sites[0]->set_a(_a0, KalmanState::Projected);
  _kalman_sites[0]->set_covariance_matrix(C, KalmanState::Projected);
}

TMatrixD KalmanSeed::ComputeInitialStateVector(const SciFiHelicalPRTrack* seed,
                                               const SciFiSpacePointPArray &spacepoints) {
  double x, y, z;
  x = spacepoints.front()->get_position().x();
  y = spacepoints.front()->get_position().y();
  z = spacepoints.front()->get_position().z();
  // Get seed values.
  double r  = seed->get_R();
  // Get pt in MeV.
  double c  = CLHEP::c_light;
  // Charge guess should come from PR.
  // int _particle_charge = seed->get_charge();

  double pt = -_particle_charge*c*_Bz*r;

  double dsdz  = seed->get_dsdz();
  double tan_lambda = 1./dsdz;

  double pz = pt*tan_lambda;

  double kappa = _particle_charge*fabs(1./pz);
  double phi_0 = seed->get_phi0();

  if ( _tracker == 0 ) {
    phi_0 = seed->get_phi().back();
  }
  double phi = phi_0 + TMath::PiOver2();
  double px  = pt*cos(phi);
  double py  = pt*sin(phi);

  TMatrixD a(_n_parameters, 1);
  a(0, 0) = x;
  a(1, 0) = px*fabs(kappa);
  a(2, 0) = y;
  a(3, 0) = py*fabs(kappa);
  a(4, 0) = kappa;

  return a;
}

TMatrixD KalmanSeed::ComputeInitialStateVector(const SciFiStraightPRTrack* seed,
                                               const SciFiSpacePointPArray &spacepoints) {
  double x, y, z;

  x = spacepoints.front()->get_position().x();
  y = spacepoints.front()->get_position().y();
  z = spacepoints.front()->get_position().z();

  double mx = seed->get_mx();
  double my = seed->get_my();

  TMatrixD a(_n_parameters, 1);
  a(0, 0) = x;
  a(1, 0) = mx;
  a(2, 0) = y;
  a(3, 0) = my;

  return a;
}

void KalmanSeed::RetrieveClusters(SciFiSpacePointPArray &spacepoints) {
  size_t numb_spacepoints = spacepoints.size();

  for ( size_t i = 0; i < numb_spacepoints; ++i ) {
    SciFiSpacePoint *spacepoint = spacepoints[i];
    size_t numb_clusters = spacepoint->get_channels()->GetLast() + 1;
    for ( size_t j = 0; j < numb_clusters; ++j ) {
      SciFiCluster *cluster = static_cast<SciFiCluster*>(spacepoint->get_channels()->At(j));
      _clusters.push_back(cluster);
    }
  }

  std::sort(_clusters.begin(), _clusters.end(), SortByZ<SciFiCluster>);
  std::sort(spacepoints.begin(), spacepoints.end(), SortByZ<SciFiSpacePoint>);
}

} // ~namespace MAUS
