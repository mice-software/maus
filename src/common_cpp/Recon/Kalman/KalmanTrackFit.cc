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
#include <math.h>

#define PI 3.14159265

KalmanTrackFit::KalmanTrackFit() {
  std::cout << "---------------------Birth of Kalman Filter--------------------" << std::endl;
}

KalmanTrackFit::~KalmanTrackFit() {
  std::cout << "---------------------Death of Kalman Filter--------------------" << std::endl;
}

bool sort_by_id(SciFiCluster *a, SciFiCluster *b ) {
  int tracker = a->get_tracker();
  if ( tracker == 0 ) {
  //  Descending site number.
  return ( a->get_id() > b->get_id() );
  } else if ( tracker == 1 ) {
  //  Ascending site number.
  return ( a->get_id() < b->get_id() );
  }
}

//
// Global track fit.
//
void KalmanTrackFit::process(Hep3Vector &tof0, Hep3Vector &se, Hep3Vector &tof1) {
/*  std::vector<KalmanSite> sites;
  KalmanTrack *track = new GlobalTrack();
  initialise_global_track(tof0, se, tof1, sites);
  // Filter the first state.
  std::cout << "Filtering site 0" << std::endl;
  filter(sites, track, 0);

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

  KalmanMonitor monitor;
  monitor.save_global_track(sites);
  monitor.print_info(sites);
  delete track;
*/
}

void KalmanTrackFit::initialise_global_track(Hep3Vector &tof0, Hep3Vector &se,
                                             Hep3Vector &tof1, std::vector<KalmanSite> &sites) {
  double se_tof1_sep = 60.0; // cm
  double se_tof0_sep = 720.0; // cm

  double mx_pr = ( tof1.x()-se.x() ) / se_tof1_sep;
  double x_pr = se_tof0_sep*(1.-mx_pr);
  double my_pr = ( tof1.y()-se.y() ) / se_tof1_sep;
  double y_pr = se_tof0_sep*(1.-my_pr);
  double p_z  = 423.0; // MeV/c

  // std::vector<SciFiSpacePoint> spacepoints = seed.get_spacepoints();
  // std::vector<SciFiCluster*> clusters;
  // process_clusters(spacepoints, clusters);
  // the clusters are sorted by now.

  int numb_sites = 3; // clusters.size();

  /* double z = clusters[0]->get_position().z();
  // int tracker = clusters[0]->get_tracker();
  double mx, my, x, y;
  if ( tracker == 0 ) {
    mx = mx_pr;
    my = my_pr;
    x  = - x_pr - mx*z;
    y  = - y_pr - my*z;
  } else if ( tracker == 1 ) {
    mx = mx_pr;
    my = my_pr;
    x  = x_pr + mx*z;
    y  = y_pr + my*z;
  }

  KalmanSite first_site;
  TMatrixD a(5, 1);
  a(0, 0) = x_pr;
  a(1, 0) = mx_pr;
  a(2, 0) = y_pr;
  a(3, 0) = my_pr;
  a(4, 0) = 1/p_z;
  first_plane.set_projected_a(a);

  TMatrixD C(5, 5);
  C(0, 0) = 150.*150./12.;
  C(1, 1) = 150.*150./12.;
  C(2, 2) = 10.;
  C(3, 3) = 10.;
  C(4, 4) = 100.;

  first_plane.set_projected_covariance_matrix(C);
  first_plane.set_measurement(clusters[0]->get_alpha());
  first_plane.set_direction(clusters[0]->get_direction());
  first_plane.set_z(clusters[0]->get_position().z());
  first_plane.set_id(clusters[0]->get_id());
  // first_plane
  sites.push_back(first_plane);

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

//
// Straight track fit.
//
void KalmanTrackFit::process(std::vector<SciFiStraightPRTrack> straight_tracks) {
  int num_tracks = straight_tracks.size();

  for ( int i = 0; i < num_tracks; ++i ) {
    std::vector<KalmanSite> sites;

    SciFiStraightPRTrack seed = straight_tracks[i];
    KalmanTrack *track = new StraightTrack();
    initialise(seed, sites);

    // Filter the first state.
    std::cerr << "Filtering site 0" << std::endl;
    filter(sites, track, 0);

    int numb_measurements = sites.size();

    assert(numb_measurements < 16);

    for ( int i = 1; i < numb_measurements; ++i ) {
      // Predict the state vector at site i...
      std::cerr << "Extrapolating to site " << i << std::endl;
      extrapolate(sites, track, i);
      // ... Filter...
      std::cerr << "Filtering site " << i << std::endl;
      filter(sites, track, i);
    }
    sites[numb_measurements-1].set_smoothed_a(sites[numb_measurements-1].get_a());
    // ...and Smooth back all sites.
    for ( int i = numb_measurements-2; i >= 0; --i ) {
      std::cerr << "Smoothing site " << i << std::endl;
      smooth(sites, track, i);
    }

    KalmanMonitor monitor;
    // monitor.save(sites);
    // monitor.save_mc(sites);
    // monitor.print_info(sites);
    delete track;
  }
}

//
// Helical track fit.
//
void KalmanTrackFit::process(std::vector<SciFiHelicalPRTrack> helical_tracks) {
  int num_tracks  = helical_tracks.size();

  for ( int i = 0; i < num_tracks; ++i ) {
    std::vector<KalmanSite> sites;

    SciFiHelicalPRTrack seed = helical_tracks[i];
    KalmanTrack *track = new HelicalTrack(seed);
    initialise(seed, sites);

    // Filter the first state.
    std::cerr << "Filtering site 0" << std::endl;
    filter(sites, track, 0);

    int numb_measurements = sites.size();

    assert(numb_measurements < 16);

    for ( int i = 1; i < numb_measurements; ++i ) {
      // Predict the state vector at site i...
      std::cerr << "Extrapolating to site " << i << std::endl;
      extrapolate(sites, track, i);
      // ... Filter...
      std::cerr << "Filtering site " << i << std::endl;
      filter(sites, track, i);
    }
    sites[numb_measurements-1].set_smoothed_a(sites[numb_measurements-1].get_a());
    // ...and Smooth back all sites.
    for ( int i = numb_measurements-2; i >= 0; --i ) {
      std::cerr << "Smoothing site " << i << std::endl;
      smooth(sites, track, i);
    }

    KalmanMonitor monitor;
    // monitor.save(sites);
    monitor.save_mc(sites);
    monitor.print_info(sites);
    delete track;
  }
}

void KalmanTrackFit::initialise(SciFiHelicalPRTrack &seed, std::vector<KalmanSite> &sites) {
  double x0 = seed.get_x0();
  double y0 = seed.get_y0();
  double r = seed.get_R();
  double p = 200.0;
  // double pz = seed.get_pz();
  double phi_0 = seed.get_phi0();
  double tan_lambda = seed.get_dzds();

  std::vector<SciFiCluster*> clusters;

  std::vector<SciFiSpacePoint> spacepoints = seed.get_spacepoints();
  process_clusters(spacepoints, clusters);
  // the clusters are sorted by now.

  int numb_sites = clusters.size();
  int tracker = clusters[0]->get_tracker();
  std::cerr << "PR: " << tracker << " " << r << " " << phi_0 << " " << tan_lambda << std::endl;
  KalmanSite first_plane;
  double site_0_turning_angle, x, y;
  if ( tracker == 0 ) {
    double delta_phi = 1100./(r*tan_lambda);
    site_0_turning_angle = (phi_0+delta_phi); // *PI/180.;
    x = -(x0 + r*cos(site_0_turning_angle));
    y = -(y0 + r*sin(site_0_turning_angle));
  } else {
    site_0_turning_angle = phi_0; // *PI/180.;
    x = x0 + r*cos(site_0_turning_angle);
    y = y0 + r*sin(site_0_turning_angle);
  }

  // double x = x0 + r*cos(site_0_turning_angle);
  // double y = y0 + r*sin(site_0_turning_angle);
  std::cerr << "SEED: " << x << " " << y << " " << site_0_turning_angle << std::endl;
  double kappa = 1./p;

  TMatrixD a(5, 1);
  a(0, 0) = x;
  a(1, 0) = y;
  a(2, 0) = r;
  a(3, 0) = site_0_turning_angle;
  a(4, 0) = tan_lambda;

  first_plane.set_projected_a(a);
  // std::cout << "Seed state: " << std::endl;
  // a.Print();
  // first_plane.set_state_vector(x, y, tan_lambda, phi_0, kappa);
  TMatrixD C(5, 5);
  C(0, 0) = 150.*150./12.;
  C(1, 1) = 150.*150./12.;
  C(2, 2) = 10.;
  C(3, 3) = 10.;
  C(4, 4) = 100.;
  // for ( int i = 0; i < 5; ++i ) {
  //   C(i, i) = 200; // dummy values
  // }
  first_plane.set_projected_covariance_matrix(C);
  first_plane.set_measurement(clusters[0]->get_alpha());
  first_plane.set_direction(clusters[0]->get_direction());
  first_plane.set_z(clusters[0]->get_position().z());
  first_plane.set_id(clusters[0]->get_id());
  // first_plane
  sites.push_back(first_plane);

  for ( int j = 1; j < numb_sites; ++j ) {
    KalmanSite a_site;
    a_site.set_measurement(clusters[j]->get_alpha());
    a_site.set_direction(clusters[j]->get_direction());
    a_site.set_z(clusters[j]->get_position().z());
    a_site.set_id(clusters[j]->get_id());
    sites.push_back(a_site);
  }
  if ( _mc_run ) {
    for ( int j = 0; j < numb_sites; ++j ) {
      CLHEP::Hep3Vector true_position = clusters[j]->get_true_position();
      CLHEP::Hep3Vector true_momentum = clusters[j]->get_true_momentum();
      sites[j].set_true_position(true_position);
      sites[j].set_true_momentum(true_momentum);
    }
  }
}

void KalmanTrackFit::initialise(SciFiStraightPRTrack &seed, std::vector<KalmanSite> &sites) {
  double x_pr = seed.get_x0();
  double y_pr = seed.get_y0();
  double mx_pr = seed.get_mx();
  double my_pr = seed.get_my();
  double p_pr  = 210.0; // MeV/c

  std::vector<SciFiSpacePoint> spacepoints = seed.get_spacepoints();
  std::vector<SciFiCluster*> clusters;
  process_clusters(spacepoints, clusters);
  // the clusters are sorted by now.

  int numb_sites = clusters.size();

  double z = clusters[0]->get_position().z();
  int tracker = clusters[0]->get_tracker();
  double mx, my, x, y;
  if ( tracker == 0 ) {
    mx = mx_pr;
    my = my_pr;
    x  = - x_pr - mx*z;
    y  = - y_pr - my*z;
  } else if ( tracker == 1 ) {
    mx = mx_pr;
    my = my_pr;
    x  = x_pr + mx*z;
    y  = y_pr + my*z;
  }

  KalmanSite first_plane;
  TMatrixD a(5, 1);
  a(0, 0) = x;
  a(1, 0) = y;
  a(2, 0) = mx;
  a(3, 0) = my;
  a(4, 0) = 1/p_pr;
  first_plane.set_projected_a(a);

  TMatrixD C(5, 5);
  C(0, 0) = 150.*150./12.;
  C(1, 1) = 150.*150./12.;
  C(2, 2) = 10.;
  C(3, 3) = 10.;
  C(4, 4) = 100.;

  first_plane.set_projected_covariance_matrix(C);
  first_plane.set_measurement(clusters[0]->get_alpha());
  first_plane.set_direction(clusters[0]->get_direction());
  first_plane.set_z(clusters[0]->get_position().z());
  first_plane.set_id(clusters[0]->get_id());
  // first_plane
  sites.push_back(first_plane);

  for ( int j = 1; j < numb_sites; ++j ) {
    KalmanSite a_site;
    a_site.set_measurement(clusters[j]->get_alpha());
    a_site.set_direction(clusters[j]->get_direction());
    a_site.set_z(clusters[j]->get_position().z());
    a_site.set_id(clusters[j]->get_id());
    sites.push_back(a_site);
  }
  if ( _mc_run ) {
    for ( int j = 0; j < numb_sites; ++j ) {
      CLHEP::Hep3Vector true_position = clusters[j]->get_true_position();
      CLHEP::Hep3Vector true_momentum = clusters[j]->get_true_momentum();
      sites[j].set_true_position(true_position);
      sites[j].set_true_momentum(true_momentum);
    }
  }
}
//
// General purpose routines.
//
void KalmanTrackFit::filter(std::vector<KalmanSite> &sites,
                            KalmanTrack *track, int current_site) {
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

void KalmanTrackFit::extrapolate(std::vector<KalmanSite> &sites, KalmanTrack *track, int i) {
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

/*
void KalmanTrackFit::monitor() {
  TFile hfile("h.root","RECREATE","KalTest");
  TNtupleD *hTrackMonitor = new TNtupleD("track", "", "ndf:chi2:cl:cpa");
  Int_t    ndf  = kaltrack.GetNDF();
  Double_t chi2 = kaltrack.GetChi2();
  Double_t cl   = TMath::Prob(chi2, ndf);
  Double_t cpa  = kaltrack.GetCurSite().GetCurState()(2, 0);
  hTrackMonitor->Fill(ndf, chi2, cl, cpa);
  hfile.Write();
}

void KalmanTrackFit::DebugPrint() const
{
   cerr << " dchi2 = " << GetDeltaChi2()   << endl;
   cerr << " res_d = " << (*(TKalTrackSite *)this).GetResVec()(0,0) << endl;
   cerr << " res_z = " << (*(TKalTrackSite *)this).GetResVec()(1,0) << endl;
   fHitPtr->DebugPrint();
}


Int_t KalmanTrackFit::GetNDF(Bool_t self)
{
   Int_t ndf    = 0;
   Int_t nsites = GetEntries();
   for (Int_t isite=1; isite<nsites; isite++) {
       TVKalSite &site = *static_cast<TVKalSite *>(At(isite));
       if (!site.IsLocked()) ndf += site.GetDimension();
   }
   if (self) ndf -= GetCurSite().GetCurState().GetDimension();
   return ndf;
}


//  for each state added to the system:
 fChi2 += next.GetDeltaChi2();

   // Calculate chi2 increment
  // GetDeltaChi2 returns fDeltaChi2
   fR      = fV - fH * curC *fHt;
   if (!CalcExpectedMeasVec(a,h)) return kFALSE;
   fResVec = fM - h;
   TKalMatrix curResVect = TKalMatrix(TKalMatrix::kTransposed, fResVec);
   fDeltaChi2 = (curResVect * G * fResVec + Kpullt * preCinv * Kpull)(0,0);
*/

void KalmanTrackFit::process_clusters(std::vector<SciFiSpacePoint> &spacepoints,
                                      std::vector<SciFiCluster*> &clusters) {
  // This admits there is only one track...
  // SciFiStraightPRTrack seed = event.straightprtracks()[0];
  // std::vector<SciFiSpacePoint> spacepoints = seed.get_spacepoints(); // Get CLUSTERS!
  int numb_spacepoints = spacepoints.size();

  for ( unsigned int i = 0; i < numb_spacepoints; ++i ) {
    SciFiSpacePoint spacepoint = spacepoints[i];
    int num_clusters = spacepoint.get_channels().size();
    for ( unsigned int j = 0; j < num_clusters; ++j ) {
      SciFiCluster *cluster = spacepoint.get_channels()[j];
      clusters.push_back(cluster);
    }
  }

  std::sort(clusters.begin(), clusters.end(), sort_by_id);
}
