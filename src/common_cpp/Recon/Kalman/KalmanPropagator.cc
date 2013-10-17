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
#include "src/common_cpp/Utils/Constants.hh"

// TODO: add straggling to propagation. (optional)

namespace MAUS {

KalmanPropagator::KalmanPropagator() : _n_parameters(0) {
  Json::Value *json = Globals::GetConfigurationCards();
  _use_MCS          = (*json)["SciFiKalman_use_MCS"].asBool();
  _use_Eloss        = (*json)["SciFiKalman_use_Eloss"].asBool();
  // Set Fibre Parameters.
  FibreParameters.Z              = (*json)["SciFiParams_Z"].asDouble();
  FibreParameters.Plane_Width    = (*json)["SciFiParams_Plane_Width"].asDouble();
  FibreParameters.Radiation_Length = (*json)["SciFiParams_Radiation_Length"].asDouble();
  FibreParameters.Density        = (*json)["SciFiParams_Density"].asDouble();
  FibreParameters.Mean_Excitation_Energy = (*json)["SciFiParams_Mean_Excitation_Energy"].asDouble();
  FibreParameters.A              = (*json)["SciFiParams_A"].asDouble();
  FibreParameters.Pitch          = (*json)["SciFiParams_Pitch"].asDouble();
  FibreParameters.Station_Radius = (*json)["SciFiParams_Station_Radius"].asDouble();
  FibreParameters.RMS            = (*json)["SciFiParams_RMS"].asDouble();

  AirParameters.Z                       = (*json)["AirParams_Z"].asDouble();
  AirParameters.Radiation_Length         = (*json)["AirParams_Radiation_Length"].asDouble();
  AirParameters.Density                 = (*json)["AirParams_Density"].asDouble();
  AirParameters.Mean_Excitation_Energy  = (*json)["AirParams_Mean_Excitation_Energy"].asDouble();
  AirParameters.A                       = (*json)["AirParams_A"].asDouble();
}

void KalmanPropagator::Extrapolate(KalmanStatesPArray sites, int i) {
  // Get current site...
  KalmanState *new_site = sites.at(i);

  // ... and the site we will extrapolate from.
  const KalmanState *old_site = sites.at(i-1);

  // Calculate prediction for the state vector.
  CalculatePredictedState(old_site, new_site);

  // Calculate the energy loss for the projected state.
  if ( _n_parameters == 5 && _use_Eloss )
    SubtractEnergyLoss(old_site, new_site);

  // Calculate the system noise...
  if ( _use_MCS )
    CalculateSystemNoise(old_site, new_site);

  // ... so that we can add it to the prediction for the
  // covariance matrix.
  CalculateCovariance(old_site, new_site);

  new_site->set_current_state(KalmanState::Projected);
}

// C_proj = _F * C_old * _Ft + _Q;
void KalmanPropagator::CalculateCovariance(const KalmanState *old_site,
                                           KalmanState *new_site) {
  TMatrixD C_old = old_site->covariance_matrix(KalmanState::Filtered);

  TMatrixD F_transposed(_n_parameters, _n_parameters);
  F_transposed.Transpose(_F);

  TMatrixD C_new(_n_parameters, _n_parameters);
  C_new = _F*C_old*F_transposed + _Q;

  new_site->set_covariance_matrix(C_new, KalmanState::Projected);
}

// Returns (beta) * (-dE/dx). Formula and constants from PDG.
double KalmanPropagator::BetheBlochStoppingPower(double p) {
  double muon_mass      = Recon::Constants::MuonMass;
  double electron_mass  = Recon::Constants::ElectronMass;
  double muon_mass2     = muon_mass*muon_mass;

  double E = TMath::Sqrt(muon_mass2+p*p);

  double beta = p/E;
  double beta2= beta*beta;
  double gamma = E/muon_mass;
  double gamma2= gamma*gamma;

  double K = Recon::Constants::BetheBlochParameters::K();
  double A = FibreParameters.A;
  double I = FibreParameters.Mean_Excitation_Energy;
  double I2= I*I;
  double Z = FibreParameters.Z;

  double outer_term = K*Z/(A*beta2);

  double Tmax = 2.*electron_mass*beta2*gamma2/(1.+(2.*gamma*electron_mass/muon_mass) +
                (electron_mass*electron_mass/(muon_mass*muon_mass)));

  double log_term = TMath::Log(2.*electron_mass*beta2*gamma2*Tmax/(I2));
  double last_term = Tmax*Tmax/(gamma2*muon_mass2);
  double density = FibreParameters.Density;
  double plasma_energy = 28.816*TMath::Sqrt(density*Z/A); // eV
  double density_term = TMath::Log(plasma_energy/I)+TMath::Log(beta*gamma)-0.5;
  double dEdx = outer_term*(0.5*log_term-beta2-density_term/2.+last_term/8.);

  return beta*dEdx;
}

void KalmanPropagator::SubtractEnergyLoss(const KalmanState *old_site,
                                          KalmanState *new_site) {
  //
  // Get the momentum vector to be corrected.
  TMatrixD a_old(_n_parameters, 1);
  a_old = new_site->a(KalmanState::Projected);
  double kappa = a_old(4, 0);
  double px    = a_old(1, 0)/kappa;
  double py    = a_old(3, 0)/kappa;
  double pz = 1./fabs(kappa);
  int sign = static_cast<int> (kappa/fabs(kappa));
  ThreeVector old_momentum(px, py, pz);
  //
  // Compute the correction using Bethe Bloch's formula.
  double plane_width = FibreParameters.Plane_Width;
  double Delta_p = BetheBlochStoppingPower(old_momentum.mag())*plane_width;
  //
  // Reduce/increase momentum vector accordingly.
  // Indeed, for tracker 0, we want to ADD energy
  // beause we are not following the energy loss.
  if ( old_site->id() < 0 ) {
    Delta_p = -Delta_p;
  }
  double reduction_factor = (old_momentum.mag()-Delta_p)/old_momentum.mag();
  ThreeVector new_momentum = old_momentum*reduction_factor;
  //
  // Update momentum estimate at the site.
  TMatrixD a_subtracted(_n_parameters, 1);
  a_subtracted = new_site->a(KalmanState::Projected);
  a_subtracted(1, 0) = new_momentum.x()/new_momentum.z();
  a_subtracted(3, 0) = new_momentum.y()/new_momentum.z();
  a_subtracted(4, 0) = sign/new_momentum.z();
  new_site->set_a(a_subtracted, KalmanState::Projected);
}

void KalmanPropagator::CalculateSystemNoise(const KalmanState *old_site,
                                            const KalmanState *new_site) {
  // Define fibre material parameters.
  double plane_width = FibreParameters.Plane_Width;
  int numb_planes = abs(new_site->id() - old_site->id());
  double total_plane_length = numb_planes*plane_width;
  // Plane lenght in units of radiation lenght (~0.0015 per plane).
  double effective_plane_lenght = 0.5*total_plane_length;
  double plane_L0 = FibreParameters.R0(effective_plane_lenght);
  // Get particle's parameters (gradients and total momentum).
  TMatrixD a = new_site->a(KalmanState::Projected);
  double mx  = a(1, 0);
  double my  = a(3, 0);
  double p   = GetTrackMomentum(old_site);

  // Compute the fibre effect.
  TMatrixD Q1(_n_parameters, _n_parameters);
  Q1 = BuildQ(plane_L0, effective_plane_lenght, mx, my, p);

  // Compute Air effect (if necessary).
  TMatrixD Q2(_n_parameters, _n_parameters);
  double deltaZ = new_site->z() - old_site->z();
  // Check if we need to add propagation in air.
  if ( deltaZ > 3.*plane_width ) {
    double air_lenght = deltaZ-total_plane_length;
    double air_L0     = AirParameters.R0(air_lenght);
    Q2 = BuildQ(air_L0, air_lenght, mx, my, p);
  }
  Q2.Zero();
  _Q = Q1+Q2;
}

TMatrixD KalmanPropagator::BuildQ(double L0,
                                  double deltaZ,
                                  double mx,
                                  double my,
                                  double p) {
  double deltaZ_squared = deltaZ*deltaZ;

  double muon_mass = Recon::Constants::MuonMass;
  double muon_mass2 = muon_mass*muon_mass;
  double E = TMath::Sqrt(muon_mass2+p*p);
  double beta = p/E;

  double C = HighlandFormula(L0, beta, p);

  double C2 = C*C;

  double c_mx_mx = C2 * (1. + mx*mx) *
                        (1.+ mx*mx + my*my);

  double c_my_my = C2 * (1. + my*my) *
                        (1.+ mx*mx + my*my);

  double c_mx_my = C2 * mx*my * (1.+ mx*mx + my*my);

  TMatrixD Q(_n_parameters, _n_parameters);
  // x x
  Q(0, 0) = deltaZ_squared*c_mx_mx;
  // x mx
  Q(0, 1) = -deltaZ*c_mx_mx;
  // x y
  Q(0, 2) = deltaZ_squared*c_mx_my;
  // x my
  Q(0, 3) = -deltaZ*c_mx_my;

  // mx x
  Q(1, 0) = -deltaZ*c_mx_mx;
  // mx mx
  Q(1, 1) = c_mx_mx;
  // mx y
  Q(1, 2) = -deltaZ*c_mx_my;
  // mx my
  Q(1, 3) = c_mx_my;

  // y x
  Q(2, 0) = deltaZ_squared*c_mx_my;
  // y mx
  Q(2, 1) = -deltaZ*c_mx_my;
  // y y
  Q(2, 2) = deltaZ_squared*c_my_my;
  // y my
  Q(2, 3) = -deltaZ*c_my_my;

  // my x
  Q(3, 0) = -deltaZ*c_mx_my;
  // my mx
  Q(3, 1) = c_mx_my;
  // my y
  Q(3, 2) = -deltaZ*c_my_my;
  // my my
  Q(3, 3) = c_my_my;

  return Q;
}

double KalmanPropagator::HighlandFormula(double L0, double beta, double p) {
  double HighlandConstant = Recon::Constants::HighlandConstant;
  // Note that the z factor (charge of the incoming particle) is omitted.
  // We don't need to consider |z| > 1.
  double result = HighlandConstant*TMath::Sqrt(L0)*(1.+0.038*TMath::Log(L0))/(beta*p);
  return result;
}

////////////////
void KalmanPropagator::PrepareForSmoothing(KalmanStatesPArray sites) {
  KalmanState *last_site = sites.back();
  TMatrixD a_smooth = last_site->a(KalmanState::Filtered);
  last_site->set_a(a_smooth, KalmanState::Smoothed);

  TMatrixD C_smooth = last_site->covariance_matrix(KalmanState::Filtered);
  last_site->set_covariance_matrix(C_smooth, KalmanState::Smoothed);

  TMatrixD residual(2, 1);
  residual = last_site->residual(KalmanState::Filtered);
  last_site->set_residual(residual, KalmanState::Smoothed);
  last_site->set_current_state(KalmanState::Smoothed);

  // Set smoothed chi2.
  double f_chi2 = last_site->chi2(KalmanState::Filtered);
  last_site->set_chi2(f_chi2, KalmanState::Smoothed);
}

void KalmanPropagator::Smooth(KalmanStatesPArray sites, int id) {
  // Get site to be smoothed...
  KalmanState *smoothing_site = sites.at(id);

  // ... and the already perfected site.
  const KalmanState *optimum_site = sites.at(id+1);

  // Set the propagator right.
  UpdatePropagator(optimum_site, smoothing_site);

  // Compute A_k.
  UpdateBackTransportationMatrix(optimum_site, smoothing_site);

  // Compute smoothed a_k and C_k
  SmoothBack(optimum_site, smoothing_site);

  smoothing_site->set_current_state(KalmanState::Smoothed);
}

void KalmanPropagator::UpdateBackTransportationMatrix(const KalmanState *optimum_site,
                                                      const KalmanState *smoothing_site) {
  UpdatePropagator(smoothing_site, optimum_site);
  TMatrixD Cp(_n_parameters, _n_parameters);
  Cp = optimum_site->covariance_matrix(KalmanState::Projected);
  Cp.Invert();

  TMatrixD C(_n_parameters, _n_parameters);
  C = smoothing_site->covariance_matrix(KalmanState::Filtered);

  TMatrixD _F_transposed(_n_parameters, _n_parameters);
  _F_transposed.Transpose(_F);

  _A = C*_F_transposed*Cp;
}

void KalmanPropagator::SmoothBack(const KalmanState *optimum_site,
                                  KalmanState *smoothing_site) {
  // Set smoothed state.
  TMatrixD a     = smoothing_site->a(KalmanState::Filtered);
  TMatrixD a_opt = optimum_site->a(KalmanState::Smoothed);
  TMatrixD ap    = optimum_site->a(KalmanState::Projected);

  TMatrixD a_smooth = a + _A* (a_opt - ap);

  smoothing_site->set_a(a_smooth, KalmanState::Smoothed);

  // Set the smoothed covariance matrix.
  TMatrixD C     = smoothing_site->covariance_matrix(KalmanState::Filtered);
  TMatrixD C_opt = optimum_site->covariance_matrix(KalmanState::Smoothed);
  TMatrixD Cp    = optimum_site->covariance_matrix(KalmanState::Projected);

  TMatrixD temp1    = _A*(C_opt - Cp);
  TMatrixD temp2    = TMatrixD(temp1, TMatrixD::kMultTranspose, _A);
  TMatrixD C_smooth = C+temp2;

  smoothing_site->set_covariance_matrix(C_smooth, KalmanState::Smoothed);
}

} // ~namespace MAUS
