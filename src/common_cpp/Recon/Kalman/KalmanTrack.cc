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

KalmanTrack::KalmanTrack() : _chi2(0.), _ndf(0.), _tracker(-1),
                             _mass(0.), _momentum(0.), _conv_factor(0.),
                             _active_radius(150.), _channel_width(1.4945) {
  _H.ResizeTo(2, 5);
  _H.Zero();
  _F.ResizeTo(5, 5);
  _F.Zero();
  _A.ResizeTo(5, 5);
  _A.Zero();
  _V.ResizeTo(2, 2);
  _V.Zero();
  _K.ResizeTo(5, 2);
  _K.Zero();
  _Q.ResizeTo(5, 5);
  _Q.Zero();
}

//
// ------- Prediction ------------
//
void KalmanTrack::calc_predicted_state(KalmanSite *old_site, KalmanSite *new_site) {
  TMatrixD a = old_site->get_a();

  TMatrixD a_projected = TMatrixD(_F, TMatrixD::kMult, a);

  new_site->set_projected_a(a_projected);
}

//
// C_proj = _F * C * _Ft + _Q;
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

void KalmanTrack::subtract_energy_loss(KalmanSite *old_site, KalmanSite *new_site) {
  double minimum_ionization_energy = 1.; // MeV cm2 / g

  TMatrixD a_old_site(5, 1);
  a_old_site = old_site->get_a();
  double mx = a_old_site(1, 0);
  double my = a_old_site(3, 0);
  double kappa = a_old_site(4, 0);
  double pz = 1./kappa;
  double px = mx*pz;
  double py = my*pz;
  double momentum = pow(px*px+py*py+pz*pz, 0.5);

  double tau = momentum/_mass;
  double tau_squared = tau*tau;

  double thickness, density;
  get_site_properties(old_site, thickness, density);

  double F_tau = pow(1.+tau_squared, 1.5)/(11.528*tau*tau*tau)*
                 (9.0872+2.*log(tau)-tau_squared/(1.+tau_squared));

  double delta_p =minimum_ionization_energy*density*thickness*F_tau;
  TMatrixD a_new(5, 1);
  a_new = new_site->get_projected_a();
  double x  = a_new(0, 0);
  double new_mx = a_new(1, 0);
  double y  = a_new(2, 0);
  double new_my = a_new(3, 0);
  double new_kappa = a_new(4, 0);

  TMatrixD a_subtracted(5, 1);
  a_subtracted(0, 0) = x;
  a_subtracted(1, 0) = new_mx;
  a_subtracted(2, 0) = y;
  a_subtracted(3, 0) = new_my;
  a_subtracted(4, 0) = 1./(1./new_kappa - delta_p);

  new_site->set_projected_a(a_subtracted);
}

void KalmanTrack::get_site_properties(KalmanSite *site, double &thickness, double &density) {
  switch ( site->get_type() ) {
    case 0 :
      thickness = 25.;
      density = 1.023; // density of EJ-204
      break;
    case 1 :
      thickness = 25.;
      density   = 1.023;
      break;
    case 2 :
      thickness = 0.670;
      density   = 1.0;
      break;
    case 3 : // Cherenkov
      thickness = 0.1*5000.; // 5 mil sheets of 0.1 mm
      density   = 0.934; // g/cm3
      break;
    default :
      thickness = 0.0;
      density  = 0.0;
  }
}

void KalmanTrack::calc_system_noise(KalmanSite *old_site, KalmanSite *new_site) {
  // Find dz.
  double new_z = new_site->get_z();
  double old_z = old_site->get_z();

  double deltaZ = (new_z-old_z); // deltaZ in mm
  double deltaZ_squared = deltaZ*deltaZ;

  TMatrixD a(5, 1);
  a = old_site->get_a();
  double mx    = a(1, 0);
  double my    = a(3, 0);
  double kappa = a(4, 0);

  double Z = 1.;
  double r0 = 0.00167; // cm3/g
  double p = 1./kappa; // MeV/c
  double v = p/105.7;
  double C = 13.6*Z*pow(r0, 0.5)*(1.+0.038*log(r0))/(v*p);
  double C2 = C*C;

  double c_mx_mx = (1.+pow(mx, 2.))*(1.+pow(mx, 2.)+pow(my, 2.))*C2;
  double c_my_my = (1.+pow(my, 2.))*(1.+pow(mx, 2.)+pow(my, 2.))*C2;
  double c_mx_my = mx*my*(1.+pow(mx, 2.)+pow(my, 2.))*C2;

  _Q.Zero();
  // x x
  _Q(0, 0) = deltaZ_squared*c_mx_mx;
  // x mx
  _Q(0, 1) = deltaZ*c_mx_mx;
  _Q(1, 0) = deltaZ*c_mx_mx;
  // x y
  _Q(0, 2) = deltaZ_squared*c_mx_my;
  _Q(2, 0) = deltaZ_squared*c_mx_my;
  // x my
  _Q(0, 3) = deltaZ*c_mx_my;
  _Q(3, 0) = deltaZ*c_mx_my;
  // mx mx
  _Q(1, 1) = c_mx_mx;
  // mx y
  _Q(1, 2) = deltaZ*c_mx_my;
  _Q(2, 1) = deltaZ*c_mx_my;
  // mx my
  _Q(1, 3) = c_mx_my;
  _Q(3, 1) = c_mx_my;
  // y y
  _Q(2, 2) = deltaZ_squared*c_my_my;
  // y my
  _Q(2, 3) = deltaZ*c_my_my;
  _Q(3, 2) = deltaZ*c_my_my;
  // my my
  _Q(3, 3) = c_my_my;

  _Q.Zero();
}

//
// ------- Filtering ------------
//
void KalmanTrack::update_V(KalmanSite *a_site) {
  double sig_beta, SIG_ALPHA;
  switch ( a_site->get_type() ) {
    case 0 : // TOF0
      SIG_ALPHA = 1.0/sqrt(12.);
      sig_beta = 10./sqrt(12.);
      // SIG_ALPHA = 40./sqrt(12.);
      // sig_beta  = 400./sqrt(12.);
      break;
    case 1 : // TOF1
      SIG_ALPHA = 1.0/sqrt(12.);
      sig_beta = 7./sqrt(12.);
      // SIG_ALPHA = 60./sqrt(12.);
      // sig_beta  = 420./sqrt(12.);
      break;
    case 2 : // SciFi
      double alpha = (a_site->get_measurement())(0, 0);
      double l = pow(_active_radius*_active_radius -
                 (alpha*_channel_width)*(alpha*_channel_width), 0.5);
      if ( l != l ) l = 150.;
      sig_beta = (l/_channel_width)/sqrt(12.);
      SIG_ALPHA = 1.0/sqrt(12.);
  }
  _V.Zero();
  _V(0, 0) = SIG_ALPHA*SIG_ALPHA;
  _V(1, 1) = sig_beta*sig_beta;
}

void KalmanTrack::update_H(KalmanSite *a_site) {
  CLHEP::Hep3Vector dir = a_site->get_direction();
  double dx = dir.x();
  double dy = dir.y();

  switch ( a_site->get_type() ) {
    case 0 :
      _conv_factor = 40.;
      _H.Zero();
      _H(0, 0) =  dy/_conv_factor;
      _H(0, 2) =  dx/_conv_factor;
      break;
    case 1 :
      _conv_factor = 60.;
      _H.Zero();
      _H(0, 0) =  dy/_conv_factor;
      _H(0, 2) =  dx/_conv_factor;
      break;
    case 2 :
      _conv_factor = (7.*0.427)/2.;
      _H.Zero();
      _H(0, 0) = -dy/_conv_factor;
      _H(0, 2) =  dx/_conv_factor;
      break;
    default : // not a detector. H and measurement are 0.
      _H.Zero();
  }
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
}

void KalmanTrack::calc_filtered_state(KalmanSite *a_site) {
  /////////////////////////////////////////////////////////////////////
  // PULL = m - ha
  //
  // TMatrixD m(1, 1);
  TMatrixD m(2, 1);
  m = a_site->get_measurement();

  TMatrixD a(5, 1);
  a = a_site->get_projected_a();
  // TMatrixD ha(1, 1);
  TMatrixD ha(2, 1);
  ha = TMatrixD(_H, TMatrixD::kMult, a);
  // Extrapolation converted to expected measurement.
  double alpha_model = ha(0, 0);
  a_site->set_projected_alpha(alpha_model);

  TMatrixD pull(2, 1);
  pull = TMatrixD(m, TMatrixD::kMinus, ha);
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

  // Residuals. x and y.
  double res_x = a_filt(0, 0) - a(0, 0);
  double res_y = a_filt(2, 0) - a(2, 0);

  a_site->set_residual_x(res_x);
  a_site->set_residual_y(res_y);
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

void KalmanTrack::prepare_for_smoothing(std::vector<KalmanSite> &sites) {
  int numb_measurements = sites.size();
  KalmanSite *smoothing_site = &sites[numb_measurements-1];

  TMatrixD a_smooth(5, 1);
  a_smooth = smoothing_site->get_a();
  smoothing_site->set_smoothed_a(a_smooth);

  TMatrixD C_smooth(5, 5);
  C_smooth = smoothing_site->get_covariance_matrix();
  smoothing_site->set_smoothed_covariance_matrix(C_smooth);

  // Save chi2 for this site.
  TMatrixD m(2, 1);
  m = smoothing_site->get_measurement();
  double alpha = m(0, 0);

  TMatrixD ha(2, 1);
  update_H(smoothing_site);
  ha = TMatrixD(_H, TMatrixD::kMult, a_smooth);

  // Extrapolation converted to expected measurement.
  double alpha_model = ha(0, 0);
  smoothing_site->set_smoothed_alpha(alpha_model);
  double sigma_measurement_squared = 1./12.;
  double chi2_i = pow(alpha-alpha_model, 2.)/sigma_measurement_squared;
  smoothing_site->set_chi2(chi2_i);
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
  smoothing_site->set_smoothed_alpha(alpha_model);
  double sigma_measurement_squared = 1./12.;
  double chi2_i = pow(alpha-alpha_model, 2.)/sigma_measurement_squared;
  smoothing_site->set_chi2(chi2_i);

  // do the same for covariance matrix
  TMatrixD C(5, 5);
  C = smoothing_site->get_covariance_matrix();

  TMatrixD C_opt(5, 5);
  C_opt = optimum_site->get_smoothed_covariance_matrix();
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
  int id = sites[0].get_id();

  if ( id <= 14 ) _tracker = 0;
  if ( id > 14 ) _tracker = 1;

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

} // ~namespace MAUS
