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

// Ascending z.
template <class element>
bool SortByZ(const element *a, const element *b) {
  return ( a->get_position().z() > b->get_position().z() );
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
  _seed_cov       = (*json)["SciFiSeedCovariance"].asDouble();
  _pos_resolution = (*json)["SciFi_sigma_triplet"].asDouble();
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
     _clusters[i] = seed._clusters[i];
  }


  _kalman_sites.resize(seed._kalman_sites.size());
  for (size_t i = 0; i < seed._kalman_sites.size(); ++i) {
    _kalman_sites[i] = new KalmanState(*seed._kalman_sites[i]);
  }
}

void KalmanSeed::BuildKalmanStates() {
  size_t n_scifi_planes = 15;

  int sign =1;
  if ( _tracker == 0 ) sign = -1;

  for ( size_t j = n_scifi_planes; j >= 1; --j ) {
    KalmanState* a_site = new KalmanState();
    a_site->Initialise(_n_parameters);

    int id = sign*j;
    a_site->set_id(id);
    // Add more info if the plane registered a measurement:
    for ( size_t i = 0; i < _clusters.size(); ++i ) {
      if ( _clusters.at(i)->get_id() == id ) {
            a_site->set_spill(_clusters.at(i)->get_spill());
            a_site->set_event(_clusters.at(i)->get_event());
            a_site->set_id(id);
            a_site->contains_measurement(true);
            a_site->set_measurement(_clusters.at(i)->get_alpha());
            a_site->set_direction(_clusters.at(i)->get_direction());
            a_site->set_z(_clusters.at(i)->get_position().z());
            a_site->set_cluster(_clusters.at(i));
            std::map<int, SciFiPlaneGeometry>::iterator iterator;
            iterator = _geometry_map.find(_clusters.at(i)->get_id());
            SciFiPlaneGeometry this_plane = (*iterator).second;
            ThreeVector plane_position  = this_plane.Position;
            TMatrixD shift(3, 1);
            shift(0, 0) = plane_position.x();
            shift(1, 0) = plane_position.y();
            shift(2, 0) = 0.0;
            a_site->set_input_shift(shift);
      }
    }
    _kalman_sites.push_back(a_site);
  }
  //
  // Set up first state and its covariance.
  _kalman_sites[0]->set_a(_a0, KalmanState::Projected);
  _kalman_sites[0]->set_covariance_matrix(_full_covariance, KalmanState::Projected);
}

TMatrixD KalmanSeed::ComputeInitialStateVector(const SciFiHelicalPRTrack* seed,
                                               const SciFiSpacePointPArray &spacepoints) {
  // Get seed values.
  double r  = seed->get_R();
  // Get pt in MeV.
  double c  = CLHEP::c_light;
  // Charge guess should come from PR.
  _particle_charge = seed->get_charge();
  // Downstream reconstruction goes in reverse.
  if ( _tracker == 1 ) {
    _Bz = -_Bz;
  }

  // Length of tracker
  double length = 1100.0;

  double pt = _particle_charge*c*_Bz*r;
  double dsdz  = fabs(seed->get_dsdz());
  double x0 = seed->get_circle_x0(); // Circle Center x
  double y0 = seed->get_circle_y0(); // Circle Center y
  double s = seed->get_line_sz_c() - _particle_charge*length*dsdz; // Path length at start plane
  double phi_0 = s / r; // Phi at start plane
  double phi = phi_0 + TMath::PiOver2(); // Direction of momentum

  ThreeVector patrec_momentum( pt*cos(phi), pt*sin(phi), fabs(pt/dsdz) );
  double P = patrec_momentum.mag();
  double patrec_bias = (P - 1.4) / P;
  patrec_momentum = patrec_bias * patrec_momentum;

  double x = x0 + r*cos(phi_0);
  double px = patrec_momentum.x();
  double y = y0 + r*sin(phi_0);
  double py = patrec_momentum.y();
  double kappa = _particle_charge / patrec_momentum.z();

  TMatrixD a(_n_parameters, 1);
  a(0, 0) = x;
  a(1, 0) = px; 
  a(2, 0) = y;
  a(3, 0) = py;
  a(4, 0) = kappa;

  return a;
}

TMatrixD KalmanSeed::ComputeInitialStateVector(const SciFiStraightPRTrack* seed,
                                               const SciFiSpacePointPArray &spacepoints) {
  // Length of tracker
  double length = 1100.0;

  double x0 = seed->get_x0();
  double y0 = seed->get_y0();
  double mx = seed->get_mx();
  double my = seed->get_my();

  double x, y;

  // Downstream tracker starts at the other end.
  if ( _tracker == 0 ) {
    x = x0 + mx*length;
    y = y0 + my*length;
  } else {
    x = x0 + mx*length;
    y = y0 + my*length;
  }

  TMatrixD a(_n_parameters, 1);
  a(0, 0) = x;
  a(1, 0) = mx;
  a(2, 0) = y;
  a(3, 0) = my;

  double spx = spacepoints.front()->get_position().x();
  double spy = spacepoints.front()->get_position().y();
  double spz = spacepoints.front()->get_position().z();

  std::cerr << "Space Point = " << spx << ", " << mx << ", " << spy << ", " << my << ", " << spz << '\n';

  std::cerr << "Straight Seed = " << x << ", " << mx << ", " << y << ", " << my << '\n';

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

TMatrixD KalmanSeed::ComputeInitialCovariance(const SciFiHelicalPRTrack* seed) {
//  TMatrixD covariance(_n_parameters, _n_parameters);
//  for ( int i = 0; i < _n_parameters; ++i ) {
//    covariance(i, i) = _seed_cov;
//  }

  std::vector<double> cov = seed->get_covariance();
  TMatrixD patrec_covariance( _n_parameters, _n_parameters );
  TMatrixD covariance( _n_parameters, _n_parameters );

  double length = 1100.0;
  double dsdz  = fabs(seed->get_dsdz());
  double mc = _particle_charge*CLHEP::c_light*_Bz; // Magnetic constant
  double r = seed->get_R();
  double s = seed->get_line_sz_c() - _particle_charge*length*dsdz; // Path length at start plane
  double phi = s / r; // Phi at start plane // TODO: Is this the correct phi?!
  double sin = std::sin( phi );
  double cos = std::cos( phi );
  double sin_plus = std::sin( phi + TMath::PiOver2() );
  double cos_plus = std::cos( phi + TMath::PiOver2() );
  double ts = seed->get_dsdz();

  TMatrixD jacobian( _n_parameters, _n_parameters );
  jacobian(0,0) = 1.0;
  jacobian(0,2) = cos + phi*sin;
  jacobian(0,3) = -sin;

  jacobian(1,2) = mc*cos_plus + mc*phi*sin_plus;
  jacobian(1,3) = -mc*sin_plus;

  jacobian(2,1) = 1.0;
  jacobian(2,2) = sin - phi*cos;
  jacobian(2,3) = cos;

  jacobian(3,2) = mc*sin_plus - mc*phi*cos_plus;
  jacobian(1,3) = mc*cos_plus;

  jacobian(4,3) = -ts / (mc*r*r);
  jacobian(4,4) = 1.0 / (mc*r);

  TMatrixD jacobianT(_n_parameters, _n_parameters);
  jacobianT.Transpose( jacobian );

  if ( cov.size() != _n_parameters*_n_parameters ) {
    throw MAUS::Exception( MAUS::Exception::recoverable, 
                          "Dimension of covariance matrix does not match the state vector",
                          "KalmanSeed::ComputeInitalCovariance(Helical)");
  }

  for ( int i = 0; i < _n_parameters; ++i ) {
    for ( int j = 0; j < _n_parameters; ++j ) {
      patrec_covariance(i,j) = cov.at( i*_n_parameters + j );
    }
  }

  covariance = jacobian*patrec_covariance*jacobianT;

  patrec_covariance.Print();
  covariance.Print();

  return covariance;
}

TMatrixD KalmanSeed::ComputeInitialCovariance(const SciFiStraightPRTrack* seed) {
//  _full_covariance.Zero();
//  for ( int i = 0; i < _n_parameters; ++i ) {
//    _full_covariance(i, i) = _seed_cov;
//  }

  std::vector<double> cov = seed->get_covariance();
  TMatrixD patrec_covariance( _n_parameters, _n_parameters );
  TMatrixD covariance( _n_parameters, _n_parameters );

  TMatrixD jacobian( _n_parameters, _n_parameters );
  jacobian(0,0) = 1.0;
  jacobian(1,1) = 1.0;
  jacobian(2,2) = 1.0;
  jacobian(3,3) = 1.0;
  jacobian(0,1) = 1100.0; // TODO: Read the correct value from the geometry
  jacobian(2,3) = 1100.0;

  TMatrixD jacobianT(_n_parameters, _n_parameters);
  jacobianT.Transpose( jacobian );

  if ( cov.size() != _n_parameters*_n_parameters ) {
    throw MAUS::Exception( MAUS::Exception::recoverable, 
                          "Dimension of covariance matrix does not match the state vector",
                          "KalmanSeed::ComputeInitalCovariance(Straight)");
  }

  for ( int i = 0; i < _n_parameters; ++i ) {
    for ( int j = 0; j < _n_parameters; ++j ) {
      patrec_covariance(i,j) = cov.at( i*_n_parameters + j );
    }
  }

  covariance = jacobian*patrec_covariance*jacobianT;

  patrec_covariance.Print();
  covariance.Print();

  return covariance;
}

} // ~namespace MAUS
