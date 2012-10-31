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

namespace MAUS {

KalmanTrackFitSS::~KalmanTrackFitSS() {}

void KalmanTrackFitSS::process(Json::Value event) {
  // std::vector<double> masses;

  // masses.push_back(pion_mass);
  // masses.push_back(muon_mass);
  // for ( int i = 0; i < masses.size(); ++i ) {
  // double particle_mass = masses[i];
  // for ( double displacement = -200.; displacement < 200.; displacement+=2. ) {
  KalmanMonitor monitor;
  double displacement = 0.0;
  double particle_momentum, particle_mass;

  std::vector<KalmanSite> sites;
  KalmanTrack *track = new KalmanSSTrack();

  // Pass measurements and PR to sites.
  initialise_global_track(event, sites, particle_momentum, particle_mass, displacement);

  // if (particle_mass > 150.) return;
  if (particle_momentum < 100.) return;
  track->set_mass(particle_mass);
  track->set_momentum(particle_momentum);

  // Filter the first state.
  std::cout << "Filtering site 0" << std::endl;
  filter(sites, track, 0);

  for ( int site_i = 1; site_i < 11; site_i++ ) {
    std::cout << "Extrapolating to site " << site_i << std::endl;
    extrapolate(sites, track, site_i);
    if ( site_i == 1 || site_i == 6 || site_i == 7 ||
         site_i == 8 || site_i == 9 || site_i == 10 ) {
      std::cout << "Filtering site "        << site_i << std::endl;
      filter(sites, track, site_i);
    } else {
      std::cout << "Filtering VIRTUAL site "        << site_i << std::endl;
      filter_virtual(sites, site_i);
    }
  }

  int numb_measurements = sites.size();
  // assert(numb_measurements == 3);

  track->prepare_for_smoothing(sites);
  // ...and Smooth back all sites.
  for ( int i = numb_measurements-2; i >= 0; --i ) {
      std::cerr << "Smoothing site " << i << std::endl;
      smooth(sites, track, i);
  }

  track->compute_chi2(sites);
  double chi2 = track->get_chi2();

  std::ofstream out2("displacements.txt", std::ios::out | std::ios::app);
  out2  << displacement << " " << chi2 << "\n";
  out2.close();


  monitor.print_info(sites);
  monitor.fill(sites);

  delete track;
  // }
}

void KalmanTrackFitSS::initialise_global_track(Json::Value event,
                                               std::vector<KalmanSite> &sites,
                                               double &momentum, double &mass, double displacement) {
  double x_pr, y_pr, mx_pr, my_pr;
  perform_elementar_pattern_recon(event,
                                  x_pr, y_pr,
                                  mx_pr, my_pr,
                                  momentum, mass, displacement);

/*
  std::cerr << "Pattern Recognition: " << x_pr << " " << y_pr << " "
            << pr_mom(0) << " " << pr_mom(1) << " " << pr_mom(2) << "\n";
  std::cerr << "TOF0 measurement: " << tof0(0) << " " << tof0(1) << "\n";
  std::cerr << "SS mesurement: "    << ss(0) << " " << ss(1) << " " << ss(2) << "\n";
  std::cerr << "TOF1 measurement: " << tof1(0) << " " << tof1(1) << "\n";
*/
  double p_z = momentum;
  TMatrixD a(5, 1);
  a(0, 0) = x_pr; // mm
  a(1, 0) = mx_pr;
  a(2, 0) = y_pr; // mm
  a(3, 0) = my_pr;
  a(4, 0) = 1./p_z;

  TMatrixD C(5, 5);
  C.Zero();
  for ( int i = 0; i < 5; ++i ) {
     C(i, i) = _seed_cov/20.; // dummy values
  }

  KalmanSite first_site;
  first_site.set_projected_a(a);
  first_site.set_projected_covariance_matrix(C);

  // TOFs channel-renumbering factors
  double tof0_central_slab_number = (10.-1.)/2.;
  double tof1_central_slab_number = (7.-1.)/2.;
  CLHEP::Hep3Vector tof_hor(1., 0., 0.);
  CLHEP::Hep3Vector tof_ver(0., 1., 0.);
  CLHEP::Hep3Vector ss_2(-0.86, -.5, 0.);
  CLHEP::Hep3Vector ss_1(0.86, -.5, 0.);
  CLHEP::Hep3Vector ss_0(0., 1., 0.);

  double tof0_horizontal_z = 569.5;
  double tof0_vertical_z   = 594.5;
  double cherenkov_A_z = 792.8;
  double cherenkov_B_z = 1281.3;
  double vp1_z = 3684.1;
  double vp2_z = 6857.1;
  double ss_2_z  = 7514.05+ displacement;
  double ss_1_z  = 7514.7 + displacement;
  double ss_0_z  = 7515.35+ displacement;
  double tof1_vertical_z  = 8002.2;
  double tof1_horizontal_z= 8027.2;

  double tof0_slabx = event["TOF0"]["slabX"].asDouble();
  double tof0_slaby = event["TOF0"]["slabY"].asDouble();

  double tof1_slabx = event["TOF1"]["slabX"].asDouble();
  double tof1_slaby = event["TOF1"]["slabY"].asDouble();
  double ss_plane0  = event["SS"]["Plane0"].asDouble();
  double ss_plane1  = event["SS"]["Plane1"].asDouble();
  double ss_plane2  = event["SS"]["Plane2"].asDouble();

  // Site 0 - tof0, horizontal
  first_site.set_measurement(tof0_slabx-tof0_central_slab_number);
  first_site.set_direction(tof_hor);
  first_site.set_id(0);
  first_site.set_type(0);
  first_site.set_z(tof0_horizontal_z);
  sites.push_back(first_site);
  // Site 1 - tof0, vertical
  KalmanSite site_1;
  site_1.set_measurement((tof0_slaby-tof0_central_slab_number));
  site_1.set_direction(tof_ver);
  site_1.set_id(1);
  site_1.set_type(0);
  site_1.set_z(tof0_vertical_z);
  sites.push_back(site_1);

  // Site 2 - CHERENKOV A
  KalmanSite site_2;
  site_2.set_measurement(0.0);
  site_2.set_direction((0., 0., 0.));
  site_2.set_id(2);
  site_2.set_z(cherenkov_A_z);
  sites.push_back(site_2);
  // Site 3 - CHERENKOV B
  KalmanSite site_3;
  site_3.set_measurement(0.0);
  site_3.set_direction((0., 0., 0.));
  site_3.set_id(3);
  site_3.set_z(cherenkov_B_z);
  sites.push_back(site_3);

  // Site 4 - VIRTUAL
  KalmanSite site_4;
  site_4.set_measurement(0.0);
  site_4.set_direction((0., 0., 0.));
  site_4.set_id(4);
  site_4.set_z(vp1_z);
  sites.push_back(site_4);
  // Site 5 - VIRTUAL
  KalmanSite site_5;
  site_5.set_measurement(0.0);
  site_5.set_direction((0., 0., 0.));
  site_5.set_id(5);
  site_5.set_z(vp2_z);
  sites.push_back(site_5);

  // Site 6 - plane2
  KalmanSite site_6;
  site_6.set_measurement(ss_plane2);
  site_6.set_direction(ss_2);
  site_6.set_id(6);
  site_6.set_type(2);
  site_6.set_z(ss_2_z);
  sites.push_back(site_6);
  // Site 7 - plane1
  KalmanSite site_7;
  site_7.set_measurement(ss_plane1);
  site_7.set_direction(ss_1);
  site_7.set_id(7);
  site_7.set_type(2);
  site_7.set_z(ss_1_z);
  sites.push_back(site_7);
  // Site 8 - plane0
  KalmanSite site_8;
  site_8.set_measurement(ss_plane0);
  site_8.set_direction(ss_0);
  site_8.set_id(8);
  site_8.set_type(2);
  site_8.set_z(ss_0_z);
  sites.push_back(site_8);

  // Site 9 - tof1, vertical
  KalmanSite site_9;
  site_9.set_measurement((tof1_slaby-tof1_central_slab_number));
  site_9.set_direction(tof_ver);
  site_9.set_id(9);
  site_9.set_type(1);
  site_9.set_z(tof1_vertical_z);
  sites.push_back(site_9);
  // Site 10 - tof1, horizontal
  KalmanSite site_10;
  site_10.set_measurement(tof1_slabx-tof1_central_slab_number);
  site_10.set_direction(tof_hor);
  site_10.set_id(10);
  site_10.set_type(1);
  site_10.set_z(tof1_horizontal_z);
  sites.push_back(site_10);
}

void KalmanTrackFitSS::filter_virtual(std::vector<KalmanSite> &sites,
                                      int current_site) {
  // Get Site...
  KalmanSite *a_site = &sites[current_site];

  // Filtered States = Projected States
  TMatrixD C(5, 5);
  C = a_site->get_projected_covariance_matrix();
  a_site->set_covariance_matrix(C);

  TMatrixD a(5, 1);
  a = a_site->get_projected_a();
  a_site->set_a(a);
  if ( current_site == 4 || current_site == 5 ) {
    assert(a(0, 0) < 200. && a(2, 0) < 200. && "Assert extrapolation is reasonable.");
  }
}

void KalmanTrackFitSS::perform_elementar_pattern_recon(Json::Value event,
                                                       double &x_pr, double &y_pr,
                                                       double &mx_pr, double &my_pr,
                                                       double &p_z, double &mass, double ss_displacement) {
  double protron_mass = 938.272; // MeV/c
  double pion_mass    = 139.6; // MeV/c
  double muon_mass    = 105.7; // MeV/c
  double positron_mass= 0.511; // MeV/c

  // Run elementar TOF Recon.
  double tof0_slabx = event["TOF0"]["slabX"].asDouble();
  double tof0_slaby = event["TOF0"]["slabY"].asDouble();
  double tof0_time  = event["TOF0"]["time"].asDouble();
  double tof1_slabx = event["TOF1"]["slabX"].asDouble();
  double tof1_slaby = event["TOF1"]["slabY"].asDouble();
  double tof1_time  = event["TOF1"]["time"].asDouble();
  double ss_x  = event["SS"]["position"]["x"].asDouble();
  double ss_y  = event["SS"]["position"]["y"].asDouble();
  double numb_sp  = event["number_ss_spacepoints"].asDouble();

  double tof0_x = -(tof0_slaby - (tof0_num_slabs - 1.)/2.)*tof0_a;
  double tof0_y =  (tof0_slabx - (tof0_num_slabs - 1.)/2.)*tof0_a;
  // Hep3Vector tof0_sp(tof0_x, tof0_y, tof0_time);

  double tof1_x = -(tof1_slaby - (tof1_num_slabs - 1.)/2.)*tof1_a;
  double tof1_y =  (tof1_slabx - (tof1_num_slabs - 1.)/2.)*tof1_a;
  //  Hep3Vector tof1_sp(tof1_x, tof1_y, tof1_time);

  // Basic PR
  // 7824.1 -> my old guess
  double distance_TOFs = 7432.7;    // mm
  double distance_TOF0_SS = distance_TOFs - 500.+ss_displacement; // mm
  double delta_x = (tof1_x-tof0_x); // mm
  double delta_y = (tof1_y-tof0_y); // mm
  double delta_z = distance_TOFs;   // mm
  double deltaT = tof1_time - tof0_time; // ns
  x_pr = tof0_x; // mm
  y_pr = tof0_y; // mm
  mx_pr   = delta_x/delta_z;
  my_pr   = delta_y/delta_z;

  std::cerr << tof1_x << " " << tof0_x+distance_TOFs*mx_pr << "\n";
  std::cerr << tof1_y << " " << tof0_y+distance_TOFs*my_pr << "\n";

  assert(tof1_x == tof0_x+distance_TOFs*mx_pr);
  assert(tof1_y == tof0_y+distance_TOFs*my_pr);

  // std::cerr << "DeltaT: " << deltaT << "\n";
  if ( deltaT > 25. && deltaT < 26.5 ) {
    mass = positron_mass;
  } else if ( deltaT > 25. && deltaT < 29.5 ) {
    mass = muon_mass;
  } else if ( deltaT > 29.5 && deltaT < 40. ) {
    mass = pion_mass;
  } else if ( deltaT > 40. ) {
    mass = protron_mass;
  } else {
    assert(false && "timing issue");
  }

  double v_z = (distance_TOFs/deltaT)*1000000.;
  double c = 300000000.;
  double beta = v_z/c;
  double gamma = pow(1.-beta*beta, -0.5);
  p_z   = gamma*mass*beta;

  std::ofstream out2("detectors.txt", std::ios::out | std::ios::app);
  out2  << deltaT << " " << mass << " " << p_z << " " << numb_sp << " "
        << tof0_x << " " << tof0_y << " "
        << tof1_x << " " << tof1_y << " " << ss_x << " " << ss_y << "\n";
  out2.close();
}
}
