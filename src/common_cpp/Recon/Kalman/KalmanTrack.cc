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
#include <iostream>
#include <fstream>

namespace MAUS {

// Initialize geometry constants.
const double KalmanTrack::A = 2./(7.*0.427);
const double KalmanTrack::ACTIVE_RADIUS = 150.;
const double KalmanTrack::CHAN_WIDTH = 1.333;

KalmanTrack::KalmanTrack() {
  // Initialise member matrices:
  _V.ResizeTo(2, 2);
  _H.ResizeTo(2, 5);
  _F.ResizeTo(5, 5);
  _A.ResizeTo(5, 5);
  _K.ResizeTo(5, 2);
  _Q.ResizeTo(5, 5);

  _chi2 = 0.0;
  _ndf  = 0.0;
  _tracker=-1;
}

//
// ------- Prediction ------------
//
void KalmanTrack::calc_predicted_state(KalmanSite *old_site, KalmanSite *new_site) {
  // std::cout <<" ----------------------- Projection ----------------------- \n";
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
  // C_pred.Print();
}

void KalmanTrack::calc_system_noise(KalmanSite *site) {
  _Q.Zero();
/*
  TMatrixD a(5, 1);
  a = site->get_a();
  double mx = a(2, 0);
  double my = a(3, 0);
  double kappa = a(4, 0);
  double Z = 1.;
  double r0 = 0.00167; // cm3/g
  double p = 1/kappa; // MeV/c
  double v = p/105.7;
  double C = 13.6*Z*pow(r0, 0.5)*(1+0.038*log(r0))/(v*p);

  _Q(2, 2) = (1+pow(mx, 2.))*(1+pow(mx, 2.)+pow(my, 2.))*C;
  _Q(3, 3) = (1+pow(my, 2.))*(1+pow(mx, 2.)+pow(my, 2.))*C;
  _Q(2, 3) = mx*my*(1+mx*mx+my*my)*C;
  _Q(3, 2) = mx*my*(1+mx*mx+my*my)*C;

  _Q(4, 4) = kappa*kappa*my*my*C/(1+mx*mx);
  _Q(3, 4) = kappa * my * (1+mx*mx+my*my) * C /(1+mx*mx);
*/
}

//
// ------- Filtering ------------
//
void KalmanTrack::update_V(KalmanSite *a_site) {
  // std::cout <<"Site ID:" << a_site->get_id() << "\n";
  // std::cout <<" ----------------------- Filtering ----------------------- \n";
  double alpha = (a_site->get_measurement())(0, 0);
  double l = pow(ACTIVE_RADIUS*ACTIVE_RADIUS -
                 (alpha*CHAN_WIDTH)*(alpha*CHAN_WIDTH), 0.5);
  double sig_beta = l/CHAN_WIDTH;
  double SIG_ALPHA = 1.0;
  _V.Zero();
  _V(0, 0) = SIG_ALPHA*SIG_ALPHA/12.;
  _V(1, 1) = sig_beta*sig_beta/12.;
}

void KalmanTrack::update_H(KalmanSite *a_site) {
  CLHEP::Hep3Vector dir = a_site->get_direction();
  double dx = dir.x();
  double dy = dir.y();

  _H.Zero();
  _H(0, 0) = -A*dy;
  _H(0, 2) =  A*dx;
}

void KalmanTrack::update_covariance(KalmanSite *a_site) {
  // Cp = (C-KHC)
  TMatrixD C = a_site->get_projected_covariance_matrix();
  TMatrixD temp1(5, 5);
  TMatrixD temp2(5, 5);
  temp1 = TMatrixD(_K, TMatrixD::kMult, _H);
  temp2 = TMatrixD(temp1, TMatrixD::kMult, C);
  TMatrixD Cp(5, 5);
  Cp = TMatrixD(C, TMatrixD::kMinus, temp2);
  a_site->set_covariance_matrix(Cp);
  // std::cout << "Updated Covariance \n";
  // Cp.Print();
}

void KalmanTrack::calc_filtered_state(KalmanSite *a_site) {
  /////////////////////////////////////////////////////////////////////
  // PULL = m - ha
  //
  TMatrixD m(2, 1);
  m = a_site->get_measurement();

  TMatrixD a(5, 1);
  a = a_site->get_projected_a();
  // std::cout << "Projected state: " << std::endl;
  // a.Print();
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
  // std::cout << "Pull: " << std::endl;
  // pull.Print();
  /////////////////////////////////////////////////////////////////////
  //
  // Kalman Gain: K = C Ht (V + H C Ht)-1
  //              K =  A   (V +  B )-1
  TMatrixD C(5, 5);
  C = a_site->get_projected_covariance_matrix();

  TMatrixD A(5, 2);
  A = TMatrixD(C, TMatrixD::kMultTranspose, _H);

  TMatrixD temp1(2, 5);
  temp1 = TMatrixD(_H, TMatrixD::kMult, C);
  TMatrixD B(2, 2);
  B = TMatrixD(temp1, TMatrixD::kMultTranspose, _H);
  TMatrixD temp2(2, 2);
  temp2 = TMatrixD(_V, TMatrixD::kPlus, B);
  // double det;
  temp2.Invert();
  // assert(det != 0);

  _K.Zero();
  _K = TMatrixD(A, TMatrixD::kMult, temp2);
  //////////////////////////////////////////////////////////////////////
  // ap = a + K*pull;
  TMatrixD temp4(5, 1);
  temp4 = TMatrixD(_K, TMatrixD::kMult, pull);
  TMatrixD a_filt(5, 1);
  a_filt = TMatrixD(a, TMatrixD::kPlus, temp4);
  a_site->set_a(a_filt);
  // std::cout << "Filtered state: " << std::endl;
  // a_filt.Print();
  // Residuals. x and y.
  double res_x = a_filt(0, 0) - a(0, 0);
  double res_y = a_filt(2, 0) - a(2, 0);

  a_site->set_residual_x(res_x);
  a_site->set_residual_y(res_y);
/*
  std::cout << "Filtered State: \n";
  a_filt.Print();
  std::cout << "x  residual: "  << res_x << "\n";
  std::cout << "y  residual: "  << res_y << "\n";
*/
}

//
// Smoothing
//
void KalmanTrack::update_back_transportation_matrix(KalmanSite *optimum_site,
                                                    KalmanSite *smoothing_site) {
  update_propagator(smoothing_site, optimum_site);
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

  TMatrixD a_opt(5, 1);
  a_opt = optimum_site->get_smoothed_a();

  TMatrixD ap(5, 1);
  ap = optimum_site->get_projected_a();

  TMatrixD temp1(5, 1);
  temp1 == TMatrixD(a_opt, TMatrixD::kMinus, ap);

  TMatrixD temp2(5, 1);
  temp2 = TMatrixD(_A, TMatrixD::kMult, temp1);

  // _A.Print();
  // temp2.Print();

  TMatrixD a_smooth(5, 1);
  a_smooth =  TMatrixD(a, TMatrixD::kPlus, temp2);
  smoothing_site->set_smoothed_a(a_smooth);

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
  smoothing_site->set_smoothed_covariance_matrix(C_smooth);
}

void KalmanTrack::compute_chi2(const std::vector<KalmanSite> &sites) {
  int number_parameters = 5;
  int number_of_sites = sites.size();
  double sigma_measurement2 = 1./12.;
  int id = sites[0].get_id();
  if ( id <= 14 ) _tracker = 0;
  if ( id > 14 ) _tracker = 1;

  double alpha, model_alpha;
  for ( int i = 0; i < number_of_sites; ++i ) {
    KalmanSite site = sites[i];
    // Convert smoothed value to alpha measurement.
    TMatrixD a(5, 1);
    a = site.get_smoothed_a();
    update_H(&site);
    TMatrixD ha(2, 1);
    ha = TMatrixD(_H, TMatrixD::kMult, a);
    model_alpha = ha(0, 0);
    // Actual measurement.
    alpha = site.get_alpha();
    // Compute chi2.
    _chi2 += pow(alpha-model_alpha, 2.);
  }
  _chi2 = _chi2*(1./sigma_measurement2);
  _ndf = number_of_sites - number_parameters;
  std::ofstream output("chi2.txt", std::ios::out | std::ios::app);
  output << _tracker << " " << _chi2 << " " << _ndf << "\n";
  output.close();
}

} // ~namespace MAUS
