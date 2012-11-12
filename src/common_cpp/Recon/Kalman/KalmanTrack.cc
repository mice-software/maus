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
                             _mass(0.), _momentum(0.),
                             _active_radius(150.) {
  // Measurement equation.
  _H.ResizeTo(2, 5);
  _H.Zero();
  // Propagator.
  _F.ResizeTo(5, 5);
  _F.Zero();
  // Back transport.
  _A.ResizeTo(5, 5);
  _A.Zero();
  // Measurement error.
  _V.ResizeTo(2, 2);
  _V.Zero();
  // Kalman gain.
  _K.ResizeTo(5, 2);
  _K.Zero();
  // MCS error.
  _Q.ResizeTo(5, 5);
  _Q.Zero();
  // Alignment shifts
  _S.ResizeTo(2, 3);
  _S.Zero();
  // Alignment rotations
  _R.ResizeTo(2, 3);
  _R.Zero();

  _W.ResizeTo(2, 2);
  _W.Zero();
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
  /*
  double mx = a_old_site(1, 0);
  double my = a_old_site(3, 0);
  double kappa = a_old_site(4, 0);
  double pz = 1./kappa;
  double px = mx*pz;
  double py = my*pz;
  */
  double px = a_old_site(1, 0);
  double py = a_old_site(3, 0);
  double kappa = a_old_site(4, 0);
  double pz = 1./kappa;

  // assert(pz > 80. && "Previous site Pz is bad.");

  double momentum = pow(px*px+py*py+pz*pz, 0.5);
  // assert(momentum < 10000.);
  assert(_mass == _mass && "mass is not defined");

  double tau = momentum/_mass;
  double tau_squared = tau*tau;

  double thickness, density;
  get_site_properties(old_site, thickness, density);

  double F_tau = pow(1.+tau_squared, 1.5)/(11.528*tau*tau*tau)*
                 (9.0872+2.*log(tau)-tau_squared/(1.+tau_squared));

  double delta_p =minimum_ionization_energy*density*thickness*F_tau/6.;

  double lambda = atan(pow(px*px+py*py,0.5)/pz);
  // assert(tan(lambda)<1 && "Lambda: pt < pz");

  double delta_pt = delta_p * tan(lambda);
  double delta_pz = delta_p * cos(lambda);

  double pt = tan(lambda)*pz;
  double phi = acos(px/pt);
  double delta_px = delta_pt*cos(phi);
  double delta_py = delta_pt*sin(phi);

  TMatrixD a_projected(5, 1);
  a_projected = new_site->get_projected_a();
  double x  = a_projected(0, 0);
  double px_projected = a_projected(1, 0);
  double y  = a_projected(2, 0);
  double py_projected = a_projected(3, 0) - delta_py;
  double kappa_projected = a_projected(4, 0);

  double new_px = px_projected - delta_px;
  double new_py = py_projected - delta_py;
  double new_kappa = 1./(1./kappa_projected - delta_pz);

  TMatrixD a_subtracted(5, 1);
  a_subtracted(0, 0) = x;
  a_subtracted(1, 0) = new_px;
  a_subtracted(2, 0) = y;
  a_subtracted(3, 0) = new_py;
  a_subtracted(4, 0) = new_kappa;

  new_site->set_projected_a(a_subtracted);
/*
  std::ofstream out2("e_loss.txt", std::ios::out | std::ios::app);
  out2 << new_site->get_id() << " " << px << " " << delta_px << " " << py << " "
       << delta_py << " " << pz << " " << delta_pz << "\n";
  out2.close();
*/
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

  double Z = 9.;
  double r0 = 0.00167; // cm3/g
  double pz = 1./kappa; // MeV/c
  double px = mx/kappa;
  double py = my/kappa;
  double p = pow(px*px+py*py+pz*pz, 0.5);

  double v = p/_mass;
  double C = 13.6*Z*pow(r0, 0.5)*(1.+0.038*log(r0))/(v*p);
  double C2 = C*C;
  double grad_to_mom = 1.; // pow(1./kappa, 1.); // convertion factor: gradients to momentum

  double c_mx_mx = grad_to_mom*(1.+pow(mx, 2.))*(1.+pow(mx, 2.)+pow(my, 2.))*C2;
  double c_my_my = grad_to_mom*(1.+pow(my, 2.))*(1.+pow(mx, 2.)+pow(my, 2.))*C2;
  double c_mx_my = grad_to_mom*mx*my*(1.+pow(mx, 2.)+pow(my, 2.))*C2;

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

  // _Q.Zero();
}

//
// ------- Filtering ------------
//
void KalmanTrack::update_V(KalmanSite *a_site) {
  double sigma_beta;
  switch ( a_site->get_type() ) {
    case 0 : // TOF0
      sigma_beta = 10./sqrt(12.);
      break;
    case 1 : // TOF1
      sigma_beta = 7./sqrt(12.);
      break;
    case 2 : // SciFi
      double alpha = (a_site->get_measurement())(0, 0);
      double pitch = a_site->get_pitch();
      double l = pow(_active_radius*_active_radius -
                 (alpha*pitch)*(alpha*pitch), 0.5);
      if ( l != l ) l = 150.;
      sigma_beta = (l/pitch)/sqrt(12.);
  }

  double sigma_alpha = 1.0/sqrt(12.);

  _V.Zero();
  _V(0, 0) = sigma_alpha*sigma_alpha;
  _V(1, 1) = sigma_beta*sigma_beta;
}

void KalmanTrack::update_H(KalmanSite *a_site) {
  double pitch = a_site->get_pitch();

  CLHEP::Hep3Vector dir = a_site->get_direction();
  double sin_theta = dir.x();
  double cos_theta = dir.y();

  TMatrixD a(5, 1);
  a = a_site->get_a();
  double x_0 = a(0, 0);
  double y_0 = a(2, 0);

  TMatrixD shift_misalign(3,1);
  shift_misalign = a_site->get_shifts();
  TMatrixD rotation_misalign(3,1);
  rotation_misalign = a_site->get_rotations();

  double x_d = shift_misalign(0, 0);
  double y_d = shift_misalign(1, 0);
  double theta_y = rotation_misalign(1, 0);

  // Define useful factors
  double cos_theta_cos_thetay = cos_theta*cos(theta_y);
  double sin_theta_sin_thetay = sin_theta*sin(theta_y);
  double sin_theta_cos_thetay = sin_theta*cos(theta_y);
  double cos_theta_sin_thetay = cos_theta*sin(theta_y);

  if ( pitch ) {
    _H.Zero();
    _H(0, 0) = - (cos_theta_cos_thetay+sin_theta_sin_thetay)/pitch;
    _H(0, 2) =   (sin_theta_cos_thetay-cos_theta_sin_thetay)/pitch;
    _S.Zero();
    _S(0, 0) = (cos_theta_cos_thetay-sin_theta_sin_thetay)/pitch;
    _S(0, 1) = -(sin_theta_cos_thetay-cos_theta_sin_thetay)/pitch;
    _R.Zero();
    _R(0, 1) = -((x_0-x_d)/pitch)*(-cos_theta_sin_thetay+sin_theta_cos_thetay)+
                ((y_0-y_d)/pitch)*(-sin_theta_sin_thetay-cos_theta_cos_thetay);
  } else {
    _H.Zero();
    _S.Zero();
    _R.Zero();
  }
}

// W = [ V + H C_k-1 Ht + S cov_S_k-1 St + R cov_R_k-1 Rt ]-1
// W = [ V + A + B + C]-1
void KalmanTrack::update_W(KalmanSite *a_site) {
  TMatrixD C_a = a_site->get_covariance_matrix();
  TMatrixD C_r = a_site->get_R_covariance();
  TMatrixD C_s = a_site->get_S_covariance();

  TMatrixD A = TMatrixD(TMatrixD(_H, TMatrixD::kMult, C_a),
                        TMatrixD::kMultTranspose,
                        _H);
  TMatrixD B = TMatrixD(TMatrixD(_S, TMatrixD::kMult, C_s),
                        TMatrixD::kMultTranspose,
                        _S);
  TMatrixD C = TMatrixD(TMatrixD(_R, TMatrixD::kMult, C_r),
                        TMatrixD::kMultTranspose,
                        _R);

  _W.Zero();
  _W = TMatrixD(TMatrixD(TMatrixD(_V, TMatrixD::kPlus, A), TMatrixD::kPlus, B), TMatrixD::kPlus, C);
  _W.Invert();
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

TMatrixD KalmanTrack::solve_measurement_equation(KalmanSite *site) {
  TMatrixD a(5, 1);
  a = site->get_projected_a();
  TMatrixD ha(2, 1);
  ha = TMatrixD(_H, TMatrixD::kMult, a);

  TMatrixD s(3, 1);
  s = site->get_shifts();
  TMatrixD Ss(2, 1);
  Ss = TMatrixD(_S, TMatrixD::kMult, s);

  TMatrixD r(3, 1);
  r = site->get_rotations();
  TMatrixD Rr(2, 1);
  Rr = TMatrixD(_R, TMatrixD::kMult, r);

  TMatrixD measurement(2, 1);
  TMatrixD temp1(2, 1);

  temp1 = TMatrixD(ha,TMatrixD::kPlus,Ss);
  measurement = TMatrixD(temp1,TMatrixD::kPlus,Rr);

  return measurement;
}

void KalmanTrack::update_misaligments(KalmanSite *a_site) {
  // ***************************
  // Calculate the pull,
  TMatrixD pull(2, 1);
  pull = get_pull(a_site);
/*
  // ***************************
  // update the Kalman gain,
  _K.Zero();
  _K = get_kalman_gain(a_site);
  // ***************************
  // compute the filtered state via
  // a' = a + K*pull
  TMatrixD temp(5, 1);
  temp = TMatrixD(_K, TMatrixD::kMult, pull);
  TMatrixD a_filt(5, 1);
  TMatrixD a(5, 1);
  a = a_site->get_projected_a();
  a_filt = TMatrixD(a, TMatrixD::kPlus, temp);
  a_site->set_a(a_filt);

  // Residuals. x and y.
  double res_x = a_filt(0, 0) - a(0, 0);
  double res_y = a_filt(2, 0) - a(2, 0);

  a_site->set_residual_x(res_x);
  a_site->set_residual_y(res_y);
*/
  //////////////////////////////////////////////////////////////////////
  // Misalignment vector updates
  // sk = sk-1 + Ks*pull;
/*
  TMatrixD Ks(3, 2);
  TMatrixD Cov_s(3, 3);
  Cov_s = a_site->get_S_covariance();
  TMatrixD temp3(3, 2);
  temp3 = TMatrixD(Cov_s, TMatrixD::kMultTranspose, _S);
  Ks = TMatrixD(temp3, TMatrixD::kMult, _W);
*/
  TMatrixD Cov_s = a_site->get_S_covariance();
  TMatrixD Ks = TMatrixD(TMatrixD(Cov_s, TMatrixD::kMultTranspose, _S),
                         TMatrixD::kMult,
                         _W);
  Ks.Print();

  TMatrixD shifts(3, 1);
  TMatrixD new_shifts(3, 1);
  shifts = a_site->get_shifts();
  TMatrixD temp5(3, 1);
  temp5 = TMatrixD(Ks, TMatrixD::kMult, pull);

  new_shifts = shifts + temp5;
  // new covariance
  TMatrixD new_Cov_s(3, 3);
  TMatrixD temp7(2, 3);
  temp7 = TMatrixD(_S, TMatrixD::kMult, Cov_s);
  TMatrixD temp8(3, 3);
  temp8 = TMatrixD(Ks, TMatrixD::kMult, temp7);
  new_Cov_s = TMatrixD(Cov_s, TMatrixD::kPlus, temp8);
  new_shifts.Print();
  new_Cov_s.Print();
  a_site->set_shifts(new_shifts);
  a_site->set_S_covariance(new_Cov_s);
  //////////////////////////////////////////////////////////////////////
  TMatrixD Kr(3, 2);
  TMatrixD Cov_r(3, 3);
  Cov_r = a_site->get_R_covariance();

  TMatrixD temp4(3, 2);
  temp4 = TMatrixD(Cov_r, TMatrixD::kMultTranspose, _R);
  Kr = TMatrixD(temp4, TMatrixD::kMult, _W);

  TMatrixD rotations(3, 1);
  TMatrixD new_rotations(3, 1);
  rotations = a_site->get_rotations();
  TMatrixD temp6(2, 2);
  temp6 = TMatrixD(Kr, TMatrixD::kMult, pull);

  new_rotations = shifts + temp6;
  // new covariance
  TMatrixD new_Cov_r(3, 3);
  TMatrixD temp9(3, 3);
  temp9 = TMatrixD(_R, TMatrixD::kMult, Cov_r);
  TMatrixD temp10(3, 3);
  temp10 = TMatrixD(Kr, TMatrixD::kMult, temp9);
  new_Cov_r = TMatrixD(Cov_r, TMatrixD::kPlus, temp10);
  new_rotations.Print();
  new_Cov_r.Print();
  a_site->set_rotations(new_rotations);
  a_site->set_R_covariance(new_Cov_r);
}


TMatrixD KalmanTrack::get_pull(KalmanSite *a_site) {
  // PULL = m - ha
  TMatrixD m = a_site->get_measurement();
  //m = a_site->get_measurement();

  TMatrixD a = a_site->get_projected_a();
  //a = a_site->get_projected_a();
  // TMatrixD ha(1, 1);
  TMatrixD f(2, 1);
  f = solve_measurement_equation(a_site);
  // Extrapolation converted to expected measurement.
  double alpha_model = f(0, 0);
  a_site->set_projected_alpha(alpha_model);

  TMatrixD pull(2, 1);
  pull = TMatrixD(m, TMatrixD::kMinus, f);
  pull.Print();
  return pull;
}

TMatrixD KalmanTrack::get_kalman_gain(KalmanSite *a_site) {
  // Kalman Gain: K = C Ht (V + H C Ht)-1
  //              K =  A   (V +  B )-1
  //              K =  A W
  TMatrixD C = a_site->get_projected_covariance_matrix();
  //C = a_site->get_projected_covariance_matrix();

  TMatrixD A(5, 2);
  A = TMatrixD(C, TMatrixD::kMultTranspose, _H);

  TMatrixD temp1(2, 5);
  temp1 = TMatrixD(_H, TMatrixD::kMult, C);
  TMatrixD B(2, 2);
  B = TMatrixD(temp1, TMatrixD::kMultTranspose, _H);
  TMatrixD temp2(2, 2);
  temp2 = TMatrixD(_V, TMatrixD::kPlus, B);
  temp2.Invert();

  update_W(a_site);
  std::cerr << " ------------- W -------------\n";
  _W.Print();
  temp2.Print();
  std::cerr << " ------------- ENDS -------------\n";
  assert( abs(_W(0, 0)-temp2(0, 0))<0.1  && (_W(1, 1)-temp2(1, 1))<0.1 && "W calculation gone wrong.");
  //TMatrixD K(5, 2);
  //K.Zero();
  TMatrixD K = TMatrixD(A, TMatrixD::kMult, temp2);

  //TMatrixD K = TMatrixD(TMatrixD(C, TMatrixD::kMultTranspose, _H),
  //             TMatrixD::kMult,
  //             _W);
  K.Print();
  return K;
}

void KalmanTrack::calc_filtered_state(KalmanSite *a_site) {
  // ***************************
  // Calculate the pull,
  TMatrixD pull = get_pull(a_site);
  //pull = get_pull(a_site);
  // ***************************
  // update the Kalman gain,
  //_K.Zero();
  _K = get_kalman_gain(a_site);
  // ***************************
  // compute the filtered state via
  // a' = a + K*pull
  TMatrixD temp(5, 1);
  temp = TMatrixD(_K, TMatrixD::kMult, pull);
  TMatrixD a_filt(5, 1);
  TMatrixD a(5, 1);
  a = a_site->get_projected_a();
  a_filt = TMatrixD(a, TMatrixD::kPlus, temp);
  a_site->set_a(a_filt);

  // Residuals. x and y.
  double res_x = a_filt(0, 0) - a(0, 0);
  double res_y = a_filt(2, 0) - a(2, 0);

  a_site->set_residual_x(res_x);
  a_site->set_residual_y(res_y);

  //if ( _chi2 )
    //update_misaligments(a_site);
}

//
// Smoothing
//
void KalmanTrack::update_back_transportation_matrix(KalmanSite *optimum_site,
                                                    KalmanSite *smoothing_site) {
  update_propagator(smoothing_site, optimum_site);
  TMatrixD Cp(5, 5);
  Cp = optimum_site->get_projected_covariance_matrix();
  // std::cerr << "INVERTING\n";
  // Cp.Print();
  Cp.Invert();
  // std::cerr << "INVERTED!!!\n";
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
    std::cerr << _chi2 << " ";
  }
  std::cerr << "\n";

  _ndf = number_of_sites - number_parameters;
  std::ofstream output("chi2.txt", std::ios::out | std::ios::app);
  output << _tracker << " " << _chi2 << " " << _ndf << " " << _chi2/_ndf << "\n";
  output.close();
}

} // ~namespace MAUS
