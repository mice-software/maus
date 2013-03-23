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
#include <iostream>
#include <fstream>

namespace MAUS {

// Ascending site number.
bool SortByID(const SciFiCluster *a, const SciFiCluster *b) {
  return ( a->get_id() < b->get_id() );
}

// Ascending station number.
bool SortByStation(const SciFiSpacePoint *a, const SciFiSpacePoint *b) {
  return ( a->get_station() < b->get_station() );
}

KalmanSeed::KalmanSeed(): _straight(false), _helical(false) {
}

KalmanSeed::~KalmanSeed() {}

void KalmanSeed::Build(const SciFiStraightPRTrack* pr_track) {
  _straight = true;
  _n_parameters = 4;
  _a0.ResizeTo(_n_parameters, 1);

  ProcessMeasurements(pr_track);

  _a0 = ComputeInitialStateVector(pr_track);
}

void KalmanSeed::Build(const SciFiHelicalPRTrack* pr_track) {
  _helical = true;
  _n_parameters = 5;
  _a0.ResizeTo(_n_parameters, 1);

  ProcessMeasurements(pr_track);

  _a0 = ComputeInitialStateVector(pr_track);
}

void KalmanSeed::ProcessMeasurements(const SciFiStraightPRTrack* pr_track) {
  for ( size_t i = 0; i < pr_track->get_spacepoints().size(); ++i ) {
    SciFiSpacePoint *sp = pr_track->get_spacepoints()[i];
    _spacepoints.push_back(sp);
  }
  double pz_from_timing;

  RetrieveClusters(_spacepoints, pz_from_timing);
}

void KalmanSeed::ProcessMeasurements(const SciFiHelicalPRTrack* pr_track) {
  for ( size_t i = 0; i < pr_track->get_spacepoints().size(); ++i ) {
    SciFiSpacePoint *sp = pr_track->get_spacepoints()[i];
    _spacepoints.push_back(sp);
  }
  double pz_from_timing;

  RetrieveClusters(_spacepoints, pz_from_timing);
}

TMatrixD KalmanSeed::ComputeInitialStateVector(const SciFiHelicalPRTrack* seed) {
  // Get seed values.
  double r  = seed->get_R();
  double B = -4.;
  double pt = -0.3*B*r;

  double dsdz  = seed->get_dsdz();
  double tan_lambda = 1./dsdz;
  // PR doesnt see Eloss, so overstimates pz.
  // Total Eloss = 2 MeV/c.
  double pz = pt*tan_lambda;
  double seed_pz;

  double momentum = pow(pt*pt+pz*pz, 0.5);
  double kappa = fabs(1./pz);

  // int numb_sites = _clusters.size();

  int tracker = _clusters[0]->get_tracker();

  double PI = acos(-1.);
  double phi_0 = seed->get_phi0();
  double phi = phi_0 + PI/2.;
  double px  = pt*cos(phi);
  double py  = pt*sin(phi);

  double x = _spacepoints[0]->get_position().x();
  double y = _spacepoints[0]->get_position().y();

  TMatrixD a(_n_parameters, 1);
  a(0, 0) = x;
  a(1, 0) = px*kappa;
  a(2, 0) = y;
  a(3, 0) = py*kappa;
  a(4, 0) = kappa;

  _momentum = TMath::Sqrt(px*px+py*py+pz*pz);

  return a;
}

TMatrixD KalmanSeed::ComputeInitialStateVector(const SciFiStraightPRTrack* seed) {
  double x = _spacepoints[0]->get_position().x();
  double y = _spacepoints[0]->get_position().y();

  double mx = seed->get_mx();
  double my = seed->get_my();
  double seed_pz = 226.;

  TMatrixD a(_n_parameters, 1);
  a(0, 0) = x;
  a(1, 0) = mx;
  a(2, 0) = y;
  a(3, 0) = my;

  _momentum = seed_pz;

  return a;
}

void KalmanSeed::RetrieveClusters(std::vector<SciFiSpacePoint*> &spacepoints,
                                  double &seed_pz) {
  size_t numb_spacepoints = spacepoints.size();

  for ( size_t i = 0; i < numb_spacepoints; ++i ) {
    SciFiSpacePoint *spacepoint = spacepoints[i];
    size_t num_clusters = spacepoint->get_channels().size();
    for ( size_t j = 0; j < num_clusters; ++j ) {
      SciFiCluster *cluster = spacepoint->get_channels()[j];
      _clusters.push_back(cluster);
    }
  }

  std::sort(_clusters.begin(), _clusters.end(), SortByID);
  std::sort(_spacepoints.begin(), _spacepoints.end(), SortByStation);
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
