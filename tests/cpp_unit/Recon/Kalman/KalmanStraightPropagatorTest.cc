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


#include "src/common_cpp/Recon/Kalman/KalmanStraightPropagator.hh"
#include "gtest/gtest.h"

// TODO: Test propagation of covariance.

namespace {
class KalmanStraightPropagatorTest : public ::testing::Test {
 protected:
  KalmanStraightPropagatorTest()  {}
  virtual ~KalmanStraightPropagatorTest() {}
  virtual void SetUp() {
    old_site.Initialise(4);
    new_site.Initialise(4);
    deltaZ = 1100.0;
    new_site.set_id(0);
    new_site.set_z(deltaZ);
    old_site.set_z(0.0);

    mx = 2.0;
    my = 3.0;
    TMatrixD a(4, 1);
    a(0, 0) = 0.0;
    a(1, 0) = mx;
    a(2, 0) = 0.0;
    a(3, 0) = my;
    old_site.set_a(a, MAUS::KalmanState::Filtered);

    TMatrixD C(4, 4);
    C.Zero();
    for ( int i = 0; i < 4; ++i ) {
      C(i, i) = 100.; // dummy values
    }
    old_site.set_covariance_matrix(C, MAUS::KalmanState::Projected);
  }
  virtual void TearDown() {}
  MAUS::KalmanState old_site;
  MAUS::KalmanState new_site;
  double mx, my, deltaZ;
  void set_up_sites();
  static const double err = 1e-2;
};

TEST_F(KalmanStraightPropagatorTest, propagator_test) {
  MAUS::KalmanPropagator *propagator = new MAUS::KalmanStraightPropagator();
  propagator->CalculatePredictedState(&old_site, &new_site);

  TMatrixD a_projected(4, 1);
  a_projected = new_site.a(MAUS::KalmanState::Projected);

  double expected_x = mx*deltaZ;
  double expected_y = my*deltaZ;
  EXPECT_EQ(expected_x, a_projected(0, 0));
  EXPECT_EQ(expected_y, a_projected(2, 0));
  delete propagator;
  // track->calc_covariance(&old_site, &new_site);
  // Test COVARIANCE
}
}
