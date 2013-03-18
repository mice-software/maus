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

namespace MAUS {

KalmanTrack::KalmanTrack(bool MCS, bool Eloss) :
                             _use_MCS(MCS),
                             _use_Eloss(Eloss),
                             _f_chi2(0.),
                             _s_chi2(0.),
                             _ndf(0),
                             _P_value(0.),
                             _n_parameters(0),
                             _n_sites(0),
                             _tracker(-1),
                             _mass(105.65837),
                             _momentum(0.),
                             _particle_charge(1) {}

void KalmanTrack::init() {
  // Measurement equation.
  _H.ResizeTo(2, _n_parameters);
  _H.Zero();
  // Propagator.
  _F.ResizeTo(_n_parameters, _n_parameters);
  _F.Zero();
  // Back transportation matrix.
  _A.ResizeTo(_n_parameters, _n_parameters);
  _A.Zero();
  // Measurement error.
  _V.ResizeTo(2, 2);
  _V.Zero();
  // Kalman gain.
  _K.ResizeTo(_n_parameters, 2);
  _K.Zero();
  // MCS error.
  _Q.ResizeTo(_n_parameters, _n_parameters);
  _Q.Zero();
  // Alignment shifts
  _S.ResizeTo(2, 3);
  _S.Zero();
  // Weight matrix.
  _W.ResizeTo(2, 2);
  _W.Zero();
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ------- Extrapolation Routines ------------
//
void KalmanTrack::extrapolate(std::vector<KalmanSite> &sites, int i) {
  // Get current site...
  KalmanSite *new_site = &sites[i];

  // ... and the site we will extrapolate from.
  const KalmanSite *old_site = &sites[i-1];

  // Build the propagator matrix.
  // build_propagator(old_site, new_site);

  // Calculate prediction for the state vector.
  calc_predicted_state(old_site, new_site);

  // Calculate the energy loss for the projected state.
  if ( _n_parameters > 4 && _use_Eloss )
    subtract_energy_loss(old_site, new_site);

  // Calculate the system noise...
  if ( _use_MCS )
    calc_system_noise(old_site, new_site);

  // ... so that we can add it to the prediction for the
  // covariance matrix.
  calc_covariance(old_site, new_site);

  new_site->set_current_state(KalmanSite::Projected);
}

// C_proj = _F * C * _Ft + _Q;
void KalmanTrack::calc_covariance(const KalmanSite *old_site, KalmanSite *new_site) {
  TMatrixD C_old = old_site->get_covariance_matrix(KalmanSite::Filtered);

  TMatrixD _F_transposed(_n_parameters, _n_parameters);
  _F_transposed.Transpose(_F);

  TMatrixD C_new(_n_parameters, _n_parameters);
  C_new = _F*C_old*_F_transposed + _Q;

  new_site->set_covariance_matrix(C_new, KalmanSite::Projected);
}

// Returns (beta) * (-dE/dx). Formula and constants from PDG.
double KalmanTrack::BetheBlochStoppingPower(double p) {
  double muon_mass = 105.7; // MeV/c2
  double muon_mass2 = TMath::Power(muon_mass, 2.);
  double electron_mass = 0.511;

  double E = TMath::Sqrt(muon_mass2+p*p);

  double beta = p/E;
  double beta2= TMath::Power(beta, 2.);
  double gamma = E/muon_mass;
  double gamma2= TMath::Power(gamma, 2.);

  double K = BetheBlochParameters::K();
  double A = FibreParameters::A();
  double I = FibreParameters::Mean_Excitation_Energy();
  double I2= TMath::Power(I, 2.);
  double Z = FibreParameters::Z();

  double outer_term = K*Z/(A*beta2);

  double Tmax = 2.*electron_mass*beta2*gamma2/(1.+(2.*gamma*electron_mass/muon_mass) +
                TMath::Power(electron_mass/muon_mass, 2.));

  double log_term = TMath::Log(2*electron_mass*beta2*gamma2*Tmax/(I2));
  double last_term = TMath::Power(Tmax, 2.)/TMath::Power(gamma*muon_mass, 2);
  double density = FibreParameters::Density();
  double plasma_energy = 28.816*TMath::Sqrt(density*Z/A); // eV
  double density_term = TMath::Log(plasma_energy/I)+TMath::Log(beta*gamma)-0.5;
  double dEdx = outer_term*(0.5*log_term-beta2-density_term/2.+last_term/8.);

  return beta*dEdx;
}

void KalmanTrack::subtract_energy_loss(const KalmanSite *old_site, KalmanSite *new_site) {
  double plane_width = SciFiParams::Plane_Width();

  TMatrixD a_old(_n_parameters, 1);
  a_old = new_site->get_a(KalmanSite::Projected);
  double px = a_old(1, 0);
  double py = a_old(3, 0);
  double kappa = a_old(4, 0);
  double pz = 1./kappa;
  double pt = TMath::Sqrt(px*px+py*py);
  double lambda = atan2(pz, pt);

  double momentum = TMath::Sqrt(px*px+py*py+pz*pz);
  assert(_mass == _mass && "mass is not defined");

  int n_steps = 1;

  /* I think the planes are too thin for this integral. BB barely changes.
  double Delta_p = 0.;
  for ( int i = 0; i < n_steps; ++i ) {
    momentum += Delta_p;
    Delta_p += BetheBlochStoppingPower(momentum)*plane_width/n_steps;
  }
  */

  double Delta_p = BetheBlochStoppingPower(momentum)*plane_width;
  double new_pz;
  if ( _tracker == 0 ) {
    new_pz = (momentum+Delta_p)*sin(lambda);
  } else {
    new_pz = (momentum-Delta_p)*sin(lambda);
  }

  double new_kappa = 1./new_pz;
  TMatrixD a_subtracted(_n_parameters, 1);
  a_subtracted = new_site->get_a(KalmanSite::Projected);
  a_subtracted(4, 0) = new_kappa;

  new_site->set_a(a_subtracted, KalmanSite::Projected);
  /*
  double lambda = atan(pow(px*px+py*py, 0.5)/pz);
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
*/
}

void KalmanTrack::calc_system_noise(const KalmanSite *old_site, const KalmanSite *new_site) {
  double plane_width = SciFiParams::Plane_Width();

  double deltaZ = ( new_site->get_z() ) - ( old_site->get_z() );
  double deltaZ_squared = TMath::Power(deltaZ, 2.);

  TMatrixD a = new_site->get_a(KalmanSite::Projected);
  double mx    = a(1, 0);
  double my    = a(3, 0);

  //double kappa = a(4, 0);
  double momentum = _momentum;
  double kappa = 1./momentum;

  // Charge of incoming particle.
  double z = _particle_charge;
  // Plane lenght in units of radiation lenght (~0.0015).
  double L0 = SciFiParams::R0(plane_width);

  double pz = 1./kappa;
  double px = mx/kappa;
  double py = my/kappa;
  double p = TMath::Sqrt(px*px+py*py+pz*pz); // MeV/c

  double muon_mass = 105.7; // MeV/c2
  double muon_mass2 = TMath::Power(muon_mass, 2.);
  double E = TMath::Sqrt(muon_mass2+p*p);
  double beta = p/E;

  // Highland formula.
  double C = 13.6*z*TMath::Sqrt(L0)*(1.+0.038*TMath::Log(L0))/(beta*p);

  double C2 = TMath::Power(C, 2.);

  double c_mx_mx = C2 * (1. + TMath::Power(mx, 2.)) *
                        (1.+TMath::Power(mx, 2.)+TMath::Power(my, 2.));

  double c_my_my = C2 * (1. + TMath::Power(my, 2.)) *
                        (1.+TMath::Power(mx, 2.)+TMath::Power(my, 2.));

  double c_mx_my = C2 * mx*my * (1.+TMath::Power(mx, 2.) + TMath::Power(my, 2.));

  _Q.Zero();
  // x x
  _Q(0, 0) = deltaZ_squared*c_mx_mx;
  // x mx
  _Q(0, 1) = deltaZ*c_mx_mx; ///
  // x y
  _Q(0, 2) = deltaZ_squared*c_mx_my;
  // x my
  _Q(0, 3) = deltaZ*c_mx_my; ///

  // mx x
  _Q(1, 0) = deltaZ*c_mx_mx; ///
  // mx mx
  _Q(1, 1) = c_mx_mx;
  // mx y
  _Q(1, 2) = deltaZ*c_mx_my; ///
  // mx my
  _Q(1, 3) = c_mx_my;

  // y x
  _Q(2, 0) = deltaZ_squared*c_mx_my;
  // y mx
  _Q(2, 1) = deltaZ*c_mx_my; ///
  // y y
  _Q(2, 2) = deltaZ_squared*c_my_my;
  // y my
  _Q(2, 3) = deltaZ*c_my_my; ///

  // my x
  _Q(3, 0) = deltaZ*c_mx_my; ///
  // my mx
  _Q(3, 1) = c_mx_my;
  // my y
  _Q(3, 2) = deltaZ*c_my_my; ///
  // my my
  _Q(3, 3) = c_my_my;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ------- Filtering Routines ------------
//
void KalmanTrack::filter(std::vector<KalmanSite> &sites, int current_site) {
  // Get Site...
  KalmanSite *a_site = &sites[current_site];

  // Update measurement error:
  update_V(a_site);

  // Update H (depends on plane direction.)
  update_H(a_site);
  update_W(a_site);
  update_K(a_site);
  compute_pull(a_site);
  // a_k = a_k^k-1 + K_k x pull
  calc_filtered_state(a_site);

  // Cp = (C-KHC)
  update_covariance(a_site);

  a_site->set_current_state(KalmanSite::Filtered);
}

void KalmanTrack::update_V(const KalmanSite *a_site) {
  // Fibre constants.
  double pitch         = FibreParameters::Pitch();
  // Active radius in units of channel width.
  double active_radius = FibreParameters::Active_Radius()/pitch;

  double alpha = (a_site->get_measurement())(0, 0);
  double lenght = 2.*TMath::Sqrt(active_radius*active_radius -
                                 alpha*alpha);

  double sigma_beta = lenght/TMath::Sqrt(12.);
  double sigma_alpha = 1.0/TMath::Sqrt(12.);

  _V.Zero();
  _V(0, 0) = TMath::Power(sigma_alpha, 2.);
  _V(1, 1) = TMath::Power(sigma_beta, 2.);
}

void KalmanTrack::update_H(const KalmanSite *a_site) {
  CLHEP::Hep3Vector dir = a_site->get_direction();

  double dx = dir.x();
  double dy = dir.y();

  CLHEP::Hep3Vector perp = dir.orthogonal();

  double perp_x = perp.x();
  double perp_y = perp.y();

  // TMatrixD a = a_site->get_a(KalmanSite::Projected);
  // double x_0 = a(0, 0);
  // double y_0 = a(2, 0);

  // TMatrixD shifts = a_site->get_input_shift();
  // double x_d     = shifts(0, 0);
  // double y_d     = shifts(1, 0);
  // double theta_y = shifts(2, 0);

  double pitch = FibreParameters::Pitch();

  _H.Zero();
  _H(0, 0) = -dy/pitch;
  _H(0, 2) =  dx/pitch;
  _H(1, 0) = -perp_y/pitch;
  _H(1, 2) =  perp_x/pitch;

  _S.Zero();
  _S(0, 0) =  dy/pitch;
  _S(0, 1) = -dx/pitch;
  _S(1, 0) =  perp_y/pitch;
  _S(1, 1) = -perp_x/pitch;
}
/*
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
    //_R.Zero();
    //_R(0, 1) = -((x_0-x_d)/pitch)*(-cos_theta_sin_thetay+sin_theta_cos_thetay)+
    _H.Zero();
    _S.Zero();
    _R.Zero();
  }
*/

// W = [ V + H C_k-1 Ht + S cov_S_k-1 St ]-1
// W = [ V +    A       +      B         ]-1
void KalmanTrack::update_W(const KalmanSite *a_site) {
  TMatrixD C_a(_n_parameters, _n_parameters);
  C_a = a_site->get_covariance_matrix(KalmanSite::Projected);

  TMatrixD C_s = a_site->get_input_shift_covariance();

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

void KalmanTrack::update_K(const KalmanSite *a_site) {
  // Kalman Gain: K = C Ht (V + H C Ht)-1
  //              K =  A   (V +  B )-1
  //              K =  A W
  TMatrixD C = a_site->get_covariance_matrix(KalmanSite::Projected);

  TMatrixD A = TMatrixD(C, TMatrixD::kMultTranspose, _H);

  _K.Zero();
  _K = A * _W;
}

void KalmanTrack::compute_pull(KalmanSite *a_site) {
  // PULL = m - ha
  TMatrixD measurement = a_site->get_measurement();
  TMatrixD a           = a_site->get_a(KalmanSite::Projected);
  TMatrixD shifts      = a_site->get_input_shift();

  TMatrixD HA = solve_measurement_equation(a, shifts);

  TMatrixD pull(2, 1);
  pull = measurement - HA;

  a_site->set_residual(pull, KalmanSite::Projected);
}

TMatrixD KalmanTrack::solve_measurement_equation(const TMatrixD &a,
                                                 const TMatrixD &s) {
  TMatrixD ha(2, 1);
  ha = _H * a;

  TMatrixD Ss(2, 1);
  Ss = _S * s;

  TMatrixD result(2, 1);
  result = ha + Ss;

  return result;
}

void KalmanTrack::calc_filtered_state(KalmanSite *a_site) {
  // Calculate the pull,
  TMatrixD pull = a_site->get_residual(KalmanSite::Projected);

  // compute the filtered state via
  // a' = a + K*pull
  TMatrixD a = a_site->get_a(KalmanSite::Projected);

  TMatrixD a_filt(_n_parameters, 1);
  a_filt = a + _K*pull;

  a_site->set_a(a_filt, KalmanSite::Filtered);

  // Residual.
  set_residual(a_site, KalmanSite::Filtered);
}

void KalmanTrack::set_residual(KalmanSite *a_site, KalmanSite::State kalman_state) {
  // PULL = measurement - ha
  TMatrixD measurement = a_site->get_measurement();
  TMatrixD a = a_site->get_a(kalman_state);
  TMatrixD s = a_site->get_input_shift();

  // Solve the measurement equation again,
  // this time passing the FILTERED state to find the
  // residual.
  TMatrixD HA = solve_measurement_equation(a, s);

  TMatrixD residual(2, 1);
  residual = measurement - HA;

  a_site->set_residual(residual, kalman_state);

  TMatrixD C = a_site->get_covariance_matrix(kalman_state);

  TMatrixD _H_transposed(_n_parameters, 2);
  _H_transposed.Transpose(_H);

  TMatrixD cov_residual(2, 2);
  cov_residual = _V - (_H*C*_H_transposed);

  a_site->set_covariance_residual(cov_residual, kalman_state);

  TMatrixD residual_transposed(1, 2);
  residual_transposed.Transpose(residual);

  TMatrixD chi2(1, 1);
  cov_residual.Invert();
  chi2 = residual_transposed * cov_residual *residual;

  a_site->set_chi2(fabs(chi2(0, 0)), kalman_state);
/*
  std::cerr << "Witch hunt." <<std::endl;
  measurement.Print();
  a.Print();
s.Print();
  std::cerr << "expected measurement." <<std::endl;
HA.Print();
residual.Print();
std::cerr <<"covariance"<<std::endl;
cov_residual.Print();
chi2.Print();
*/
}

void KalmanTrack::update_covariance(KalmanSite *a_site) {
  // Cp = (1-KH)C
  TMatrixD C_old = a_site->get_covariance_matrix(KalmanSite::Projected);

  TMatrixD I(_n_parameters, _n_parameters);
  I.UnitMatrix();

  TMatrixD C_new(_n_parameters, _n_parameters);
  C_new = ( I - _K*_H ) * C_old;

  a_site->set_covariance_matrix(C_new, KalmanSite::Filtered);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ------- Smoothing Routines ------------
//
void KalmanTrack::smooth(std::vector<KalmanSite> &sites, int id) {
  // Get site to be smoothed...
  KalmanSite *smoothing_site = &sites[id];

  // ... and the already perfected site.
  const KalmanSite *optimum_site = &sites[id+1];

  // Set the propagator right.
  update_propagator(optimum_site, smoothing_site);

  // H and V are necessary for the residual calculation.
  update_H(smoothing_site);
  update_V(smoothing_site);

  // Compute A_k.
  update_back_transportation_matrix(optimum_site, smoothing_site);

  // Compute smoothed a_k and C_k.
  smooth_back(optimum_site, smoothing_site);

  smoothing_site->set_current_state(KalmanSite::Smoothed);
}

void KalmanTrack::update_back_transportation_matrix(const KalmanSite *optimum_site,
                                                    const KalmanSite *smoothing_site) {
  update_propagator(smoothing_site, optimum_site);
  TMatrixD Cp(_n_parameters, _n_parameters);
  Cp = optimum_site->get_covariance_matrix(KalmanSite::Projected);
  Cp.Invert();

  TMatrixD C(_n_parameters, _n_parameters);
  C = smoothing_site->get_covariance_matrix(KalmanSite::Filtered);

  TMatrixD temp(_n_parameters, _n_parameters);

  TMatrixD _F_transposed(_n_parameters, _n_parameters);
  _F_transposed.Transpose(_F);

  _A = C*_F_transposed*Cp;
}

void KalmanTrack::prepare_for_smoothing(KalmanSite *last_site) {
  TMatrixD a_smooth = last_site->get_a(KalmanSite::Filtered);
  last_site->set_a(a_smooth, KalmanSite::Smoothed);

  TMatrixD C_smooth(_n_parameters, _n_parameters);
  C_smooth = last_site->get_covariance_matrix(KalmanSite::Filtered);
  last_site->set_covariance_matrix(C_smooth, KalmanSite::Smoothed);

  TMatrixD residual(2, 1);
  residual = last_site->get_residual(KalmanSite::Filtered);
  last_site->set_residual(residual, KalmanSite::Smoothed);
  last_site->set_current_state(KalmanSite::Smoothed);

  // Set smoothed chi2.
  double f_chi2 = last_site->get_chi2(KalmanSite::Filtered);
  last_site->set_chi2(f_chi2, KalmanSite::Smoothed);
}

void KalmanTrack::smooth_back(const KalmanSite *optimum_site,
                              KalmanSite *smoothing_site) {
  // Set smoothed state.
  TMatrixD a     = smoothing_site->get_a(KalmanSite::Filtered);
  TMatrixD a_opt = optimum_site->get_a(KalmanSite::Smoothed);
  TMatrixD ap    = optimum_site->get_a(KalmanSite::Projected);

  TMatrixD a_smooth = a + _A* (a_opt - ap);

  smoothing_site->set_a(a_smooth, KalmanSite::Smoothed);

  // Set the smoothed covariance matrix.
  TMatrixD C     = smoothing_site->get_covariance_matrix(KalmanSite::Filtered);
  TMatrixD C_opt = optimum_site->get_covariance_matrix(KalmanSite::Smoothed);
  TMatrixD Cp    = optimum_site->get_covariance_matrix(KalmanSite::Projected);

  TMatrixD temp1    = _A*(C_opt - Cp);
  TMatrixD temp2    = TMatrixD(temp1, TMatrixD::kMultTranspose, _A);
  TMatrixD C_smooth = C+temp2;

  smoothing_site->set_covariance_matrix(C_smooth, KalmanSite::Smoothed);

  // Finally, set smoothed residual & chi2.
  set_residual(smoothing_site, KalmanSite::Smoothed);
}

void KalmanTrack::exclude_site(KalmanSite *site) {
  update_H(site);
  update_V(site);
  // a' = a + K pull
  // The pull.
  TMatrixD a_smoothed  = site->get_a(KalmanSite::Smoothed);
  TMatrixD shifts      = site->get_input_shift();
  TMatrixD HA          = solve_measurement_equation(a_smoothed, shifts);
  TMatrixD measurement = site->get_measurement();
  TMatrixD pull        = measurement-HA;

  TMatrixD C_smoothed  = site->get_covariance_matrix(KalmanSite::Smoothed);

  // Intermediate calculations.
  TMatrixD H_transposed(_n_parameters, 2);
  H_transposed.Transpose(_H);
  TMatrixD TEMP = _V*(-1.)+_H*C_smoothed*H_transposed;
  TEMP.Invert();
  TMatrixD Kn(_n_parameters, 2);
  Kn = C_smoothed*H_transposed*TEMP;

  // State vector when the measurement is excluded.
  TMatrixD an(_n_parameters, 1);
  an = a_smoothed + Kn*pull;
  site->set_a(an, KalmanSite::Excluded);

  a_smoothed.Print();
  an.Print();

  // The covariance associated.
  TMatrixD covariance(_n_parameters, _n_parameters);
  TMatrixD I(_n_parameters, _n_parameters);
  I.UnitMatrix();
  covariance = ( I - Kn*_H ) * C_smoothed;
  site->set_covariance_matrix(covariance, KalmanSite::Excluded);

  // Compute residual.
  set_residual(site, KalmanSite::Excluded);
}

void KalmanTrack::compute_chi2(const std::vector<KalmanSite> &sites) {
  _f_chi2 = 0.;
  _s_chi2 = 0.;
  _n_sites = sites.size();

  _ndf = _n_sites - _n_parameters;

  int id = sites[0].get_id();

  if ( id <= 14 ) _tracker = 0;
  if ( id > 14 )  _tracker = 1;

  for ( size_t i = 0; i < _n_sites; ++i ) {
    KalmanSite site = sites[i];
    _f_chi2 += site.get_chi2(KalmanSite::Filtered);
    _s_chi2 += site.get_chi2(KalmanSite::Smoothed);
    std::cerr << site.get_chi2(KalmanSite::Filtered) << std::endl;
  }
  _P_value = TMath::Prob(_f_chi2, _ndf);
  std::cerr <<_f_chi2<< " " << _P_value << std::endl;
}

// +++++++++++++++++++++++++++++
// Alignment Routine
//
void KalmanTrack::update_misaligments(std::vector<KalmanSite> &sites,
                                       std::vector<KalmanSite> &sites_copy,
                                       int station_i) {
  for ( int plane = 3; plane > 0 ; --plane ) {
    int plane_i = 3*(station_i)-plane;

    KalmanSite *excluded = &sites_copy[plane_i];
    // Update all matrices.
    excluded->set_current_state(KalmanSite::Excluded);
    update_V(excluded);
    update_H(excluded);
    update_W(excluded);

    TMatrixD old_s(3, 1);
    TMatrixD old_E(3, 3);
    if ( plane == 3 ) {
      old_s = excluded->get_input_shift();
      old_E = excluded->get_input_shift_covariance();
    } else {
      old_s = sites_copy[plane_i-1].get_shift();
      old_E = sites_copy[plane_i-1].get_shift_covariance();
    }

    // Get smoothed value when the station measurements are excluded.
    TMatrixD a           = excluded->get_a(KalmanSite::Smoothed);
    TMatrixD measurement = excluded->get_measurement();

    // Compute pull.
    TMatrixD HA   = solve_measurement_equation(a, old_s);
    TMatrixD pull = measurement - HA;

    // Compute new shift.
    // TMatrixD old_E = excluded->get_input_shift_covariance();
    TMatrixD S_transposed(3, 2);
    S_transposed.Transpose(_S);
    TMatrixD Ks    = old_E * S_transposed * _W;
    TMatrixD new_s = old_s + Ks*pull;
    excluded->set_shift(new_s);

    // New covariance.
    TMatrixD I(3, 3);
    I.UnitMatrix();
    TMatrixD new_E(3, 3);
    new_E = ( I - Ks*_S ) * old_E;
    excluded->set_shift_covariance(new_E);

    // Compare chi2's.
    // old smoothed chi2 is computed with the full
    // track fit with the current station ignored.
    double old_chi2 = sites[plane_i].get_chi2(KalmanSite::Smoothed);
    excluded->set_input_shift(new_s);
    excluded->set_input_shift_covariance(new_E);
    // After setting the input shifts as the new ones,
    // we look for the residual in this station.
    set_residual(excluded, KalmanSite::Smoothed);
    double new_chi2 = excluded->get_chi2(KalmanSite::Smoothed);

    if ( old_chi2 < new_chi2 ) {
      excluded->set_shift(old_s);
      excluded->set_shift_covariance(old_E);
    }
  }
}

} // ~namespace MAUS
