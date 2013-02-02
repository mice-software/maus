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
#include "Interface/Squeal.hh"

#include "src/common_cpp/Recon/Kalman/KalmanTrack.hh"

namespace MAUS {

KalmanTrack::KalmanTrack() : _f_chi2(0.), _ndf(0), _P_value(0.),
                             _n_parameters(0), _n_sites(0),
                             _tracker(-1), _mass(0.), _momentum(0.) {
  // Measurement equation.
  _H.ResizeTo(2, 5);
  _H.Zero();
  // Propagator.
  _F.ResizeTo(5, 5);
  _F.Zero();
  // Back transportation matrix.
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
  // Weight matrix.
  _W.ResizeTo(2, 2);
  _W.Zero();
}

//
// ------- Prediction ------------
//
void KalmanTrack::calc_predicted_state(const KalmanSite *old_site, KalmanSite *new_site) {
  TMatrixD a = old_site->get_a(KalmanSite::Filtered);

  TMatrixD a_projected = TMatrixD(_F, TMatrixD::kMult, a);

  new_site->set_a(a_projected, KalmanSite::Projected);
}

//
// C_proj = _F * C * _Ft + _Q;
//
void KalmanTrack::calc_covariance(const KalmanSite *old_site, KalmanSite *new_site) {
  TMatrixD C_old = old_site->get_covariance_matrix(KalmanSite::Filtered);

  TMatrixD _F_transposed(5, 5);
  _F_transposed.Transpose(_F);

  TMatrixD C_new(5, 5);
  C_new = _F*C_old*_F_transposed + _Q;

  new_site->set_covariance_matrix(C_new, KalmanSite::Projected);
}

// Returns (beta) * (-dE/dx). Formula and constants from PDG.
double KalmanTrack::BetheBlochStoppingPower(double p) {
  double muon_mass = 105.7; // MeV/c2
  double muon_mass2 = TMath::Power(muon_mass, 2.);
  double electron_mass = 0.511;

  double E = TMath::Sqrt(muon_mass2+p*p);
  // double E2 = TMath::Power(E, 2.);
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
  // std::cerr << outer_term << " " << log_term << " " << beta2 << " " << Tmax << std::endl;
  return beta*dEdx;
/*
   double Z       = MuELMaterial::Z(mt);
   double A       = MuELMaterial::A(mt);
   double Z_A     = Z/A;              // in mol/gr
   double a2      = kAem2;            // (em coupling const)^2
   double Na      = kNA;              // Avogadro's number
   double lamda2 =  kLe2/units::cm2;  // (e compton wavelength)^2 in cm^2
   double me      = kElectronMass;    // in GeV
   double me2     = kElectronMass2; 
   double mmu     = kMuonMass;        // in GeV
   double mmu2    = kMuonMass2;
   double E2      = TMath::Power(E,2);
   double beta    = TMath::Sqrt(E2-mmu2)/E;
   double beta2   = TMath::Power(beta,2);
   double gamma   = E/mmu;
   double gamma2  = TMath::Power(gamma,2);
   double I       = BetheBlochMaterialParams::IonizationPotential(mt) * units::eV; 
   double I2      = TMath::Power(I,2); // in GeV^2

   // Calculate the maximum energy transfer to the electron (in GeV)

   double p2      = E2-mmu2;
   double Emaxt   = 2*me*p2 / (me2 + mmu2 + 2*me*E);
   double Emaxt2  = TMath::Power(Emaxt,2);

   // Calculate the density correction factor delta

   double X0 =  BetheBlochMaterialParams::DensityCorrection_X0(mt);
   double X1 =  BetheBlochMaterialParams::DensityCorrection_X1(mt);
   double a  =  BetheBlochMaterialParams::DensityCorrection_a(mt);
   double m  =  BetheBlochMaterialParams::DensityCorrection_m(mt);
   double C  =  BetheBlochMaterialParams::DensityCorrection_C(mt);
   double X  =  TMath::Log10(beta*gamma);

   double delta = 0;
   if(X0<X && X<X1) delta = 4.6052*X + a*TMath::Power(X1-X,m) + C;
   if(X>X1)         delta = 4.6052*X + C;

   // Calculate the -dE/dx
   double de_dx =  a2 * (2*kPi*Na*lamda2) * Z_A * (me/beta2) *
                    (TMath::Log( 2*me*beta2*gamma2*Emaxt/I2 ) - 
                                      2*beta2 + 0.25*(Emaxt2/E2) - delta);

   de_dx *= (units::GeV/(units::g/units::cm2));
   return de_dx; // in GeV^-2
*/
}

void KalmanTrack::subtract_energy_loss(const KalmanSite *old_site, KalmanSite *new_site) {
  double plane_width = SciFiParams::Plane_Width();

  TMatrixD a_old(5, 1);
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

  double Delta_p = 0.;
  for ( int i = 0; i < n_steps; ++i ) {
    momentum += Delta_p;
    Delta_p += BetheBlochStoppingPower(momentum)*plane_width/n_steps;
  }
  double new_pz = (momentum-Delta_p)*sin(lambda);
  double new_kappa = 1./new_pz;

  TMatrixD a_subtracted(5, 1);
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

  TMatrixD a(5, 1);
  a = new_site->get_a(KalmanSite::Projected);
  double mx    = a(1, 0);
  double my    = a(3, 0);
  double kappa = a(4, 0);

  // Charge of incoming particle.
  double z = 1.;
  // ~0.0015 plane lenght in units of radiation lenght.
  double L0 = SciFiParams::R0(plane_width);

  double pz = 1./kappa;
  double px = mx/kappa;
  double py = my/kappa;
  double p = TMath::Sqrt(px*px+py*py+pz*pz); // MeV/c

  double muon_mass = 105.7; // MeV/c2
  double muon_mass2 = TMath::Power(muon_mass, 2.);
  double E = TMath::Sqrt(muon_mass2+p*p);
  // double gamma = E/muon_mass;
  double beta = p/E;
  // double v = p/muon_mass;

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
// ------- Filtering ------------
//
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
  _S(1, 2) =  -perp_x/pitch;
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
  TMatrixD C_a(5, 5);
  C_a = a_site->get_covariance_matrix(KalmanSite::Projected);

  TMatrixD C_s = a_site->get_input_shift_covariance();

  TMatrixD A = TMatrixD(TMatrixD(_H, TMatrixD::kMult, C_a),
                        TMatrixD::kMultTranspose,
                        _H);
  TMatrixD B = TMatrixD(TMatrixD(_S, TMatrixD::kMult, C_s),
                        TMatrixD::kMultTranspose,
                        _S);
  // ED
  B.Zero();

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
  // ***************************
  // Calculate the pull,
  TMatrixD pull = a_site->get_residual(KalmanSite::Projected);

  // compute the filtered state via
  // a' = a + K*pull
  TMatrixD a = a_site->get_a(KalmanSite::Projected);

  TMatrixD a_filt(5, 1);
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

  TMatrixD _H_transposed(5, 2);
  _H_transposed.Transpose(_H);

  TMatrixD cov_residual(2, 2);
  cov_residual = _V - (_H*C*_H_transposed);

  a_site->set_covariance_residual(cov_residual, kalman_state);

  TMatrixD residual_transposed(1, 2);
  residual_transposed.Transpose(residual);

  TMatrixD chi2(1, 1);
  cov_residual.Invert();
  chi2 = residual_transposed * cov_residual *residual;

  a_site->set_chi2(chi2(0, 0), kalman_state);
}

void KalmanTrack::update_covariance(KalmanSite *a_site) {
  // Cp = (1-KH)C
  TMatrixD C_old = a_site->get_covariance_matrix(KalmanSite::Projected);

  TMatrixD I(5, 5);
  I.UnitMatrix();

  TMatrixD C_new(5, 5);
  C_new = ( I - _K*_H ) * C_old;

  a_site->set_covariance_matrix(C_new, KalmanSite::Filtered);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Smoothing
//
void KalmanTrack::update_back_transportation_matrix(const KalmanSite *optimum_site,
                                                    const KalmanSite *smoothing_site) {
  update_propagator(smoothing_site, optimum_site);
  TMatrixD Cp(5, 5);
  Cp = optimum_site->get_covariance_matrix(KalmanSite::Projected);

  Cp.Invert();

  TMatrixD C(5, 5);
  C = smoothing_site->get_covariance_matrix(KalmanSite::Filtered);

  TMatrixD temp(5, 5);

  TMatrixD _F_transposed(5, 5);
  _F_transposed.Transpose(_F);

  _A = C*_F_transposed*Cp;
}

void KalmanTrack::prepare_for_smoothing(KalmanSite *last_site) {
  // size_t numb_sites = sites.size();
  // KalmanSite *smoothing_site = &sites[numb_sites-1];

  TMatrixD a_smooth = last_site->get_a(KalmanSite::Filtered);
  last_site->set_a(a_smooth, KalmanSite::Smoothed);

  TMatrixD C_smooth(5, 5);
  C_smooth = last_site->get_covariance_matrix(KalmanSite::Filtered);
  last_site->set_covariance_matrix(C_smooth, KalmanSite::Smoothed);

  TMatrixD residual(2, 1);
  residual = last_site->get_residual(KalmanSite::Filtered);
  last_site->set_residual(residual, KalmanSite::Smoothed);
  last_site->set_current_state(KalmanSite::Smoothed);
}

void KalmanTrack::smooth_back(const KalmanSite *optimum_site,
                              KalmanSite *smoothing_site) {
  TMatrixD a(5, 1);
  a = smoothing_site->get_a(KalmanSite::Filtered);

  TMatrixD a_opt(5, 1);
  a_opt = optimum_site->get_a(KalmanSite::Smoothed);

  TMatrixD ap(5, 1);
  ap = optimum_site->get_a(KalmanSite::Projected);

  TMatrixD a_smooth(5, 1);
  a_smooth = a + _A* (a_opt - ap);

  smoothing_site->set_a(a_smooth, KalmanSite::Smoothed);
  // _________________________________________
  TMatrixD measurement(2, 1);
  measurement = smoothing_site->get_measurement();
  TMatrixD shifts = smoothing_site->get_input_shift();
  update_H(smoothing_site);
  TMatrixD model = solve_measurement_equation(a_smooth, shifts);
  TMatrixD s_residual(2, 1);
  s_residual = TMatrixD(measurement, TMatrixD::kMinus, model);
  smoothing_site->set_residual(s_residual, KalmanSite::Smoothed);
  // _________________________________________
  // do the same for covariance matrix
  TMatrixD C(5, 5);
  C = smoothing_site->get_covariance_matrix(KalmanSite::Filtered);

  TMatrixD C_opt(5, 5);
  C_opt = optimum_site->get_covariance_matrix(KalmanSite::Smoothed);
  TMatrixD Cp(5, 5);
  Cp = optimum_site->get_covariance_matrix(KalmanSite::Projected);
  TMatrixD temp3(5, 5);
  temp3 = TMatrixD(C_opt, TMatrixD::kMinus, Cp);

  TMatrixD temp4(5, 5);
  temp4 = TMatrixD(_A, TMatrixD::kMult, temp3);
  TMatrixD temp5(5, 5);
  temp5= TMatrixD(temp4, TMatrixD::kMultTranspose, _A);
  TMatrixD C_smooth(5, 5);
  C_smooth =  TMatrixD(C, TMatrixD::kPlus, temp5);
  smoothing_site->set_covariance_matrix(C_smooth, KalmanSite::Smoothed);
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
  TMatrixD H_transposed(5, 2);
  H_transposed.Transpose(_H);
  TMatrixD TEMP = _V*(-1.)+_H*C_smoothed*H_transposed;
  TEMP.Invert();
  TMatrixD Kn(5, 2);
  Kn = C_smoothed*H_transposed*TEMP;

  // State vector when the measurement is excluded.
  TMatrixD an(5, 1);
  an = a_smoothed + Kn*pull;
  site->set_a(an, KalmanSite::Excluded);
  std::cerr << "Before and after exclusion of site " << site->get_id() << std::endl;
  a_smoothed.Print();
  an.Print();

  // The covariance associated.
  TMatrixD covariance(5, 5);
  TMatrixD I(5, 5);
  I.UnitMatrix();
  covariance = ( I - Kn*_H ) * C_smoothed;
  site->set_covariance_matrix(covariance, KalmanSite::Excluded);

  // Compute residual.
  set_residual(site, KalmanSite::Excluded);
}

void KalmanTrack::compute_chi2(const std::vector<KalmanSite> &sites) {
  _n_sites = sites.size();

  _ndf = _n_sites - _n_parameters;

  int id = sites[0].get_id();

  if ( id <= 14 ) _tracker = 0;
  if ( id > 14 )  _tracker = 1;

  for ( size_t i = 0; i < _n_sites; ++i ) {
    KalmanSite site = sites[i];
    _f_chi2 += site.get_chi2(KalmanSite::Filtered);
    _s_chi2 += site.get_chi2(KalmanSite::Smoothed);
  }

  _P_value = TMath::Prob(_f_chi2, _ndf);
}

// +++++++++++++++++++++++++++++
// Alignment Routines
//
void KalmanTrack::update_misaligments(KalmanSite *a_site) {
  // ***************************
  // Get the pull.
  // TMatrixD residual = a_site->get_residual(KalmanSite::Excluded);
  TMatrixD residual = a_site->get_residual(KalmanSite::Projected);
  // ***************************
  TMatrixD shifts(3, 1);
  TMatrixD Cov_s(3, 3);

  // shifts = alignment_projection_site->get_shifts();
  // Cov_s  = alignment_projection_site->get_S_covariance();

  shifts = a_site->get_input_shift();
  Cov_s  = a_site->get_input_shift_covariance();

  std::cout<< "Updating misalignments! Matrices to be updated are: " << std::endl;
  shifts.Print();
  Cov_s.Print();

  TMatrixD S_transposed(3, 2);
  S_transposed.Transpose(_S);

  TMatrixD Ks(3, 2);
  Ks = Cov_s * S_transposed * _W;

  TMatrixD new_shifts(3, 1);
  new_shifts = shifts + Ks*residual;

  a_site->set_shift_A(new_shifts);

  TMatrixD I(3, 3);
  I.UnitMatrix();

  // new covariance
  TMatrixD new_Cov_s(3, 3);
  new_Cov_s = ( I - Ks*_S ) * Cov_s;
  a_site->set_shift_A_covariance(new_Cov_s);

  std::cout<< "Updated values are: " << std::endl;
  new_shifts.Print();
  new_Cov_s.Print();
  std::cout << "Pull was: " << std::endl;
  residual.Print();
}

} // ~namespace MAUS
