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

const double HelicalTrack::_alpha = 1.0/(0.3*4.);

const double HelicalTrack::Pi = 3.14159265;

HelicalTrack::HelicalTrack(SciFiHelicalPRTrack const &seed) {
  // Initialise straight-track member matrices:
  _G.ResizeTo(2, 2);
  _H.ResizeTo(2, 5);
  _F.ResizeTo(5, 5);
  _A.ResizeTo(5, 5);

  _Q.ResizeTo(5, 5);
  _Q.Zero(); // mcs is off.

  double x0 = seed.get_x0();
  double y0 = seed.get_y0();
  double r  = seed.get_R();
  double phi_0 = seed.get_phi0();

  _xc = x0 - r *cos(phi_0);
  _yc = y0 - r *sin(phi_0);

  _h = -1;
}

void HelicalTrack::update_propagator(KalmanSite *old_site, KalmanSite *new_site) {
  // Reset propagator.
  _F.Zero();

  // Find dz.
  double new_z = new_site->get_z();
  double old_z = old_site->get_z();

  double deltaZ = new_z-old_z;
  if ( new_site->get_id() < 15 ) {
    deltaZ = - deltaZ;
  }

  // Get old state vector...
  TMatrixD prev_site(5, 1);
  prev_site = old_site->get_a();
  // Change Coordinate reference frame to the centre of the circle.
  double old_x   = prev_site(0, 0)-_xc;
  double old_y   = prev_site(1, 0)-_yc;
  double old_r   = prev_site(2, 0);
  double old_kappa  = prev_site(3, 0);
  double old_tan_lambda = prev_site(4, 0);

  double my_xc = 0.0;
  double my_yc = 0.0;
  double old_phi = atan2((old_y-my_yc),(old_x-my_xc));
  while (old_phi < 0.)      old_phi += 2.0*Pi;
  while (old_phi > 2.0*Pi)  old_phi -= 2.0*Pi;

  // phi derivatives
  double argument =(old_y-my_yc)/(old_x-my_xc);
  double a_factor = 1./(1.+argument*argument);
  double dphi_dx = - a_factor*argument*1./(old_x-my_xc);
  double dphi_dy =  a_factor/(old_x-my_xc);
  // Compute d_rho.
  double circle_x = my_xc + old_r*cos(old_phi);
  double circle_y = my_yc + old_r*sin(old_phi);

  double drho_x   = -( old_x - circle_x );
  double drho_y   = -( old_y - circle_y );

  double delta_phi = _h*deltaZ/(old_r * old_tan_lambda);
  double new_phi   = (old_phi+delta_phi);
  // drho partial derivatives
  double drho_x_dx, drho_x_dy, drho_x_dR;
  if ( drho_x == 0.0 ) {
    drho_x_dx = 0.0;
    drho_x_dy = 0.0;
    drho_x_dR = 0.0;
  } else {
    drho_x_dx = -1.-old_r*sin(old_phi)*dphi_dx;
    drho_x_dy = -old_r*sin(old_phi)*dphi_dy;
    drho_x_dR = cos(old_phi);
  }
  double drho_y_dx, drho_y_dy, drho_y_dR;
  if ( drho_y == 0.0 ) {
    drho_y_dx = 0.0;
    drho_y_dy = 0.0;
    drho_y_dR = 0.0;
  } else {
    drho_y_dx = old_r*cos(old_phi)*dphi_dx;
    drho_y_dy = -1.+old_r*cos(old_phi)*dphi_dy;
    drho_y_dR = sin(old_phi);
  }
/*
  std::cout << "Track Parameters: " << "\n"
            << "circle centre:     " << _xc << " " << _yc << "\n"
            << "old x:     " << old_x << "\n"
            << "old y:     " << old_y << "\n"
            << "old R:     " << old_r << "\n"
            << "old kappa: " << old_kappa << "\n"
            << "old tan_lambda: " << old_tan_lambda << "\n"
            << "old phi:   " << old_phi << "\n"
            << "new phi:   " << new_phi << "\n"
            << "dphi_dx:   " << dphi_dx << "\n"
            << "dphi_dy:   " << dphi_dy << "\n"
            << "circle_x:  " << circle_x << "\n"
            << "circle_y:  " << circle_y << "\n"
            << "drho x:      " << drho_x << "\n"
            << "drho y:      " << drho_y << "\n"
            << "drhox derivatives: " << drho_x_dx << " " << drho_x_dy << " "
                                     << drho_x_dR << "\n"
            << "drhoy derivatives: " << drho_y_dx << " " << drho_y_dy << " "
                                     << drho_y_dR << std::endl;
*/
  _projected_x = old_x+_xc+drho_x+_h*old_r*(cos(old_phi)-cos(new_phi));
  _projected_y = old_y+_yc+drho_y+_h*old_r*(sin(old_phi)-sin(new_phi));
/*
  std::cout << "New Parameters: " << "\n"
            << "x:     " << _projected_x << "\n"
            << "y:     " << _projected_y << "\n";
  std::cout << "Signed Radius: " << _h*old_r << "\n";
*/
  // Build _F.
  _F(0, 0) = 1.0 + drho_x_dx - _h*old_r*dphi_dx*(sin(old_phi)-sin(new_phi));
  _F(0, 1) = drho_x_dy - _h*old_r*dphi_dy*(sin(old_phi)-sin(new_phi));
  _F(0, 2) = drho_x_dR + _h*(cos(old_phi)-cos(new_phi));

  _F(1, 0) = drho_y_dx + _h*old_r*dphi_dx*(cos(old_phi)-cos(new_phi));
  _F(1, 1) = 1.0 + drho_y_dy+_h*old_r*dphi_dy*(cos(old_phi)-cos(new_phi));
  _F(1, 2) = drho_y_dR + _h*(sin(old_phi)-sin(new_phi));

  _F(2, 2) = 1.0;
  _F(3, 3) = 1.0;
  _F(4, 4) = 1.0;
}

void HelicalTrack::calc_predicted_state(KalmanSite *old_site, KalmanSite *new_site) {
  std::cout <<" ----------------------- Projection ----------------------- \n";
  TMatrixD a = old_site->get_a();
  std::cout << "Old  filtered state: " << std::endl;
  a.Print();
  a(0, 0) = a(0, 0) - _xc;
  a(1, 0) = a(1, 0) - _yc;
  //a.Print();
  TMatrixD a_projected = TMatrixD(_F, TMatrixD::kMult, a);
  a_projected(0, 0) = a_projected(0, 0) + _xc;
  a_projected(1, 0) = a_projected(1, 0) + _yc;
  new_site->set_projected_a(a_projected);

  //_F.Print();
  std::cout << "New projected state: " << std::endl;
  a_projected.Print();
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

} // ~namespace MAUS
