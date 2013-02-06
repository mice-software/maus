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

KalmanTrackFit::KalmanTrackFit(): _seed_cov(1000.),
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
    // std::cerr << "Filtering site 0" << std::endl;
    track->filter(sites, 0);

    run_filter(track, sites);

    monitor.fill(sites);
    // monitor.print_info(sites);
    track->compute_chi2(sites);
    // track->compute_emittance(sites.front());

    // Misalignment search.
    if ( _update_misalignments && track->get_f_chi2() < 25. && numb_measurements == 15 ) {
      for ( int station_i = 2; station_i < 5; ++station_i ) {
        std::cerr << "Ignoring station " << station_i << std::endl;
        // Fit without station i.
        run_filter(track, sites, station_i);
        // get difference between spacepoint (x, y, phi) and fitted (x, y, phi)
        int plane_i = 3*(station_i)-2;
        track->update_misaligments2(&sites[plane_i], seed, station_i);
        //update misaligment estimation and covariance
        //double st2_x, st2y;
      }
      kalman_align.update(sites);
    }
/*
    if ( _update_misalignments && track->get_f_chi2() < 25. && numb_measurements == 15 ) {
      for ( size_t j = 6; j < numb_measurements; ++j ) {
        track->update_misaligments(sites, j);
      }
      kalman_align.update(sites);
    }
*/
    save(track, sites, event);
    delete track;
  }
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
    // std::cerr << "Extrapolating to site " << j << std::endl;
    track->extrapolate(sites, j);
    // ... Filter...
    // std::cerr << "Filtering site " << j << std::endl;
    track->filter(sites, j);
  }
  track->prepare_for_smoothing(&sites.back());
  // ...and Smooth back all sites.
  for ( int k = static_cast<int> (numb_measurements-2); k > -1; --k ) {
    // std::cerr << "Smoothing site " << k << std::endl;
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
    // std::cerr << "Extrapolating to site " << j << std::endl;
    track->extrapolate(sites, j);
    // ... Filter...
    if ( j!=site_removed_1 && j!=site_removed_2 && j!=site_removed_3 ) {
      // std::cerr << "Filtering site " << j << std::endl;
      track->filter(sites, j);
    } else {
      // std::cerr << "Filtering VIRTUAL site " << j << std::endl;
      filter_virtual(sites[j]);
    }
  }
  track->prepare_for_smoothing(&sites.back());
  // ...and Smooth back all sites.
  for ( int k = static_cast<int> (numb_measurements-2); k > -1; --k ) {
    // std::cerr << "Smoothing site " << k << std::endl;
    track->smooth(sites, k);
  }
}

void KalmanTrackFit::filter_virtual(KalmanSite &a_site) {
  // Filtered States = Projected States
  TMatrixD C(5, 5);
  C = a_site.get_covariance_matrix(KalmanSite::Projected);
  a_site.set_covariance_matrix(C, KalmanSite::Filtered);

  TMatrixD a(5, 1);
  a = a_site.get_a(KalmanSite::Projected);
  a_site.set_a(a, KalmanSite::Filtered);
  a_site.set_current_state(KalmanSite::Filtered);
}

/*
std::vector<KalmanSite> KalmanTrackFit::exclude_station(int station, std::vector<KalmanSite> sites) {
  std::vector<KalmanSite> new_sites = sites;
  int starting_index = 3*(station-1);
  int last_index = starting_index + 3;
  // erase the first 3 elements:
  new_sites.erase (new_sites.begin()+starting_index,new_sites.begin()+last_index)
  return new_sites;
}
*/

void KalmanTrackFit::save(const KalmanTrack *kalman_track,
                          std::vector<KalmanSite> sites,
                          SciFiEvent &event) {
  SciFiTrack *track = new SciFiTrack(kalman_track);
  // track->add_track_points(sites);
  event.add_scifitrack(track);
}

} // ~namespace MAUS
