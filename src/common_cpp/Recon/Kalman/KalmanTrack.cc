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
#include "src/common_cpp/Recon/Kalman/KalmanTrack.hh"

KalmanTrack::KalmanTrack() {
  // Initialise member matrices:
  _G.ResizeTo(2, 2);
  _H.ResizeTo(2, 5);
  _F.ResizeTo(5, 5);
  _A.ResizeTo(5, 5);

  _Q.ResizeTo(5, 5);
  _Q.Zero(); // mcs is off.
}

//
// ------- Prediction ------------
//
// void KalmanTrack::calc_system_noise(KalmanSite &site) {
// }

void KalmanTrack::calc_predicted_state(KalmanSite *old_site, KalmanSite *new_site) {
  TMatrixD a = old_site->get_a();

  TMatrixD a_projected = TMatrixD(_F, TMatrixD::kMult, a);
  new_site->set_projected_a(a_projected);
}

//
// C_pred = _F * C * _Ft + _Q;
//
void KalmanTrack::calc_covariance(KalmanSite *old_site, KalmanSite *new_site) {
  TMatrixD C(5, 5);
  C = old_site->get_covariance_matrix();

  TMatrixD temp1(5, 5);
  TMatrixD temp2(5, 5);
  temp1 = TMatrixD(_F, TMatrixD::kMult, C);
  temp2 = TMatrixD(temp1, TMatrixD::kMultTranspose, _F);

  TMatrixD C_pred(5, 5);
  C_pred = TMatrixD(temp2, TMatrixD::kPlus, _Q);
  new_site->set_projected_covariance_matrix(C_pred);
}

//
// ------- Filtering ------------
//
void KalmanTrack::update_G(KalmanSite *a_site) {
  double alpha = (a_site->get_measurement())(0, 0);
  double l = pow(ACTIVE_RADIUS*ACTIVE_RADIUS -
                 (alpha*CHAN_WIDTH)*(alpha*CHAN_WIDTH), 0.5);
  double sig_beta = l/CHAN_WIDTH;
  double SIG_ALPHA = 1.0;
  _G.Zero();
  _G(0, 0) = SIG_ALPHA*SIG_ALPHA;
  _G(1, 1) = sig_beta*sig_beta;
  _G.Invert();
}

void KalmanTrack::update_H(KalmanSite *a_site) {
  Hep3Vector dir = a_site->get_direction();
  double dx = dir.x();
  double dy = dir.y();

  _H.Zero();
  _H(0, 0) = -A*dy;
  _H(0, 1) =  A*dx;
}

void KalmanTrack::update_covariance(KalmanSite *a_site) {
  // calc_covariance_matrix_of_residual
  // Cp = ( C_inv + Ht*G*H )-1;
  TMatrixD C = a_site->get_projected_covariance_matrix();
  TMatrixD C_inv = C.Invert();

  TMatrixD temp1(5, 2);
  TMatrixD temp2(5, 5);
  temp1 = TMatrixD(_H, TMatrixD::kTransposeMult, _G);
  temp2 = TMatrixD(temp1, TMatrixD::kMult, _H);
  TMatrixD Cp(5, 5);
  Cp = TMatrixD(C_inv, TMatrixD::kPlus, temp2);
  _H.Print();
  _G.Print();
  Cp.Print();
  Cp.Invert();
  a_site->set_covariance_matrix(Cp);
  // assert(site.state() == "filtered");
}

// h1(a_1^0)
void KalmanTrack::calc_filtered_state(KalmanSite *a_site) {
  /////////////////////////////////////////////////////////////////////
  // PULL = m - ha
  //
  TMatrixD m(2, 1);
  m = a_site->get_measurement();

  TMatrixD a(5, 1);
  a = a_site->get_projected_a();
  TMatrixD ha(2, 1);
  // double beta  = 0;
  // double alpha = a(0, 0)*_H(0, 0) + a(1, 0)*_H(0, 1);
  // ha(0, 0) = alpha;
  // ha(1, 0) = beta;
  ha = TMatrixD(_H, TMatrixD::kMult, a);
  // Extrapolation converted to expected measurement.
  double alpha = ha(0, 0);
  a_site->set_projected_alpha(alpha);

  TMatrixD pull(2, 1);
  pull = TMatrixD(m, TMatrixD::kMinus, ha);
  /////////////////////////////////////////////////////////////////////
  //
  // Kalman Gain: K = Cp Ht G
  //
  TMatrixD C(5, 5);
  C = a_site->get_covariance_matrix();
  TMatrixD temp3(5, 2);
  temp3 = TMatrixD(C, TMatrixD::kMultTranspose, _H);
  TMatrixD K(5, 2);
  K = TMatrixD(temp3, TMatrixD::kMult, _G);
  //////////////////////////////////////////////////////////////////////
  // ap = a + K*pull;
  TMatrixD temp4(5, 1);
  temp4 = TMatrixD(K, TMatrixD::kMult, pull);
  TMatrixD a_filt(5, 1);
  a_filt = TMatrixD(a, TMatrixD::kPlus, temp4);
  a_site->set_a(a_filt);
  // Residuals. x and y.
  double res_x = a_filt(0, 0) - a(0, 0);
  double res_y = a_filt(1, 0) - a(1, 0);
  a_site->set_residual_x(res_x);
  a_site->set_residual_y(res_y);
}

//
// Smoothing
//
void KalmanTrack::update_back_transportation_matrix(KalmanSite *optimum_site,
                                                      KalmanSite *smoothing_site) {
  // update_propagator(smoothing_site, optimum_site);
  TMatrixD Cp(5, 5);
  Cp = optimum_site->get_projected_covariance_matrix();
  Cp.Invert();
  TMatrixD C(5, 5);
  C = smoothing_site->get_covariance_matrix();

  TMatrixD temp(5, 5);
  temp = TMatrixD(C, TMatrixD::kMultTranspose, _F);
  _A = TMatrixD(temp, TMatrixD::kMult, Cp);
}

void KalmanTrack::smooth_back(KalmanSite *optimum_site, KalmanSite *smoothing_site) {
  TMatrixD a(5, 1);
  a = smoothing_site->get_a();
  a.Print();
  TMatrixD a_opt(5, 1);
  a_opt = optimum_site->get_a();

  TMatrixD ap(5, 1);
  ap = optimum_site->get_projected_a();

  TMatrixD temp1(5, 1);
  temp1 == TMatrixD(a_opt, TMatrixD::kMinus, ap);

  TMatrixD temp2(5, 1);
  temp2 = TMatrixD(_A, TMatrixD::kMult, temp1);

  TMatrixD a_smooth(5, 1);
  a_smooth =  TMatrixD(a, TMatrixD::kPlus, temp2);
  smoothing_site->set_a(a_smooth);

  // do the same for covariance matrix
  TMatrixD C(5, 5);
  C = smoothing_site->get_covariance_matrix();

  TMatrixD C_opt(5, 5);
  C_opt = optimum_site->get_covariance_matrix();
  TMatrixD Cp(5, 5);
  Cp = optimum_site->get_projected_covariance_matrix();
  TMatrixD temp3(5, 5);
  temp3 = TMatrixD(C_opt, TMatrixD::kMinus, Cp);

  TMatrixD temp4(5, 5);
  temp4 = TMatrixD(_A, TMatrixD::kMult, temp3);
  TMatrixD temp5(5, 5);
  temp5= TMatrixD(temp4, TMatrixD::kMultTranspose, _A);
  TMatrixD C_smooth(5, 5);
  C_smooth =  TMatrixD(C, TMatrixD::kPlus, temp5);
  smoothing_site->set_covariance_matrix(C_smooth);

}
