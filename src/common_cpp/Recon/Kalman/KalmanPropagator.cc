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

#include "src/common_cpp/Recon/Kalman/KalmanPropagator.hh"
#include "src/common_cpp/Utils/Globals.hh"
#include "Recon/Global/Particle.hh"

// TODO: Where do we store the mass value?
//       throw if numb_sites out of range.
//       MCS: Highland formula should be in a function.
//       add straggling to propagation. (optional)

namespace MAUS {

KalmanPropagator::KalmanPropagator() : _n_parameters(0) {
  Json::Value *json = Globals::GetConfigurationCards();
  _use_MCS          = (*json)["SciFiKalman_use_MCS"].asBool();
  _use_Eloss        = (*json)["SciFiKalman_use_Eloss"].asBool();
  // Set Fibre Parameters.
  FibreParameters.Z              = (*json)["SciFiParams_Z"].asDouble();
  FibreParameters.Plane_Width    = (*json)["SciFiParams_Plane_Width"].asDouble();
  FibreParameters.Radiation_Legth= (*json)["SciFiParams_Radiation_Legth"].asDouble();
  FibreParameters.Density        = (*json)["SciFiParams_Density"].asDouble();
  FibreParameters.Mean_Excitation_Energy = (*json)["SciFiParams_Mean_Excitation_Energy"].asDouble();
  FibreParameters.A              = (*json)["SciFiParams_A"].asDouble();
  FibreParameters.Pitch          = (*json)["SciFiParams_Pitch"].asDouble();
  FibreParameters.Active_Radius  = (*json)["SciFiParams_Active_Radius"].asDouble();
  FibreParameters.RMS            = (*json)["SciFiParams_RMS"].asDouble();
}

void KalmanPropagator::Extrapolate(KalmanSitesPArray sites, int i) {
  // Get current site...
  KalmanSite *new_site = sites.at(i);

  // ... and the site we will extrapolate from.
  const KalmanSite *old_site = sites.at(i-1);

  // Calculate prediction for the state vector.
  CalculatePredictedState(old_site, new_site);

  // Calculate the energy loss for the projected state.
  if ( old_site->a(KalmanSite::Filtered).GetNrows()==5 && _use_Eloss )
    SubtractEnergyLoss(old_site, new_site);

  // Calculate the system noise...
  if ( _use_MCS )
    CalculateSystemNoise(old_site, new_site);

  // ... so that we can add it to the prediction for the
  // covariance matrix.
  CalculateCovariance(old_site, new_site);

  new_site->set_current_state(KalmanSite::Projected);
}

// C_proj = _F * C_old * _Ft + _Q;
void KalmanPropagator::CalculateCovariance(const KalmanSite *old_site,
                                           KalmanSite *new_site) {
  TMatrixD C_old = old_site->covariance_matrix(KalmanSite::Filtered);

  TMatrixD F_transposed(_n_parameters, _n_parameters);
  F_transposed.Transpose(_F);

  TMatrixD C_new(_n_parameters, _n_parameters);
  C_new = _F*C_old*F_transposed + _Q;

  new_site->set_covariance_matrix(C_new, KalmanSite::Projected);
}

// Returns (beta) * (-dE/dx). Formula and constants from PDG.
double KalmanPropagator::BetheBlochStoppingPower(double p) {
  double _mass = 105.65837;
  double muon_mass2 = _mass*_mass;
  double electron_mass = BetheBlochParameters::Electron_Mass();

  double E = TMath::Sqrt(muon_mass2+p*p);

  double beta = p/E;
  double beta2= beta*beta;
  double gamma = E/_mass;
  double gamma2= gamma*gamma;

  double K = BetheBlochParameters::K();
  double A = FibreParameters.A;
  double I = FibreParameters.Mean_Excitation_Energy;
  double I2= I*I;
  double Z = FibreParameters.Z;

  double outer_term = K*Z/(A*beta2);

  double Tmax = 2.*electron_mass*beta2*gamma2/(1.+(2.*gamma*electron_mass/_mass) +
                (electron_mass*electron_mass/(_mass*_mass)));

  double log_term = TMath::Log(2.*electron_mass*beta2*gamma2*Tmax/(I2));
  double last_term = Tmax*Tmax/(gamma2*muon_mass2);
  double density = FibreParameters.Density;
  double plasma_energy = 28.816*TMath::Sqrt(density*Z/A); // eV
  double density_term = TMath::Log(plasma_energy/I)+TMath::Log(beta*gamma)-0.5;
  double dEdx = outer_term*(0.5*log_term-beta2-density_term/2.+last_term/8.);

  return beta*dEdx;
}

void KalmanPropagator::SubtractEnergyLoss(const KalmanSite *old_site,
                                          KalmanSite *new_site) {
  //
  // Get the momentum vector to be corrected.
  TMatrixD a_old(_n_parameters, 1);
  a_old = new_site->a(KalmanSite::Projected);
  double kappa = a_old(4, 0);
  double px    = a_old(1, 0)/kappa;
  double py    = a_old(3, 0)/kappa;
  double pz = 1./fabs(kappa);
  int sign = kappa/fabs(kappa);
  ThreeVector old_momentum(px, py, pz);
  //
  // Compute the correction using Bethe Bloch's formula.
  double plane_width = FibreParameters.Plane_Width;
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
  a_subtracted(4, 0) = sign/new_momentum.z();
  new_site->set_a(a_subtracted, KalmanSite::Projected);
}

void KalmanPropagator::CalculateSystemNoise(const KalmanSite *old_site,
                                            const KalmanSite *new_site) {
  double plane_width = FibreParameters.Plane_Width;

  double deltaZ = new_site->z() - old_site->z();
  double deltaZ_squared = deltaZ*deltaZ;

  TMatrixD a = new_site->a(KalmanSite::Projected);
  double mx    = a(1, 0);
  double my    = a(3, 0);
  double kappa = a(4, 0);

  // Charge of incoming particle is given by the sign of kappa.
  double z = kappa/fabs(kappa);
  // Plane lenght in units of radiation lenght (~0.0015).
  double L0 = FibreParameters.R0(plane_width);

  double pz = 1./kappa;
  double px = mx/kappa;
  double py = my/kappa;
  double p = TMath::Sqrt(px*px+py*py+pz*pz); // MeV/c

  // FIX!!!
  double _mass = 105.7; // MeV/c2
  double muon_mass2 = _mass*_mass;
  double E = TMath::Sqrt(muon_mass2+p*p);
  double beta = p/E;

  // Highland formula.
  // make 13.6 a const
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

////////////////
void KalmanPropagator::PrepareForSmoothing(KalmanSitesPArray sites) {
  KalmanSite *last_site = sites.back();
  TMatrixD a_smooth = last_site->a(KalmanSite::Filtered);
  last_site->set_a(a_smooth, KalmanSite::Smoothed);

  TMatrixD C_smooth = last_site->covariance_matrix(KalmanSite::Filtered);
  last_site->set_covariance_matrix(C_smooth, KalmanSite::Smoothed);

  TMatrixD residual(2, 1);
  residual = last_site->residual(KalmanSite::Filtered);
  last_site->set_residual(residual, KalmanSite::Smoothed);
  last_site->set_current_state(KalmanSite::Smoothed);

  // Set smoothed chi2.
  double f_chi2 = last_site->chi2(KalmanSite::Filtered);
  last_site->set_chi2(f_chi2, KalmanSite::Smoothed);
}

void KalmanPropagator::Smooth(KalmanSitesPArray sites, int id) {
  // Get site to be smoothed...
  KalmanSite *smoothing_site = sites.at(id);

  // ... and the already perfected site.
  const KalmanSite *optimum_site = sites.at(id+1);

  // Set the propagator right.
  UpdatePropagator(optimum_site, smoothing_site);

  // Compute A_k.
  UpdateBackTransportationMatrix(optimum_site, smoothing_site);

  // Compute smoothed a_k and C_k
  SmoothBack(optimum_site, smoothing_site);

  smoothing_site->set_current_state(KalmanSite::Smoothed);
}

void KalmanPropagator::UpdateBackTransportationMatrix(const KalmanSite *optimum_site,
                                                      const KalmanSite *smoothing_site) {
  UpdatePropagator(smoothing_site, optimum_site);
  TMatrixD Cp(_n_parameters, _n_parameters);
  Cp = optimum_site->covariance_matrix(KalmanSite::Projected);
  Cp.Invert();

  TMatrixD C(_n_parameters, _n_parameters);
  C = smoothing_site->covariance_matrix(KalmanSite::Filtered);

  TMatrixD _F_transposed(_n_parameters, _n_parameters);
  _F_transposed.Transpose(_F);

  _A = C*_F_transposed*Cp;
}

void KalmanPropagator::SmoothBack(const KalmanSite *optimum_site,
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
}

} // ~namespace MAUS
