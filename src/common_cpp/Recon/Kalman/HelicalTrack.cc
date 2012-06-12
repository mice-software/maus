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

HelicalTrack::HelicalTrack(SeedFinder const &seed) {
  // Initialise straight-track member matrices:
  _G.ResizeTo(2, 2);
  _H.ResizeTo(2, 5);
  _F.ResizeTo(5, 5);
  _A.ResizeTo(5, 5);

  _Q.ResizeTo(5, 5);
  _Q.Zero(); // mcs is off.

  _x0 = seed.get_x0();
  _y0 = seed.get_y0();
  _r  = seed.get_r();
}

void HelicalTrack::update_propagator(KalmanSite *old_site, KalmanSite *new_site) {
  // Reset.
  _F.Zero();

  // Find dz.
  double new_z = new_site->get_z();
  double old_z = old_site->get_z();
  double deltaZ = new_z-old_z;

  TMatrixD prev_site(5, 1);
  prev_site = old_site->get_state_vector();
  double old_site_x   = prev_site(0, 0);
  double old_site_y   = prev_site(1, 0);
  double old_site_tan_lambda = prev_site(2, 0);
  double old_site_phi  = prev_site(3, 0);
  double old_site_kappa = prev_site(4, 0);

  // Find d_rho
  double circle_x = _x0+_r*cos(old_site_phi*PI/180.);
  double circle_y = _y0+_r*sin(old_site_phi*PI/180.);
  double d_rho = pow(pow(old_site_x-circle_x, 2) +
                     pow(old_site_y-circle_y, 2), 0.5);

  double new_phi = - old_site_kappa * deltaZ/(_alpha * old_site_tan_lambda);

  // Build _F.
  for ( int i = 0; i < 5; i++ ) {
    _F(i, i) = 1.;
  }

  _F(0, 3) = -d_rho*sin(old_site_phi) + (_alpha/old_site_kappa) *
             (-sin(old_site_phi) + sin(old_site_phi+new_phi));
  _F(0, 4) = - (_alpha/pow(old_site_kappa, 2)) * (cos(old_site_phi) - cos(old_site_phi+new_phi));

  _F(1, 3) =  d_rho*cos(old_site_phi) + (_alpha/old_site_kappa) *
              (cos(old_site_phi) - cos(old_site_phi+new_phi));
  _F(1, 4) = - (_alpha/pow(old_site_kappa, 2)) * (sin(old_site_phi) - sin(old_site_phi+new_phi));
}
