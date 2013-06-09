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

KalmanTrackFit::KalmanTrackFit() : _propagator(NULL),
                                   _filter(NULL) {
  //
  // Get Configuration values.
  //
  Json::Value *json = Globals::GetConfigurationCards();
  _use_MCS          = (*json)["SciFiKalman_use_MCS"].asBool();
  _use_Eloss        = (*json)["SciFiKalman_use_Eloss"].asBool();
  _verbose          = (*json)["SciFiKalmanVerbose"].asBool();
}

KalmanTrackFit::~KalmanTrackFit() {}

void KalmanTrackFit::Process(std::vector<KalmanSeed*> seeds,
                             SciFiEvent &event) {
  // Prepare to loop over seeds. 1 seed = 1 track hypothesis
  //
  for ( size_t i = 0; i < seeds.size(); ++i ) {
    // Current seed.
    KalmanSeed* seed = seeds[i];

    SciFiTrack *track = new SciFiTrack();
    track->SetKalmanSites(seed->GetKalmanSites());
    if ( seed->is_straight() ) {
      _propagator = new KalmanStraightPropagator();
      _filter     = new KalmanFilter(4);
    } else if ( seed->is_helical() ) {
      double Bz   = seed->GetField();
      _propagator = new KalmanHelicalPropagator(Bz);
      _filter     = new KalmanFilter(5);
    }
    // Filter the first state.
    _filter->Process(track, 0);

    // Run the extrapolation & filter chain.
    size_t numb_measurements = track->GetNumberKalmanSites();
    for ( size_t j = 1; j < numb_measurements; ++j ) {
      // Predict the state vector at site i...
      _propagator->Extrapolate(track, j);
      // ... Filter...
      _filter->Process(track, j);
    }
    _propagator->PrepareForSmoothing(track);
    // ...and Smooth back all sites.
    for ( int k = static_cast<int> (numb_measurements-2); k > -1; --k ) {
      _propagator->Smooth(track, k);
      _filter->UpdateH(track->GetKalmanSite(k));
      _filter->SetResidual(track->GetKalmanSite(k), KalmanSite::Smoothed);
    }
    // Calculate the chi2 of this track.
    ComputeChi2(track);
    // Optional printing.
    if ( _verbose )
      DumpInfo(track);

    // Save(event, track);
    // Free memory allocated and reset pointers to NULL.
    delete _propagator;
    delete _filter;
    _propagator = NULL;
    _filter     = NULL;
  }
}

void KalmanTrackFit::ComputeChi2(SciFiTrack *track) {
  double f_chi2 = 0.;
  double s_chi2 = 0.;
  int n_sites = track->GetNumberKalmanSites();

  int n_parameters;
  if ( track->GetAlgorithmUsed() == SciFiTrack::kalman_straight ) {
    n_parameters = 4;
  } else {
    n_parameters = 5;
  }

  int ndf = n_sites - n_parameters;

  for ( size_t i = 0; i < n_sites; ++i ) {
    KalmanSite *site = track->GetKalmanSite(i);
    f_chi2 += site->chi2(KalmanSite::Filtered);
    s_chi2 += site->chi2(KalmanSite::Smoothed);
  }
  double P_value = TMath::Prob(f_chi2, ndf);
  track->set_f_chi2(f_chi2);
  track->set_s_chi2(s_chi2);
  track->set_ndf(ndf);
  track->set_P_value(P_value);
}

void KalmanTrackFit::Save(SciFiEvent &event, SciFiTrack *track) {
  double pvalue = track->P_value();
  if ( pvalue != pvalue ) return;
  for ( size_t i = 0; i < track->GetNumberKalmanSites(); ++i ) {
    SciFiTrackPoint *track_point = new SciFiTrackPoint(track->GetKalmanSite(i));
    track->add_scifitrackpoint(track_point);
  }
  event.add_scifitrack(track);
}

void KalmanTrackFit::DumpInfo(SciFiTrack *track) {
  size_t numb_sites = track->GetNumberKalmanSites();

  for ( size_t i = 0; i < numb_sites; ++i ) {
    KalmanSite* site = track->GetKalmanSite(i);
    Squeak::mout(Squeak::info)
    << "=========================================="  << "\n"
    << "SITE ID: " << site->id() << "\n"
    << "SITE Z: " << site->z()   << "\n"
    << "Measurement: " << (site->measurement())(0, 0) << "\n"
    << "Projection: " << (site->a(KalmanSite::Projected))(0, 0) << " "
                      << (site->a(KalmanSite::Projected))(1, 0) << " "
                      << (site->a(KalmanSite::Projected))(2, 0) << " "
                      << (site->a(KalmanSite::Projected))(3, 0) << "\n"
    << "================Residuals================"   << "\n"
    << (site->residual(KalmanSite::Projected))(0, 0)  << "\n"
    << (site->residual(KalmanSite::Filtered))(0, 0)   << "\n"
    << (site->residual(KalmanSite::Smoothed))(0, 0)   << "\n"
    << "=========================================="
    << std::endl;
  }
}

} // ~namespace MAUS
