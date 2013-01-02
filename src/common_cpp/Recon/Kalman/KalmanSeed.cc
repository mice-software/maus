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

bool sort_by_id(const SciFiCluster *a, const SciFiCluster *b) {
  // Ascending site number.
  return ( a->get_id() < b->get_id() );
}

bool sort_by_station(const SciFiSpacePoint a, const SciFiSpacePoint b) {
  // Ascending station number.
  return ( a.get_station() < b.get_station() );
}

KalmanSeed::KalmanSeed(): _straight(false), _helical(false) {}

KalmanSeed::~KalmanSeed() {}

template <class PRTrack>
void KalmanSeed::build(PRTrack* pr_track) {

  if ( pr_track->get_type() == 0 ) {
    _straight = true;
  } else if ( pr_track->get_type() == 1 ) {
    _helical = true;
  }

  // This builds the _clusters vector.
  process_measurements(pr_track);

  _a0 = compute_initial_state_vector(pr_track);
}

template <class PRTrack>
void KalmanSeed::process_measurements(PRTrack* pr_track) {

    for ( size_t i = 0; i < pr_track->get_spacepoints().size(); ++i ) {
      SciFiSpacePoint sp = *pr_track->get_spacepoints()[i];
      _spacepoints.push_back(sp);
    }
    double pz_from_timing;
    //std::vector<SciFiCluster*> clusters;
    retrieve_clusters(_spacepoints, pz_from_timing);

}

TMatrixD KalmanSeed::compute_initial_state_vector(SciFiHelicalPRTrack* seed) {
  // Get seed values.
  double r  = seed->get_R();
  double B = -4.;
  double pt = -0.3*B*r;

  double dsdz  = seed->get_dsdz();
  double tan_lambda = 1./dsdz;
  // PR doesnt see Eloss, so overstimates pz.
  // Total Eloss = 2 MeV/c.
  double pz = pt*tan_lambda - 2./sqrt(12.);
  double seed_pz;

  double momentum = pow(pt*pt+pz*pz, 0.5);
  double kappa = fabs(1./pz);

  int numb_sites = _clusters.size();

  int tracker = _clusters[0]->get_tracker();

  double pi = acos(-1.);
  double phi_0 = seed->get_phi0();
  double phi = phi_0 + pi/2.;
  double px  = pt*cos(phi);
  double py  = pt*sin(phi);

  double x = _spacepoints[0].get_position().x();
  double y = _spacepoints[0].get_position().y();

  TMatrixD a(5, 1);
  a(0, 0) = x;
  a(1, 0) = px;
  a(2, 0) = y;
  a(3, 0) = py;
  a(4, 0) = kappa;

  return a;
}

TMatrixD KalmanSeed::compute_initial_state_vector(SciFiStraightPRTrack* seed) {
  double x = _spacepoints[0].get_position().x();
  double y = _spacepoints[0].get_position().y();
  double mx = seed->get_mx();
  double my = seed->get_my();
  double seed_pz = 200.;

  TMatrixD a(5, 1);
  a(0, 0) = x;
  a(1, 0) = mx;
  a(2, 0) = y;
  a(3, 0) = my;
  a(4, 0) = 1./seed_pz;

  return a;
}


void KalmanSeed::retrieve_clusters(std::vector<SciFiSpacePoint> &spacepoints,
                                      double &seed_pz) {
  // This admits there is only one track...
  // SciFiStraightPRTrack seed = event.straightprtracks()[0];
  // std::vector<SciFiSpacePoint> spacepoints = seed->get_spacepoints(); // Get CLUSTERS!
  int numb_spacepoints = spacepoints.size();

  for ( int i = 0; i < numb_spacepoints; ++i ) {
    SciFiSpacePoint spacepoint = spacepoints[i];
    int num_clusters = spacepoint.get_channels().size();
    for ( int j = 0; j < num_clusters; ++j ) {
      SciFiCluster *cluster = spacepoint.get_channels()[j];
      _clusters.push_back(cluster);
    }
  }
  std::sort(_clusters.begin(), _clusters.end(), sort_by_id);
  std::sort(_spacepoints.begin(), _spacepoints.end(), sort_by_station);
  /*
  // Compute pz from tracker timing.
  int last_cluster = clusters.size();
  double deltaT = clusters[0]->get_time() - clusters[last_cluster-1]->get_time();
  double deltaZ = clusters[0]->get_position().z() - clusters[last_cluster-1]->get_position().z();

  std::cerr << "dt and dz: " << deltaT << " " << deltaZ << "\n";
  deltaZ = deltaZ/1000.; // mm -> m
  deltaT = deltaT/1000000000.; // ns -> s

  deltaZ = fabs(deltaZ);
  deltaT = fabs(deltaT);

  double mass = 105.; // MeV/c2
  double c = 300000000.; // m/s2
  double gamma = 1./pow(1.-pow(deltaZ/(c*deltaT), 2.), 0.5);
  seed_pz = gamma*mass*deltaZ/deltaT;
  std::cerr << "Pz: " << gamma << " " << deltaZ << " "
            << deltaT << " " << seed_pz << " "
            << clusters[0]->get_true_momentum().z() << "\n";
  */
  // seed_pz = 200.;
}

} // ~namespace MAUS
