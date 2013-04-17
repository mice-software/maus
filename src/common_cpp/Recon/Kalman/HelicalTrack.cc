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

#include "src/common_cpp/Recon/Kalman/HelicalTrack.hh"

namespace MAUS {

HelicalTrack::HelicalTrack(bool MCS,
                           bool Eloss) : KalmanTrack(MCS, Eloss) {
  _n_parameters = 5;
}

HelicalTrack::~HelicalTrack() {}

void HelicalTrack::CalculatePredictedState(const KalmanSite *old_site, KalmanSite *new_site) {
  // Find dz (mm).
  double new_z = new_site->z();
  double old_z = old_site->z();
  double deltaZ = (new_z-old_z);

  // Get old state vector...
  TMatrixD old_a = old_site->a(KalmanSite::Filtered);
  double old_x      = old_a(0, 0);
  double old_mx     = old_a(1, 0);
  double old_y      = old_a(2, 0);
  double old_my     = old_a(3, 0);
  double old_kappa  = old_a(4, 0);

  double a = -0.2998*_particle_charge*_B_field;
  double sine   = sin(a*deltaZ*old_kappa);
  double cosine = cos(a*deltaZ*old_kappa);

  double new_x  = old_x + (1./old_kappa)*( old_mx*sine/a - old_my*(1.-cosine)/a);
  double new_mx = old_mx*cosine - old_my*sine;
  double new_y  = old_y + (1./old_kappa)*( old_my*sine/a + old_mx*(1.-cosine)/a);
  double new_my = old_my*cosine + old_mx*sine;

  TMatrixD a_projected(_n_parameters, 1);
  a_projected(0, 0) = new_x;
  a_projected(1, 0) = new_mx;
  a_projected(2, 0) = new_y;
  a_projected(3, 0) = new_my;
  a_projected(4, 0) = old_kappa;

  new_site->set_a(a_projected, KalmanSite::Projected);

  UpdatePropagator(old_site, new_site);
}

void HelicalTrack::UpdatePropagator(const KalmanSite *old_site,
                                     const KalmanSite *new_site) {
  // Reset propagator.
  _F.Zero();

  // Find dz.
  double new_z = new_site->z();
  double old_z = old_site->z();

  // Delta Z in mm
  double deltaZ = (new_z-old_z);

  // Get current state vector...
  TMatrixD site = new_site->a(KalmanSite::Projected);
  double mx     = site(1, 0);
  double my     = site(3, 0);
  double kappa  = site(4, 0);
  double kappa2 = TMath::Power(kappa, 2.);

  // constant in units MeV/mm
  double a = -0.2998*_particle_charge*_B_field;

  // Define factors to be used in the matrix.
  double sine   = sin(a*deltaZ*kappa);
  double cosine = cos(a*deltaZ*kappa);

  // @x/@x
  _F(0, 0) = 1.;
  // @x/@mx
  _F(0, 1) = sine/(kappa*a);
  // @x/@y
  _F(0, 2) = 0.;
  // @x/@my
  _F(0, 3) = (cosine-1.)/(kappa*a);
  // @x/@kappa
  // _F(0, 4) = - (1./kappa2)*(mx*sine/a - my*(1.-cosine)/a)
  //            + (1./kappa) *(mx*deltaZ*cosine - my*deltaZ*sine);

  // @mx/@x
  _F(1, 0) = 0.;
  // @mx/@mx
  _F(1, 1) = cosine;
  // @mx/@y
  _F(1, 2) = 0.;
  // @mx/@my
  _F(1, 3) = -sine;
  // @mx/@kappa
  // _F(1, 4) = -mx*a*deltaZ*sine - my*a*deltaZ*cosine;

  // @y/@x
  _F(2, 0) = 0.;
  // @y/@mx
  _F(2, 1) = (1.-cosine)/(kappa*a);
  // @y/@y
  _F(2, 2) = 1.;
  // @y/@my
  _F(2, 3) = sine/(kappa*a);
  // @y/@kappa
  // _F(2, 4) =  - (1./kappa2)*(my*sine/a - mx*(1.-cosine)/a)
  //            + (1./kappa) *(my*deltaZ*cosine - mx*deltaZ*sine);

  // @my/@x
  _F(3, 0) = 0.;
  // @py/@mx
  _F(3, 1) = sine;
  // @my/@y
  _F(3, 2) = 0.;
  // @my/@my
  _F(3, 3) = cosine;
  // @my/@kappa
  // _F(3, 4) = -my*a*deltaZ*sine + mx*a*deltaZ*cosine;

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

} // ~namespace MAUS
