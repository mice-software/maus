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
  // Build a Kalman Site
  MAUS::KalmanSite a_site = MAUS::KalmanSite();
  a_site.Initialise(5);
  int id = 13;
  a_site.set_id(id);

  // Define projected states...
  TMatrixD projected_a(5, 1);
  projected_a(0, 0) = 1.0;
  projected_a(0, 0) = 2.0;
  projected_a(0, 0) = 3.0;
  projected_a(0, 0) = 4.0;
  projected_a(0, 0) = 5.0;
  TMatrixD pull(2, 1);
  pull(0, 0) = 1.2;
  pull(1, 0) = 1.;

  // ... filtered states...
  TMatrixD a(5, 1);
  a(0, 0) = 1.1;
  a(0, 0) = 2.1;
  a(0, 0) = 3.1;
  a(0, 0) = 4.1;
  a(0, 0) = 5.1;
  TMatrixD residual(2, 1);
  residual(0, 0) = 0.8;
  residual(1, 0) = 2.;
  double f_chi2 = 2.;

  // ... and smoothed states.
  TMatrixD smoothed_a(5, 1);
  smoothed_a(0, 0) = 1.2;
  smoothed_a(0, 0) = 2.2;
  smoothed_a(0, 0) = 3.2;
  smoothed_a(0, 0) = 4.2;
  smoothed_a(0, 0) = 5.2;
  TMatrixD smoothed_residual(2, 1);
  smoothed_residual(0, 0) = 0.6;
  smoothed_residual(1, 0) = 3.;
  double s_chi2 = 1.;

  // Set them all.
  a_site.set_a(projected_a, MAUS::KalmanSite::Projected);
  a_site.set_residual(pull, MAUS::KalmanSite::Projected);

  a_site.set_a(a, MAUS::KalmanSite::Filtered);
  a_site.set_residual(residual, MAUS::KalmanSite::Filtered);
  a_site.set_chi2(f_chi2, MAUS::KalmanSite::Filtered);

  a_site.set_a(smoothed_a, MAUS::KalmanSite::Smoothed);
  a_site.set_residual(smoothed_residual, MAUS::KalmanSite::Smoothed);
  a_site.set_chi2(s_chi2, MAUS::KalmanSite::Smoothed);

  // Now, we can quickly check that these states were set as expected.
  // Expectations can't handle TMatrices, so we'll be comparing first element of each.
  EXPECT_EQ(a_site.id(), id);
  EXPECT_EQ(a_site.residual(MAUS::KalmanSite::Projected)(0, 0),
            pull(0, 0));
  EXPECT_EQ(a_site.residual(MAUS::KalmanSite::Filtered)(0, 0),
            residual(0, 0));
  EXPECT_EQ(a_site.residual(MAUS::KalmanSite::Smoothed)(0, 0),
            smoothed_residual(0, 0));
  EXPECT_EQ(a_site.a(MAUS::KalmanSite::Projected)(0, 0),
            projected_a(0, 0));
  EXPECT_EQ(a_site.a(MAUS::KalmanSite::Filtered)(0, 0),
            a(0, 0));
  EXPECT_EQ(a_site.a(MAUS::KalmanSite::Smoothed)(0, 0),
            smoothed_a(0, 0));
  EXPECT_EQ(a_site.chi2(MAUS::KalmanSite::Filtered),
            f_chi2);
  EXPECT_EQ(a_site.chi2(MAUS::KalmanSite::Smoothed),
            s_chi2);

  // Are the copy constructor and the assignment operator working? Check them.
  // First, copy...
  MAUS::KalmanSite copy = MAUS::KalmanSite(a_site);
  EXPECT_EQ(copy.id(), id);
  EXPECT_EQ(copy.residual(MAUS::KalmanSite::Projected)(0, 0),
            pull(0, 0));
  EXPECT_EQ(copy.residual(MAUS::KalmanSite::Filtered)(0, 0),
            residual(0, 0));
  EXPECT_EQ(copy.residual(MAUS::KalmanSite::Smoothed)(0, 0),
            smoothed_residual(0, 0));
  EXPECT_EQ(copy.a(MAUS::KalmanSite::Projected)(0, 0),
            projected_a(0, 0));
  EXPECT_EQ(copy.a(MAUS::KalmanSite::Filtered)(0, 0),
            a(0, 0));
  EXPECT_EQ(copy.a(MAUS::KalmanSite::Smoothed)(0, 0),
            smoothed_a(0, 0));
  EXPECT_EQ(copy.chi2(MAUS::KalmanSite::Filtered),
            f_chi2);
  EXPECT_EQ(copy.chi2(MAUS::KalmanSite::Smoothed),
            s_chi2);

  // Now, the assignment.
  MAUS::KalmanSite second_copy;
  second_copy = copy;
  EXPECT_EQ(second_copy.id(), id);
  EXPECT_EQ(second_copy.residual(MAUS::KalmanSite::Projected)(0, 0),
            pull(0, 0));
  EXPECT_EQ(second_copy.residual(MAUS::KalmanSite::Filtered)(0, 0),
            residual(0, 0));
  EXPECT_EQ(second_copy.residual(MAUS::KalmanSite::Smoothed)(0, 0),
            smoothed_residual(0, 0));
  EXPECT_EQ(second_copy.a(MAUS::KalmanSite::Projected)(0, 0),
            projected_a(0, 0));
  EXPECT_EQ(second_copy.a(MAUS::KalmanSite::Filtered)(0, 0),
            a(0, 0));
  EXPECT_EQ(second_copy.a(MAUS::KalmanSite::Smoothed)(0, 0),
            smoothed_a(0, 0));
  EXPECT_EQ(second_copy.chi2(MAUS::KalmanSite::Filtered),
            f_chi2);
  EXPECT_EQ(second_copy.chi2(MAUS::KalmanSite::Smoothed),
            s_chi2);
}

}
