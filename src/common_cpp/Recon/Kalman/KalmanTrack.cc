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

KalmanTrack::KalmanTrack() : _f_chi2(0.),
                             _s_chi2(0.),
                             _ndf(0),
                             _P_value(0.),
                             _n_parameters(0),
                             _n_sites(0),
                             _tracker(-1),
                             _mass(105.65837),
                             _momentum(0.),
                             _particle_charge(1) {}

void KalmanTrack::Initialise() {
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
  // _S.ResizeTo(2, 3);
  // _S.Zero();
  // Weight matrix.
  _W.ResizeTo(2, 2);
  _W.Zero();
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ------- Extrapolation Routines ------------
//
// C_proj = _F * C_old * _Ft + _Q;
void KalmanTrack::CalculateCovariance(const KalmanSite *old_site, KalmanSite *new_site) {
  TMatrixD C_old = old_site->covariance_matrix(KalmanSite::Filtered);

  TMatrixD _F_transposed(_n_parameters, _n_parameters);
  _F_transposed.Transpose(_F);

  TMatrixD C_new(_n_parameters, _n_parameters);
  C_new = _F*C_old*_F_transposed + _Q;

  new_site->set_covariance_matrix(C_new, KalmanSite::Projected);
}

// Returns (beta) * (-dE/dx). Formula and constants from PDG.
double KalmanTrack::BetheBlochStoppingPower(double p) {
  double muon_mass2 = TMath::Power(_mass, 2.);
  double electron_mass = BetheBlochParameters::Electron_Mass();

  double E = TMath::Sqrt(muon_mass2+p*p);

  double beta = p/E;
  double beta2= TMath::Power(beta, 2.);
  double gamma = E/_mass;
  double gamma2= TMath::Power(gamma, 2.);

  double K = BetheBlochParameters::K();
  double A = FibreParameters::A();
  double I = FibreParameters::Mean_Excitation_Energy();
  double I2= TMath::Power(I, 2.);
  double Z = FibreParameters::Z();

  double outer_term = K*Z/(A*beta2);

  double Tmax = 2.*electron_mass*beta2*gamma2/(1.+(2.*gamma*electron_mass/_mass) +
                TMath::Power(electron_mass/_mass, 2.));

  double log_term = TMath::Log(2.*electron_mass*beta2*gamma2*Tmax/(I2));
  double last_term = TMath::Power(Tmax, 2.)/TMath::Power(gamma*_mass, 2);
  double density = FibreParameters::Density();
  double plasma_energy = 28.816*TMath::Sqrt(density*Z/A); // eV
  double density_term = TMath::Log(plasma_energy/I)+TMath::Log(beta*gamma)-0.5;
  double dEdx = outer_term*(0.5*log_term-beta2-density_term/2.+last_term/8.);

  return beta*dEdx;
}

void KalmanTrack::SubtractEnergyLoss(const KalmanSite *old_site, KalmanSite *new_site) {
  //
  // Get the momentum vector to be corrected.
  TMatrixD a_old(_n_parameters, 1);
  a_old = new_site->a(KalmanSite::Projected);
  double kappa = a_old(4, 0);
  double px    = a_old(1, 0)/kappa;
  double py    = a_old(3, 0)/kappa;
  double pz = 1./kappa;
  ThreeVector old_momentum(px, py, pz);
  //
  // Compute the correction using Bethe Bloch's formula.
  double plane_width = SciFiParams::Plane_Width();
  double Delta_p = BetheBlochStoppingPower(old_momentum.mag())*plane_width;
  //
  // Reduce momentum vector accordingly.
  double reduction_factor = (old_momentum.mag()-Delta_p)/old_momentum.mag();
  ThreeVector new_momentum = old_momentum*reduction_factor;
  //
  // Update momentum estimate at the site.
  TMatrixD a_subtracted(_n_parameters, 1);
  a_subtracted = new_site->a(KalmanSite::Projected);
  a_subtracted(1, 0) = new_momentum.x()/new_momentum.z();
  a_subtracted(3, 0) = new_momentum.y()/new_momentum.z();
  a_subtracted(4, 0) = 1./new_momentum.z();
  new_site->set_a(a_subtracted, KalmanSite::Projected);
}

void KalmanTrack::CalculateSystemNoise(const KalmanSite *old_site, const KalmanSite *new_site) {
  double plane_width = SciFiParams::Plane_Width();

  double deltaZ = new_site->z() - old_site->z();
  double deltaZ_squared = deltaZ*deltaZ;

  TMatrixD a = new_site->a(KalmanSite::Projected);
  double mx  = a(1, 0);
  double my  = a(3, 0);

  // double kappa = a(4, 0);
  double momentum = _momentum;
  double kappa    = 1./momentum;

  // Charge of incoming particle.
  double z = _particle_charge;
  // Plane lenght in units of radiation lenght (~0.0015).
  double L0 = SciFiParams::R0(plane_width);

  double pz = 1./kappa;
  double px = mx/kappa;
  double py = my/kappa;
  double p = TMath::Sqrt(px*px+py*py+pz*pz); // MeV/c

  // double muon_mass = 105.7; // MeV/c2
  double muon_mass2 = _mass*_mass;
  double E = TMath::Sqrt(muon_mass2+p*p);
  double beta = p/E;

  // Highland formula.
  double C = 13.6*z*TMath::Sqrt(L0)*(1.+0.038*TMath::Log(L0))/(beta*p);

  double C2 = C*C;

  double c_mx_mx = C2 * (1. + mx*mx) *
                        (1.+ mx*mx + my*my);

  double c_my_my = C2 * (1. + my*my) *
                        (1.+ mx*mx + my*my);

  double c_mx_my = C2 * mx*my * (1.+ mx*mx + my*my);

  _Q.Zero();
  // x x
  _Q(0, 0) = deltaZ_squared*c_mx_mx;
  // x mx
  _Q(0, 1) = deltaZ*c_mx_mx;
  // x y
  _Q(0, 2) = deltaZ_squared*c_mx_my;
  // x my
  _Q(0, 3) = deltaZ*c_mx_my;

  // mx x
  _Q(1, 0) = deltaZ*c_mx_mx;
  // mx mx
  _Q(1, 1) = c_mx_mx;
  // mx y
  _Q(1, 2) = deltaZ*c_mx_my;
  // mx my
  _Q(1, 3) = c_mx_my;

  // y x
  _Q(2, 0) = deltaZ_squared*c_mx_my;
  // y mx
  _Q(2, 1) = deltaZ*c_mx_my;
  // y y
  _Q(2, 2) = deltaZ_squared*c_my_my;
  // y my
  _Q(2, 3) = deltaZ*c_my_my;

  // my x
  _Q(3, 0) = deltaZ*c_mx_my;
  // my mx
  _Q(3, 1) = c_mx_my;
  // my y
  _Q(3, 2) = deltaZ*c_my_my;
  // my my
  _Q(3, 3) = c_my_my;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ------- Filtering Routines ------------
//
void KalmanTrack::UpdateV(const KalmanSite *a_site) {
  // Fibre constants.
  double pitch         = FibreParameters::Pitch();
  // Active radius in units of channel width.
  double active_radius = FibreParameters::Active_Radius()/pitch;

  double alpha = (a_site->measurement())(0, 0);
  double lenght = 2.*TMath::Sqrt(active_radius*active_radius -
                                 alpha*alpha);

  double sigma_beta = lenght/TMath::Sqrt(12.);
  double sigma_alpha = 1.0/TMath::Sqrt(12.);

  _V.Zero();
  _V(0, 0) = sigma_alpha*sigma_alpha;
  _V(1, 1) = sigma_beta*sigma_beta;
}

void KalmanTrack::UpdateH(const KalmanSite *a_site) {
  ThreeVector dir = a_site->direction();

  double dx = dir.x();
  double dy = dir.y();

  ThreeVector perp = dir.Orthogonal();

  double perp_x = perp.x();
  double perp_y = perp.y();

  double pitch = FibreParameters::Pitch();

  _H.Zero();
  _H(0, 0) = -dy/pitch;
  _H(0, 2) =  dx/pitch;
  _H(1, 0) = -perp_y/pitch;
  _H(1, 2) =  perp_x/pitch;
  /*
  _S.Zero();
  _S(0, 0) =  dy/pitch;
  _S(0, 1) = -dx/pitch;
  _S(1, 0) =  perp_y/pitch;
  _S(1, 1) = -perp_x/pitch;
  */
}

// W = [ V + H C_k-1 Ht + S cov_S_k-1 St ]-1
// W = [ V +    A       +      B         ]-1
void KalmanTrack::UpdateW(const KalmanSite *a_site) {
  TMatrixD C_a(_n_parameters, _n_parameters);
  C_a = a_site->covariance_matrix(KalmanSite::Projected);

  TMatrixD C_s = a_site->input_shift_covariance();

  TMatrixD A = TMatrixD(TMatrixD(_H, TMatrixD::kMult, C_a),
                        TMatrixD::kMultTranspose,
                        _H);

  _W.Zero();
  _W = _V + A;
  _W.Invert();
}

void KalmanTrack::UpdateK(const KalmanSite *a_site) {
  // Kalman Gain: K = C Ht (V + H C Ht)-1
  //              K =  A   (V +  B )-1
  //              K =  A W
  TMatrixD C = a_site->covariance_matrix(KalmanSite::Projected);

  TMatrixD A = TMatrixD(C, TMatrixD::kMultTranspose, _H);

  _K.Zero();
  _K = A * _W;
}

void KalmanTrack::ComputePull(KalmanSite *a_site) {
  // PULL = m - ha
  TMatrixD measurement = a_site->measurement();
  TMatrixD a           = a_site->a(KalmanSite::Projected);
  TMatrixD shifts      = a_site->input_shift();

  TMatrixD HA = _H*a;

  TMatrixD pull(2, 1);
  pull = measurement - HA;

  a_site->set_residual(pull, KalmanSite::Projected);
}
/*
TMatrixD KalmanTrack::SolveMeasurementEquation(const TMatrixD &a,
                                               const TMatrixD &s) {
  TMatrixD ha(2, 1);
  ha = _H * a;

  return ha;
}
*/
void KalmanTrack::CalculateFilteredState(KalmanSite *a_site) {
  // Calculate the pull,
  TMatrixD pull = a_site->residual(KalmanSite::Projected);

  // compute the filtered state via
  // a' = a + K*pull
  TMatrixD a = a_site->a(KalmanSite::Projected);

  TMatrixD a_filt(_n_parameters, 1);
  a_filt = a + _K*pull;

  a_site->set_a(a_filt, KalmanSite::Filtered);

  // Residual.
  SetResidual(a_site, KalmanSite::Filtered);
}

void KalmanTrack::SetResidual(KalmanSite *a_site, KalmanSite::State kalman_state) {
  // PULL = measurement - ha
  TMatrixD measurement = a_site->measurement();
  TMatrixD a = a_site->a(kalman_state);
  TMatrixD s = a_site->input_shift();

  // Solve the measurement equation again,
  // this time passing the FILTERED state to find the
  // residual.
  TMatrixD HA = _H * a;

  TMatrixD residual(2, 1);
  residual = measurement - HA;

  a_site->set_residual(residual, kalman_state);

  TMatrixD C = a_site->covariance_matrix(kalman_state);

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
}

void KalmanTrack::UpdateCovariance(KalmanSite *a_site) {
  // Cp = (1-KH)C
  TMatrixD C_old = a_site->covariance_matrix(KalmanSite::Projected);

  TMatrixD I(_n_parameters, _n_parameters);
  I.UnitMatrix();

  TMatrixD C_new(_n_parameters, _n_parameters);
  C_new = ( I - _K*_H ) * C_old;

  a_site->set_covariance_matrix(C_new, KalmanSite::Filtered);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ------- Smoothing Routines ------------
//
void KalmanTrack::UpdateBackTransportationMatrix(const KalmanSite *optimum_site,
                                                 const KalmanSite *smoothing_site) {
  UpdatePropagator(smoothing_site, optimum_site);
  TMatrixD Cp(_n_parameters, _n_parameters);
  Cp = optimum_site->covariance_matrix(KalmanSite::Projected);
  Cp.Invert();

  TMatrixD C(_n_parameters, _n_parameters);
  C = smoothing_site->covariance_matrix(KalmanSite::Filtered);

  TMatrixD temp(_n_parameters, _n_parameters);

  TMatrixD _F_transposed(_n_parameters, _n_parameters);
  _F_transposed.Transpose(_F);

  _A = C*_F_transposed*Cp;
}

void KalmanTrack::SmoothBack(const KalmanSite *optimum_site,
                             KalmanSite *smoothing_site) {
  // Set smoothed state.
  TMatrixD a     = smoothing_site->a(KalmanSite::Filtered);
  TMatrixD a_opt = optimum_site->a(KalmanSite::Smoothed);
  TMatrixD ap    = optimum_site->a(KalmanSite::Projected);

  TMatrixD a_smooth = a + _A* (a_opt - ap);

  smoothing_site->set_a(a_smooth, KalmanSite::Smoothed);

  // Set the smoothed covariance matrix.
  TMatrixD C     = smoothing_site->covariance_matrix(KalmanSite::Filtered);
  TMatrixD C_opt = optimum_site->covariance_matrix(KalmanSite::Smoothed);
  TMatrixD Cp    = optimum_site->covariance_matrix(KalmanSite::Projected);

  TMatrixD temp1    = _A*(C_opt - Cp);
  TMatrixD temp2    = TMatrixD(temp1, TMatrixD::kMultTranspose, _A);
  TMatrixD C_smooth = C+temp2;

  smoothing_site->set_covariance_matrix(C_smooth, KalmanSite::Smoothed);

  // Finally, set smoothed residual & chi2.
  SetResidual(smoothing_site, KalmanSite::Smoothed);
}

void KalmanTrack::ComputeChi2(const std::vector<KalmanSite> &sites) {
  _f_chi2 = 0.;
  _s_chi2 = 0.;
  _n_sites = sites.size();

  _ndf = _n_sites - _n_parameters;

  int id = sites[0].id();

  if ( id < 0 ) _tracker = 0;
  if ( id > 0 ) _tracker = 1;

  for ( size_t i = 0; i < _n_sites; ++i ) {
    KalmanSite site = sites[i];
    _f_chi2 += site.chi2(KalmanSite::Filtered);
    _s_chi2 += site.chi2(KalmanSite::Smoothed);
  }
  _P_value = TMath::Prob(_f_chi2, _ndf);
}

} // ~namespace MAUS
