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
#include "src/common_cpp/Recon/Kalman/KalmanStraightPropagator.hh"

namespace MAUS {

KalmanStraightPropagator::KalmanStraightPropagator() : KalmanPropagator() {
  _n_parameters = 4;
  // Propagator matrix.
  _F.ResizeTo(_n_parameters, _n_parameters);
  _F.Zero();
  // MCS error.
  _Q.ResizeTo(_n_parameters, _n_parameters);
  _Q.Zero();
  // Backpropagation matrix.
  _A.ResizeTo(_n_parameters, _n_parameters);
  _A.Zero();
}

KalmanStraightPropagator::~KalmanStraightPropagator() {}

void KalmanStraightPropagator::UpdatePropagator(const KalmanState *old_site,
                                                const KalmanState *new_site) {
  // Find dz between sites.
  double new_z = new_site->z();
  double old_z = old_site->z();
  double deltaZ = new_z-old_z;

  // Build _F.
  _F.UnitMatrix();
  _F(0, 1) = deltaZ;
  _F(2, 3) = deltaZ;
}

void KalmanStraightPropagator::CalculatePredictedState(const KalmanState *old_site,
                                                       KalmanState *new_site) {
  // Set up F.
  UpdatePropagator(old_site, new_site);

  TMatrixD a = old_site->a(KalmanState::Filtered);

  TMatrixD a_projected = _F*a;

  new_site->set_a(a_projected, KalmanState::Projected);
}

double KalmanStraightPropagator::GetTrackMomentum(const KalmanState *a_site) {
  // For the straight track case, we can't estimate pz without the TOF's.
  // So we just assume 200 MeV/c.
  return 200.;
}

TMatrixD KalmanStraightPropagator::GetIntermediateState(const KalmanState *site,
                                                       double delta_z,
                                                       TMatrixD &F) {
  // Build F.
  F.ResizeTo(_n_parameters, _n_parameters);
  F.UnitMatrix();
  F(0, 1) = delta_z;
  F(2, 3) = delta_z;

  TMatrixD a = site->a(KalmanState::Filtered);
  TMatrixD a_projected = F*a;

  return a_projected;
}

TMatrixD KalmanStraightPropagator::BuildQ(TMatrixD a, double L0, double deltaZ) {
  double deltaZ_squared = deltaZ*deltaZ;

  // double L0 = FibreParameters.R0(total_plane_length);
  // TMatrixD a = new_site->a(KalmanState::Projected);
  double mx    = a(1, 0);
  double my    = a(3, 0);
  double p   = GetTrackMomentum();
  double p2  = p*p;

  double muon_mass = Recon::Constants::MuonMass;
  double muon_mass2 = muon_mass*muon_mass;
  double E = TMath::Sqrt(muon_mass2+p2);
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

} // ~namespace MAUS
