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
bool SortByZ(const SciFiCluster *a, const SciFiCluster *b) {
  return ( a->get_position().z() < b->get_position().z() );
}

// Ascending station number.
bool SortByStation(const SciFiSpacePoint *a, const SciFiSpacePoint *b) {
  return ( a->get_station() < b->get_station() );
}

KalmanSeed::KalmanSeed() : _Bz(0.),
                           _straight(false),
                           _helical(false),
                           _n_parameters(-1),
                           _particle_charge(-1) {}

KalmanSeed::KalmanSeed(SciFiGeometryMap map): _Bz(0.),
                                              _geometry_map(map),
                                              _straight(false),
                                              _helical(false),
                                              _n_parameters(-1),
                                              _particle_charge(-1) {
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
    _clusters[i] = new SciFiCluster(*rhs._clusters[i]);
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
    _clusters[i] = new SciFiCluster(*seed._clusters[i]);
  }

  _kalman_sites.resize(seed._kalman_sites.size());
  for (size_t i = 0; i < seed._kalman_sites.size(); ++i) {
    _kalman_sites[i] = new KalmanState(*seed._kalman_sites[i]);
  }
}

void KalmanSeed::BuildKalmanStates() {
  size_t numb_sites = _clusters.size();
  for ( size_t j = 0; j < numb_sites; ++j ) {
    SciFiCluster& cluster = (*_clusters[j]);
    KalmanState* a_site = new KalmanState();
    a_site->Initialise(_n_parameters);

    int id = cluster.get_id();
    a_site->set_id(id);
    a_site->set_measurement(cluster.get_alpha());
    a_site->set_direction(cluster.get_direction());
    a_site->set_z(cluster.get_position().z());

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

  for ( size_t j = 0; j < numb_sites; ++j ) {
    ThreeVector true_position = _clusters[j]->get_true_position();
    ThreeVector true_momentum = _clusters[j]->get_true_momentum();
    _kalman_sites[j]->set_true_position(true_position);
    _kalman_sites[j]->set_true_momentum(true_momentum);
  }
}

TMatrixD KalmanSeed::ComputeInitialStateVector(const SciFiHelicalPRTrack* seed,
                                               const SciFiSpacePointPArray &spacepoints) {
  double x, y, z;
  double mc_x, mc_y, mc_z, mc_px, mc_py, mc_pz;
  x = spacepoints.front()->get_position().x();
  y = spacepoints.front()->get_position().y();
  z = spacepoints.front()->get_position().z();
  mc_x  = spacepoints.front()->get_channels()[0]->get_true_position().x();
  mc_y  = spacepoints.front()->get_channels()[0]->get_true_position().y();
  mc_z  = spacepoints.front()->get_channels()[0]->get_true_position().z();
  mc_px = spacepoints.front()->get_channels()[0]->get_true_momentum().x();
  mc_py = spacepoints.front()->get_channels()[0]->get_true_momentum().y();
  mc_pz = spacepoints.front()->get_channels()[0]->get_true_momentum().z();

  // Get seed values.
  double r  = seed->get_R();
  // Get pt in MeV.
  double c  = CLHEP::c_light;
  double pt = _particle_charge*c*_Bz*r;

  double dsdz  = seed->get_dsdz();
  double tan_lambda = 1./dsdz;

  double pz = pt*tan_lambda;

  double kappa = _particle_charge*fabs(1./pz);

  double phi_0 = seed->get_phi0();
  double phi = phi_0 + TMath::PiOver2();
  double px  = pt*cos(phi);
  double py  = pt*sin(phi);

  TMatrixD a(_n_parameters, 1);
  a(0, 0) = x;
  a(1, 0) = px*kappa;
  a(2, 0) = y;
  a(3, 0) = py*kappa;
  a(4, 0) = kappa;

  std::cerr << "Tracker " << _tracker << " has field " << _Bz <<". c is " << c << std::endl;
  std::cerr << "P: " << px << " " << py << " " << pz << std::endl;
  a.Print();
  std::cerr << "MC pos and mom: " << mc_x << " " <<  mc_y<< " " <<  mc_z << " "
            <<  mc_px<< " " <<  mc_py<< " " <<  mc_pz << std::endl;

  return a;
}

TMatrixD KalmanSeed::ComputeInitialStateVector(const SciFiStraightPRTrack* seed,
                                               const SciFiSpacePointPArray &spacepoints) {
  double x, y, z;
  double mc_x, mc_y, mc_z, mc_px, mc_py, mc_pz;

  x = spacepoints.front()->get_position().x();
  y = spacepoints.front()->get_position().y();
  z = spacepoints.front()->get_position().z();
  mc_x  = spacepoints.front()->get_channels()[0]->get_true_position().x();
  mc_y  = spacepoints.front()->get_channels()[0]->get_true_position().y();
  mc_z  = spacepoints.front()->get_channels()[0]->get_true_position().z();
  mc_px = spacepoints.front()->get_channels()[0]->get_true_momentum().x();
  mc_py = spacepoints.front()->get_channels()[0]->get_true_momentum().y();
  mc_pz = spacepoints.front()->get_channels()[0]->get_true_momentum().z();

  double mx = seed->get_mx();
  double my = seed->get_my();

  TMatrixD a(_n_parameters, 1);
  a(0, 0) = x;
  a(1, 0) = mx;
  a(2, 0) = y;
  a(3, 0) = my;

  std::cerr << "Tracker " << _tracker << std::endl;
  std::cerr << "Gradients: " << mx << " " << my << std::endl;
  a.Print();
  std::cerr << "MC pos and gradient: " << mc_x << " " <<  mc_y<< " " <<  mc_z << " "
  <<  mc_px/mc_pz<< " " <<  mc_py/mc_pz << std::endl;

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

  std::sort(_clusters.begin(), _clusters.end(), SortByZ);
  std::sort(spacepoints.begin(), spacepoints.end(), SortByStation);
}

} // ~namespace MAUS
