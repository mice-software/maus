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

#include "src/common_cpp/Recon/SingleStation/KalmanTrackFitSS.hh"
#include <math.h>


// namespace MAUS {
KalmanTrackFitSS::KalmanTrackFitSS() {
  std::cout << "---------------------Birth of Kalman Filter--------------------" << std::endl;
}

KalmanTrackFitSS::~KalmanTrackFitSS() {
  std::cout << "---------------------Death of Kalman Filter--------------------" << std::endl;
}

//
// Global track fit.
//
void KalmanTrackFitSS::process(CLHEP::Hep3Vector &tof0,
                               CLHEP::Hep3Vector &ss,
                               CLHEP::Hep3Vector &tof1,
                               CLHEP::Hep3Vector &pr_pos,
                               CLHEP::Hep3Vector &pr_mom) {
  // For now, do only triplets
  int numb_sites = 0;
  for ( int i = 0; i < 3; ++i ) {
    if ( ss(i) != 666 ) {
      numb_sites += 1;
    }
  }
  if ( numb_sites < 3 ) return;

  std::vector<KalmanSite> sites;
  KalmanSSTrack *track = new KalmanSSTrack();

  // Pass measurements and PR to sites.
  initialise_global_track(tof0, ss, tof1, pr_pos, pr_mom, sites);

  // Filter the first state.
  std::cout << "Filtering site 0" << std::endl;
  filter(sites, track, 0);

  // Extrapolate to TOF0 horizontal slabs
  std::cout << "Extrapolating to site " << 1 << std::endl;
  extrapolate(sites, track, 1);
  std::cout << "Filtering site "        << 1 << std::endl;
  filter(sites, track, 1);

  //Extrapolate to Cherenkov A
  std::cout << "Extrapolating to site " << 2 << std::endl;
  extrapolate(sites, track, 2);
  std::cout << "Filtering VIRTUAL site "        << 2 << std::endl;
  filter_virtual(sites, track, 2);
  //Extrapolate to Cherenkov B
  std::cout << "Extrapolating to site " << 3 << std::endl;
  extrapolate(sites, track, 3);
  std::cout << "Filtering VIRTUAL site "        << 3 << std::endl;
  filter_virtual(sites, track, 3);

  //Extrapolate to first virtual plane
  std::cout << "Extrapolating to site " << 4 << std::endl;
  extrapolate(sites, track, 4);
  std::cout << "Filtering VIRTUAL site "        << 4 << std::endl;
  filter_virtual(sites, track, 4);
  //Extrapolate to second virtual plane
  std::cout << "Extrapolating to site " << 5 << std::endl;
  extrapolate(sites, track, 5);
  std::cout << "Filtering VIRTUAL site "        << 5 << std::endl;
  filter_virtual(sites, track, 5);

  // Single Station
  std::cout << "Extrapolating to site " << 6 << std::endl;
  extrapolate(sites, track, 6);
  std::cout << "Filtering site "        << 6 <<std::endl;
  filter(sites, track, 6);

  std::cout << "Extrapolating to site " << 7 << std::endl;
  extrapolate(sites, track, 7);
  std::cout << "Filtering site "        << 7 <<std::endl;
  filter(sites, track, 7);

  std::cout << "Extrapolating to site " << 8 << std::endl;
  extrapolate(sites, track, 8);
  std::cout << "Filtering site "        << 8 <<std::endl;
  filter(sites, track, 8);

  // TOF1
  std::cout << "Extrapolating to site " << 9 << std::endl;
  extrapolate(sites, track, 9);
  std::cout << "Filtering site "        << 9 << std::endl;
  filter(sites, track, 9);

  std::cout << "Extrapolating to site " << 10 << std::endl;
  extrapolate(sites, track, 10);
  std::cout << "Filtering site "        << 10 << std::endl;
  filter(sites, track, 10);

/*
  int numb_measurements = sites.size();
  assert(numb_measurements == 3);

  for ( int i = 1; i < numb_measurements; ++i ) {
    // Predict the state vector at site i...
    std::cout << "Extrapolating to site " << i << std::endl;
    extrapolate(sites, track, i);
    // ... Filter...
    std::cout << "Filtering site " << i << std::endl;
    filter(sites, track, i);
  }

  // ...and Smooth back all sites.
  for ( int i = numb_measurements-2; i >= 0; --i ) {
    std::cerr << "Smoothing site " << i << std::endl;
    smooth(sites, track, i);
  }
*/

  KalmanMonitor monitor;
  monitor.save(sites);
  monitor.print_info(sites);
  delete track;
}

void KalmanTrackFitSS::initialise_global_track(CLHEP::Hep3Vector &tof0, CLHEP::Hep3Vector &ss,
                                               CLHEP::Hep3Vector &tof1,
                                               CLHEP::Hep3Vector &pr_pos, CLHEP::Hep3Vector &pr_mom,
                                               std::vector<KalmanSite> &sites) {
  // double ss_tof1_sep = 60.0; // cm
  // double ss_tof0_sep = 720.0; // cm

  double x_pr  = pr_pos(0);
  double y_pr  = pr_pos(1);
  double p_z   = pr_mom(2);
  double mx_pr = pr_mom(0);
  double my_pr = pr_mom(1);

/*
  std::cerr << "Pattern Recognition: " << x_pr << " " << y_pr << " "
            << pr_mom(0) << " " << pr_mom(1) << " " << pr_mom(2) << "\n";
  std::cerr << "TOF0 measurement: " << tof0(0) << " " << tof0(1) << "\n";
  std::cerr << "SS mesurement: "    << ss(0) << " " << ss(1) << " " << ss(2) << "\n";
  std::cerr << "TOF1 measurement: " << tof1(0) << " " << tof1(1) << "\n";
*/

  TMatrixD a(5, 1);
  a(0, 0) = x_pr; // mm
  a(1, 0) = mx_pr;
  a(2, 0) = y_pr; // mm
  a(3, 0) = my_pr;
  a(4, 0) = 1./p_z;

  TMatrixD C(5, 5);
  C(0, 0) = 1000.;
  C(1, 1) = 1000.;
  C(2, 2) = 1000.;
  C(3, 3) = 1000.;
  C(4, 4) = 1000.;

  KalmanSite first_site;
  first_site.set_projected_a(a);
  first_site.set_projected_covariance_matrix(C);
  // first_site.set_z(clusters[0]->get_position().z());

  int numb_sites = 4;
  for ( int i = 0; i < 3; ++i ) {
    if ( ss(i) != 666 ) {
      numb_sites += 1;
    }
  }

  std::cerr << "Number of sites: " << numb_sites << std::endl;
  // TOFs channel-renumbering factors
  double tof0_central_slab_number = (10.-1.)/2.;
  double tof1_central_slab_number = (7.-1.)/2.;
  CLHEP::Hep3Vector tof_hor(1., 0., 0.);
  CLHEP::Hep3Vector tof_ver(0., 1., 0.);
  CLHEP::Hep3Vector ss_2(-0.86, -.5, 0.);
  CLHEP::Hep3Vector ss_1(0.86, -.5, 0.);
  CLHEP::Hep3Vector ss_0(0., 1., 0.);
  double tof0_horizontal_z = 0.;
  double tof0_vertical_z   = 12.5;
  double vp1_z = 3911.85055;
  double vp2_z = 7078.6;
  double ss_2_z  = 7207.9-0.6273;
  double ss_1_z  = 7207.9;
  double ss_0_z  = 7207.9+0.6273;
  double tof1_vertical_z  = 3924.55+3899.55-12.5/2.;
  double tof1_horizontal_z= 3924.55+3899.55+12.5/2.;

  double cherenkov_A_z = 1;
  double cherenkov_B_z = 2;

  // Site 0 - tof0, horizontal
  first_site.set_measurement(tof0(0)-tof0_central_slab_number);
  first_site.set_direction(tof_hor);
  first_site.set_id(0);
  first_site.set_z(tof0_horizontal_z);
  sites.push_back(first_site);
  // Site 1 - tof0, vertical
  KalmanSite site_1;
  site_1.set_measurement(9-tof0(1)-tof0_central_slab_number);
  site_1.set_direction(tof_ver);
  site_1.set_id(1);
  site_1.set_z(tof0_vertical_z);
  sites.push_back(site_1);

  // Site 2 - VIRTUAL
  KalmanSite site_2;
  site_2.set_measurement(0.0);
  site_2.set_direction((0., 1., 0.));
  site_2.set_id(2);
  site_2.set_z(vp1_z);
  sites.push_back(site_2);
  // Site 3 - VIRTUAL
  KalmanSite site_3;
  site_3.set_measurement(0.0);
  site_3.set_direction((0., 1., 0.));
  site_3.set_id(3);
  site_3.set_z(vp2_z);
  sites.push_back(site_3);

  // Site 4 - plane2
  KalmanSite site_4;
  site_4.set_measurement(ss(2));
  site_4.set_direction(ss_2);
  site_4.set_id(4);
  site_4.set_z(ss_2_z);
  sites.push_back(site_4);
  // Site 5 - plane1
  KalmanSite site_5;
  site_5.set_measurement(ss(1));
  site_5.set_direction(ss_1);
  site_5.set_id(5);
  site_5.set_z(ss_1_z);
  sites.push_back(site_5);
  // Site 6 - plane0
  KalmanSite site_6;
  site_6.set_measurement(ss(0));
  site_6.set_direction(ss_0);
  site_6.set_id(6);
  site_6.set_z(ss_0_z);
  sites.push_back(site_6);

  // Site 7 - tof1, vertical
  KalmanSite site_7;
  site_7.set_measurement(6-tof1(1)-tof1_central_slab_number);
  site_7.set_direction(tof_ver);
  site_7.set_id(7);
  site_7.set_z(tof1_vertical_z);
  sites.push_back(site_7);
  // Site 8 - tof1, horizontal
  KalmanSite site_8;
  site_8.set_measurement(tof1(0)-tof1_central_slab_number);
  site_8.set_direction(tof_hor);
  site_8.set_id(8);
  site_8.set_z(tof1_horizontal_z);
  sites.push_back(site_8);
/*
  for ( int j = 1; j < numb_sites; ++j ) {
    KalmanSite a_site;
    a_site.set_measurement(clusters[j]->get_alpha());
    a_site.set_direction(clusters[j]->get_direction());
    a_site.set_z(clusters[j]->get_position().z());
    a_site.set_id(clusters[j]->get_id());
    sites.push_back(a_site);
  }
*/
}

void KalmanTrackFitSS::filter_virtual(std::vector<KalmanSite> &sites,
                                      KalmanSSTrack *track, int current_site) {
  // Get Site...
  KalmanSite *a_site = &sites[current_site];

  // Filtered States = Projected States
  TMatrixD C(5, 5);
  C = a_site->get_projected_covariance_matrix();
  a_site->set_covariance_matrix(C);

  TMatrixD a(5, 1);
  a = a_site->get_projected_a();
  a_site->set_a(a);
}

void KalmanTrackFitSS::filter(std::vector<KalmanSite> &sites,
                            KalmanSSTrack *track, int current_site) {
  // Get Site...
  KalmanSite *a_site = &sites[current_site];

  // Update measurement error:
  // (non-const std for the perp direction)
  track->update_G(a_site);

  // Update H (depends on plane direction.
  track->update_H(a_site);

  // Ck = ( (C_k^k-1)-1 + HT*G*H )-1
  track->update_covariance(a_site);

  // a_k = a_k^k-1 + K_k x pull
  track->calc_filtered_state(a_site);
}

void KalmanTrackFitSS::extrapolate(std::vector<KalmanSite> &sites, KalmanSSTrack *track, int i) {
  // Get current site...
  KalmanSite *new_site = &sites[i];

  // ... and the site we will extrapolate from.
  KalmanSite *old_site = &sites[i-1];

  // Calculate the system noise...
  track->calc_system_noise(old_site);

  // The propagator matrix...
  track->update_propagator(old_site, new_site);

  // Now, calculate prediction.
  track->calc_predicted_state(old_site, new_site);

  // .. so has the sistem noise matrix...
  // track.calc_system_noise(old_site, new_site);
  // ... so that we can compute the prediction for the
  // covariance matrix.
  track->calc_covariance(old_site, new_site);
}
/*
void KalmanTrackFit::smooth(std::vector<KalmanSite> &sites, KalmanTrack *track, int k) {
  // Get site to be smoothed...
  KalmanSite *smoothing_site = &sites[k];

  // ... and the already perfected site.
  KalmanSite *optimum_site = &sites[k+1];

  // Set the propagator right.
  track->update_propagator(optimum_site, smoothing_site);

  // Compute A_k.
  track->update_back_transportation_matrix(optimum_site, smoothing_site);

  // Compute smoothed a_k and C_k.
  track->smooth_back(optimum_site, smoothing_site);
}
*/
// }
