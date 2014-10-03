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
#include "src/common_cpp/Recon/Kalman/KalmanState.hh"
#include "gtest/gtest.h"

namespace {
class KalmanStateTest : public ::testing::Test {
 protected:
  KalmanStateTest()  {}
  virtual ~KalmanStateTest() {}
  virtual void SetUp()    {}
  virtual void TearDown() {}
};

TEST_F(KalmanStateTest, getters_and_setters_test) {
  // Build a Kalman Site
  MAUS::KalmanState a_site = MAUS::KalmanState();
  a_site.Initialise(5);
  int id = 13;
  a_site.set_id(id);

  // Define projected states...
  TMatrixD projected_a(5, 1);
  projected_a(0, 0) = 1.0;
  projected_a(1, 0) = 2.0;
  projected_a(2, 0) = 3.0;
  projected_a(3, 0) = 4.0;
  projected_a(4, 0) = 5.0;
  // TMatrixD pull(2, 1);
  TMatrixD pull(1, 1);
  pull(0, 0) = 1.2;
  // pull(1, 0) = 1.;

  // ... filtered states...
  TMatrixD a(5, 1);
  a(0, 0) = 1.1;
  a(1, 0) = 2.1;
  a(2, 0) = 3.1;
  a(3, 0) = 4.1;
  a(4, 0) = 5.1;
  // TMatrixD residual(2, 1);
  TMatrixD residual(1, 1);
  residual(0, 0) = 0.8;
  // residual(1, 0) = 2.;
  double chi2 = 2.;

  // ... and smoothed states.
  TMatrixD smoothed_a(5, 1);
  smoothed_a(0, 0) = 1.2;
  smoothed_a(1, 0) = 2.2;
  smoothed_a(2, 0) = 3.2;
  smoothed_a(3, 0) = 4.2;
  smoothed_a(4, 0) = 5.2;

  TMatrixD smoothed_residual(1, 1);
  smoothed_residual(0, 0) = 0.6;

  TMatrixD covariance_residual(1, 1);
  covariance_residual(0, 0) = 1.;
  /*
  TMatrixD smoothed_residual(2, 1);
  smoothed_residual(0, 0) = 0.6;
  smoothed_residual(1, 0) = 3.;

  TMatrixD covariance_residual(2, 2);
  covariance_residual(0, 0) = 1.;
  covariance_residual(0, 1) = 2.;
  covariance_residual(1, 0) = 3.;
  covariance_residual(1, 1) = 4.;
  */
  // Set them all.
  a_site.set_a(projected_a, MAUS::KalmanState::Projected);
  a_site.set_residual(pull, MAUS::KalmanState::Projected);

  a_site.set_a(a, MAUS::KalmanState::Filtered);
  a_site.set_residual(residual, MAUS::KalmanState::Filtered);
  a_site.set_chi2(chi2);

  a_site.set_a(smoothed_a, MAUS::KalmanState::Smoothed);
  a_site.set_residual(smoothed_residual, MAUS::KalmanState::Smoothed);
  a_site.set_covariance_residual(covariance_residual, MAUS::KalmanState::Smoothed);

  // Now, we can quickly check that these states were set as expected.
  // Expectations can't handle TMatrices, so we'll be comparing first element of each.
  EXPECT_EQ(a_site.id(), id);
  EXPECT_EQ(a_site.residual(MAUS::KalmanState::Projected)(0, 0),
            pull(0, 0));
  EXPECT_EQ(a_site.residual(MAUS::KalmanState::Filtered)(0, 0),
            residual(0, 0));
  EXPECT_EQ(a_site.residual(MAUS::KalmanState::Smoothed)(0, 0),
            smoothed_residual(0, 0));
  EXPECT_EQ(a_site.a(MAUS::KalmanState::Projected)(0, 0),
            projected_a(0, 0));
  EXPECT_EQ(a_site.a(MAUS::KalmanState::Filtered)(0, 0),
            a(0, 0));
  EXPECT_EQ(a_site.a(MAUS::KalmanState::Smoothed)(0, 0),
            smoothed_a(0, 0));
  EXPECT_EQ(a_site.chi2(), chi2);

  // Are the copy constructor and the assignment operator working? Check them.
  // First, copy...
  MAUS::KalmanState copy = MAUS::KalmanState(a_site);
  EXPECT_EQ(copy.id(), id);
  EXPECT_EQ(copy.residual(MAUS::KalmanState::Projected)(0, 0),
            pull(0, 0));
  EXPECT_EQ(copy.residual(MAUS::KalmanState::Filtered)(0, 0),
            residual(0, 0));
  EXPECT_EQ(copy.residual(MAUS::KalmanState::Smoothed)(0, 0),
            smoothed_residual(0, 0));
  EXPECT_EQ(copy.a(MAUS::KalmanState::Projected)(0, 0),
            projected_a(0, 0));
  EXPECT_EQ(copy.a(MAUS::KalmanState::Filtered)(0, 0),
            a(0, 0));
  EXPECT_EQ(copy.a(MAUS::KalmanState::Smoothed)(0, 0),
            smoothed_a(0, 0));
  EXPECT_EQ(copy.chi2(), chi2);
  EXPECT_EQ(copy.covariance_residual(MAUS::KalmanState::Smoothed)(0, 0),
            covariance_residual(0, 0));

  // Now, the assignment.
  MAUS::KalmanState second_copy;
  second_copy = copy;
  EXPECT_EQ(second_copy.id(), id);
  EXPECT_EQ(second_copy.residual(MAUS::KalmanState::Projected)(0, 0),
            pull(0, 0));
  EXPECT_EQ(second_copy.residual(MAUS::KalmanState::Filtered)(0, 0),
            residual(0, 0));
  EXPECT_EQ(second_copy.residual(MAUS::KalmanState::Smoothed)(0, 0),
            smoothed_residual(0, 0));
  EXPECT_EQ(second_copy.a(MAUS::KalmanState::Projected)(0, 0),
            projected_a(0, 0));
  EXPECT_EQ(second_copy.a(MAUS::KalmanState::Filtered)(0, 0),
            a(0, 0));
  EXPECT_EQ(second_copy.a(MAUS::KalmanState::Smoothed)(0, 0),
            smoothed_a(0, 0));
  EXPECT_EQ(second_copy.chi2(), chi2);
  EXPECT_EQ(second_copy.covariance_residual(MAUS::KalmanState::Smoothed)(0, 0),
            covariance_residual(0, 0));

  // Bad requests
  EXPECT_THROW(a_site.set_covariance_residual(covariance_residual, MAUS::KalmanState::Initialized),
               MAUS::Exception);
  EXPECT_THROW(a_site.set_residual(smoothed_residual, MAUS::KalmanState::Initialized),
               MAUS::Exception);
  EXPECT_THROW(a_site.set_covariance_matrix(covariance_residual, MAUS::KalmanState::Initialized),
               MAUS::Exception);
}
}
