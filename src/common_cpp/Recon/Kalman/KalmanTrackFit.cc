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

KalmanTrackFit::KalmanTrackFit(): _seed_cov(200.) {
  //
  // Get Configuration values.
  //
  Json::Value *json = Globals::GetConfigurationCards();
  _seed_cov             = (*json)["SciFiSeedCovariance"].asDouble();
  _update_misalignments = (*json)["SciFiUpdateMisalignments"].asBool();
  _type_of_dataflow     = (*json)["type_of_dataflow"].asString();
  _use_MCS              = (*json)["SciFiKalman_use_MCS"].asBool();
  _use_Eloss            = (*json)["SciFiKalman_use_Eloss"].asBool();
  _verbose              = (*json)["SciFiKalmanVerbose"].asBool();
}

KalmanTrackFit::~KalmanTrackFit() {}

void KalmanTrackFit::Process(std::vector<KalmanSeed*> seeds,
                             SciFiEvent &event) {
  //
  // Initialise Alignment object
  // This loads misalignments and may
  // be used to conduct a misalignment search,
  // if the flag is set to do so.
  //
  KalmanSciFiAlignment kalman_align;
  kalman_align.LoadMisaligments();
  //
  // Prepare to loop over seeds. 1 seed = 1 track hypotesis
  //
  size_t num_seeds = seeds.size();
  for ( size_t i = 0; i < num_seeds; ++i ) {
    // Current seed.
    KalmanSeed* seed = seeds[i];

    // Create pointer to abstract class KalmanTrack.
    KalmanTrack *track = 0;
    if ( seed->is_straight() ) {
      track = new StraightTrack(_use_MCS, _use_Eloss);
    } else if ( seed->is_helical() ) {
      track = new HelicalTrack(_use_MCS, _use_Eloss);
    }
    //
    // Initialize Track's member matrices.
    track->Initialise();
    //
    // Set up KalmanSites to be used. KalmanSite = Measurement Plane
    KalmanSitesVector sites;
    Initialise(seed, sites, kalman_align);

    size_t numb_measurements = sites.size();
    //
    // For now, we are only interested in events where all 15 planes are hit.
    // The high efficiency of the fibers makes this a reasonable assumption.
    if ( numb_measurements != 15 ) continue;
    //
    // Set the momentum of the track hypostesis (in MeV/c)
    double momentum = seed->momentum();
    track->set_momentum(momentum);
    //
    // Run the KALMAN FILTER
    //
    RunFilter(track, sites);
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
    Save(track, sites, event);

    // Misalignment search. Optional.
    // This must be run in single thread mode.
    if ( _update_misalignments && track->f_chi2() < 20. &&
         numb_measurements == 15 && _type_of_dataflow == "pipeline_single_thread") {
      LaunchMisaligmentSearch(track, sites, kalman_align);
    }
    delete track;
    delete seed;
  }
}

void KalmanTrackFit::Initialise(KalmanSeed *seed,
                                KalmanSitesVector &sites,
                                KalmanSciFiAlignment &kalman_align) {
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
  first_plane.set_input_shift(kalman_align.get_shifts(id_0));
  first_plane.set_input_shift_covariance(kalman_align.get_cov_shifts(id_0));
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
    a_site.set_input_shift(kalman_align.get_shifts(id));
    a_site.set_input_shift_covariance(kalman_align.get_cov_shifts(id));
    sites.push_back(a_site);
  }

  for ( size_t j = 0; j < numb_sites; ++j ) {
    ThreeVector true_position = clusters[j]->get_true_position();
    ThreeVector true_momentum = clusters[j]->get_true_momentum();
    sites[j].set_true_position(true_position);
    sites[j].set_true_momentum(true_momentum);
  }
}

void KalmanTrackFit::RunFilter(KalmanTrack *track, KalmanSitesVector &sites) {
  // Filter the first state.
  track->Filter(sites, 0);

  size_t numb_measurements = sites.size();
  for ( size_t j = 1; j < numb_measurements; ++j ) {
    // Predict the state vector at site i...
    track->Extrapolate(sites, j);
    // ... Filter...
    track->Filter(sites, j);
  }
  track->PrepareForSmoothing(&sites.back());
  // ...and Smooth back all sites.
  for ( int k = static_cast<int> (numb_measurements-2); k > -1; --k ) {
    track->Smooth(sites, k);
  }
}

void KalmanTrackFit::RunFilter(KalmanTrack *track, KalmanSitesVector &sites, int ignore_i) {
  size_t numb_measurements = sites.size();

  if ( ignore_i < 2 || ignore_i > 4 ) {
    throw(Squeal(Squeal::recoverable,
          "Bad request.",
          "KalmanTrackFit::RunFilter"));
  }

  size_t site_removed_1 = 3*(ignore_i-1);
  size_t site_removed_2 = site_removed_1 + 1;
  size_t site_removed_3 = site_removed_1 + 2;
  for ( size_t j = 1; j < numb_measurements; ++j ) {
    // Predict the state vector at site i...
    track->Extrapolate(sites, j);
    // ... Filter...
    if ( j != site_removed_1 && j != site_removed_2 && j != site_removed_3 ) {
      track->Filter(sites, j);
    } else {
      FilterVirtual(sites[j]);
    }
  }
  track->PrepareForSmoothing(&sites.back());
  // ...and Smooth back all sites.
  for ( int k = static_cast<int> (numb_measurements-2); k > -1; --k ) {
    track->Smooth(sites, k);
  }
}

void KalmanTrackFit::LaunchMisaligmentSearch(KalmanTrack *track,
                                               KalmanSitesVector &sites,
                                               KalmanSciFiAlignment &kalman_align) {
  //
  // Set up the ROOT output file where we save progress
  // in TGraphs.
  //
  kalman_align.SetUpRootOutput();
  //
  // Get the current track smoothed Chi2.
  //
  double old_track_chi2 = track->s_chi2();
  //
  // Fit excluding a station.
  //
  for ( int station_i = 2; station_i < 5; ++station_i ) {
    KalmanSitesVector sites_copy(sites);
    // Fit without station i.
    RunFilter(track, sites_copy, station_i);
    track->ComputeChi2(sites_copy);
    // Store new misalignment IF Chi2 of the track improves when
    // we run the filter without the station.
    double new_track_chi2 = track->s_chi2();
    if ( new_track_chi2 < old_track_chi2 ) {
      // Compute inovation in local misalignments when station is removed.
      track->UpdateMisaligments(sites, sites_copy, station_i);
      int site_i = 3*(station_i)-1;
      kalman_align.Update(sites_copy[site_i]);
    }
  }
  kalman_align.Save();
  kalman_align.CloseRootFile();
}

void KalmanTrackFit::FilterVirtual(KalmanSite &a_site) {
  // Filtered States = Projected States
  TMatrixD C = a_site.covariance_matrix(KalmanSite::Projected);
  a_site.set_covariance_matrix(C, KalmanSite::Filtered);

  TMatrixD a = a_site.a(KalmanSite::Projected);
  a_site.set_a(a, KalmanSite::Filtered);
  a_site.set_current_state(KalmanSite::Filtered);
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
  //for ( auto kalmansite : sites ) {
  //  SciFiTrackPoint *track_point = new SciFiTrackPoint(&kalmansite);
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
    << "================Residuals================"   << "\n"
    << (site.residual(KalmanSite::Projected))(0, 0)  << "\n"
    << (site.residual(KalmanSite::Filtered))(0, 0)   << "\n"
    << (site.residual(KalmanSite::Smoothed))(0, 0)   << "\n"
    << "=========================================="
    << std::endl;
  }
}

} // ~namespace MAUS
