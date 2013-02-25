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

#include <iostream>
#include <fstream>

namespace MAUS {

KalmanTrackFit::KalmanTrackFit(): _seed_cov(200.),
                                  _update_misalignments(false) {
  // Get Configuration values.
  Json::Value *json = Globals::GetConfigurationCards();
  _seed_cov  = (*json)["SciFiSeedCovariance"].asDouble();
  _update_misalignments = (*json)["SciFiUpdateMisalignments"].asBool();
  // type_of_dataflow = 'pipeline_single_thread'

  std::cerr << "---------------------Birth of Kalman Filter--------------------" << std::endl;
}

KalmanTrackFit::~KalmanTrackFit() {
  std::cerr << "---------------------Death of Kalman Filter--------------------" << std::endl;
}

void KalmanTrackFit::process(std::vector<KalmanSeed*> seeds, SciFiEvent &event) {
  KalmanMonitor monitor;
  KalmanSciFiAlignment kalman_align;
  kalman_align.load_misaligments();

  size_t num_tracks = seeds.size();
  for ( size_t i = 0; i < num_tracks; ++i ) {
    KalmanSeed* seed = seeds[i];
    std::vector<KalmanSite> sites;

    initialise(seed, sites, kalman_align);

    size_t numb_measurements = sites.size();
    if ( numb_measurements != 15 ) continue;

    KalmanTrack *track = 0;
    if ( seed->is_straight() ) {
      track = new StraightTrack();
    } else if ( seed->is_helical() ) {
      track = new HelicalTrack();
    } else {
      throw(Squeal(Squeal::recoverable,
            "Can't initialise KalmanTrack; seed undefined.",
            "KalmanTrackFit::process"));
    }
    // muon assumption for now.
    double muon_mass    = 105.7; // MeV/c
    track->set_mass(muon_mass);
    double momentum = seed->get_momentum(); // MeV/c
    track->set_momentum(momentum);
    // Filter the first state.
    track->filter(sites, 0);

    run_filter(track, sites);

    monitor.fill(sites);
    // monitor.print_info(sites);
    track->compute_chi2(sites);
    // track->compute_emittance(sites.front());

    // Misalignment search.
    if ( _update_misalignments && track->f_chi2() < 20. && numb_measurements == 15 ) {
      launch_misaligment_search(track, sites, kalman_align);
    }
    save(track, sites, event);
    delete track;
  }
}

void KalmanTrackFit::launch_misaligment_search(KalmanTrack *track,
                                               std::vector<KalmanSite> &sites,
                                               KalmanSciFiAlignment &kalman_align) {
  double old_track_chi2 = track->s_chi2();
  for ( int station_i = 2; station_i < 5; ++station_i ) {
    std::vector<KalmanSite> sites_copy(sites);
    // Fit without station i.
    run_filter(track, sites_copy, station_i);
    track->compute_chi2(sites_copy);
    double new_track_chi2 = track->s_chi2();
    if ( new_track_chi2 < old_track_chi2 ) {
      // Compute inovation in local misalignments when station is removed.
      track->update_misaligments(sites, sites_copy, station_i);
      int site_i = 3*(station_i)-1;
      kalman_align.update(sites_copy[site_i]);
    }
  }
  kalman_align.save();
}

void KalmanTrackFit::initialise(KalmanSeed *seed,
                                std::vector<KalmanSite> &sites,
                                KalmanSciFiAlignment &kalman_align) {
  TMatrixD a0 = seed->get_initial_state_vector();

  TMatrixD C(5, 5);
  C.Zero();
  C(0, 0) = 2.;        // x covariance
  C(1, 1) = _seed_cov; // dummy covariance
  C(2, 2) = 2.;        // y covariance
  C(3, 3) = _seed_cov; // dummy covariance
  C(4, 4) = _seed_cov; // dummy covariance

  std::vector<SciFiCluster*> clusters = seed->get_clusters();
  KalmanSite first_plane;
  first_plane.set_a(a0, KalmanSite::Projected);
  first_plane.set_covariance_matrix(C, KalmanSite::Projected);
  first_plane.set_measurement(clusters[0]->get_alpha());
  first_plane.set_direction(clusters[0]->get_direction());
  first_plane.set_z(clusters[0]->get_position().z());
  int id_0 = clusters[0]->get_id();
  first_plane.set_id(id_0);
  first_plane.set_input_shift(kalman_align.get_shifts(id_0));
  first_plane.set_input_shift_covariance(kalman_align.get_cov_shifts(id_0));
  sites.push_back(first_plane);

  size_t numb_sites = clusters.size();
  for ( size_t j = 1; j < numb_sites; ++j ) {
    KalmanSite a_site;
    a_site.set_measurement(clusters[j]->get_alpha());
    a_site.set_direction(clusters[j]->get_direction());
    a_site.set_z(clusters[j]->get_position().z());
    int id = clusters[j]->get_id();
    a_site.set_id(id);
    a_site.set_input_shift(kalman_align.get_shifts(id));
    a_site.set_input_shift_covariance(kalman_align.get_cov_shifts(id));
    sites.push_back(a_site);
  }

  for ( size_t j = 0; j < numb_sites; ++j ) {
    Hep3Vector true_position = clusters[j]->get_true_position();
    Hep3Vector true_momentum = clusters[j]->get_true_momentum();
    sites[j].set_true_position(true_position);
    sites[j].set_true_momentum(true_momentum);
  }
}

void KalmanTrackFit::run_filter(KalmanTrack *track, std::vector<KalmanSite> &sites) {
  size_t numb_measurements = sites.size();
  for ( size_t j = 1; j < numb_measurements; ++j ) {
    // Predict the state vector at site i...
    track->extrapolate(sites, j);
    // ... Filter...
    track->filter(sites, j);
  }
  track->prepare_for_smoothing(&sites.back());
  // ...and Smooth back all sites.
  for ( int k = static_cast<int> (numb_measurements-2); k > -1; --k ) {
    track->smooth(sites, k);
  }
}

void KalmanTrackFit::run_filter(KalmanTrack *track, std::vector<KalmanSite> &sites, int ignore_i) {
  size_t numb_measurements = sites.size();

  size_t site_removed_1 = 3*(ignore_i-1);
  size_t site_removed_2 = site_removed_1 + 1;
  size_t site_removed_3 = site_removed_1 + 2;
  for ( size_t j = 1; j < numb_measurements; ++j ) {
    // Predict the state vector at site i...
    track->extrapolate(sites, j);
    // ... Filter...
    if ( j!=site_removed_1 && j!=site_removed_2 && j!=site_removed_3 ) {
      track->filter(sites, j);
    } else {
      filter_virtual(sites[j]);
    }
  }
  track->prepare_for_smoothing(&sites.back());
  // ...and Smooth back all sites.
  for ( int k = static_cast<int> (numb_measurements-2); k > -1; --k ) {
    track->smooth(sites, k);
  }
}

void KalmanTrackFit::filter_virtual(KalmanSite &a_site) {
  // Filtered States = Projected States
  TMatrixD C = a_site.get_covariance_matrix(KalmanSite::Projected);
  a_site.set_covariance_matrix(C, KalmanSite::Filtered);

  TMatrixD a = a_site.get_a(KalmanSite::Projected);
  a_site.set_a(a, KalmanSite::Filtered);
  a_site.set_current_state(KalmanSite::Filtered);
}

void KalmanTrackFit::save(const KalmanTrack *kalman_track,
                          std::vector<KalmanSite> sites,
                          SciFiEvent &event) {
  KalmanSite *plane_0 = &sites[0];
  assert(plane_0->get_id()==0 ||plane_0->get_id()==15);

  TMatrixD a = plane_0->get_a(KalmanSite::Smoothed);
  TMatrixD C = plane_0->get_covariance_matrix(KalmanSite::Smoothed);
  CLHEP::Hep3Vector mc_pos = plane_0->get_true_position();
  CLHEP::Hep3Vector mc_mom = plane_0->get_true_momentum();

  int tracker = kalman_track->tracker();
  if ( tracker == 0 ) {
    mc_pos.setX(-1.*mc_pos.getX());
    mc_mom.setX(-1.*mc_mom.getX());
  }

  Json::Value *json = Globals::GetConfigurationCards();
  double energy  = (*json)["simulation_reference_particle"]["energy"].asDouble();
  double emittance_in= (*json)["beam"]["definitions"][static_cast<Json::UInt> (0)]["transverse"]["emittance_4d"].asDouble();

  std::ofstream file;
  file.open ("emittance.txt", std::ios::out | std::ios::app);
  file << energy << "\t" << emittance_in << "\t" << tracker << "\t"
       << a(0,0) << "\t" << C(0,0) << "\t" << mc_pos.getX() << "\t"
       << a(1,0) << "\t" << C(1,1) << "\t" << mc_mom.getX() << "\t"
       << a(2,0) << "\t" << C(2,2) << "\t" << mc_pos.getY() << "\t"
       << a(3,0) << "\t" << C(3,3) << "\t" << mc_mom.getY() << "\t"
       << a(4,0) << "\t" << C(4,4) << "\t" << mc_mom.getZ() << "\n";
  file.close();

  SciFiTrack *track = new SciFiTrack(kalman_track);
  // track->add_track_points(sites);
  event.add_scifitrack(track);
}

} // ~namespace MAUS
