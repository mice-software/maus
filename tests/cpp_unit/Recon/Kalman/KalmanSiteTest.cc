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
#include "src/common_cpp/Recon/Kalman/KalmanSite.hh"
#include "gtest/gtest.h"

namespace {
class KalmanSiteTest : public ::testing::Test {
 protected:
  KalmanSiteTest()  {}
  virtual ~KalmanSiteTest() {}
  virtual void SetUp()    {}
  virtual void TearDown() {}
};

TEST_F(KalmanSiteTest, getters_and_setters_test) {
  MAUS::KalmanSite a_site = MAUS::KalmanSite();

  TMatrixD projected_a(5, 1);
  projected_a(0, 0) = 1.0;
  projected_a(0, 0) = 2.0;
  projected_a(0, 0) = 3.0;
  projected_a(0, 0) = 4.0;
  projected_a(0, 0) = 5.0;
  a_site.set_a(projected_a, MAUS::KalmanSite::Projected);

  TMatrixD a(5, 1);
  a(0, 0) = 1.0;
  a(0, 0) = 2.0;
  a(0, 0) = 3.0;
  a(0, 0) = 4.0;
  a(0, 0) = 5.0;
  a_site.set_a(a, MAUS::KalmanSite::Filtered);

  TMatrixD smoothed_a(5, 1);
  smoothed_a(0, 0) = 1.0;
  smoothed_a(0, 0) = 2.0;
  smoothed_a(0, 0) = 3.0;
  smoothed_a(0, 0) = 4.0;
  smoothed_a(0, 0) = 5.0;
  a_site.set_a(smoothed_a, MAUS::KalmanSite::Smoothed);

/*
  set_projected_covariance_matrix(TMatrixD Cp);
  set_smoothed_covariance_matrix(TMatrixD C);
  set_covariance_matrix(TMatrixD C);
  set_measurement(double alpha);
*/
//  EXPECT_EQ(projected_a, a_site.get_projected_a());
//  EXPECT_EQ(a, a_site.get_a());
//  EXPECT_EQ(smoothed_a, a_site.get_smoothed_a());
/*
  TMatrixD get_covariance_matrix();
  TMatrixD get_projected_covariance_matrix();
  TMatrixD get_smoothed_covariance_matrix();
*/
  // Test copy constructor.
  MAUS::KalmanSite copy = MAUS::KalmanSite(a_site);
  // EXPECT_EQ(copy.get_spill(), spill);
  // EXPECT_EQ(copy.get_event(), event);
  // Test = operator.
  MAUS::KalmanSite second_copy;
  second_copy = copy;
  // EXPECT_EQ(second_copy.get_spill(), spill);
  // EXPECT_EQ(second_copy.get_event(), event);
}
}
