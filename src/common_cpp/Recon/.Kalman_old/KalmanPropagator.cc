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
  FibreParameters.Density_Correction = (*json)["SciFiParams_Density_Correction"].asDouble();

  GasParameters.Z                       = (*json)["GasParams_Z"].asDouble();
  GasParameters.Radiation_Length        = (*json)["GasParams_Radiation_Length"].asDouble();
  GasParameters.Density                 = (*json)["GasParams_Density"].asDouble();
  GasParameters.Mean_Excitation_Energy  = (*json)["GasParams_Mean_Excitation_Energy"].asDouble();
  GasParameters.A                       = (*json)["GasParams_A"].asDouble();
  GasParameters.Density_Correction      = (*json)["GasParams_Density_Correction"].asDouble();

  MylarParameters.Z                     = (*json)["MylarParams_Z"].asDouble();
  MylarParameters.Plane_Width           = (*json)["MylarParams_Plane_Width"].asDouble();
  MylarParameters.Radiation_Length      = (*json)["MylarParams_Radiation_Length"].asDouble();
  MylarParameters.Density               = (*json)["MylarParams_Density"].asDouble();
  MylarParameters.Mean_Excitation_Energy = (*json)["MylarParams_Mean_Excitation_Energy"].asDouble();
  MylarParameters.A                     = (*json)["MylarParams_A"].asDouble();
  MylarParameters.Density_Correction    = (*json)["MylarParams_Density_Correction"].asDouble();

  _w_scifi = FibreParameters.Plane_Width; // mm

  _w_mylar = MylarParameters.Plane_Width;  // mm

  _mm_to_cm = 0.1;
}

void KalmanPropagator::Extrapolate(KalmanStatesPArray sites, int i) {
  // Get current site...
  KalmanState *new_site = sites.at(i);

  // ... and the site we will extrapolate from.
  const KalmanState *old_site = sites.at(i-1);

  // Calculate prediction for the state vector.
  // This will also compute F. For straight tracks, F is computed
  // first and then the extrapolation is Fa.
  // For the helical case, a is calculated first and then F.
  CalculatePredictedState(old_site, new_site);

  // Calculate the energy loss for the projected state.
  // if ( _n_parameters == 5 && _use_Eloss )
  //  SubtractEnergyLoss(old_site, new_site);

  // Calculate the system noise...
  // if ( _use_MCS )
  // CalculateSystemNoise(old_site, new_site);

  // ... so that we can add it to the prediction for the
  // covariance matrix.

  CalculateCovariance(old_site, new_site);

  _F.Print();

  new_site->set_current_state(KalmanState::Projected);
}

TMatrixD KalmanPropagator::AddSystemNoise(const KalmanState *old_site, TMatrixD C_old) {
  std::vector<double> widths;
  widths.push_back(_w_scifi);
  widths.push_back(_w_mylar);

  std::vector<std::string> materials;
  materials.push_back("scifi");
  materials.push_back("mylar");
//  TMatrixD old_state = old_site->a(KalmanState::Projected);
//  double temp_pt = sqrt(old_state(1, 0)*old_state(1, 0) + old_state(3, 0)*old_state(3, 0));
//  double temp_pz = fabs(1.0/old_state(4, 0));
//  double temp_p = sqrt(temp_pt*temp_pt + temp_pz*temp_pz);
//  double circle_factor = sqrt( 1 + (temp_pt*temp_pt / (temp_pz*temp_pz)) );

  int id = abs(old_site->id());
  // If we are extrapolating between stations, there's gas.
  if ( id == 13 || id == 10 || id == 7 || id == 4 ) {
    double w_gas;
    if ( id == 4  ) w_gas = 200.0;
    if ( id == 7  ) w_gas = 250.0;
    if ( id == 10 ) w_gas = 300.0;
    if ( id == 13 ) w_gas = 350.0;
    widths.push_back(w_gas);
    materials.push_back("gas");
  }

  int n_steps = materials.size();

  TMatrixD C = C_old;
  double delta_z = 0;
  for ( int i = 0; i < n_steps; i++ ) {
    delta_z += widths.at(i);

    TMatrixD F;
    TMatrixD a = GetIntermediateState(old_site, delta_z, F);

    double L = GetL(materials.at(i), widths.at(i));
    TMatrixD Q = BuildQ(a, L, widths.at(i));

    TMatrixD F_transposed(_n_parameters, _n_parameters);
    F_transposed.Transpose(F);

    C = F*C*F_transposed + Q;
  }
  return C;
}

// C_proj = _F * C_old * _Ft + _Q;
void KalmanPropagator::CalculateCovariance(const KalmanState *old_site,
                                           KalmanState *new_site) {
  TMatrixD C_old = old_site->covariance_matrix(KalmanState::Filtered);
  TMatrixD C_new(_n_parameters, _n_parameters);

  if ( _use_MCS ) {
    C_new = AddSystemNoise(old_site, C_old);
  } else {
    // Take the propagator which was prepared when
    // 'CalculatePredictedState' was called and project C.
    TMatrixD F_transposed(_n_parameters, _n_parameters);
    F_transposed.Transpose(_F);
    C_new = _F*C_old*F_transposed;
  }

  if ( _use_Eloss && _n_parameters == 5 ) {
    SubtractEnergyLoss(new_site);
  }

  new_site->set_covariance_matrix(C_new, KalmanState::Projected);
}

// Returns (beta) * (-dE/dx). Formula and constants from PDG.
double KalmanPropagator::BetheBlochStoppingPower(double p, std::string material) {
  double muon_mass      = Recon::Constants::MuonMass;
  double electron_mass  = Recon::Constants::ElectronMass;
  double muon_mass2     = muon_mass*muon_mass;

  double E = TMath::Sqrt(muon_mass2+p*p);

  double beta   = p/E;
  double beta2  = beta*beta;
  double gamma  = E/muon_mass;
  double gamma2 = gamma*gamma;

  double K = Recon::Constants::BetheBlochParameters::K();
  double A = GetA(material);
  double I = GetMeanExcitationEnergy(material);
  double I2= I*I;
  double Z = GetZ(material);
  double density = GetDensity(material);
  double density_correction = GetDensityCorrection(material);

  double outer_term = K*Z/(A*beta2);
  double Tmax = 2.*electron_mass*beta2*gamma2/(1.+(2.*gamma*electron_mass/muon_mass) +
                (electron_mass*electron_mass/(muon_mass*muon_mass)));
  double log_term = TMath::Log(2.*electron_mass*beta2*gamma2*Tmax/(I2));
  // double last_term = Tmax*Tmax/(gamma2*muon_mass2);

  // double plasma_energy = 28.816*TMath::Sqrt(density*Z/A); // eV
  // double density_term = TMath::Log(plasma_energy/I)+TMath::Log(beta*gamma)-0.5;
  // double dEdx = outer_term*(0.5*log_term-beta2-density_term/2.+last_term/8.);
  double dEdx = outer_term*(0.5*log_term-beta2-density_correction/2.);
  // std::cerr << material << " " << p << " " << dEdx << std::endl;
  // return beta*dEdx;
  return beta*dEdx*density;
}

void KalmanPropagator::SubtractEnergyLoss(KalmanState *new_site) {
  // Get the momentum vector to be corrected.
  double old_momentum = GetTrackMomentum(new_site);

  std::vector<double> widths;
  widths.push_back(_w_scifi);
  widths.push_back(_w_mylar);

  std::vector<std::string> materials;
  materials.push_back("scifi");
  materials.push_back("mylar");
//  TMatrixD new_state = new_site->a(KalmanState::Projected);
//  double temp_pt = sqrt(new_state(1, 0)*new_state(1, 0) + new_state(3, 0)*new_state(3, 0));
//  double temp_pz = fabs(1.0/new_state(4, 0));
//  double temp_p = sqrt(temp_pt*temp_pt + temp_pz*temp_pz);
//  double circle_factor = sqrt( 1 + (temp_pt*temp_pt / (temp_pz*temp_pz)) );

  int id = abs(new_site->id());
  // If we are extrapolating between stations, there's gas.
  if ( id == 12 || id == 9 || id == 6 || id == 3 ) {
    double w_gas;
    if ( id == 3  ) w_gas = 200.0;
    if ( id == 6  ) w_gas = 250.0;
    if ( id == 9  ) w_gas = 300.0;
    if ( id == 12 ) w_gas = 350.0;
    widths.push_back(w_gas);
    materials.push_back("gas");
  }
  // Reduce/increase momentum vector accordingly.
  // Indeed, for tracker 0, we want to ADD energy
  // beause we are not following the energy loss.
  double e_loss_sign = 1.;
  if ( new_site->id() > 0 ) {
    e_loss_sign = -1.;
  }

  double delta_p = 0;
  int n_steps = materials.size();
  double momentum = old_momentum;
  for ( int i = 0; i < n_steps; i++ ) {
    delta_p  += BetheBlochStoppingPower(momentum, materials.at(i))*widths.at(i)*_mm_to_cm;
    momentum += e_loss_sign*delta_p;
  }

  double reduction_factor = momentum/old_momentum;
  TMatrixD a = new_site->a(KalmanState::Projected);
  double px = a(1, 0);
  double py = a(3, 0);
  double pz = fabs(1./a(4, 0));
  int sign = a(4, 0)/fabs(a(4, 0));
  ThreeVector old_p(px, py, pz);
  ThreeVector new_momentum = old_p*reduction_factor;
  //
  // Update momentum estimate at the site.
  TMatrixD a_subtracted(_n_parameters, 1);
  a_subtracted = new_site->a(KalmanState::Projected);
  a_subtracted(1, 0) = new_momentum.x();
  a_subtracted(3, 0) = new_momentum.y();
  a_subtracted(4, 0) = sign/new_momentum.z();
  new_site->set_a(a_subtracted, KalmanState::Projected);
}

double KalmanPropagator::HighlandFormula(double L, double beta, double p) {
  double HighlandConstant = Recon::Constants::HighlandConstant;
  // Note that the z factor (charge of the incoming particle) is omitted.
  // We don't need to consider |z| > 1.
  double result = HighlandConstant*TMath::Sqrt(L)*(1.+0.038*TMath::Log(L))/(beta*p);
  // std::cerr << "Highland: " << result << std::endl;
  return result;
}

// At the last site, the filtered values are also the smoothed values.
void KalmanPropagator::PrepareForSmoothing(KalmanState *last_site) {
  TMatrixD a_smooth = last_site->a(KalmanState::Filtered);
  last_site->set_a(a_smooth, KalmanState::Smoothed);

  TMatrixD C_smooth = last_site->covariance_matrix(KalmanState::Filtered);
  last_site->set_covariance_matrix(C_smooth, KalmanState::Smoothed);

  // TMatrixD residual(2, 1);
  TMatrixD residual(1, 1);
  residual = last_site->residual(KalmanState::Filtered);
  last_site->set_residual(residual, KalmanState::Smoothed);
  last_site->set_current_state(KalmanState::Smoothed);

  // Set smoothed chi2.
  double chi2 = last_site->chi2();
  last_site->set_chi2(chi2);
}

void KalmanPropagator::Smooth(KalmanStatesPArray sites, int i) {
  // Get site to be smoothed...
  KalmanState *smoothing_site = sites.at(i);

  // ... and the already perfected site.
  const KalmanState *optimum_site = sites.at(i+1);

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

double KalmanPropagator::GetL(std::string material, double material_w) {
  double L;
  if ( material == "scifi" ) {
    L = FibreParameters.L(material_w);
  } else if ( material == "mylar" ) {
    L = MylarParameters.L(material_w);
  } else if ( material == "gas" ) {
    L = GasParameters.L(material_w);
  } else {
    L = 0; // effectively, computes no correction.
    std::cerr << "Unkown material!" << std::endl;
  }
  return L;
}

double KalmanPropagator::GetA(std::string material) {
  double A;
  if ( material == "scifi" ) {
    A = FibreParameters.A;
  } else if ( material == "mylar" ) {
    A = MylarParameters.A;
  } else if ( material == "gas" ) {
    A = GasParameters.A;
  } else {
    A = 0;
    std::cerr << "Unkown material!" << std::endl;
  }
  return A;
}

double KalmanPropagator::GetZ(std::string material) {
  double Z;
  if ( material == "scifi" ) {
    Z = FibreParameters.Z;
  } else if ( material == "mylar" ) {
    Z = MylarParameters.Z;
  } else if ( material == "gas" ) {
    Z = GasParameters.Z;
  } else {
    Z = 0;
    std::cerr << "Unkown material!" << std::endl;
  }
  return Z;
}

double KalmanPropagator::GetMeanExcitationEnergy(std::string material) {
  double E;
  if ( material == "scifi" ) {
    E = FibreParameters.Mean_Excitation_Energy;
  } else if ( material == "mylar" ) {
    E = MylarParameters.Mean_Excitation_Energy;
  } else if ( material == "gas" ) {
    E = GasParameters.Mean_Excitation_Energy;
  } else {
    E = 0;
    std::cerr << "Unkown material!" << std::endl;
  }
  return E;
}

double KalmanPropagator::GetDensity(std::string material) {
  double E;
  if ( material == "scifi" ) {
    E = FibreParameters.Density;
  } else if ( material == "mylar" ) {
    E = MylarParameters.Density;
  } else if ( material == "gas" ) {
    E = GasParameters.Density;
  } else {
    E = 0;
    std::cerr << "Unkown material!" << std::endl;
  }
  return E;
}

double KalmanPropagator::GetDensityCorrection(std::string material) {
  double E;
  if ( material == "scifi" ) {
    E = FibreParameters.Density_Correction;
  } else if ( material == "mylar" ) {
    E = MylarParameters.Density_Correction;
  } else if ( material == "gas" ) {
    E = GasParameters.Density_Correction;
  } else {
    E = 0;
    std::cerr << "Unkown material!" << std::endl;
  }
  return E;
}

} // ~namespace MAUS
