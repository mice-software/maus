/* This file is part of MAUS: http://  micewww.pp.rl.ac.uk:8080/projects/maus
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
 * along with MAUS.  If not, see <http://  www.gnu.org/licenses/>.
 */

/* Author: Peter Lane
 */


#include <cstdlib>
#include <ctime>
#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>

#include "CLHEP/Random/JamesRandom.h"
#include "CLHEP/Random/RandGauss.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "gtest/gtest.h"

#include "Maths/Matrix.hh"
#include "Maths/SymmetricMatrix.hh"
#include "src/common_cpp/Optics/CovarianceMatrix.hh"

using MAUS::CovarianceMatrix;
using MAUS::Matrix;
using MAUS::Vector;
using MAUS::SymmetricMatrix;
using ::CLHEP::HepSymMatrix;

class CovarianceMatrixTest : public ::testing::Test {
 public:
  CovarianceMatrixTest() {
  }
 protected:
  // CLHEP::HepRandomEngine * random_engine_;
  CLHEP::RandGauss * random_gauss_;

  static const double mass_;
  static const double momentum_;
  static const double charge_;
  static const double Bz_;
  static const double Ltwiddle_t_;

  static const double emittance_t_;
  static const double beta_t_;
  static const double alpha_t_;

  static const double emittance_x_;
  static const double beta_x_;
  static const double alpha_x_;

  static const double emittance_y_;
  static const double beta_y_;
  static const double alpha_y_;

  static const double emittance_l_;
  static const double beta_l_;
  static const double alpha_l_;

  static const double dispersion_x_;
  static const double dispersion_prime_x_;
  static const double dispersion_y_;
  static const double dispersion_prime_y_;

  static const double rotation_angle_;

  static const double means_data_[6];
  static const MAUS::Vector<double> means_;

  static const double penn_covariances_data_[36];
  static const MAUS::CovarianceMatrix penn_covariances_;

  static const double twiss_covariances_data_[36];
  static const MAUS::CovarianceMatrix twiss_covariances_;

  static const double positive_definite_data_[36];
  static const MAUS::CovarianceMatrix positive_definite_matrix_;

  static const double non_positive_definite_data_[36];
  static const MAUS::CovarianceMatrix non_positive_definite_matrix_;

  static const double rotated_penn_covariance_data_[36];
  static const MAUS::CovarianceMatrix rotated_penn_covariances_;

  virtual void SetUp() {
    // setup Normally-distributed, random number generator
    // random_engine_ = new CLHEP::HepJamesRandom(-1);
    random_gauss_ = new CLHEP::RandGauss(new CLHEP::HepJamesRandom(-1));
  }

  virtual void TearDown() {
    delete random_gauss_;
  }
};

// ************************************************//
// CovarianceMatrixTest static const initializations//
// ************************************************//
const double CovarianceMatrixTest::mass_        = 105.658367;         // MeV/c^2
const double CovarianceMatrixTest::momentum_    = 199.804294;         // MeV/c
const double CovarianceMatrixTest::charge_      = -1.0;               // e
const double CovarianceMatrixTest::Bz_          = 0.004;              // T?
const double CovarianceMatrixTest::Ltwiddle_t_  = 0.00650534498541;   // mm?

const double CovarianceMatrixTest::emittance_t_ = 5.90976389295;      // mm
const double CovarianceMatrixTest::beta_t_      = 325.783144453;      // mm
const double CovarianceMatrixTest::alpha_t_     = 0.981532870919;

const double CovarianceMatrixTest::emittance_x_ = 8.21460387276;      // mm
const double CovarianceMatrixTest::beta_x_      = 234.053018912;      // mm
const double CovarianceMatrixTest::alpha_x_     = 0.665453972738;

const double CovarianceMatrixTest::emittance_y_ = 8.25981656173;      // mm
const double CovarianceMatrixTest::beta_y_      = 233.413185153;      // mm
const double CovarianceMatrixTest::alpha_y_     = 0.742730085244;

const double CovarianceMatrixTest::emittance_l_ = 18.6075680584;  // mm
const double CovarianceMatrixTest::beta_l_      = 19464.8337931;  // mm
const double CovarianceMatrixTest::alpha_l_     = 0.0434076437382;

const double CovarianceMatrixTest::dispersion_x_        = 14.7689567014;
const double CovarianceMatrixTest::dispersion_prime_x_  = 45.1060971239;
const double CovarianceMatrixTest::dispersion_y_        = 14.7689567014;
const double CovarianceMatrixTest::dispersion_prime_y_  = 45.1060971239;

const double CovarianceMatrixTest::rotation_angle_ = 0.628319;  // pi / 5

// from BeamMaker/xboa {ct, E, x, px, y, py}
const double CovarianceMatrixTest::means_data_[6] = {
  +99740014.0111,  226.033429351,
  +0.843461236593,  -0.0217829119313,
  -0.250110599127,   0.76387175952
};

// from BeamMaker + xboa {ct, E, x, px, y, py}
const double CovarianceMatrixTest::penn_covariances_data_[36] = {
  +1.91531138e+05, -8.53413918e+01,
  +0.00000000e+00,  0.00000000e+00,  0.00000000e+00,  0.00000000e+00,

  -8.53413918e+01,  2.02192558e+01,
  +1.32112012e+00,  4.03485323e+00, -1.03025553e+01,  5.79008832e+00,

  +0.00000000e+00,  1.32112012e+00,
  +1.01811730e+03, -6.12884830e+02,  0.00000000e+00,  6.06385733e+02,

  +0.00000000e+00,  4.03485323e+00,
  -6.12884830e+02,  1.11306115e+03, -6.06385733e+02,  0.00000000e+00,

  +0.00000000e+00, -1.03025553e+01,
  +0.00000000e+00, -6.06385733e+02,  1.01811730e+03, -6.12884830e+02,

  +0.00000000e+00,  5.79008832e+00,
  +6.06385733e+02,  0.00000000e+00, -6.12884830e+02,  1.11306115e+03
};

// from BeamMaker + xboa {ct, E, x, px, y, py}
const double CovarianceMatrixTest::twiss_covariances_data_[36] = {
  +1.91531139e+05,  -8.53413918e+01,
  +1.25053024e+02,  1.94593790e+00, -3.57346382e+02,  8.03446694e+01,

  -8.53413918e+01,  2.02192558e+01,
  +1.32112013e+00,  4.03485324e+00, -1.03025553e+01,  5.79008832e+00,

  +1.25053024e+02, 1.32112013e+00,
  +1.01671668e+03, -5.77575205e+02, -4.63520838e+01,  6.28878344e+02,

  +1.94593790e+00,  4.03485324e+00,
  -5.77575205e+02,  1.06904491e+03, -5.84662003e+02,  1.32913208e+01,

  -3.57346382e+02, -1.03025553e+01,
  -4.63520838e+01, -5.84662003e+02,  1.01951792e+03, -6.48194455e+02,

  +8.03446694e+01,  5.79008832e+00,
  +6.28878344e+02,  1.32913208e+01, -6.48194455e+02,  1.15916946e+03
};

const double CovarianceMatrixTest::positive_definite_data_[36] = {
  1.,  0.,   0.,   0.,   0.,   0.,
  0.,  1.,   0.,   0.,   0.,   0.,
  0.,  0.,   1.,   0.,   0.,   0.,
  0.,  0.,   0.,   1.,   0.,   0.,
  0.,  0.,   0.,   0.,   1.,   0.,
  0.,  0.,   0.,   0.,   0.,   1.
};

const double CovarianceMatrixTest::non_positive_definite_data_[36] = {
  0.,  1.,   2.,   3.,   4.,   5.,
  1.,  6.,   7.,   8.,   9.,   10.,
  2.,  7.,   11.,  12.,  13.,  14.,
  3.,  8.,   12.,  15.,  16.,  17.,
  4.,  9.,   13.,  16.,  18.,  19.,
  5.,  10.,  14.,  17.,  19.,  20.
};

// Rotated penn_covariances_ using Mathematica
const double CovarianceMatrixTest::rotated_penn_covariance_data_[36] = {
  +1.91531138e+05, -8.53413918e+01,
  +0.00000000e+00,  0.00000000e+00,  0.00000000e+00,  0.00000000e+00,

  -8.53413918e+01,  2.02192558e+01,
  -4.98688144+00,   6.66759336e+00, -9.11147725e+00,  2.31265263e+00,

  +0.00000000e+00, -4.98688144e+00,
  +1.01811730e+03, -6.12884830e+02,  0.00000000e+00,  6.06385735e+02,

  +0.00000000e+00,  6.66759336e+00,
  -6.12884831e+02,  1.11306115e+03, -1.87383497e+02,  0.00000000e+00,

  +0.00000000e+00, -9.11147725e+00,
  +0.00000000e+00, -6.06385733e+02,  1.01811730e+03, -6.12884830e+02,

  +0.00000000e+00,  2.31265263e+00,
  +6.06385735e+02,  0.00000000e+00, -6.12884831e+02,  1.11306115e+03
};

const MAUS::Vector<double> CovarianceMatrixTest::means_
  = MAUS::Vector<double>(CovarianceMatrixTest::means_data_, 6);

const MAUS::CovarianceMatrix CovarianceMatrixTest::penn_covariances_
  = MAUS::CovarianceMatrix(CovarianceMatrixTest::penn_covariances_data_);

const MAUS::CovarianceMatrix CovarianceMatrixTest::twiss_covariances_
  = MAUS::CovarianceMatrix(CovarianceMatrixTest::twiss_covariances_data_);

const MAUS::CovarianceMatrix CovarianceMatrixTest::positive_definite_matrix_
  = MAUS::CovarianceMatrix(CovarianceMatrixTest::positive_definite_data_);

const MAUS::CovarianceMatrix CovarianceMatrixTest::non_positive_definite_matrix_
  = MAUS::CovarianceMatrix(CovarianceMatrixTest::non_positive_definite_data_);

const MAUS::CovarianceMatrix CovarianceMatrixTest::rotated_penn_covariances_
  = MAUS::CovarianceMatrix(CovarianceMatrixTest::rotated_penn_covariance_data_);

// ***********
// test cases
// ***********

TEST_F(CovarianceMatrixTest, DefaultConstructor) {
  const CovarianceMatrix zeros;
  double total = 0.0;
  ASSERT_EQ(zeros.size(), static_cast<size_t>(6));

  for (size_t row = 1; row <= 6; ++row) {
    for (size_t column = 1; column <= 6; ++column) {
    total += zeros(row, column);
    }
  }
  ASSERT_TRUE(total < 1.e-9);
}

TEST_F(CovarianceMatrixTest, CopyConstructors) {
  // CovarianceMatrix copy
  const CovarianceMatrix twiss_ellipse
    = CovarianceMatrix::CreateFromTwissParameters(
      mass_, momentum_,
      emittance_x_, beta_x_, alpha_x_,
      emittance_y_, beta_y_, alpha_y_,
      emittance_l_, beta_l_, alpha_l_,
      dispersion_x_, dispersion_prime_x_, dispersion_y_, dispersion_prime_y_);
  const CovarianceMatrix twiss_too(twiss_ellipse);
  ASSERT_EQ(twiss_too, twiss_ellipse);

  // Matrix<double> copies
  const Matrix<double> double_matrix(6, 6, non_positive_definite_data_);
  const CovarianceMatrix double_covariances(double_matrix);
  ASSERT_EQ(double_covariances, non_positive_definite_matrix_);

  const Matrix<double> rows_too_small(4, 6, 1.);
  bool testpass = true;
  try {
    const CovarianceMatrix rows_too_small_covariances(rows_too_small);
    testpass = false;
  } catch (Exceptions::Exception exc) {}
  ASSERT_TRUE(testpass);

  const Matrix<double> columns_too_small(4, 6, 1.);
  testpass = true;
  try {
    const CovarianceMatrix columns_too_small_covariances(columns_too_small);
    testpass = false;
  } catch (Exceptions::Exception exc) {}
  ASSERT_TRUE(testpass);

  // SymmetricMatrix copies
  const SymmetricMatrix symmetric_matrix(6, non_positive_definite_data_);
  const CovarianceMatrix symmetric_covariances(symmetric_matrix);
  ASSERT_EQ(symmetric_covariances, non_positive_definite_matrix_);

  const SymmetricMatrix too_small(4, 1.);
  testpass = true;
  try {
    const CovarianceMatrix too_small_covariances(too_small);
    testpass = false;
  } catch (Exceptions::Exception exc) {}
  ASSERT_TRUE(testpass);

  // HepSymMatrix copy
  const HepSymMatrix hep_sym_matrix(6, 1.);
  const CovarianceMatrix identity(hep_sym_matrix);
  ASSERT_EQ(identity, positive_definite_matrix_);

  const HepSymMatrix hep_too_small(4, 1.);
  testpass = true;
  try {
    const CovarianceMatrix too_small_hep(hep_too_small);
    testpass = false;
  } catch (Exceptions::Exception exc) {}
  ASSERT_TRUE(testpass);

  // TMatrixDSym copy
  const TMatrixDSym root_sym_matrix(6, non_positive_definite_data_);
  const CovarianceMatrix root_covariances(root_sym_matrix);
  ASSERT_EQ(root_covariances, non_positive_definite_matrix_);

  TMatrixDSym root_too_small(4);
  Double_t seed = 2.;
  root_too_small.RandomizePD(-2, 2, seed);
  testpass = true;
  try {
    const CovarianceMatrix too_small_covariances(root_too_small);
    testpass = false;
  } catch (Exceptions::Exception exc) {}
  ASSERT_TRUE(testpass);
}

TEST_F(CovarianceMatrixTest, PennParametrization) {
  const CovarianceMatrix penn_ellipse
    = CovarianceMatrix::CreateFromPennParameters(
      mass_, momentum_, charge_, Bz_, Ltwiddle_t_,
      emittance_t_, beta_t_, alpha_t_,
      emittance_l_, beta_l_, alpha_l_,
      dispersion_x_, dispersion_prime_x_, dispersion_y_, dispersion_prime_y_);
  CovarianceMatrix difference(penn_covariances_ - penn_ellipse);
  double error;
/*
  for (size_t row = 1; row <= 6; ++row) {
    for (size_t column = 1; column <= 6; ++column) {
fprintf(stdout, "%f ", penn_ellipse(row, column));
    }
fprintf(stdout, "\n");
  }
*/
  for (size_t row = 1; row <= 6; ++row) {
    for (size_t column = 1; column <= 6; ++column) {
      // percent difference
      if (penn_covariances_(row, column) > 0.0) {
        error = fabs(difference(row, column) / penn_covariances_(row, column));
      } else {
        error = fabs(difference(row, column));
      }
/*
fprintf(stdout, "difference (%d, %d): %f\t", row, column, difference(row, column));
fprintf(stdout, "percent error (%d, %d): %f\n", row, column, error);
*/
      // FIXME(plane1@hawk.iit.edu) Independently verify t & E covariances
      if ((row < 3) && (column < 3)) {
        // EXPECT_TRUE(error < 1e-4);
      }
      if ((row > 2) && (column > 2) && (row < 5) && (column < 5)) {
        EXPECT_TRUE(error < 1e-4);
      }
      if ((row > 4) && (column > 4) && (row < 7) && (column < 7)) {
        EXPECT_TRUE(error < 1e-4);
      }
    }
  }
// fflush(stdout);
}

TEST_F(CovarianceMatrixTest, TwissParametrization) {
  const CovarianceMatrix twiss_ellipse
    = CovarianceMatrix::CreateFromTwissParameters(
      mass_, momentum_,
      emittance_x_, beta_x_, alpha_x_,
      emittance_y_, beta_y_, alpha_y_,
      emittance_l_, beta_l_, alpha_l_,
      dispersion_x_, dispersion_prime_x_, dispersion_y_, dispersion_prime_y_);
  CovarianceMatrix difference(twiss_covariances_ - twiss_ellipse);
  double error;
// FIXME(plane1@hawk.iit.edu) Figure out a way to calculate
// mixed axis covariances (i.e. <x y>, <x, py>, etc...).
/*
  for (size_t row = 1; row <= 6; ++row) {
    for (size_t column = 1; column <= 6; ++column) {
fprintf(stdout, "%f ", twiss_ellipse(row, column));
    }
fprintf(stdout, "\n");
  }
*/
  for (size_t row = 1; row <= 6; ++row) {
    for (size_t column = 1; column <= 6; ++column) {
      // percent difference
      error = fabs(difference(row, column) / twiss_covariances_(row, column));
/*
fprintf(stdout, "difference (%d, %d): %f\t", row, column, difference(row, column));
fprintf(stdout, "percent error (%d, %d): %f\n", row, column, error);
*/
      if ((row < 3) && (column < 3)) {
        EXPECT_TRUE(error < 1e-4);
      }
      if ((row > 2) && (column > 2) && (row < 5) && (column < 5)) {
        EXPECT_TRUE(error < 1e-4);
      }
      if ((row > 4) && (column > 4) && (row < 7) && (column < 7)) {
        EXPECT_TRUE(error < 1e-4);
      }
    }
  }
// fflush(stdout);
}

TEST_F(CovarianceMatrixTest, PositiveDefiniteness) {
  ASSERT_TRUE(positive_definite_matrix_.IsPositiveDefinite());

  const CovarianceMatrix twiss_ellipse
    = CovarianceMatrix::CreateFromTwissParameters(
      mass_, momentum_,
      emittance_x_, beta_x_, alpha_x_,
      emittance_y_, beta_y_, alpha_y_,
      emittance_l_, beta_l_, alpha_l_,
      dispersion_x_, dispersion_prime_x_, dispersion_y_, dispersion_prime_y_);
  ASSERT_TRUE(twiss_ellipse.IsPositiveDefinite());

  ASSERT_FALSE(non_positive_definite_matrix_.IsPositiveDefinite());

  const CovarianceMatrix penn_ellipse
    = CovarianceMatrix::CreateFromPennParameters(
      mass_, momentum_, charge_, Bz_, Ltwiddle_t_,
      emittance_t_, beta_t_, alpha_t_,
      emittance_l_, beta_l_, alpha_l_,
      dispersion_x_, dispersion_prime_x_, dispersion_y_, dispersion_prime_y_);
  ASSERT_FALSE(penn_ellipse.IsPositiveDefinite());
}

TEST_F(CovarianceMatrixTest, RotateEllipse) {
  const CovarianceMatrix rotated_penn_ellipse
    = rotate(penn_covariances_, rotation_angle_);
  CovarianceMatrix difference(rotated_penn_covariances_ - rotated_penn_ellipse);
  double error;
  for (size_t row = 1; row <= 6; ++row) {
    for (size_t column = 1; column <= 6; ++column) {
      // percent difference
      if (penn_covariances_(row, column) > 0.0) {
        error = fabs(difference(row, column)
                     / rotated_penn_covariances_(row, column));
      } else {
        error = fabs(difference(row, column));
      }
      EXPECT_TRUE(error < 1e-4);
    }
  }
}
