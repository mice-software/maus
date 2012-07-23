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

HelicalTrack::HelicalTrack(SciFiHelicalPRTrack const &seed) {
  // Initialise straight-track member matrices:
  _G.ResizeTo(2, 2);
  _H.ResizeTo(2, 5);
  _F.ResizeTo(5, 5);
  _A.ResizeTo(5, 5);

  _Q.ResizeTo(5, 5);
  _Q.Zero(); // mcs is off.

  _x0 = seed.get_x0();
  _y0 = seed.get_y0();
  // pid -13 is mu+
  // tracker 0: B = 4T,
  // tracker 1: B = -4T.
  int tracker = seed.get_tracker();
  if ( tracker == 0 ) {
    _sign = 1;
  } else {
    _sign = -1;
  }
  // _r  = seed.get_R();
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
  double old_x   = prev_site(0, 0);
  double old_y   = prev_site(1, 0);
  double old_r = prev_site(2, 0);
  double old_phi  = prev_site(3, 0);
  double old_tan_lambda = prev_site(4, 0);


  // Find d_rho.
  double circle_x = _x0+old_r*cos(old_phi);
  double circle_y = _y0+old_r*sin(old_phi);
  double d_rho = pow(pow(old_x-circle_x, 2) +
                     pow(old_y-circle_y, 2), 0.5);

  std::cout << "Drho: " << d_rho << " "
            << "" << std::endl;

  // double new_phi = - old_site_kappa * deltaZ/(_alpha * old_site_tan_lambda);
  double delta_phi = _sign*deltaZ/(old_r * old_tan_lambda);
  double new_phi   = (old_phi+delta_phi);
  std::cerr << "Phi: " << old_phi << " " << new_phi << std::endl;
  // double old_phi_degrees = old_site_phi;
  // Build _F.
  _F(0, 0) = 1.0;
  _F(1, 0) = 0.0;
  _F(2, 0) = _sign*( cos(old_phi) - cos(new_phi) ) -
             sin(new_phi)*deltaZ/(old_tan_lambda*old_r);
  _F(3, 0) = -d_rho*sin(old_phi) +
             _sign*old_r*( -sin(old_phi) + sin(new_phi) );
  _F(4, 0) = - sin(new_phi)*
             deltaZ/(old_tan_lambda*old_tan_lambda);

  _F(0, 1) = 0.0;
  _F(1, 1) = 1.0;
  _F(2, 1) = _sign*( sin(old_phi) - sin(new_phi) ) -
             cos(new_phi)*deltaZ/(old_tan_lambda*old_r);

  _F(3, 1) = d_rho*cos(old_phi) +
             _sign*old_r*(cos(old_phi) - cos(new_phi));
  _F(4, 1) = cos(new_phi)*
             deltaZ/(old_tan_lambda*old_tan_lambda);

  _F(0, 2) = 0.0;
  _F(1, 2) = 0.0;
  _F(2, 2) = 1.0;
  _F(3, 2) = 0.0;
  _F(4, 2) = 0.0;

  _F(0, 3) = 0.0;
  _F(1, 3) = 0.0;
  _F(2, 3) = -delta_phi/old_r;
  _F(3, 3) = 1.0;
  _F(4, 3) = -delta_phi/old_tan_lambda;

  _F(0, 4) = 0.0;
  _F(1, 4) = 0.0;
  _F(2, 4) = 0.0;
  _F(3, 4) = 0.0;
  _F(4, 4) = 1.0;


  // for ( int i = 0; i < 5; i++ ) {
    // _F(i, i) = 1.;
  // }

  // _F(0, 3) = -d_rho*sin(old_site_phi) + (_alpha/old_site_kappa) *
  //           (-sin(old_site_phi) + sin(old_site_phi+new_phi));
  // _F(0, 4) = - (_alpha/pow(old_site_kappa, 2)) * (cos(old_site_phi) - cos(old_site_phi+new_phi));

  // _F(1, 3) =  d_rho*cos(old_site_phi) + (_alpha/old_site_kappa) *
  //            (cos(old_site_phi) - cos(old_site_phi+new_phi));
  // _F(1, 4) = - (_alpha/pow(old_site_kappa, 2)) * (sin(old_site_phi) - sin(old_site_phi+new_phi));
}

void HelicalTrack::calc_system_noise(KalmanSite *site) {
  _Q.Zero();
/*  TMatrixD a(5, 1);
  a = site->get_a();
  double mx = a(2, 0);
  double my = a(3, 0);
  double kappa = a(4, 0);
  double Z = 1.;
  double r0 = 0.00167; // cm3/g
  double p = 1/kappa; // MeV/c
  double v = p/105.7;
  double C = 13.6*Z*pow(r0, 0.5)*(1+0.038*log(r0))/(v*p);

  _Q(2, 2) = (1+pow(mx, 2.))*(1+pow(mx, 2.)+pow(my, 2.))*C;
  _Q(3, 3) = (1+pow(my, 2.))*(1+pow(mx, 2.)+pow(my, 2.))*C;
  _Q(2, 3) = mx*my*(1+mx*mx+my*my)*C;
  _Q(3, 2) = mx*my*(1+mx*mx+my*my)*C;

  _Q(4, 4) = kappa*kappa*my*my*C/(1+mx*mx);
  _Q(3, 4) = kappa * my * (1+mx*mx+my*my) * C /(1+mx*mx);
*/
}

