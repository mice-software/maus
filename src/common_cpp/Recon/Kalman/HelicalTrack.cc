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
#define PI 3.14159265

namespace MAUS {

HelicalTrack::~HelicalTrack() {}

void HelicalTrack::update_propagator(KalmanSite *old_site, KalmanSite *new_site) {
  // Reset propagator.
  _F.Zero();

  // Find dz.
  double new_z = new_site->get_z();
  double old_z = old_site->get_z();

  double deltaZ = (new_z-old_z); // deltaZ in mm

  // Get old state vector...
  TMatrixD prev_site(5, 1);
  prev_site = old_site->get_a();

  double old_kappa = prev_site(4, 0);
  double old_px = prev_site(1, 0);
  double old_py = prev_site(3, 0);

  double Q = 1.;
  double B = -4.;
  double a = -0.2998*Q*B; // MeV/mm

  // @x/@x
  _F(0, 0) = 1.;
  // @x/@px
  _F(0, 1) = sin(a*deltaZ*old_kappa)/a;
  // @x/@y
  _F(0, 2) = 0.;
  // @x/@py
  _F(0, 3) = -(1.-cos(a*deltaZ*old_kappa))/a;
  // @x/@kappa
  _F(0, 4) = 0.;

  // @px/@x
  _F(1, 0) = 0.;
  // @px/@px
  _F(1, 1) = cos(a*deltaZ*old_kappa);
  // @px/@y
  _F(1, 2) = 0.;
  // @px/@py
  _F(1, 3) = -sin(a*deltaZ*old_kappa);
  // @px/@kappa
  _F(1, 4) = 0.;

  // @y/@x
  _F(2, 0) = 0.;
  // @y/@px
  _F(2, 1) = (1.-cos(a*deltaZ*old_kappa))/a;
  // @y/@y
  _F(2, 2) = 1.;
  // @y/@py
  _F(2, 3) = sin(a*deltaZ*old_kappa)/a;
  // @y/@kappa
  _F(2, 4) = 0.;


  // @py/@x
  _F(3, 0) = 0.;
  // @py/@px
  _F(3, 1) = sin(a*deltaZ*old_kappa);
  // @py/@y
  _F(3, 2) = 0.;
  // @py/@py
  _F(3, 3) = cos(a*deltaZ*old_kappa);
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
