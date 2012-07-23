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
#include "src/common_cpp/Recon/Kalman/StraightTrack.hh"

void StraightTrack::update_propagator(KalmanSite *old_site, KalmanSite *new_site) {
  // Reset.
  _F.Zero();

  // Find dz between sites.
  double new_z = new_site->get_z();
  double old_z = old_site->get_z();
  double deltaZ = new_z-old_z;

  // Build _F.
  for ( int i = 0; i < 5; i++ ) {
    _F(i, i) = 1.;
  }
  _F(0, 2) = fabs(deltaZ);
  _F(1, 3) = fabs(deltaZ);
  // _F.Print();
}

void StraightTrack::calc_system_noise(KalmanSite *site) {
  TMatrixD a(5, 1);
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
}
