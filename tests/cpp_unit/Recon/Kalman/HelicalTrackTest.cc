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

#include "gtest/gtest.h"

namespace {
class HelicalTrackTest : public ::testing::Test {
 protected:
  HelicalTrackTest()  {}
  virtual ~HelicalTrackTest() {}
  virtual void SetUp()    {}
  virtual void TearDown() {}
  void set_up_seed();
  SeedFinder seed;
  double x0, y0, radius, pt, pz;
  KalmanSite old_site, new_site;
  void set_up_sites();
};

void HelicalTrackTest::set_up_seed() {
  x0 = 0.0;
  y0 = 0.0;
  radius = 8.0; // mm
  pt = 1.2*radius; // MeV/c
  pz = 220.0;
  seed.set_x0(x0);
  seed.set_y0(y0);
  seed.set_r(radius);
  seed.set_pt(pt);
  seed.set_pz(pz);
  seed.set_phi_0(0.0);
  seed.set_tan_lambda(pz/pt);
}

void HelicalTrackTest::set_up_sites() {
  old_site.set_z(10.);
  TMatrixD a(5, 1);
  // a(0, 0) =
  // old_site.set_a(a);

  new_site.set_z(100.);
}

TEST_F(HelicalTrackTest, test_constructor) {
  set_up_seed();
  // HelicalTrack *track = new HelicalTrack(seed);
  // EXPECT_EQ(x0, track->get_x0());
  // EXPECT_EQ(y0, track->get_y0());
  // EXPECT_EQ(radius, track->get_r());
}

TEST_F(HelicalTrackTest, test_propagator) {
  set_up_seed();
  set_up_sites();
  // HelicalTrack *track = new HelicalTrack(seed);
/*
old_site.get_z();
old_site.get_state_vector();

new_site.get_z();
*/
}

} // namespace
