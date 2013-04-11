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
  _type_of_dataflow     = (*json)["type_of_dataflow"].asString();
  _use_MCS              = (*json)["SciFiKalman_use_MCS"].asBool();
  _use_Eloss            = (*json)["SciFiKalman_use_Eloss"].asBool();
  std::cerr << "---------------------Birth of Kalman Filter--------------------" << std::endl;
}

KalmanTrackFit::~KalmanTrackFit() {
  std::cerr << "---------------------Death of Kalman Filter--------------------" << std::endl;
}

void KalmanTrackFit::Process(std::vector<KalmanSeed*> seeds, SciFiEvent &event) {
  KalmanSciFiAlignment kalman_align;
  kalman_align.LoadMisaligments();

  std::cout << "number of seeds: " << seeds.size() << "\n";
  size_t num_tracks = seeds.size();
  for ( size_t i = 0; i < num_tracks; ++i ) {
    std::cout << "processing track number " << i << "\n";
    // Get the seed.
    KalmanSeed* seed = seeds[i];

    // Create the Track object.
    KalmanTrack *track = 0;
    if ( seed->is_straight() ) {
      track = new StraightTrack(_use_MCS, _use_Eloss);
    } else if ( seed->is_helical() ) {
      track = new HelicalTrack(_use_MCS, _use_Eloss);
    }

    // Initialize member matrices.
    track->Initialise();

    // Set up KalmanSites to be used.
    std::vector<KalmanSite> sites;
    Initialise(seed, sites, kalman_align);

    size_t numb_measurements = sites.size();
    std::cout << numb_measurements << std::endl;
    if ( numb_measurements != 15 ) continue;

    double momentum = seed->momentum(); // MeV/c
    track->set_momentum(momentum);

    RunFilter(track, sites);

    track->ComputeChi2(sites);
    DumpInfo(sites);
    Save(track, sites, event);

    // Misalignment search.
    if ( _update_misalignments && track->f_chi2() < 20. && numb_measurements == 15 ) {
      // if ( _type_of_dataflow != "pipeline_single_thread" ) throw
      LaunchMisaligmentSearch(track, sites, kalman_align);
    }
    delete track;
  }
}

void KalmanTrackFit::Initialise(KalmanSeed *seed,
                                std::vector<KalmanSite> &sites,
                                KalmanSciFiAlignment &kalman_align) {
  TMatrixD a0 = seed->initial_state_vector();

  int n_param = seed->n_parameters();

  TMatrixD C(n_param, n_param);
  C.Zero();
  for ( int i = 0; i < n_param; i++ )
    C(i, i) = _seed_cov;

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
    Hep3Vector true_position = clusters[j]->get_true_position();
    Hep3Vector true_momentum = clusters[j]->get_true_momentum();
    sites[j].set_true_position(true_position);
    sites[j].set_true_momentum(true_momentum);
  }
}

void KalmanTrackFit::RunFilter(KalmanTrack *track, std::vector<KalmanSite> &sites) {
  // Filter the first state.
  track->Filter(sites, 0);

  size_t numb_measurements = sites.size();
  for ( size_t j = 1; j < numb_measurements; ++j ) {
    // Predict the state vector at site i...
    std::cout << "Extrapolating " << j << std::endl;
    track->Extrapolate(sites, j);
    // ... Filter...
    std::cout << "Filtering " << j << std::endl;
    track->Filter(sites, j);
  }
  track->PrepareForSmoothing(&sites.back());
  // ...and Smooth back all sites.
  for ( int k = static_cast<int> (numb_measurements-2); k > -1; --k ) {
    std::cout << "Smoothing " << k << std::endl;
    track->Smooth(sites, k);
  }
}

void KalmanTrackFit::RunFilter(KalmanTrack *track, std::vector<KalmanSite> &sites, int ignore_i) {
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
                                               std::vector<KalmanSite> &sites,
                                               KalmanSciFiAlignment &kalman_align) {
  // kalman_align.SetRootOutput();
  double old_track_chi2 = track->s_chi2();
  for ( int station_i = 2; station_i < 5; ++station_i ) {
    std::vector<KalmanSite> sites_copy(sites);
    // Fit without station i.
    RunFilter(track, sites_copy, station_i);
    track->ComputeChi2(sites_copy);
    double new_track_chi2 = track->s_chi2();
    if ( new_track_chi2 < old_track_chi2 ) {
      // Compute inovation in local misalignments when station is removed.
      track->UpdateMisaligments(sites, sites_copy, station_i);
      int site_i = 3*(station_i)-1;
      kalman_align.Update(sites_copy[site_i]);
    }
  }
  // kalman_align.CloseRootFile();
  kalman_align.Save();
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
                          std::vector<KalmanSite> sites,
                          SciFiEvent &event) {
/*
  KalmanSite *plane_0 = &sites[0];
  assert(plane_0->get_id() == 0 || plane_0->get_id() == 15);

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
  double emittance_in= (*json)["beam"]["definitions"]
                       [static_cast<Json::UInt> (0)]["transverse"]
                       ["emittance_4d"].asDouble();

  std::ofstream file;
  file.open("emittance.txt", std::ios::out | std::ios::app);
  file << energy << "\t" << emittance_in << "\t" << tracker << "\t"
       << a(0, 0) << "\t" << C(0, 0) << "\t" << mc_pos.getX() << "\t"
       << a(1, 0) << "\t" << C(1, 1) << "\t" << mc_mom.getX() << "\t"
       << a(2, 0) << "\t" << C(2, 2) << "\t" << mc_pos.getY() << "\t"
       << a(3, 0) << "\t" << C(3, 3) << "\t" << mc_mom.getY() << "\t"
       << a(4, 0) << "\t" << C(4, 4) << "\t" << mc_mom.getZ() << "\n";
  file.close();
*/
  SciFiTrack *track = new SciFiTrack(kalman_track);


  size_t n_sites = sites.size();
  for ( size_t i = 0; i < n_sites; ++i ) {
    SciFiTrackPoint *track_point = new SciFiTrackPoint(&sites[i]);
    track->add_scifitrackpoint(track_point);
  }

  event.add_scifitrack(track);
}

void KalmanTrackFit::DumpInfo(std::vector<KalmanSite> const &sites) {
  size_t numb_sites = sites.size();

  for ( size_t i = 0; i < numb_sites; ++i ) {
    KalmanSite site = sites[i];
    std::cerr << "==========================================" << std::endl;
    std::cerr << "SITE ID: " << site.id() << std::endl;
    std::cerr << "SITE Z: " << site.z() << std::endl;
    std::cerr << "Momentum: " << site.true_momentum() << std::endl;
    site.a(KalmanSite::Projected).Print();
    // std::cerr << "SITE Z: " << site.get_z() << std::endl;
    // std::cerr << "SITE Direction: " << "(" << site.get_direction().x() << ", " <<
    //                                   site.get_direction().y() << ", " <<
    //                                   site.get_direction().z() << ")" << std::endl;
    std::cerr << "Measurement: " << (site.measurement())(0, 0) << std::endl;
    // std::cerr << "Shift: " <<  "(" << (site.get_input_shift_A())(0, 0) << ", " <<
    //                                  (site.get_input_shift_A())(1, 0) << ", " <<
    //                                  (site.get_input_shift_A())(2, 0) << ")" << std::endl;
    std::cerr << "================Residuals================" << std::endl;
    std::cerr << (site.residual(KalmanSite::Projected))(0, 0) << std::endl;
    std::cerr << (site.residual(KalmanSite::Filtered))(0, 0) << std::endl;
    std::cerr << (site.residual(KalmanSite::Smoothed))(0, 0) << std::endl;
    // std::cerr << "================Projection================" << std::endl;
    // site.get_a(KalmanSite::Projected).Print();
    // site.get_a(KalmanSite::Filtered).Print();
    // site.get_a(KalmanSite::Smoothed).Print();
    // site.get_a().Print();
    // site.get_smoothed_a().Print();
    // site.get_projected_covariance_matrix().Print();
    // std::cerr << "=================Filtered=================" << std::endl;
    // site.get_a().Print();
    // site.get_covariance_matrix().Print();
    std::cerr << "==========================================" << std::endl;
  }
}

} // ~namespace MAUS
