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

// TODO: Add covariance to data structure.

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
    KalmanStatesPArray sites = seed->GetKalmanStates();
    SciFiTrack *track = new SciFiTrack();
    if ( seed->is_straight() ) {
      track->SetAlgorithmUsed(SciFiTrack::kalman_straight);
      _propagator = new KalmanStraightPropagator();
    } else if ( seed->is_helical() ) {
      track->SetAlgorithmUsed(SciFiTrack::kalman_helical);
      double Bz   = seed->GetField();
      _propagator = new KalmanHelicalPropagator(Bz);
    }
    // The Filter needs to know the dimension of its matrices.
    // The seed has that information.
    int n_parameters = seed->n_parameters();
    _filter = new KalmanFilter(n_parameters);

    // Filter the first state.
    _filter->Process(sites.front());

    // Run the extrapolation & filter chain.
    size_t numb_measurements = sites.size();
    for ( size_t j = 1; j < numb_measurements; ++j ) {
      // Predict the state vector at site i...
      _propagator->Extrapolate(sites, j);
      // ... Filter...
      _filter->Process(sites.at(j));
    }
    _propagator->PrepareForSmoothing(sites);
    // ...and Smooth back all sites.
    for ( int k = static_cast<int> (numb_measurements-2); k > -1; --k ) {
      _propagator->Smooth(sites, k);
      _filter->UpdateH(sites.at(k));
      _filter->SetResidual(sites.at(k), KalmanState::Smoothed);
    }

    track->set_tracker(seed->tracker());
    track->set_charge(seed->charge());

    // Calculate the chi2 of this track.
    ComputeChi2(track, sites);
    // Optional printing.
    if ( _verbose )
      DumpInfo(sites);
    Save(event, track, sites);
    // Free memory allocated and reset pointers to NULL.
    delete _propagator;
    delete _filter;
    _propagator = NULL;
    _filter     = NULL;
  }
}

void KalmanTrackFit::ComputeChi2(SciFiTrack *track, KalmanStatesPArray sites) {
  double f_chi2 = 0.;
  double s_chi2 = 0.;
  // Find the ndf for this track: numb measurements - numb parameters to be estimated
  size_t n_sites = sites.size();
  // Find n_parameters by looking at the dimension of the state vector.
  int n_parameters = sites.at(0)->a(KalmanState::Filtered).GetNrows();

  int ndf = n_sites - n_parameters;

  for ( size_t i = 0; i < n_sites; ++i ) {
    KalmanState *site = sites.at(i);
    f_chi2 += site->chi2(KalmanState::Filtered);
    s_chi2 += site->chi2(KalmanState::Smoothed);
  }
  double P_value = TMath::Prob(f_chi2, ndf);
  track->set_f_chi2(f_chi2);
  track->set_s_chi2(s_chi2);
  track->set_ndf(ndf);
  track->set_P_value(P_value);
}

void KalmanTrackFit::Save(SciFiEvent &event, SciFiTrack *track, KalmanStatesPArray sites) {
  double pvalue = track->P_value();
  if ( pvalue != pvalue ) return;
  for ( size_t i = 0; i < sites.size(); ++i ) {
    SciFiTrackPoint *track_point = new SciFiTrackPoint(sites.at(i));
    track->add_scifitrackpoint(track_point);
  }
  event.add_scifitrack(track);
}

void KalmanTrackFit::DumpInfo(KalmanStatesPArray sites) {
  size_t numb_sites = sites.size();

  for ( size_t i = 0; i < numb_sites; ++i ) {
    KalmanState* site = sites.at(i);
    Squeak::mout(Squeak::info)
    << "=========================================="  << "\n"
    << "SITE ID: " << site->id() << "\n"
    << "SITE Z: " << site->z()   << "\n"
    << "Measurement: " << (site->measurement())(0, 0) << "\n"
    << "Projection: " << (site->a(KalmanState::Projected))(0, 0) << " "
                      << (site->a(KalmanState::Projected))(1, 0) << " "
                      << (site->a(KalmanState::Projected))(2, 0) << " "
                      << (site->a(KalmanState::Projected))(3, 0) << " "
                      << (site->a(KalmanState::Projected))(4, 0) << "\n"
    << "Filtered: " << (site->a(KalmanState::Filtered))(0, 0) << " "
                      << (site->a(KalmanState::Filtered))(1, 0) << " "
                      << (site->a(KalmanState::Filtered))(2, 0) << " "
                      << (site->a(KalmanState::Filtered))(3, 0) << " "
                      << (site->a(KalmanState::Filtered))(4, 0) << "\n"
    << "================Residuals================"   << "\n"
    << (site->residual(KalmanState::Projected))(0, 0)  << "\n"
    << (site->residual(KalmanState::Filtered))(0, 0)   << "\n"
    << (site->residual(KalmanState::Smoothed))(0, 0)   << "\n"
    << "=========================================="
    << std::endl;
  }
}

} // ~namespace MAUS
