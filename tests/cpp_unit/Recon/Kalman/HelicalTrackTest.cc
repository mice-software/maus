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
  SciFiHelicalPRTrack seed;
  KalmanSite old_site;
  KalmanSite new_site;
  int tracker;
  double x0, y0, radius, pt, pz, phi0, tan_lambda, kappa;
  void set_up_sites();
  TMatrixD a;
  static const double err = 1e-2;
};

void HelicalTrackTest::set_up_seed() {
  // mu+; B = 4T.
  int sign_BQ = 1;
  int h = -sign_BQ;
  int sign= 1;
  int Q = 1;
  tracker = 0;
  x0 = 5.0;
  y0 = 0.0;
  radius = 8.0; // mm
  pt = 1.2*radius; // MeV/c
  pz = 220.0;
  tan_lambda = pz/pt;
  kappa = Q/pt;
  phi0 = 0.0;
  // set_spacepoints(std::vector<SciFiSpacePoint> spoints) { _spoints = spoints; }
  seed.set_x0(x0);
  seed.set_y0(y0);
  seed.set_phi0(phi0);
  seed.set_dsdz(1./tan_lambda);
  seed.set_R(radius);
  seed.set_tracker(tracker);
}

void HelicalTrackTest::set_up_sites() {
  old_site.set_z(0.0);
  new_site.set_z(-1100.0/5.);
  new_site.set_id(10);

  a.ResizeTo(5, 1);
  a(0, 0) = x0+radius*cos(phi0)+1.0;
  a(1, 0) = y0+radius*sin(phi0);
  a(2, 0) = radius;
  a(3, 0) = kappa;
  a(4, 0) = tan_lambda;
  old_site.set_a(a);
}

TEST_F(HelicalTrackTest, test_propagator) {
  set_up_seed();
  set_up_sites();

  HelicalTrack *track = new HelicalTrack(seed);
  EXPECT_EQ(x0, track->get_x0());
  EXPECT_EQ(y0, track->get_y0());

  track->update_propagator(&old_site, &new_site);
  TMatrixD F(5, 5);
  F = track->get_propagator();
  TMatrixD a_temp(5, 1);
  a_temp = TMatrixD(F, TMatrixD::kMult, a);
  a.Print();
  F.Print();
  a_temp.Print();
  double projected_x = track->get_projected_x();
  double projected_y = track->get_projected_y();

  EXPECT_TRUE(abs(projected_x-a_temp(0, 0)) < err);
  EXPECT_TRUE(abs(projected_y-a_temp(1, 0)) < err);
}

} // namespace
