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

#include "src/common_cpp/Recon/SingleStation/KalmanSSTrack.hh"
#include <math.h>
#include <iostream>
#include <fstream>

// Initialize geometry constants.
// const double KalmanSSTrack::A = 2./(7.*0.427);
const double KalmanSSTrack::ACTIVE_RADIUS = 150.;
const double KalmanSSTrack::CHAN_WIDTH = 1.333;

KalmanSSTrack::KalmanSSTrack() {
  // Initialise member matrices:
  _G.ResizeTo(2, 2);
  _H.ResizeTo(2, 5);
  _F.ResizeTo(5, 5);
  _A.ResizeTo(5, 5);
  _V.ResizeTo(2, 2);
  _K.ResizeTo(5, 2);

  _Q.ResizeTo(5, 5);

  _chi2    = 0.0;
  _ndf     = 0.0;
  _tracker = -1;
}

KalmanSSTrack::~KalmanSSTrack() {}

//
// ------- Prediction ------------
//

//
// C_pred = _F * C * _Ft + _Q;
//
void KalmanSSTrack::calc_covariance(KalmanSite *old_site, KalmanSite *new_site) {
  TMatrixD C(5, 5);
  C = old_site->get_covariance_matrix();

  TMatrixD temp1(5, 5);
  TMatrixD temp2(5, 5);
  temp1 = TMatrixD(_F, TMatrixD::kMult, C);
  temp2 = TMatrixD(temp1, TMatrixD::kMultTranspose, _F);
  // _F.Print();

  TMatrixD C_pred(5, 5);
  _Q.Zero();
  C_pred = TMatrixD(temp2, TMatrixD::kPlus, _Q);
  // _Q.Print();
  new_site->set_projected_covariance_matrix(C_pred);
  // C_pred.Print();
}

// Only straight lines for now
void KalmanSSTrack::update_propagator(KalmanSite *old_site, KalmanSite *new_site) {
  // Find dz between sites.
  double new_z = new_site->get_z();
  double old_z = old_site->get_z();
  double deltaZ = new_z-old_z;

  // Reset.
  _F.Zero();
  bool magnets_on = true;
  if ( magnets_on && old_site->get_id() == 4 ) {
    magnet_drift();
  } else {
    straight_line(deltaZ);
  }
}

void KalmanSSTrack::magnet_drift() {
  TMatrixD M_Q7(5, 5);
  M_Q7.Zero();
  TMatrixD M_0(5, 5);
  M_0.Zero();
  TMatrixD M_Q8(5, 5);
  M_Q8.Zero();
  TMatrixD M_1(5, 5);
  M_1.Zero();
  TMatrixD M_Q9(5, 5);
  M_Q9.Zero();

  double kappa_q7 = 1.0;  // m2
  double sqrt_kappa_q7 = pow(kappa_q7, 0.5)/1000.; // m-1 -> mm-1
  double kappa_q8 = 1.52; // m2
  double sqrt_kappa_q8 = pow(kappa_q8, 0.5)/1000.; // m-1 -> mm-1
  double kappa_q9 = 1.3;  // m2
  double sqrt_kappa_q9 = pow(kappa_q9, 0.5)/1000.; // m-1 -> mm-1

  double L = 660.;   // mm
  double D_0 = 500.; // mm
  double D_1 = 500.; // mm

  for ( int i = 0; i < 5; i++ ) {
    M_0(i, i) = 1.;
    M_1(i, i) = 1.;
  }
  M_0(0, 1) = D_0;
  M_0(2, 3) = D_0;
  M_1(0, 1) = D_1;
  M_1(2, 3) = D_1;

  // Q 7 focusses in x...
  double phi_q7 = sqrt_kappa_q7*L;
  M_Q7(0, 0) = cos(phi_q7);
  M_Q7(0, 1) = sin(phi_q7)/sqrt_kappa_q7;
  M_Q7(1, 0) =-sin(phi_q7)*sqrt_kappa_q7;
  M_Q7(1, 1) = cos(phi_q7);
  // Q 7 de-focusses in y...
  M_Q7(2, 2) = cosh(phi_q7);
  M_Q7(2, 3) = sinh(phi_q7)/sqrt_kappa_q7;
  M_Q7(3, 2) = sqrt_kappa_q7*sinh(phi_q7);
  M_Q7(3, 3) = cosh(phi_q7);
  M_Q7(4, 4) = 1.;

  // Q 8 focusses in y...
  double phi_q8 = sqrt_kappa_q8*L;
  M_Q8(2, 2) = cos(phi_q8);
  M_Q8(2, 3) = sin(phi_q8)/sqrt_kappa_q8;
  M_Q8(3, 2) = -sin(phi_q8)*sqrt_kappa_q8;
  M_Q8(3, 3) = cos(phi_q8);
  // Q 8 de-focusses in x...
  M_Q8(0, 0) = cosh(phi_q8);
  M_Q8(0, 1) = sinh(phi_q8)/sqrt_kappa_q8;
  M_Q8(1, 0) = sqrt_kappa_q7*sinh(phi_q8);
  M_Q8(1, 1) = cosh(phi_q8);
  M_Q8(4, 4) = 1.;

  // Q 9 focusses in x...
  double phi_q9 = sqrt_kappa_q9*L;
  M_Q9(0, 0) = cos(phi_q9);
  M_Q9(0, 1) = sin(phi_q9)/sqrt_kappa_q9;
  M_Q9(1, 0) = -sin(phi_q9)*sqrt_kappa_q9;
  M_Q9(1, 1) = cos(phi_q9);
  // Q 9 de-focusses in y...
  M_Q9(2, 2) = cosh(phi_q9);
  M_Q9(2, 3) = sinh(phi_q9)/sqrt_kappa_q9;
  M_Q9(3, 2) = sqrt_kappa_q9*sinh(phi_q9);
  M_Q9(3, 3) = cosh(phi_q9);
  M_Q9(4, 4) = 1.;

  TMatrixD temp1(5, 5);
  temp1 = TMatrixD(M_Q7, TMatrixD::kMult, M_0);
  TMatrixD temp2(5, 5);
  temp2 = TMatrixD(temp1, TMatrixD::kMult, M_Q8);
  TMatrixD temp3(5, 5);
  temp3 = TMatrixD(temp2, TMatrixD::kMult, M_1);
  // F = M_Q7 * M_0 * M_Q8 * M_1 * M_Q9
  _F = TMatrixD(temp3, TMatrixD::kMult, M_Q9);
  M_Q7.Print();
  M_0.Print();
  M_Q8.Print();
  M_1.Print();
  M_Q9.Print();
  std::cerr << "Triplet Projection Matrix is: " << "\n";
  _F.Print();

}

void KalmanSSTrack::straight_line(double deltaZ) {
  for ( int i = 0; i < 5; i++ ) {
    _F(i, i) = 1.;
  }
  _F(0, 1) = deltaZ;
  _F(2, 3) = deltaZ;
}

void KalmanSSTrack::calc_predicted_state(KalmanSite *old_site, KalmanSite *new_site) {
  // std::cout <<" ----------------------- Projection ----------------------- \n";
  TMatrixD a = old_site->get_a();

  TMatrixD a_projected = TMatrixD(_F, TMatrixD::kMult, a);

  new_site->set_projected_a(a_projected);

  // a_projected.Print();
}

void KalmanSSTrack::calc_system_noise(KalmanSite *site) {
  _Q.Zero();
/*  TMatrixD a(5, 1);
  a = site->get_a();
  double mx = a(2, 0);
  double my = a(3, 0);
  double kappa = a(4, 0);
  double Z = 1.;
  double r0 = 0.00167; // cm3/g
  double p = 1./kappa; // MeV/c
  double v = p/105.7;
  double C = 13.6*Z*pow(r0, 0.5)*(1.+0.038*log(r0))/(v*p);

  _Q(2, 2) = (1.+pow(mx, 2.))*(1.+pow(mx, 2.)+pow(my, 2.))*C;
  _Q(3, 3) = (1.+pow(my, 2.))*(1.+pow(mx, 2.)+pow(my, 2.))*C;
  _Q(2, 3) = mx*my*(1.+mx*mx+my*my)*C;
  _Q(3, 2) = mx*my*(1.+mx*mx+my*my)*C;

  _Q(4, 4) = kappa*kappa*my*my*C/(1.+mx*mx);
  _Q(3, 4) = kappa * my * (1.+mx*mx+my*my) * C /(1.+mx*mx);
*/
}

//
// ------- Filtering ------------
//
void KalmanSSTrack::update_G(KalmanSite *a_site) {
  // std::cout <<"Site ID:" << a_site->get_id() << "\n";
  // std::cout <<" ----------------------- Filtering ----------------------- \n";
  double sig_beta, SIG_ALPHA;
  switch ( a_site->get_id() ) {
    case 0 : case 1: // TOF0
      SIG_ALPHA = 40./sqrt(12.);
      sig_beta  = 400./sqrt(12.);
      break;
    case 9 : case 10: // TOF1
      SIG_ALPHA = 60./sqrt(12.);
      sig_beta  = 420./sqrt(12.);
      break;
    default :
      double alpha = (a_site->get_measurement())(0, 0);
      double l = pow(ACTIVE_RADIUS*ACTIVE_RADIUS -
                 (alpha*CHAN_WIDTH)*(alpha*CHAN_WIDTH), 0.5);
      sig_beta = (l/CHAN_WIDTH)/sqrt(12.);
      SIG_ALPHA = 1.0/sqrt(12.);
  }
  _G.Zero();
  _G(0, 0) = SIG_ALPHA*SIG_ALPHA;
  _G(1, 1) = sig_beta*sig_beta;
  _V.Zero();
  _V = _G;
  _G.Invert();
}

void KalmanSSTrack::update_H(KalmanSite *a_site) {
  CLHEP::Hep3Vector dir = a_site->get_direction();
  double dx = dir.x();
  double dy = dir.y();
  std::cerr << "dir" << dx << " " << dy << "\n";
  double A; // mm to channel conversion factor.
  switch ( a_site->get_id() ) {
    case 0 : case 1 :
      A = 40.;
      _H.Zero();
      _H(0, 0) =  dy/A;
      _H(0, 2) =  dx/A;
      break;
    case 9: case 10 :
      A = 60.;
      _H.Zero();
      _H(0, 0) =  dy/A;
      _H(0, 2) =  dx/A;
      break;
    default :
      A = 2./(7.*0.427);
      _H.Zero();
      _H(0, 0) = -A*dy;
      _H(0, 2) =  A*dx;
  }
}

void KalmanSSTrack::update_covariance(KalmanSite *a_site) {
  // calc_covariance_matrix_of_residual
  // Cp = ( C_inv + Ht*G*H )-1;
/*
  TMatrixD C = a_site->get_projected_covariance_matrix();
  TMatrixD C_inv = C.Invert();

  TMatrixD temp1(5, 2);
  TMatrixD temp2(5, 5);
  temp1 = TMatrixD(_H, TMatrixD::kTransposeMult, _G);
  temp2 = TMatrixD(temp1, TMatrixD::kMult, _H);
  TMatrixD Cp(5, 5);
  Cp = TMatrixD(C_inv, TMatrixD::kPlus, temp2);
  Cp.Invert();
  a_site->set_covariance_matrix(Cp);
  std::cout << "Updated Covariance \n";
  Cp.Print();
  // assert(site.state() == "filtered");
*/

  // calc_covariance_matrix_of_residual
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

// h1(a_1^0)
void KalmanSSTrack::calc_filtered_state(KalmanSite *a_site) {
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
  // Kalman Gain: K = Cp Ht G
  //
/*
  TMatrixD C(5, 5);
  C = a_site->get_covariance_matrix();
  TMatrixD temp3(5, 2);
  temp3 = TMatrixD(C, TMatrixD::kMultTranspose, _H);
  TMatrixD K(5, 2);
  K = TMatrixD(temp3, TMatrixD::kMult, _G);

  std::cout << "Kalman Gain: \n";
  K.Print();
*/

  // std::cout << "Pull: \n";
  // pull.Print();
  // std::cout << "Alpha measured \n";
  // m.Print();
  // std::cout << "Alpha projected \n";
  // ha.Print();

  TMatrixD C(5, 5);
  C = a_site->get_projected_covariance_matrix();

  TMatrixD term1(5, 2);
  term1 = TMatrixD(C, TMatrixD::kMultTranspose, _H);

  TMatrixD temp3(2, 5);
  temp3 = TMatrixD(_H, TMatrixD::kMult, C);
  TMatrixD temp31(2, 2);
  temp31 = TMatrixD(temp3, TMatrixD::kMultTranspose, _H);
  TMatrixD term2(2, 2);
  term2 = TMatrixD(temp31, TMatrixD::kPlus, _V);
  term2.Invert();

  _K.Zero();
  _K = TMatrixD(term1, TMatrixD::kMult, term2);
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
  double res_y = a_filt(1, 0) - a(1, 0);

  a_site->set_residual_x(res_x);
  a_site->set_residual_y(res_y);

  // std::cout << "Filtered State: \n";
  // a_filt.Print();
  // std::cout << "x  residual: "  << res_x << "\n";
  // std::cout << "y  residual: "  << res_y << "\n";
}

//
// Smoothing
//
void KalmanSSTrack::update_back_transportation_matrix(KalmanSite *optimum_site,
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

void KalmanSSTrack::smooth_back(KalmanSite *optimum_site, KalmanSite *smoothing_site) {
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

  TMatrixD a_smooth(5, 1);
  a_smooth =  TMatrixD(a, TMatrixD::kPlus, temp2);
  smoothing_site->set_smoothed_a(a_smooth);

  // Save chi2 for this site.
  TMatrixD m(2, 1);
  m = smoothing_site->get_measurement();
  double alpha = m(0, 0);

  TMatrixD ha(2, 1);
  update_H(smoothing_site);
  ha = TMatrixD(_H, TMatrixD::kMult, a_smooth);
  // Extrapolation converted to expected measurement.
  double alpha_model = ha(0, 0);

  double sigma_measurent_squared;
  double chi2_i;
  int id = smoothing_site->get_id();
  if ( id == 0 || id == 1 ) {
    sigma_measurent_squared = 11.5*11.5; // mm
    chi2_i = pow(alpha-alpha_model, 2.)/sigma_measurent_squared;
  } else if ( id == 9 || id == 10 ) {
    sigma_measurent_squared = 17.3*17.3; // mm
    chi2_i = pow(alpha-alpha_model, 2.)/sigma_measurent_squared;
  } else if ( id == 6 || id == 7 || id == 8 ) {
    sigma_measurent_squared = 1./12.; // mm
    chi2_i = pow(alpha-alpha_model, 2.)/sigma_measurent_squared;
  } else {
    chi2_i = 0.;
  }

  smoothing_site->set_chi2(chi2_i);

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

void KalmanSSTrack::compute_chi2(const std::vector<KalmanSite> &sites) {
  int number_parameters = 5;
  int number_of_sites = 7;
  int id = sites[0].get_id();
  _tracker = 0;

  // double alpha, model_alpha;
  for ( int i = 0; i < number_of_sites; ++i ) {
    KalmanSite site = sites[i];
    _chi2 += site.get_chi2();
  }
  _ndf = number_of_sites - number_parameters;
  std::ofstream output("chi2.txt", std::ios::out | std::ios::app);
  output << _tracker << " " << _chi2 << " " << _ndf << "\n";
  output.close();
}
