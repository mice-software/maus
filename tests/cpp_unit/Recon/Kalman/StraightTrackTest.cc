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
#include "gtest/gtest.h"

// TODO: Test propagation of covariance.

namespace {
class StraightTrackTest : public ::testing::Test {
 protected:
  StraightTrackTest()  {}
  virtual ~StraightTrackTest() {}
  virtual void SetUp()    {}
  virtual void TearDown() {}
  MAUS::KalmanSite old_site;
  MAUS::KalmanSite new_site;
  double mx, my, deltaZ;
  void set_up_sites();
  static const double err = 1e-2;
};

void StraightTrackTest::set_up_sites() {
  deltaZ = 1100.0;
  new_site.set_id(0);
  new_site.set_z(deltaZ);
  old_site.set_z(0.0);

  mx = 2.0;
  my = 3.0;
  TMatrixD a(5, 1);
  a(0, 0) = 0.0;
  a(1, 0) = mx;
  a(2, 0) = 0.0;
  a(3, 0) = my;
  a(4, 0) = 1./200.;
  old_site.set_a(a);

  TMatrixD C(5, 5);
  C.Zero();
  for ( int i = 0; i < 5; ++i ) {
     C(i, i) = 100.; // dummy values
  }
  old_site.set_projected_covariance_matrix(C);
}

TEST_F(StraightTrackTest, propagator_test) {
  set_up_sites();

  MAUS::KalmanTrack *track = new MAUS::StraightTrack();
  track->update_propagator(&old_site, &new_site);
  track->calc_predicted_state(&old_site, &new_site);

  TMatrixD a_projected(5, 1);
  a_projected = new_site.get_projected_a();

  double expected_x = mx*deltaZ;
  double expected_y = my*deltaZ;
  EXPECT_EQ(expected_x, a_projected(0, 0));
  EXPECT_EQ(expected_y, a_projected(2, 0));

  track->calc_covariance(&old_site, &new_site);
  // DO SOMETHING WITH COVARIANCE

}
/*
TEST_F(StraightTrackTest, noise_test) {
  set_up_sites();
  MAUS::StraightTrack *track = new MAUS::StraightTrack();
  track->calc_system_noise(&new_site);
  TMatrixD Q(5, 5);
  Q = track->get_system_noise();
  Q.Print();

  double kappa = 1./200.;
  double Z = 1.;
  double r0 = 0.00167; // cm3/g
  double p = 1./kappa; // MeV/c
  double v = p/105.7;
  double C = 13.6*Z*pow(r0, 0.5)*(1.+0.038*log(r0))/(v*p);
  double expected_value = (1.+pow(mx, 2.))*(1.+pow(mx, 2.)+pow(my, 2.))*C;
  EXPECT_EQ(expected_value,Q(2, 2));
}
*/
}
