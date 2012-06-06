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

KalmanTrackFit::KalmanTrackFit() {
  std::cout << "---------------------Birth of Kalman Filter--------------------" << std::endl;
}

KalmanTrackFit::~KalmanTrackFit() {
  std::cout << "---------------------Death of Kalman Filter--------------------" << std::endl;
}

bool sort_by_id(SciFiCluster *a, SciFiCluster *b ) {
  if (a->get_tracker() == 0 )
    return ( a->get_id() > b->get_id() ); //  descending order
  if (a->get_tracker() == 1 )
    return ( a->get_id() < b->get_id() ); //  ascending order
}

void KalmanTrackFit::process(SciFiEvent &event) {
  KalmanTrack *track = new StraightTrack();

  std::vector<KalmanSite> sites;

  // This will: initialise the state vector;
  // Set covariance matrix;
  // Add plane measurents to all sites;
  initialise(event, sites);

  // Filter the first state.
  std::cout << "Filtering site 0" << std::endl;
  filter(sites, track, 0);

  int numb_measurements = sites.size();

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
  monitor.save(sites);
}

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

  // The propagator matrix must be constructed...
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

void TKalTrackSite::DebugPrint() const
{
   cerr << " dchi2 = " << GetDeltaChi2()   << endl;
   cerr << " res_d = " << (*(TKalTrackSite *)this).GetResVec()(0,0) << endl;
   cerr << " res_z = " << (*(TKalTrackSite *)this).GetResVec()(1,0) << endl;
   fHitPtr->DebugPrint();
}


Int_t TVKalSystem::GetNDF(Bool_t self)
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

void KalmanTrackFit::initialise(SciFiEvent &event, std::vector<KalmanSite> &sites) {
  // For now, expect only one track.
  // SciFiHelicalPRTrack helical_seed = event.helicalprtracks()[0];
  SciFiStraightPRTrack seed = event.straightprtracks()[0];
  double x0 = seed.get_x0();
  double y0 = seed.get_y0();
  double mx = seed.get_mx();
  double my = seed.get_my();
  double p  = 210.0; // MeV/c

  if ( (seed.get_spacepoints())[0].get_tracker() == 0 ) {
    x0 = -x0;
    y0 = -y0;
    mx = -mx;
    my = -my;
  }


  std::vector<SciFiCluster*> clusters;
  process_clusters(event, clusters);
  // the clusters are sorted by now.

  int numb_sites = clusters.size();

  KalmanSite first_plane;
  first_plane.set_state_vector(x0, y0, mx, my, p);
  TMatrixD C(5, 5);
  C(0, 0) = 70;
  C(1, 1) = 70;
  C(2, 2) = 0.5;
  C(3, 3) = 0.5;
  C(4, 4) = 1000;
  // for ( int i = 0; i < 5; ++i ) {
  //   C(i, i) = 200; // dummy values
  // }
  first_plane.set_covariance_matrix(C);
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
}

void KalmanTrackFit::process_clusters(SciFiEvent &event, std::vector<SciFiCluster*> &clusters) {
  // This admits there is only one track...
  SciFiStraightPRTrack seed = event.straightprtracks()[0];
  std::vector<SciFiSpacePoint> spacepoints = seed.get_spacepoints(); // Get CLUSTERS!
  int numb_spacepoints = spacepoints.size();

  for ( unsigned int i = 0; i < numb_spacepoints; ++i ) {
    SciFiSpacePoint spacepoint = spacepoints[i];
    int num_clusters = spacepoint.get_channels().size();
    for ( unsigned int j = 0; j < num_clusters; ++j ) {
      SciFiCluster *cluster = spacepoint.get_channels()[j];
      clusters.push_back(cluster);
    }
  }
  // Plane 0 of station 1 will be the first plane
  std::sort(clusters.begin(), clusters.end(), sort_by_id); // sort according to station and plane.
}
