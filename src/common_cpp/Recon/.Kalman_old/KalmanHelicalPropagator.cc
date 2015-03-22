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

#include "src/common_cpp/Recon/Kalman/KalmanHelicalPropagator.hh"

namespace MAUS {
KalmanHelicalPropagator::KalmanHelicalPropagator() : KalmanPropagator() {}

KalmanHelicalPropagator::KalmanHelicalPropagator(double Bz) : KalmanPropagator() {
  _Bz = Bz;
  _n_parameters = 5;
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

KalmanHelicalPropagator::~KalmanHelicalPropagator() {}

TMatrixD KalmanHelicalPropagator::GetIntermediateState(const KalmanState *site,
                                                       double delta_z,
                                                       TMatrixD &F) {
  if ( site->id() > 0 ) {
    delta_z = -delta_z;
  }

  // Get old state vector...
  TMatrixD old_a    = site->a(KalmanState::Filtered);
  double old_x      = old_a(0, 0);
  double old_px     = old_a(1, 0);
  double old_y      = old_a(2, 0);
  double old_py     = old_a(3, 0);
  double old_kappa  = old_a(4, 0);
  double charge = old_kappa/fabs(old_kappa);

  double c      = CLHEP::c_light;
  double u      = charge*c*_Bz;
  double delta_theta = u*delta_z*fabs(old_kappa);
  double sine   = sin(delta_theta);
  double cosine = cos(delta_theta);

  // Calculate the new track parameters.
  double new_x  = old_x + old_px*sine/u
                  - old_py*(1.-cosine)/u;

  double new_px = old_px*cosine - old_py*sine;

  double new_y  = old_y + (old_py*sine)/u
                  + old_px*(1.-cosine)/u;

  double new_py = old_py*cosine + old_px*sine;

  TMatrixD a_projected(_n_parameters, 1);
  a_projected(0, 0) = new_x;
  a_projected(1, 0) = new_px;
  a_projected(2, 0) = new_y;
  a_projected(3, 0) = new_py;
  a_projected(4, 0) = old_kappa;

//  double dtheta = c*_Bz*delta_z//

  // Setup F.
  F.ResizeTo(_n_parameters, _n_parameters);
  // @x/@x
  F(0, 0) = 1.;
  // @x/@px
  F(0, 1) = sine/u;
  // @x/@y
  F(0, 2) = 0.;
  // @x/@py
  F(0, 3) = (cosine-1.)/u;
  // @x/@kappa
  // F(0, 4) = new_px*cosine*dtheta/u - new_py*sine*dtheta/u;
  // F(0, 4) = 0.;
  F(0, 4) = delta_z*(old_px*cosine - old_py*sine);

  // @px/@x
  F(1, 0) = 0.;
  // @px/@px
  F(1, 1) = cosine;
  // @px/@y
  F(1, 2) = 0.;
  // @px/@py
  F(1, 3) = -sine;
  // @px/@kappa
  // F(1, 4) = -new_px*sine*dtheta - new_py*cosine*dtheta;
  // F(1, 4) = 0.;
  F(1, 4) = -u*delta_z*(old_px*sine + old_py*cosine);

  // @y/@x
  F(2, 0) = 0.;
  // @y/@px
  F(2, 1) = (1.-cosine)/u;
  // @y/@y
  F(2, 2) = 1.;
  // @y/@py
  F(2, 3) = sine/u;
  // @y/@kappa
  // F(2, 4) = new_py*cosine*dtheta/u + new_px*sine*dtheta/u;
  // F(2, 4) = 0.;
  F(2, 4) = delta_z*(old_px*sine + old_py*cosine);

  // @py/@x
  F(3, 0) = 0.;
  // @py/@px
  F(3, 1) = sine;
  // @py/@y
  F(3, 2) = 0.;
  // @py/@py
  F(3, 3) = cosine;
  // @py/@kappa
  // F(3, 4) = -new_py*sine*dtheta + new_px*cosine*dtheta;
  // F(3, 4) = 0.;
  F(3, 4) = u*delta_z*(old_px*cosine - old_py*sine);

  // @kappa/@x
  F(4, 0) = 0.;
  // @kappa/@px
  F(4, 1) = 0.;
  // @kappa/@y
  F(4, 2) = 0.;
  // @kappa/@py
  F(4, 3) = 0.;
  // @kappa/@kappa
  F(4, 4) = 1.;

  return a_projected;
}

void KalmanHelicalPropagator::CalculatePredictedState(const KalmanState *old_site,
                                                      KalmanState *new_site) {
  // Find dz (mm).
  double new_z  = new_site->z();
  double old_z  = old_site->z();
  double deltaZ = new_z-old_z;
  if ( new_site->id() < 0 ) {
    deltaZ = -deltaZ;
  }

  // Get old state vector...
  TMatrixD old_a   = old_site->a(KalmanState::Filtered);
  double old_x     = old_a(0, 0);
  double old_px    = old_a(1, 0);
  double old_y     = old_a(2, 0);
  double old_py    = old_a(3, 0);
  double old_kappa = old_a(4, 0);
  double charge    = old_kappa/fabs(old_kappa);

  double c      = CLHEP::c_light;
  double u      = charge*c*_Bz;
  double delta_theta = u*deltaZ*fabs(old_kappa);
  double sine   = sin(delta_theta);
  double cosine = cos(delta_theta);

  // Calculate the new track parameters.
  double new_x  = old_x + old_px*sine/u
                  - old_py*(1.-cosine)/u;

  double new_px = old_px*cosine - old_py*sine;

  double new_y  = old_y + (old_py*sine)/u
                  + old_px*(1.-cosine)/u;

  double new_py = old_py*cosine + old_px*sine;

  TMatrixD a_projected(_n_parameters, 1);
  a_projected(0, 0) = new_x;
  a_projected(1, 0) = new_px;
  a_projected(2, 0) = new_y;
  a_projected(3, 0) = new_py;
  a_projected(4, 0) = old_kappa;

  new_site->set_a(a_projected, KalmanState::Projected);

  // Set up F.
  // The goal is: a call to this function in either
  // Helical or Straight tracks sets up the projected state
  // in the next site and prepares the propagator (which is going to be used for
  // extrapolating the covariance matrix.
  UpdatePropagator(old_site, new_site);
}

void KalmanHelicalPropagator::UpdatePropagator(const KalmanState *old_site,
                                               const KalmanState *new_site) {
  // Reset propagator.
  _F.Zero();
  // Find dz.
  double new_z = new_site->z();
  double old_z = old_site->z();

  // Delta Z in mm
  double deltaZ = (new_z-old_z);
  if ( new_site->id() < 0 ) {
    deltaZ = -deltaZ;
  }

  // Get previous state vector...
  TMatrixD old_state = old_site->a(KalmanState::Projected);
  double old_px    = old_state(1, 0);
  double old_py    = old_state(3, 0);
  double old_kappa = old_state(4, 0);

  // Get current state vector...
  TMatrixD site = new_site->a(KalmanState::Projected);
  double px    = site(1, 0);
  double py    = site(3, 0);
  double kappa  = site(4, 0);
  double charge = kappa/fabs(kappa);

  // Define factors to be used in the matrix.
  double c      = CLHEP::c_light;
  double u      = charge*c*_Bz;
  double delta_theta = u*deltaZ*fabs(old_kappa);
  double sine   = sin(delta_theta);
  double cosine = cos(delta_theta);

  // double dtheta = c*_Bz*deltaZ;

  std::cerr << "Building Propagator: Sine = " << sine << ", Cosine = " << cosine << ", u = " << u << ", delta_theta = " << delta_theta << ", delta_z = " << deltaZ << '\n';

  // @x/@x
  _F(0, 0) = 1.;
  // @x/@px
  _F(0, 1) = sine/u;
  // @x/@y
  _F(0, 2) = 0.;
  // @x/@py
  _F(0, 3) = (cosine-1.)/u;
  // @x/@kappa
  // _F(0, 4) = px*cosine*dtheta/u - py*sine*dtheta/u;
  // _F(0, 4) = 0.;
  // _F(0, 4) = deltaZ*( px*cosine - py*sine );
  _F(0, 4) = deltaZ*(old_px*cosine - old_py*sine);

  // @px/@x
  _F(1, 0) = 0.;
  // @px/@px
  _F(1, 1) = cosine;
  // @px/@y
  _F(1, 2) = 0.;
  // @px/@py
  _F(1, 3) = -sine;
  // @px/@kappa
  // _F(1, 4) = -px*sine*dtheta - py*cosine*dtheta;
  // _F(1, 4) = 0.;
  // _F(1, 4) = -u*deltaZ*( px*sine + py*cosine );
  _F(1, 4) = -u*deltaZ*(old_px*sine + old_py*cosine);

  // @y/@x
  _F(2, 0) = 0.;
  // @y/@px
  _F(2, 1) = (1.-cosine)/u;
  // @y/@y
  _F(2, 2) = 1.;
  // @y/@py
  _F(2, 3) = sine/u;
  // @y/@kappa
  // _F(2, 4) = py*cosine*dtheta/u + px*sine*dtheta/u;
  // _F(2, 4) = 0.;
  // _F(2, 4) = deltaZ*( px*sine + py*cosine );
  _F(2, 4) = deltaZ*(old_px*sine + old_py*cosine);

  // @py/@x
  _F(3, 0) = 0.;
  // @py/@px
  _F(3, 1) = sine;
  // @py/@y
  _F(3, 2) = 0.;
  // @py/@py
  _F(3, 3) = cosine;
  // @py/@kappa
  // _F(3, 4) = -py*sine*dtheta + px*cosine*dtheta;
  // _F(3, 4) = 0.;
  // _F(3, 4) = u*deltaZ*( px*cosine - py*sine );
  _F(3, 4) = u*deltaZ*(old_px*cosine - old_py*sine);

  // @kappa/@x
  _F(4, 0) = 0.;
  // @kappa/@px
  _F(4, 1) = 0.;
  // @kappa/@y
  _F(4, 2) = 0.;
  // @kappa/@py
  _F(4, 3) = 0.;
  // @kappa/@kappa
  _F(4, 4) = 1.;
}

double KalmanHelicalPropagator::GetTrackMomentum(const KalmanState *a_site) {
  TMatrixD a   = a_site->a(KalmanState::Projected);
  double px    = a(1, 0);
  double py    = a(3, 0);
  double kappa = a(4, 0);
  double pz    = fabs(1./kappa);

  double p = TMath::Sqrt(px*px+py*py+pz*pz); // MeV/c
  return p;
}

TMatrixD KalmanHelicalPropagator::BuildQ(TMatrixD a, double L, double material_w) {
  // 'a' is old_site->a(KalmanState::Filtered)
  double px    = a(1, 0);
  double py    = a(3, 0);
  double kappa = a(4, 0);
  double pz    = fabs(1./kappa);
  double p     = sqrt(px*px+py*py+pz*pz);
  double p2    = p*p;

  double muon_mass  = Recon::Constants::MuonMass;
  double muon_mass2 = muon_mass*muon_mass;
  double E    = TMath::Sqrt(muon_mass2+p2);
  double beta = p/E;

  double theta_mcs  = HighlandFormula(L, beta, p);
  double theta_mcs2 = theta_mcs*theta_mcs;

  double charge = kappa/fabs(kappa);
  double c      = CLHEP::c_light;
  double u      = charge*c*_Bz;
  double delta_theta = u*material_w*fabs(kappa);
  double sine   = sin(delta_theta);
  double cosine = cos(delta_theta);

  double dtheta_dpz = -delta_theta/pz;
  // ------------------------------------------------------------
  TMatrixD dalpha_dp(_n_parameters, 3);
  // dx, dpx
  dalpha_dp(0, 0) = sine/u;
  // dx, dpy
  dalpha_dp(0, 1) = (cosine-1.)/u;
  // dx, dpz
  dalpha_dp(0, 2) = px*dtheta_dpz*cosine/u - py*dtheta_dpz*sine/u;

  // dpx, dpx
  dalpha_dp(1, 0) = cosine;
  // dpx, dpy
  dalpha_dp(1, 1) = -sine;
  // dpx, dpz
  dalpha_dp(1, 2) = -px*dtheta_dpz*sine - py*dtheta_dpz*cosine;

  // dy, dpx
  dalpha_dp(2, 0) = (1.-cosine)/u;
  // dy, dpy
  dalpha_dp(2, 1) = sine/u;
  // dy, dpz
  dalpha_dp(2, 2) = py*dtheta_dpz*cosine/u + px*dtheta_dpz*sine/u;

  // dpy, dpx
  dalpha_dp(3, 0) = sine;
  // dpy, dpy
  dalpha_dp(3, 1) = cosine;
  // dpy, dpz
  dalpha_dp(3, 2) = -py*dtheta_dpz*sine + px*dtheta_dpz*cosine;

  // dkappa, dpx
  dalpha_dp(4, 0) = 0.;
  // dkappa, dpy
  dalpha_dp(4, 1) = 0.;
  // dkappa, dpz
  dalpha_dp(4, 2) = -charge/(pz*pz);

  TMatrixD dalpha_dp_transposed(3, _n_parameters);
  dalpha_dp_transposed.Transpose(dalpha_dp);

  // ------------------------------------------------------------
  TMatrixD dalpha_dx(_n_parameters, 3);
  dalpha_dx.Zero();
  dalpha_dx(0, 0) = 1.;
  dalpha_dx(2, 1) = 1.;

  TMatrixD dalpha_dx_transposed(3, _n_parameters);
  dalpha_dx_transposed.Transpose(dalpha_dx);

  // ------------------------------------------------------------
  TMatrixD P(3, 3);
  // Diagonal terms.
  P(0, 0) = 1.-px*px/p2;
  P(1, 1) = 1.-py*py/p2;
  P(2, 2) = 1.-pz*pz/p2;
  // Off diagonal, symmetric.
  P(0, 1) = px*py/p2;
  P(1, 0) = px*py/p2;
  P(0, 2) = px*pz/p2;
  P(2, 0) = px*pz/p2;
  P(1, 2) = py*pz/p2;
  P(2, 1) = py*pz/p2;


  TMatrixD term_1(_n_parameters, _n_parameters);
  term_1 = dalpha_dp*P*dalpha_dp_transposed;
  term_1 = p2*term_1;

  TMatrixD term_2(_n_parameters, _n_parameters);
  term_2 = dalpha_dp*P*dalpha_dx_transposed;
  term_2 = p*material_w*0.5*term_2;

  TMatrixD term_3(_n_parameters, _n_parameters);
  term_3 = dalpha_dx*P*dalpha_dp_transposed;
  term_3 = p*material_w*0.5*term_3;

  TMatrixD term_4(_n_parameters, _n_parameters);
  term_4 = dalpha_dx*P*dalpha_dx_transposed;
  term_4 = (1./3.)*material_w*material_w*term_4;

  TMatrixD Q(_n_parameters, _n_parameters);
  Q = theta_mcs2*(term_1 + term_2 + term_3 + term_4);

  return Q;
}

} // ~namespace MAUS
