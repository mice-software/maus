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

HelicalTrack::HelicalTrack(bool MCS, bool Eloss) : KalmanTrack(MCS, Eloss) {
  _n_parameters = 6;
}

HelicalTrack::~HelicalTrack() {}

void HelicalTrack::update_propagator(const KalmanSite *old_site, const KalmanSite *new_site) {
  // Reset propagator.
  _F.Zero();

  // Find dz.
  double new_z = new_site->get_z();
  double old_z = old_site->get_z();

  // Delta Z in mm
  double deltaZ = (new_z-old_z);

  // Get old state vector...
  TMatrixD prev_site = old_site->get_a(KalmanSite::Filtered);

  double old_kappa  = prev_site(4, 0);
  double old_kappa2 = TMath::Power(old_kappa, 2.);

  // constant in units MeV/mm
  double a = -0.2998*_particle_charge*_B_field;

  // Define factors to be used in the matrix.
  double sine   = sin(a*deltaZ*old_kappa);
  double cosine = cos(a*deltaZ*old_kappa);

  // @x/@x
  _F(0, 0) = 1.;
  // @x/@px
  _F(0, 1) = sine/(old_kappa*a);
  // @x/@y
  _F(0, 2) = 0.;
  // @x/@py
  _F(0, 3) = (cosine-1.)/(old_kappa*a);
  // @x/@kappa
  _F(0, 4) = 0.0;

  // @px/@x
  _F(1, 0) = 0.;
  // @px/@px
  _F(1, 1) = cosine;
  // @px/@y
  _F(1, 2) = 0.;
  // @px/@py
  _F(1, 3) = -sine;
  // @px/@kappa
  _F(1, 4) = 0.;

  // @y/@x
  _F(2, 0) = 0.;
  // @y/@px
  _F(2, 1) = (1.-cosine)/(old_kappa*a);
  // @y/@y
  _F(2, 2) = 1.;
  // @y/@py
  _F(2, 3) = sine/(old_kappa*a);
  // @y/@kappa
  _F(2, 4) = 0.0;

  // @py/@x
  _F(3, 0) = 0.;
  // @py/@px
  _F(3, 1) = sine;
  // @py/@y
  _F(3, 2) = 0.;
  // @py/@py
  _F(3, 3) = cosine;
  // @py/@kappa
  _F(3, 4) = 0.;

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
