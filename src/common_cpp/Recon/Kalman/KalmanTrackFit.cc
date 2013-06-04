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

#include "src/common_cpp/Recon/Kalman/KalmanTrackFit.hh"

namespace MAUS {

KalmanTrackFit::KalmanTrackFit(SciFiGeometryMap map) : _geometry_map(map) {
  //
  // Get Configuration values.
  //
  Json::Value *json = Globals::GetConfigurationCards();
  _seed_cov             = (*json)["SciFiSeedCovariance"].asDouble();
  _use_MCS              = (*json)["SciFiKalman_use_MCS"].asBool();
  _use_Eloss            = (*json)["SciFiKalman_use_Eloss"].asBool();
  _verbose              = (*json)["SciFiKalmanVerbose"].asBool();
}

KalmanTrackFit::~KalmanTrackFit() {}

void KalmanTrackFit::Process(std::vector<KalmanSeed*> seeds,
                             SciFiEvent &event) {
  // Prepare to loop over seeds. 1 seed = 1 track hypothesis
  //
  for ( size_t i = 0; i < seeds.size(); ++i ) {
    // Current seed.
    KalmanSeed* seed = seeds[i];

    // Create pointer to abstract class KalmanTrack.
    KalmanTrack *track = 0;
    if ( seed->is_straight() ) {
      track = new StraightTrack();
    } else if ( seed->is_helical() ) {
      track = new HelicalTrack();
    }
    track->Initialise();
    //
    // Set up KalmanSites to be used. KalmanSite = Measurement Plane
    KalmanSitesVector sites;
    Initialise(seed, sites);
    //
    // Set the momentum of the track hypostesis (in MeV/c)
    double momentum = seed->momentum();
    track->set_momentum(momentum);
    //
    // Run the KALMAN FILTER
    //
    RunKalmanFilter(track, sites);
    //
    // Calculate the chi2 of this track.
    track->ComputeChi2(sites);
    // Optional printing.
    if ( _verbose )
      DumpInfo(sites);
    //
    // Save to data structure.
    // The KalmanTrack is converted into a
    // SciFiTrack for general use.
    //
    double chi2 = track->f_chi2();
    if ( chi2 != chi2 ) {
      delete track;
      delete seed;
      return;
    } else {
      Save(track, sites, event);
      delete track;
      delete seed;
    }
  }
}

void KalmanTrackFit::Initialise(const KalmanSeed *seed, KalmanSitesVector &sites) {
  TMatrixD a0 = seed->initial_state_vector();

  int n_param = seed->n_parameters();

  TMatrixD C(n_param, n_param);
  C.Zero();
  for ( int i = 0; i < n_param; i++ ) {
    C(i, i) = _seed_cov;
  }
  C(0, 0) = 1.;
  C(2, 2) = 1.;

  std::vector<SciFiCluster*> clusters = seed->clusters();
  KalmanSite first_plane;
  first_plane.Initialise(n_param);
  first_plane.set_a(a0, KalmanSite::Projected);
  first_plane.set_covariance_matrix(C, KalmanSite::Projected);
  first_plane.set_measurement(clusters[0]->get_alpha());
  first_plane.set_direction(clusters[0]->get_direction());
  first_plane.set_z(clusters[0]->get_position().z());

  int id_0 = clusters[0]->get_id();
  first_plane.set_id(id_0);

  std::map<int, SciFiPlaneGeometry>::iterator iterator;
  iterator = _geometry_map.find(id_0);
  SciFiPlaneGeometry this_plane = (*iterator).second;
  ThreeVector plane_position  = this_plane.Position;

  TMatrixD shift(3, 1);
  shift(0, 0) = plane_position.x();
  shift(1, 0) = plane_position.y();
  shift(2, 0) = 0.0;
  first_plane.set_input_shift(shift);
  // first_plane.set_input_shift_covariance(kalman_align.get_cov_shifts(id_0));
  sites.push_back(first_plane);
  size_t numb_sites = clusters.size();
  for ( size_t j = 1; j < numb_sites; ++j ) {
    KalmanSite a_site;
    a_site.Initialise(n_param);
    a_site.set_measurement(clusters[j]->get_alpha());
    a_site.set_direction(clusters[j]->get_direction());
    a_site.set_z(clusters[j]->get_position().z());
    int id = clusters[j]->get_id();
    a_site.set_id(id);

    std::map<int, SciFiPlaneGeometry>::iterator it;
    it = _geometry_map.find(id);
    SciFiPlaneGeometry a_plane = (*it).second;
    ThreeVector a_position  = a_plane.Position;

    TMatrixD a_shift(3, 1);
    a_shift(0, 0) = a_position.x();
    a_shift(1, 0) = a_position.y();
    a_shift(2, 0) = 0.0;
    a_site.set_input_shift(a_shift);
    // a_site.set_input_shift_covariance(kalman_align.get_cov_shifts(id));
    sites.push_back(a_site);
  }

  for ( size_t j = 0; j < numb_sites; ++j ) {
    ThreeVector true_position = clusters[j]->get_true_position();
    ThreeVector true_momentum = clusters[j]->get_true_momentum();
    sites[j].set_true_position(true_position);
    sites[j].set_true_momentum(true_momentum);
  }
}

void KalmanTrackFit::RunKalmanFilter(KalmanTrack *track, KalmanSitesVector &sites) {
  // Filter the first state.
  Filter(track, sites, 0);

  size_t numb_measurements = sites.size();
  for ( size_t j = 1; j < numb_measurements; ++j ) {
    // Predict the state vector at site i...
    Extrapolate(track, sites, j);
    // ... Filter...
    Filter(track, sites, j);
  }
  PrepareForSmoothing(&sites.back());
  // ...and Smooth back all sites.
  for ( int k = static_cast<int> (numb_measurements-2); k > -1; --k ) {
    Smooth(track, sites, k);
  }
}

void KalmanTrackFit::Filter(KalmanTrack *track, KalmanSitesVector &sites, int current_site) {
  // Get Site...
  KalmanSite *a_site = &sites[current_site];

  // Update measurement error:
  track->UpdateV(a_site);

  // Update H (depends on plane direction.)
  track->UpdateH(a_site);
  track->UpdateW(a_site);
  track->UpdateK(a_site);
  track->ComputePull(a_site);
  // a_k = a_k^k-1 + K_k x pull
  track->CalculateFilteredState(a_site);

  // Cp = (C-KHC)
  track->UpdateCovariance(a_site);

  a_site->set_current_state(KalmanSite::Filtered);
}

void KalmanTrackFit::Extrapolate(KalmanTrack *track, KalmanSitesVector &sites, int i) {
  // Get current site...
  KalmanSite *new_site = &sites[i];

  // ... and the site we will extrapolate from.
  const KalmanSite *old_site = &sites[i-1];

  // Calculate prediction for the state vector.
  track->CalculatePredictedState(old_site, new_site);

  // Calculate the energy loss for the projected state.
  if ( track->GetAlgorithmUsed() == KalmanTrack::kalman_helical && _use_Eloss )
    track->SubtractEnergyLoss(old_site, new_site);

  // Calculate the system noise...
  if ( _use_MCS )
    track->CalculateSystemNoise(old_site, new_site);

  // ... so that we can add it to the prediction for the
  // covariance matrix.
  track->CalculateCovariance(old_site, new_site);

  new_site->set_current_state(KalmanSite::Projected);
}

void KalmanTrackFit::PrepareForSmoothing(KalmanSite *last_site) {
  TMatrixD a_smooth = last_site->a(KalmanSite::Filtered);
  last_site->set_a(a_smooth, KalmanSite::Smoothed);

  TMatrixD C_smooth = last_site->covariance_matrix(KalmanSite::Filtered);
  last_site->set_covariance_matrix(C_smooth, KalmanSite::Smoothed);

  TMatrixD residual(2, 1);
  residual = last_site->residual(KalmanSite::Filtered);
  last_site->set_residual(residual, KalmanSite::Smoothed);
  last_site->set_current_state(KalmanSite::Smoothed);

  // Set smoothed chi2.
  double f_chi2 = last_site->chi2(KalmanSite::Filtered);
  last_site->set_chi2(f_chi2, KalmanSite::Smoothed);
}

void KalmanTrackFit::Smooth(KalmanTrack *track, KalmanSitesVector &sites, int id) {
  // Get site to be smoothed...
  KalmanSite *smoothing_site = &sites[id];

  // ... and the already perfected site.
  const KalmanSite *optimum_site = &sites[id+1];

  // Set the propagator right.
  track->UpdatePropagator(optimum_site, smoothing_site);

  // H and V are necessary for the residual calculation.
  track->UpdateH(smoothing_site);
  track->UpdateV(smoothing_site);

  // Compute A_k.
  track->UpdateBackTransportationMatrix(optimum_site, smoothing_site);

  // Compute smoothed a_k and C_k.
  track->SmoothBack(optimum_site, smoothing_site);

  smoothing_site->set_current_state(KalmanSite::Smoothed);
}

void KalmanTrackFit::Save(const KalmanTrack *kalman_track,
                          KalmanSitesVector sites,
                          SciFiEvent &event) {
  //
  // Convert KalmanTrack to SciFiTrack
  //
  SciFiTrack *track = new SciFiTrack(kalman_track);
  //
  // Store information at each measurement plane.
  size_t n_sites = sites.size();
  for ( size_t i = 0; i < n_sites; ++i ) {
    SciFiTrackPoint *track_point = new SciFiTrackPoint(&sites[i]);
    track->add_scifitrackpoint(track_point);
  }
  // Add to data structure.
  event.add_scifitrack(track);
}

void KalmanTrackFit::DumpInfo(KalmanSitesVector const &sites) {
  size_t numb_sites = sites.size();

  for ( size_t i = 0; i < numb_sites; ++i ) {
    KalmanSite site = sites[i];
    Squeak::mout(Squeak::info)
    << "=========================================="  << "\n"
    << "SITE ID: " << site.id() << "\n"
    << "SITE Z: " << site.z()   << "\n"
    << "Measurement: " << (site.measurement())(0, 0) << "\n"
    << "Projection: " << (site.a(KalmanSite::Projected))(0, 0) << " "
                      << (site.a(KalmanSite::Projected))(1, 0) << " "
                      << (site.a(KalmanSite::Projected))(2, 0) << " "
                      << (site.a(KalmanSite::Projected))(3, 0) << "\n"
    << "================Residuals================"   << "\n"
    << (site.residual(KalmanSite::Projected))(0, 0)  << "\n"
    << (site.residual(KalmanSite::Filtered))(0, 0)   << "\n"
    << (site.residual(KalmanSite::Smoothed))(0, 0)   << "\n"
    << "=========================================="
    << std::endl;
  }
}

} // ~namespace MAUS
