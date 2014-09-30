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
  TMatrixD old_a    = old_site->a(KalmanState::Filtered);
  double old_x      = old_a(0, 0);
  double old_mx     = old_a(1, 0);
  double old_y      = old_a(2, 0);
  double old_my     = old_a(3, 0);
  double old_kappa  = old_a(4, 0);
  double charge = old_kappa/fabs(old_kappa);

  double c      = CLHEP::c_light;
  double a      = -c*charge*_Bz;
  double sine   = sin(a*deltaZ*fabs(old_kappa));
  double cosine = cos(a*deltaZ*fabs(old_kappa));

  // Calculate the new track parameters.
  double new_x  = old_x + (old_mx*sine)/(a*charge*old_kappa)
                  - old_my*(1.-cosine)/(a*charge*old_kappa);

  double new_mx = old_mx*cosine - old_my*sine;

  double new_y  = old_y + (old_my*sine)/(a*charge*old_kappa)
                  + old_mx*(1.-cosine)/(a*charge*old_kappa);

  double new_my = old_my*cosine + old_mx*sine;

  TMatrixD a_projected(_n_parameters, 1);
  a_projected(0, 0) = new_x;
  a_projected(1, 0) = new_mx;
  a_projected(2, 0) = new_y;
  a_projected(3, 0) = new_my;
  a_projected(4, 0) = old_kappa;

  new_site->set_a(a_projected, KalmanState::Projected);

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

  // Get current state vector...
  TMatrixD site = new_site->a(KalmanState::Projected);
  double kappa  = site(4, 0);
  double charge = kappa/fabs(kappa);

  // Define factors to be used in the matrix.
  double c      = CLHEP::c_light;
  double a      = -c*charge*_Bz;
  double sine   = sin(a*deltaZ*charge*kappa);
  double cosine = cos(a*deltaZ*charge*kappa);

  // @x/@x
  _F(0, 0) = 1.;
  // @x/@mx
  _F(0, 1) = sine/(a*charge*kappa);
  // @x/@y
  _F(0, 2) = 0.;
  // @x/@my
  _F(0, 3) = (cosine-1.)/(a*charge*kappa);

  // @mx/@x
  _F(1, 0) = 0.;
  // @mx/@mx
  _F(1, 1) = cosine;
  // @mx/@y
  _F(1, 2) = 0.;
  // @mx/@my
  _F(1, 3) = -sine;

  // @y/@x
  _F(2, 0) = 0.;
  // @y/@mx
  _F(2, 1) = (1.-cosine)/(a*charge*kappa);
  // @y/@y
  _F(2, 2) = 1.;
  // @y/@my
  _F(2, 3) = sine/(a*charge*kappa);

  // @my/@x
  _F(3, 0) = 0.;
  // @py/@mx
  _F(3, 1) = sine;
  // @my/@y
  _F(3, 2) = 0.;
  // @my/@my
  _F(3, 3) = cosine;

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
  TMatrixD a = a_site->a(KalmanState::Projected);
  double mx    = a(1, 0);
  double my    = a(3, 0);
  double kappa = a(4, 0);

  double pz = fabs(1./kappa);
  double px = mx/kappa;
  double py = my/kappa;
  double p = TMath::Sqrt(px*px+py*py+pz*pz); // MeV/c
  return p;
}

} // ~namespace MAUS
