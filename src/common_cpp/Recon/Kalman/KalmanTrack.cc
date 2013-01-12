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
void KalmanTrack::calc_predicted_state(KalmanSite *old_site, KalmanSite *new_site) {
  TMatrixD a = old_site->get_a();

  TMatrixD a_projected = TMatrixD(_F, TMatrixD::kMult, a);

  new_site->set_projected_a(a_projected);
}

//
// C_proj = _F * C * _Ft + _Q;
//
void KalmanTrack::calc_covariance(KalmanSite *old_site, KalmanSite *new_site) {
  TMatrixD C_old = old_site->get_covariance_matrix();

  TMatrixD _F_transposed(5, 5);
  _F_transposed.Transpose(_F);

  TMatrixD C_new(5, 5);
  C_new = _F*C_old*_F_transposed + _Q;

  new_site->set_projected_covariance_matrix(C_new);
  /*
  std::cerr << "Bethe-Bloch for 10 MeV/c: " << BetheBlochStoppingPower(10.) << "\n" 
            << "Bethe-Bloch for 300 MeV/c: " << BetheBlochStoppingPower(300.) << "\n"
            << "Bethe-Bloch for 1 GeV/c: " << BetheBlochStoppingPower(1000.) << std::endl;
*/
}

// Returns (beta) * (-dE/dx). Formula and constanst from PDG.
double KalmanTrack::BetheBlochStoppingPower(double p) {
  double muon_mass = 105.7; // MeV/c2
  double muon_mass2 = TMath::Power(muon_mass, 2.);
  double electron_mass = 0.511;

  double E = TMath::Sqrt(muon_mass2+p*p);
  double E2 = TMath::Power(E, 2.);
  double beta = p/E;
  double beta2= TMath::Power(beta, 2.);
  double gamma = E/muon_mass;
  double gamma2= TMath::Power(gamma, 2.);

  double K = 0.307075; // MeV g-1 cm2 (for A=1gmol-1
  double A = 104.15; // g.mol-1 per styrene monomer
  double I = 68.7; // eV (mean excitation energy)
  double I2= TMath::Power(I, 2.);
  double Z = 5.61291; // Z=6 for 0.922582% and Z=1 for 0.077418%

  double outer_term = K*Z/(A*beta2);

  double Tmax = 2.*electron_mass*beta2*gamma2/(1.+(2.*gamma*electron_mass/muon_mass) +
                TMath::Power(electron_mass/muon_mass, 2.));

  double log_term = TMath::Log(2*electron_mass*beta2*gamma2*Tmax/(I2));
  double last_term = TMath::Power(Tmax, 2.)/TMath::Power(gamma*muon_mass, 2);
  double density = 1.06000;// g.cm-3
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

void KalmanTrack::subtract_energy_loss(KalmanSite *old_site, KalmanSite *new_site) {
  double Delta_Z = 0.6523;

  TMatrixD a_old_site(5, 1);
  a_old_site = old_site->get_a();
  double px = a_old_site(1, 0);
  double py = a_old_site(3, 0);
  double kappa = a_old_site(4, 0);
  double pz = 1./kappa;

  double momentum = TMath::Sqrt(px*px+py*py+pz*pz);
  assert(_mass == _mass && "mass is not defined");

  // double Delta_z = fabs(old_site->get_z()-new_site->get_z());
  int n_steps = 100;

  double Delta_p = 0.;
  for ( int i = 0; i < n_steps; ++i ) {
    momentum += Delta_p;
    Delta_p += BetheBlochStoppingPower(momentum)*Delta_Z/n_steps;
  }
  std::cerr << "Total deltaP = " << Delta_p << " " << Delta_Z <<  std::endl;

  /*
  double tau = momentum/_mass;
  double tau_squared = tau*tau;

  double thickness, density;
  // get_site_properties(old_site, thickness, density);
  thickness = 0.670;
  density   = 1.0;

  double F_tau = pow(1.+tau_squared, 1.5)/(11.528*tau*tau*tau)*
                 (9.0872+2.*log(tau)-tau_squared/(1.+tau_squared));

  double delta_p =minimum_ionization_energy*density*thickness*F_tau/6.;

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
  double p = TMath::Power(px*px+py*py+pz*pz, 0.5);

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

  _Q.Zero();
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ------- Filtering ------------
//
void KalmanTrack::update_V(KalmanSite *a_site) {
  double alpha = (a_site->get_measurement())(0, 0);
  double pitch = a_site->get_pitch();
  double lenght = pow(_active_radius*_active_radius -
                 (alpha*pitch)*(alpha*pitch), 0.5);
  if ( lenght != lenght || lenght > _active_radius ) {
    throw(Squeal(Squeal::recoverable,
          "Found a bad measurement.",
          "KalmanTrack::update_V"));
  }

  double sigma_beta = (lenght/pitch)/sqrt(12.);
  double sigma_alpha = 1.0/sqrt(12.);

  _V.Zero();
  _V(0, 0) = sigma_alpha*sigma_alpha;
  _V(1, 1) = sigma_beta*sigma_beta;
}

void KalmanTrack::update_H(KalmanSite *a_site) {
  CLHEP::Hep3Vector dir = a_site->get_direction();
  double sin_theta = dir.x();
  double cos_theta = dir.y();

  TMatrixD a = a_site->get_a();
  double x_0 = a(0, 0);
  double y_0 = a(2, 0);

  TMatrixD shifts = a_site->get_shifts();
  double x_d     = shifts(0, 0);
  double y_d     = shifts(1, 0);
  double theta_y = shifts(2, 0);

  double pitch = a_site->get_pitch();

  _H.Zero();
  _H(0, 0) = -cos_theta/pitch;
  _H(0, 2) =  sin_theta/pitch;

  _S.Zero();
  _S(0, 0) =  cos_theta/pitch;
  _S(0, 1) = -sin_theta/pitch;
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
void KalmanTrack::update_W(KalmanSite *a_site) {
  TMatrixD C_a(5, 5);
  // if ( a_site->get_chi2() ) {
  //  C_a = a_site->get_smoothed_covariance_matrix();
  // } else {
  C_a = a_site->get_projected_covariance_matrix();
  // }
  TMatrixD C_s = a_site->get_S_covariance();

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

void KalmanTrack::update_covariance(KalmanSite *a_site) {
  // Cp = (1-KH)C
  TMatrixD C_old = a_site->get_projected_covariance_matrix();

  TMatrixD I(5, 5);
  I.UnitMatrix();

  TMatrixD C_new(5, 5);
  C_new = ( I - _K*_H ) * C_old;

  a_site->set_covariance_matrix(C_new);
}

TMatrixD KalmanTrack::solve_measurement_equation(TMatrixD a, TMatrixD s) {
  TMatrixD ha(2, 1);
  ha = _H * a;

  TMatrixD Ss(2, 1);
  Ss = _S * s;

  TMatrixD result(2, 1);
  result = ha + Ss;

  return result;
}

void KalmanTrack::update_misaligments(KalmanSite *a_site, KalmanSite *alignment_projection_site) {
  // ***************************
  // Get the pull,
  TMatrixD residual = a_site->get_pull();
  // ***************************
  TMatrixD shifts(3, 1);
  TMatrixD Cov_s(3, 3);

  shifts = alignment_projection_site->get_shifts();
  Cov_s  = alignment_projection_site->get_S_covariance();

  std::cout<< "Updating misalignments! Matrices to be updated are: " << std::endl;
  shifts.Print();
  Cov_s.Print();

  TMatrixD S_transposed(3, 2);
  S_transposed.Transpose(_S);

  TMatrixD Ks(3, 2);
  Ks = Cov_s * S_transposed * _W;

  TMatrixD new_shifts(3, 1);
  new_shifts = shifts + Ks*residual;

  a_site->set_shifts(new_shifts);

  TMatrixD I(3, 3);
  I.UnitMatrix();

  // new covariance
  TMatrixD new_Cov_s(3, 3);

  new_Cov_s = ( I - Ks*_S ) * Cov_s;

  a_site->set_S_covariance(new_Cov_s);

  std::cout<< "Updated values are: " << std::endl;
new_shifts.Print();
new_Cov_s.Print();
  std::cout << "Pull was: " << std::endl;
residual.Print();
}

TMatrixD KalmanTrack::get_pull(KalmanSite *a_site) {
  // PULL = m - ha
  TMatrixD measurement = a_site->get_measurement();
  TMatrixD a           = a_site->get_projected_a();
  TMatrixD shifts      = a_site->get_shifts();

  // update_H(a_site);
  TMatrixD HA = solve_measurement_equation(a, shifts);

  TMatrixD pull(2, 1);
  pull = measurement - HA;

  a_site->set_pull(pull);

  return pull;
}

void KalmanTrack::set_residual(KalmanSite *a_site) {
  // PULL = measurement - ha
  TMatrixD measurement = a_site->get_measurement();
  TMatrixD a = a_site->get_a();
  TMatrixD s = a_site->get_shifts();

  update_H(a_site);

  TMatrixD HA = solve_measurement_equation(a, s);

  TMatrixD residual(2, 1);
  residual = measurement - HA;

  a_site->set_residual(residual);

  update_V(a_site);
  TMatrixD C = a_site->get_covariance_matrix();

  TMatrixD _H_transposed(5, 2);
  _H_transposed.Transpose(_H);

  TMatrixD cov_residual(2, 2);
  cov_residual = _V - (_H*C*_H_transposed);

  a_site->set_covariance_residuals(cov_residual);

  TMatrixD residual_transposed(1, 2);
  residual_transposed.Transpose(residual);

  TMatrixD chi2(1, 1);
  cov_residual.Invert();
  chi2 = residual_transposed * cov_residual *residual;

  a_site->set_chi2(chi2(0, 0));
}

TMatrixD KalmanTrack::get_kalman_gain(KalmanSite *a_site) {
  // Kalman Gain: K = C Ht (V + H C Ht)-1
  //              K =  A   (V +  B )-1
  //              K =  A W
  TMatrixD C = a_site->get_projected_covariance_matrix();

  TMatrixD A = TMatrixD(C, TMatrixD::kMultTranspose, _H);

  update_W(a_site);

  TMatrixD K(5, 2);
  K = A * _W;

  return K;
}

void KalmanTrack::calc_filtered_state(KalmanSite *a_site) {
  // ***************************
  // Calculate the pull,
  TMatrixD pull = get_pull(a_site);
  // ***************************
  // update the Kalman gain,
  _K = get_kalman_gain(a_site);
  // ***************************
  // compute the filtered state via
  // a' = a + K*pull
  TMatrixD a = a_site->get_projected_a();

  TMatrixD a_filt(5, 1);
  a_filt = a + _K*pull;

  a_site->set_a(a_filt);
  // Residual.
  set_residual(a_site);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
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

  TMatrixD _F_transposed(5, 5);
  _F_transposed.Transpose(_F);

  _A = C*_F_transposed*Cp;
}

void KalmanTrack::prepare_for_smoothing(std::vector<KalmanSite> &sites) {
  int numb_sites = sites.size();
  KalmanSite *smoothing_site = &sites[numb_sites-1];

  TMatrixD a_smooth= smoothing_site->get_a();

  smoothing_site->set_smoothed_a(a_smooth);

  TMatrixD C_smooth(5, 5);
  C_smooth = smoothing_site->get_covariance_matrix();
  smoothing_site->set_smoothed_covariance_matrix(C_smooth);

  TMatrixD residual(2, 1);
  residual = smoothing_site->get_residual();
  smoothing_site->set_smoothed_residual(residual);
/*
  // _________________________________________
  // Save chi2 for this site.
  TMatrixD measurement(2, 1);
  measurement = smoothing_site->get_measurement();
  TMatrixD shifts = smoothing_site->get_shifts();
  update_H(smoothing_site);
  TMatrixD model = solve_measurement_equation(a_smooth, shifts);
  TMatrixD s_residual(2, 1);
  s_residual = TMatrixD(measurement, TMatrixD::kMinus, model);
  smoothing_site->set_smoothed_residual(s_residual);
  // _________________________________________
*/
}

void KalmanTrack::smooth_back(KalmanSite *optimum_site, KalmanSite *smoothing_site) {
  TMatrixD a(5, 1);
  a = smoothing_site->get_a();

  TMatrixD a_opt(5, 1);
  a_opt = optimum_site->get_smoothed_a();

  TMatrixD ap(5, 1);
  ap = optimum_site->get_projected_a();

  TMatrixD a_smooth(5, 1);
  a_smooth = a + _A* (a_opt - ap);

  smoothing_site->set_smoothed_a(a_smooth);
  // _________________________________________
  TMatrixD measurement(2, 1);
  measurement = smoothing_site->get_measurement();
  TMatrixD shifts = smoothing_site->get_shifts();
  update_H(smoothing_site);
  TMatrixD model = solve_measurement_equation(a_smooth, shifts);
  TMatrixD s_residual(2, 1);
  s_residual = TMatrixD(measurement, TMatrixD::kMinus, model);
  smoothing_site->set_smoothed_residual(s_residual);
  // _________________________________________
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

void KalmanTrack::exclude_site(KalmanSite *site) {
  update_H(site);
  update_V(site);
  // a' = a + K pull
  // The pull.
  TMatrixD a_smoothed(5, 1);
  a_smoothed = site->get_smoothed_a();

  TMatrixD shifts = site->get_shifts();
  TMatrixD HA = solve_measurement_equation(a_smoothed, shifts);

  TMatrixD measurement = site->get_measurement();
  TMatrixD pull = measurement-HA;

  // The "gain"
  TMatrixD C_smoothed(5, 5);
  C_smoothed = site->get_smoothed_covariance_matrix();

  TMatrixD H_transposed(5, 2);
  H_transposed.Transpose(_H);

  TMatrixD TEMP = _V*(-1.)+_H*C_smoothed*H_transposed;
  TEMP.Invert();

  TMatrixD Kn(5, 2);
  Kn = C_smoothed*H_transposed*TEMP;

  // new site estimation
  TMatrixD an(5, 1);
  an = a_smoothed + Kn*pull;

  site->set_excluded_state(an);
}

void KalmanTrack::compute_chi2(const std::vector<KalmanSite> &sites) {  int number_parameters = 5;
  int number_of_sites = sites.size();

  _ndf = number_of_sites - _n_parameters;

  int id = sites[0].get_id();

  if ( id <= 14 ) _tracker = 0;
  if ( id > 14 )  _tracker = 1;

  // double alpha, model_alpha;
  for ( int i = 0; i < number_of_sites; ++i ) {
    KalmanSite site = sites[i];
    _chi2 += site.get_chi2();
    // std::cerr << _chi2 << " ";
  }

  _P_value = TMath::Prob(_chi2, _ndf);
  // std::cerr << "\n";
}

} // ~namespace MAUS
