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

  // Find drho.
  TMatrixD prev_site(5, 1);
  prev_site = old_site->get_a();
  double old_site_x   = prev_site(0, 0);
  double old_site_y   = prev_site(1, 0);
  double old_site_phi  = prev_site(2, 0);
  double old_site_tan_lambda = prev_site(3, 0);
  double old_site_kappa = prev_site(4, 0);

  // Find d_rho.
  double circle_x = _x0+_r*cos(old_site_phi);
  double circle_y = _y0+_r*sin(old_site_phi);
  double d_rho = pow(pow(old_site_x-circle_x, 2) +
                     pow(old_site_y-circle_y, 2), 0.5);

  std::cout << "Drho: " << d_rho << std::endl;

  // double new_phi = - old_site_kappa * deltaZ/(_alpha * old_site_tan_lambda);
  double delta_phi = old_site_kappa*deltaZ/(ALPHA * old_site_tan_lambda);
  double new_phi_degrees = (old_site_phi+delta_phi);
  double old_phi_degrees = old_site_phi;
  // Build _F.
  _F(0, 0) = 1.0;
  _F(1, 0) = 0.0;
  _F(2, 0) = -d_rho*sin(old_phi_degrees) +
             ALPHA/old_site_kappa*(-sin(old_phi_degrees) +
             sin(new_phi_degrees));
  _F(3, 0) = -sin(new_phi_degrees)*
             deltaZ/(old_site_tan_lambda*old_site_tan_lambda);
  _F(4, 0) = -(ALPHA/old_site_kappa*old_site_kappa)*
             ( cos(old_phi_degrees) - cos(new_phi_degrees) ) +
             sin(new_phi_degrees)*deltaZ/(old_site_kappa*old_site_tan_lambda);

  _F(0, 1) = 0.0;
  _F(1, 1) = 1.0;
  _F(2, 1) = -d_rho*cos(old_phi_degrees) +
             ALPHA/old_site_kappa*(cos(old_phi_degrees) - cos(new_phi_degrees));
  _F(3, 1) = cos(new_phi_degrees)*
             deltaZ/(old_site_tan_lambda*old_site_tan_lambda);
  _F(4, 1) = -(ALPHA/old_site_kappa*old_site_kappa)*
             ( sin(old_phi_degrees) - sin(new_phi_degrees) ) -
             cos(new_phi_degrees)*deltaZ/(old_site_kappa*old_site_tan_lambda);

  _F(0, 2) = 0.0;
  _F(1, 2) = 0.0;
  _F(2, 2) = 1.0;
  _F(3, 2) = -deltaZ*old_site_kappa/(ALPHA*old_site_tan_lambda*old_site_tan_lambda);
  _F(4, 2) = deltaZ/(ALPHA*old_site_tan_lambda);

  _F(0, 3) = 0.0;
  _F(1, 3) = 0.0;
  _F(2, 3) = 0.0;
  _F(3, 3) = 1.0;
  _F(4, 3) = 0.0;

  _F(0, 4) = 0.0;
  _F(1, 4) = 0.0;
  _F(2, 4) = 0.0;
  _F(3, 4) = 0.0;
  _F(4, 4) = 1.0;
  // for ( int i = 0; i < 5; i++ ) {
    // _F(i, i) = 1.;
  //}

  // _F(0, 3) = -d_rho*sin(old_site_phi) + (_alpha/old_site_kappa) *
  //           (-sin(old_site_phi) + sin(old_site_phi+new_phi));
  // _F(0, 4) = - (_alpha/pow(old_site_kappa, 2)) * (cos(old_site_phi) - cos(old_site_phi+new_phi));

  // _F(1, 3) =  d_rho*cos(old_site_phi) + (_alpha/old_site_kappa) *
  //            (cos(old_site_phi) - cos(old_site_phi+new_phi));
  // _F(1, 4) = - (_alpha/pow(old_site_kappa, 2)) * (sin(old_site_phi) - sin(old_site_phi+new_phi));
}
