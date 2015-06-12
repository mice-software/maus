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

void KalmanTrackFit::SaveGeometry(std::vector<ThreeVector> positions,
                                  std::vector<HepRotation> rotations) {
  _RefPos = positions;
  _Rot    = rotations;
}

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
    // int first_site_id = abs(sites.front()->id());

    // Run the extrapolation & filter chain.
    size_t numb_sites = sites.size();
    // Loop over all 15 planes
    for ( size_t j = 1; j < numb_sites; ++j ) {
      // Predict the state vector at site i...
      _propagator->Extrapolate(sites, j);

      // ... Filter...
      _filter->Process(sites.at(j));
    }
    // std::cerr << "Prepare For Smoothing... " << std::endl;
    _propagator->PrepareForSmoothing(sites.back());
    // ...and Smooth back all sites.
    for ( int k = static_cast<int> (sites.size()-2); k >= 0; --k ) {
      _propagator->Smooth(sites, k);
      _filter->UpdateH(sites.at(k));
      _filter->ComputeResidual(sites.at(k), KalmanState::Smoothed);
    }

    track->set_tracker(seed->tracker());
    track->set_charge(seed->charge());

    int num_elements = seed->initial_state_vector().GetNoElements();
    double* vector_elements = seed->initial_state_vector().GetMatrixArray();
    std::vector<double> seed_vector(num_elements);
    for ( int i = 0; i < num_elements; ++i ) {
      seed_vector[i] = vector_elements[i];
    }
    track->SetSeedState(seed_vector);

    num_elements = num_elements*num_elements;
    double* matrix_elements = seed->initial_covariance_matrix().GetMatrixArray();
    std::vector<double> seed_covariance(num_elements);
    for ( int i = 0; i < num_elements; ++i ) {
      seed_covariance[i] = matrix_elements[i];
    }
    track->SetSeedCovariance(seed_covariance);

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
  // Prepare to loop over all Kalman sites...
  size_t n_sites = sites.size();
  // ... summing chi2...
  double chi2 = 0.;
  // .. and counting the numb. of measurements...
  int n_measurements = 0;
  for ( size_t i = 0; i < n_sites; ++i ) {
    KalmanState *site = sites.at(i);
    if  ( site->contains_measurement() ) {
      n_measurements++;
      chi2 += site->chi2();
    }
  }
  // Find the ndf for this track: numb measurements - numb parameters to be estimated
  int n_parameters = sites.at(0)->a(KalmanState::Filtered).GetNrows();
  int ndf = n_measurements - n_parameters;
  track->set_chi2(chi2);
  track->set_ndf(ndf);
  double P_value = TMath::Prob(chi2, ndf);
  track->set_P_value(P_value);
}

void KalmanTrackFit::Save(SciFiEvent &event, SciFiTrack *track, KalmanStatesPArray sites) {
  double pvalue = track->P_value();
  int tracker = track->tracker();
  if ( pvalue != pvalue ) return;
  for ( size_t i = 0; i < sites.size(); ++i ) {
//    if ( sites.at(i)->contains_measurement() ) {
    sites.at(i)->MoveToGlobalFrame(_RefPos[tracker]);
    SciFiTrackPoint *track_point = new SciFiTrackPoint(sites.at(i));
    track->add_scifitrackpoint(track_point);
//    }
  }

  std::cerr << "SAVING STATES:\nPredicted:\n";
  for ( size_t i = 0; i < sites.size(); ++i ) {
    TMatrixD state = sites.at(i)->a( KalmanState::Projected );
    for ( size_t j = 0; j < state.GetNrows(); ++j ) {
      std::cerr << state(j, 0) << ", ";
    }
  std::cerr << "\n";
  }
  std::cerr << "\n";

  for ( size_t i = 0; i < sites.size(); ++i ) {
    TMatrixD state = sites.at(i)->a( KalmanState::Filtered );
    for ( size_t j = 0; j < state.GetNrows(); ++j ) {
      std::cerr << state(j, 0) << ", ";
    }
  std::cerr << "\n";
  }
  std::cerr << "\n";

  for ( size_t i = 0; i < sites.size(); ++i ) {
    TMatrixD state = sites.at(i)->a( KalmanState::Smoothed );
    for ( size_t j = 0; j < state.GetNrows(); ++j ) {
      std::cerr << state(j, 0) << ", ";
    }
  std::cerr << "\n";
  }
  std::cerr << "\n\n";

  event.add_scifitrack(track);
}

void KalmanTrackFit::DumpInfo(KalmanStatesPArray sites) {
  size_t numb_sites = sites.size();

  for ( size_t i = 0; i < numb_sites; ++i ) {
    KalmanState* site = sites.at(i);
    TMatrix covariance_pre = site->covariance_matrix(KalmanState::Projected);
    TMatrix covariance_fil = site->covariance_matrix(KalmanState::Filtered);
    TMatrix covariance_smo = site->covariance_matrix(KalmanState::Smoothed);
    // Squeak::mout(Squeak::info)
    std::cerr
    << "=========================================="  << "\n"
    << "SITE ID: " << site->id() << "\n"
    << "SITE Z: " << site->z()   << "\n"
    << "Measurement: " << (site->measurement())(0, 0) << "\n"
    << "Projection: " << (site->a(KalmanState::Projected))(0, 0) << " "
                      << (site->a(KalmanState::Projected))(1, 0) << " "
                      << (site->a(KalmanState::Projected))(2, 0) << " "
                      << (site->a(KalmanState::Projected))(3, 0) << "\n"
    << "Filtered: "   << (site->a(KalmanState::Filtered))(0, 0) << " "
                      << (site->a(KalmanState::Filtered))(1, 0) << " "
                      << (site->a(KalmanState::Filtered))(2, 0) << " "
                      << (site->a(KalmanState::Filtered))(3, 0) << "\n"
    << "Smoothed: "   << (site->a(KalmanState::Smoothed))(0, 0) << " "
                      << (site->a(KalmanState::Smoothed))(1, 0) << " "
                      << (site->a(KalmanState::Smoothed))(2, 0) << " "
                      << (site->a(KalmanState::Smoothed))(3, 0) << "\n"
    << "================Residuals================"   << "\n"
    << (site->residual(KalmanState::Projected))(0, 0)  << "\n"
    << (site->residual(KalmanState::Filtered))(0, 0)   << "\n"
    << (site->residual(KalmanState::Smoothed))(0, 0)   << "\n"
    << "=========================================="
    << "\nPredicted Covariance:\n";
    unsigned int nrows = covariance_pre.GetNrows();
    unsigned int ncols = covariance_pre.GetNcols();
    for ( size_t j = 0; j < nrows; ++j ) {
      for ( size_t k = 0; k < ncols; ++k ) {
        std::cerr << covariance_pre(j, k) << "   ";
      }
      std::cerr << '\n';
    }
    std::cerr << "\nFiltered Covariance:\n";
    for ( size_t j = 0; j < nrows; ++j ) {
      for ( size_t k = 0; k < ncols; ++k ) {
        std::cerr << covariance_fil(j, k) << "   ";
      }
      std::cerr << '\n';
    }
    std::cerr << "\nSmoothed Covariance:\n";
    for ( size_t j = 0; j < nrows; ++j ) {
      for ( size_t k = 0; k < ncols; ++k ) {
        std::cerr << covariance_smo(j, k) << "   ";
      }
      std::cerr << '\n';
    }
    std::cerr << "==========================================" << std::endl;
  }
}

} // ~namespace MAUS
