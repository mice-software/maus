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
#include "src/common_cpp/Recon/Kalman/KalmanSite.hh"
#include "gtest/gtest.h"

namespace {
class HelicalTrackTest : public ::testing::Test {
 protected:
  HelicalTrackTest()  {}
  virtual ~HelicalTrackTest() {}
  virtual void SetUp()    {}
  virtual void TearDown() {}
  void set_up_seed();
  MAUS::SciFiHelicalPRTrack seed;
  MAUS::KalmanSite old_site;
  MAUS::KalmanSite new_site;
  int tracker;
  double xc, yc, radius, pt, pz, phi0, tan_lambda, kappa;
  double dx, dy;
  void set_up_sites();
  TMatrixD a;
  static const double err = 1.e-2;
};

void HelicalTrackTest::set_up_seed() {
  // mu+; B = 4T.
  int sign_BQ = 1;
  int h = -1;
  int Q = 1;
  tracker = 0;
  xc = 1.0;
  yc = 7.0;
  dx = 0.2;
  dy = 0.2;
  radius = 8.0; // mm
  pt = 1.2*radius; // MeV/c
  pz = 220.0;
  tan_lambda = pz/pt;
  kappa = Q/pow(pt*pt+pz*pz,0.5);
  double theta = 3.14159265359/2.;
  phi0 = atan2(radius*sin(theta)+dy,radius*cos(theta)+dx);
  //phi0 = atan2(radius*sin(theta)+dy+yc,radius*cos(theta)+dx+xc);
  double x0 = xc+radius*cos(phi0);
  double y0 = yc+radius*sin(phi0);
  // set_spacepoints(std::vector<SciFiSpacePoint> spoints) { _spoints = spoints; }
  // std::cerr << "Helical Test. Phi0 is " << phi0 << std::endl;
  // std::cerr << "Computed from: " << radius*sin(theta)+dy << " " << radius*cos(theta)+dx << std::endl;
  seed.set_x0(x0);
  seed.set_y0(y0);
  seed.set_phi0(phi0);
  seed.set_dsdz(1./tan_lambda);
  seed.set_R(radius);
  seed.set_tracker(tracker);
}

void HelicalTrackTest::set_up_sites() {
  old_site.set_z(1101.06);
  new_site.set_z(0.0);
  new_site.set_id(0);

  a.ResizeTo(5, 1);
  a(0, 0) = xc+radius*cos(phi0)+dx;
  a(1, 0) = yc+radius*sin(phi0)+dy;
  //a(0, 0) = 2.2; //2.2
  //a(1, 0) = 10.2; //10.2
  a(2, 0) = radius;
  a(3, 0) = kappa;
  a(4, 0) = tan_lambda;
  old_site.set_a(a);
}

TEST_F(HelicalTrackTest, test_propagator_case_1) {
  set_up_seed();
  set_up_sites();

  MAUS::HelicalTrack *track = new MAUS::HelicalTrack(seed);
  EXPECT_EQ(xc, track->get_xc());
  EXPECT_EQ(yc, track->get_yc());

  track->update_propagator(&old_site, &new_site);
  track->calc_predicted_state(&old_site, &new_site);
  //TMatrixD F(5, 5);
  //F = track->get_propagator();
  TMatrixD a_temp(5, 1);
  // move to centre of helix
  //a_temp(1, 0) = a_temp(1, 0) - x0;
  //a_temp(2, 0) = a_temp(1, 0) - y0;
  //a_temp = TMatrixD(F, TMatrixD::kMult, a);
  //a.Print();
  //F.Print();
  //a_temp.Print();
  double projected_x = track->get_projected_x();
  double projected_y = track->get_projected_y();
  // projected_x = projected_x+x0;
  // projected_y = projected_y+y0;
  a_temp = new_site.get_projected_a();
  a_temp.Print();
  std::cerr << projected_x << " " << projected_y << std::endl;
  EXPECT_TRUE(fabs(projected_x-a_temp(0, 0)) < err);
  EXPECT_TRUE(fabs(projected_y-a_temp(1, 0)) < err);
//

}

TEST_F(HelicalTrackTest, test_propagator_case_2) {
  // Set up a seed.
  int h = -1;
  MAUS::SciFiHelicalPRTrack a_seed;
  a_seed.set_x0(-12.7071);
  a_seed.set_y0(-1.56239);
  a_seed.set_phi0(3.72919);
  double tanlambda = 12.2763;
  a_seed.set_dsdz(1./tanlambda);
  a_seed.set_R(13.482);
  a_seed.set_tracker(1);

  // Set up sites.
  MAUS::KalmanSite an_old_site;
  MAUS::KalmanSite a_new_site;
  an_old_site.set_z(0.0);
  a_new_site.set_z(0.6523);
  a_new_site.set_id(16);
  a.ResizeTo(5, 1);
  a(0, 0) = -12.95;
  a(1, 0) = -1.726;
  a(2, 0) = 13.48;
  a(3, 0) = 0.005018;
  a(4, 0) = 12.28;
  an_old_site.set_a(a);

  // Test...
  MAUS::HelicalTrack *a_track = new MAUS::HelicalTrack(a_seed);
  double x_c = -1.48641;
  double y_c = 5.91146;
  EXPECT_TRUE(x_c-a_track->get_xc() < err);
  EXPECT_TRUE(y_c-a_track->get_yc() < err);

  a_track->update_propagator(&an_old_site, &a_new_site);
  a_track->calc_predicted_state(&an_old_site, &a_new_site);
  //TMatrixD F(5, 5);
  //F = track->get_propagator();
  TMatrixD a_temp(5, 1);
  // move to centre of helix
  //a_temp(1, 0) = a_temp(1, 0) - x0;
  //a_temp(2, 0) = a_temp(1, 0) - y0;
  //a_temp = TMatrixD(F, TMatrixD::kMult, a);
  //a.Print();
  //F.Print();
  //a_temp.Print();
  double projected_x = a_track->get_projected_x();
  double projected_y = a_track->get_projected_y();
  // projected_x = projected_x+x0;
  // projected_y = projected_y+y0;
  a_temp = a_new_site.get_projected_a();
  a_temp.Print();
  std::cerr << projected_x << " " << projected_y << std::endl;
  std::cerr << -12.9359 << " " << -1.95801 << std::endl;
  // -14.4268 -2.72762
  EXPECT_TRUE(fabs(projected_x-a_temp(0, 0)) < err);
  EXPECT_TRUE(fabs(projected_y-a_temp(1, 0)) < err);
}

} // namespace
