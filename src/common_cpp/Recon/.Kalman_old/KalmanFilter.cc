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

#include "src/common_cpp/Recon/Kalman/KalmanFilter.hh"
#include "src/common_cpp/Utils/Globals.hh"

namespace MAUS {

KalmanFilter::KalmanFilter() {}

KalmanFilter::KalmanFilter(int dim) : _n_parameters(dim),
                                      _measurement_dim(1) {
  Json::Value *json = Globals::GetConfigurationCards();
  FibreParameters.Station_Radius = (*json)["SciFiParams_Station_Radius"].asDouble();
  FibreParameters.Plane_Width    = (*json)["SciFiParams_Plane_Width"].asDouble();
  FibreParameters.Pitch          = (*json)["SciFiParams_Pitch"].asDouble();
  // Measurement equation.
  _H.ResizeTo(_measurement_dim, _n_parameters);
  _H.Zero();
  // Alignment shifts
  _S.ResizeTo(_measurement_dim, 3);
  _S.Zero();
  // Measurement error.
  _V.ResizeTo(_measurement_dim, _measurement_dim);
  _V.Zero();
  // Kalman gain.
  _K.ResizeTo(_n_parameters, _measurement_dim);
  _K.Zero();
  // Weight matrix.
  _W.ResizeTo(_measurement_dim, _measurement_dim);
  _W.Zero();

  _sigma_alpha = 1.0/TMath::Sqrt(12.);
  _V.Zero();
  _V(0, 0) = _sigma_alpha*_sigma_alpha;
}

KalmanFilter::~KalmanFilter() {}

void KalmanFilter::Process(KalmanState *a_site) {
  // If the site doesn't contain a measurement,
  // the filtered state is a copy of the projection.
  if ( !a_site->contains_measurement() ) {
    TMatrixD a = a_site->a(KalmanState::Projected);
    a_site->set_a(a, KalmanState::Filtered);

    TMatrixD C = a_site->covariance_matrix(KalmanState::Projected);
    a_site->set_covariance_matrix(C, KalmanState::Filtered);
    return;
  }

  // Update measurement error:
  // UpdateV(a_site);

  // Update H (depends on plane direction.)
  UpdateH(a_site);
  UpdateW(a_site);
  UpdateK(a_site);

  ComputePull(a_site);

  // a_k = a_k^k-1 + K_k x pull
  CalculateFilteredState(a_site);

  // Cp = (C-KHC)
  UpdateCovariance(a_site);

  ComputeResidual(a_site, KalmanState::Filtered);

  ComputeChi2(a_site);

  a_site->set_current_state(KalmanState::Filtered);
}


void KalmanFilter::UpdateV(const KalmanState *a_site) {
  // Fibre constants.
  double pitch          = FibreParameters.Pitch;
  // Station radius in units of channel width.
  double station_radius = FibreParameters.Station_Radius/pitch;

  double alpha = (a_site->measurement())(0, 0);
  double length = 2.*TMath::Sqrt(station_radius*station_radius -
                                 alpha*alpha);

  double sigma_beta = length/TMath::Sqrt(12.);
  // double sigma_alpha = 1.0/TMath::Sqrt(12.);

  _V.Zero();
  _V(0, 0) = _sigma_alpha*_sigma_alpha;
}

void KalmanFilter::UpdateH(const KalmanState *a_site) {
  ThreeVector dir = a_site->direction();

  double dx = dir.x();
  double dy = dir.y();

  ThreeVector perp = dir.Orthogonal();

  double perp_x = perp.x();
  double perp_y = perp.y();

  double channel_width = FibreParameters.Pitch;

  _H.Zero();
  _H(0, 0) =  dy/channel_width;
  _H(0, 2) = -dx/channel_width;
  // _H(1, 0) =  perp_y/pitch;
  // _H(1, 2) = -perp_x/pitch;

  _S.Zero();
  _S(0, 0) = -dy/channel_width;
  _S(0, 1) =  dx/channel_width;
  // _S(1, 0) = -perp_y/pitch;
  // _S(1, 1) =  perp_x/pitch;
  // _S.Zero();

  std::cerr << "\n\nPosition = " << a_site->z() << "\n";
//  std::cerr << "Measurement : S = " << _S(0,0) << "  " << _S(0,1) << "  " << _S(1,0) << "  " << _S(1,1) << '\n';
  std::cerr << "Measurement : H = " << _H(0,0) << "  " << _H(0,1)  << "  " << _H(0,2) << '\n';
}

// W = [ V + H C_k-1 Ht + S cov_S_k-1 St ]-1
// W = [ V +    A       +      B         ]-1
void KalmanFilter::UpdateW(const KalmanState *a_site) {
  TMatrixD C_a(_n_parameters, _n_parameters);
  C_a = a_site->covariance_matrix(KalmanState::Projected);

  TMatrixD C_s = a_site->input_shift_covariance();

  TMatrixD A = TMatrixD(TMatrixD(_H, TMatrixD::kMult, C_a),
                        TMatrixD::kMultTranspose,
                        _H);

  TMatrixD B = TMatrixD(TMatrixD(_S, TMatrixD::kMult, C_s),
                        TMatrixD::kMultTranspose,
                        _S);

  _W.Zero();
  _W = _V + A + B;
  _W.Invert();
}

void KalmanFilter::UpdateK(const KalmanState *a_site) {
  // Kalman Gain: K = C Ht (V + H C Ht)-1
  //              K =  A   (V +  B )-1
  //              K =  A W
  TMatrixD C = a_site->covariance_matrix(KalmanState::Projected);

  TMatrixD A = TMatrixD(C, TMatrixD::kMultTranspose, _H);

  _K.Zero();
  _K = A * _W;
}

void KalmanFilter::ComputePull(KalmanState *a_site) {
  // PULL = m - ha
  TMatrixD measurement = a_site->measurement();
  TMatrixD a           = a_site->a(KalmanState::Projected);
  TMatrixD shifts      = a_site->input_shift();

  TMatrixD HA = SolveMeasurementEquation(a, shifts);

  TMatrixD pull(_measurement_dim, 1);
  pull = measurement - HA;

  std::cerr << "PULL FOUND : Meas = " << HA(0,0) << " Pull = " << pull(0,0)  << "\n";
  std::cerr << "State = " << a(0,0) << ", " << a(1,0) << ", " << a(2,0) << ", " << a(3,0) << ", " << a(4,0) << '\n';

  a_site->set_residual(pull, KalmanState::Projected);
}

TMatrixD KalmanFilter::SolveMeasurementEquation(const TMatrixD &a,
                                                const TMatrixD &s) {
  TMatrixD ha(_measurement_dim, 1);
  ha = _H * a;

  TMatrixD Ss(_measurement_dim, 1);
  Ss = _S * s;

  TMatrixD result(_measurement_dim, 1);
  // TMatrixD result(1, 1);
  result = ha + Ss;

  return result;
}

void KalmanFilter::CalculateFilteredState(KalmanState *a_site) {
  // Calculate the pull,
  TMatrixD pull = a_site->residual(KalmanState::Projected);

  // compute the filtered state via
  // a' = a + K*pull
  TMatrixD a = a_site->a(KalmanState::Projected);

  TMatrixD a_filt(_n_parameters, 1);
  a_filt = a + _K*pull;

  a_site->set_a(a_filt, KalmanState::Filtered);
}

void KalmanFilter::ComputeResidual(KalmanState *a_site, KalmanState::State kalman_state) {
  // PULL = measurement - ha
  TMatrixD measurement = a_site->measurement();
  TMatrixD a = a_site->a(kalman_state);
  TMatrixD shifts = a_site->input_shift();

  // Solve the measurement equation again,
  // this time passing the FILTERED state to find the
  // residual.
  TMatrixD HA = SolveMeasurementEquation(a, shifts);

  TMatrixD residual(_measurement_dim, 1);
  residual = measurement - HA;

  a_site->set_residual(residual, kalman_state);
}

void KalmanFilter::ComputeChi2(KalmanState *a_site) {
  TMatrixD residual = a_site->residual(KalmanState::Filtered);
  TMatrixD C = a_site->covariance_matrix(KalmanState::Filtered);

  TMatrixD _H_transposed(_n_parameters, _measurement_dim);
  _H_transposed.Transpose(_H);

  // The covariance of the filtered residuals.
  TMatrixD R = _V - (_H*C*_H_transposed);
  R.Invert();

  a_site->set_covariance_residual(R, KalmanState::Filtered);

  TMatrixD residual_transposed(1, _measurement_dim);
  residual_transposed.Transpose(residual);

  TMatrixD chi2 = residual_transposed * R * residual;

  a_site->set_chi2(chi2(0, 0));
}

void KalmanFilter::UpdateCovariance(KalmanState *a_site) {
  // Can be computed as:
  // Cp = (1-KH)C
  // or:
  // Cp = (1-KH)C(1-KH)T + KVKT
  // Cp = A C AT + B
  TMatrixD C_old = a_site->covariance_matrix(KalmanState::Projected);

  TMatrixD I(_n_parameters, _n_parameters);
  I.UnitMatrix();
  /*
  // If in need to save computing time, use this equation instead.
  TMatrixD C_new(_n_parameters, _n_parameters);
  C_new = ( I - _K*_H ) * C_old;
  */
  TMatrixD A(_n_parameters, _n_parameters);
  A = ( I - _K*_H );
  TMatrixD A_transposed(_n_parameters, _n_parameters);
  A_transposed.Transpose(A);

  TMatrixD B(_n_parameters, _n_parameters);
  B = TMatrixD(TMatrixD(_K, TMatrixD::kMult, _V), TMatrixD::kMultTranspose, _K);

  TMatrixD C_new(_n_parameters, _n_parameters);
  C_new = A*C_old*A_transposed + B;

  a_site->set_covariance_matrix(C_new, KalmanState::Filtered);
}

} // ~namespace MAUS
