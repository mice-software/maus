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


#include <limits>
#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>

#include "gtest/gtest.h"

#include "Maths/Matrix.hh"
#include "Maths/PolynomialVector.hh"
#include "src/common_cpp/Optics/CovarianceMatrix.hh"
#include "src/common_cpp/Optics/PhaseSpaceVector.hh"
#include "src/common_cpp/Optics/PolynomialTransferMap.hh"

class PolynomialTransferMapTest : public ::testing::Test {
 public:
  PolynomialTransferMapTest() {
  }


 protected:

  static const double reflection_polynomial_coefficient_matrix_data_[42];
  static const double null_reference_trajectory_data_[6];
  static const double phase_space_vector_data_[6];
  static const double reflected_phase_space_vector_data_[6];
  static const double covariance_matrix_data_[36];
  static const double reflected_covariance_matrix_data_[36];

  static const MAUS::Matrix<double> reflection_polynomial_coefficient_matrix_;
  static const MAUS::PolynomialVector reflection_polynomial_map_;
  static const MAUS::PhaseSpaceVector null_reference_trajectory_;
  static const MAUS::PhaseSpaceVector phase_space_vector_;
  static const MAUS::PhaseSpaceVector reflected_phase_space_vector_;
  static const MAUS::CovarianceMatrix covariance_matrix_;
  static const MAUS::CovarianceMatrix reflected_covariance_matrix_;
  static const MAUS::PolynomialTransferMap reflection_transfer_map_;
};

// ************************************************//
// PolynomialTransferMapTest static const initializations//
// ************************************************//
const double PolynomialTransferMapTest::reflection_polynomial_coefficient_matrix_data_
[42]= {
  0., 0., 0., 0., 0., 0., -1.,
  0., 0., 0., 0., 0., -1., 0.,
  0., 0., 0., 0., -1., 0., 0.,
  0., 0., 0., -1., 0., 0., 0.,
  0., 0., -1., 0., 0., 0., 0.,
  0., -1., 0., 0., 0., 0., 0.
};

const double PolynomialTransferMapTest::null_reference_trajectory_data_[6] = {
  0., 0., 0., 0., 0., 0.
};

const double PolynomialTransferMapTest::phase_space_vector_data_[6] = {
  0., 1., 2., 3., 4., 5.
};

const double PolynomialTransferMapTest::reflected_phase_space_vector_data_[6] = {
  -5., -4., -3., -2., -1., 0.
};

const double PolynomialTransferMapTest::covariance_matrix_data_[36] = {
  0.,  1.,   2.,   3.,   4.,   5.,
  1.,  6.,   7.,   8.,   9.,   10.,
  2.,  7.,   11.,  12.,  13.,  14.,
  3.,  8.,   12.,  15.,  16.,  17.,
  4.,  9.,   13.,  16.,  18.,  19.,
  5.,  10.,  14.,  17.,  19.,  20.
};

const double PolynomialTransferMapTest::reflected_covariance_matrix_data_[36] = {
  20., 19.,  17.,  14.,  10.,  5.,
  19., 18.,  16.,  13.,  9.,   4.,
  17., 16.,  15.,  12.,  8.,   3.,
  14., 13.,  12.,  11.,  7.,   2.,
  10., 9.,   8.,   7.,   6.,   1.,
  5.,  4.,   3.,   2.,   1.,   0.
};

const MAUS::Matrix<double>
PolynomialTransferMapTest::reflection_polynomial_coefficient_matrix_
  = MAUS::Matrix<double>(6, 7,
      PolynomialTransferMapTest::reflection_polynomial_coefficient_matrix_data_);

const MAUS::PolynomialVector PolynomialTransferMapTest::reflection_polynomial_map_
  = MAUS::PolynomialVector(6,
      PolynomialTransferMapTest::reflection_polynomial_coefficient_matrix_);

const MAUS::PhaseSpaceVector PolynomialTransferMapTest::null_reference_trajectory_
  = MAUS::PhaseSpaceVector(
      PolynomialTransferMapTest::null_reference_trajectory_data_);

const MAUS::PhaseSpaceVector PolynomialTransferMapTest::phase_space_vector_
  = MAUS::PhaseSpaceVector(PolynomialTransferMapTest::phase_space_vector_data_);

const MAUS::PhaseSpaceVector PolynomialTransferMapTest::reflected_phase_space_vector_
  = MAUS::PhaseSpaceVector(PolynomialTransferMapTest::
      reflected_phase_space_vector_data_);

const MAUS::CovarianceMatrix PolynomialTransferMapTest::covariance_matrix_
  = MAUS::CovarianceMatrix(PolynomialTransferMapTest::covariance_matrix_data_);

const MAUS::CovarianceMatrix PolynomialTransferMapTest::reflected_covariance_matrix_
  = MAUS::CovarianceMatrix(
      PolynomialTransferMapTest::reflected_covariance_matrix_data_);

const MAUS::PolynomialTransferMap PolynomialTransferMapTest::reflection_transfer_map_
  = MAUS::PolynomialTransferMap(
      PolynomialTransferMapTest::reflection_polynomial_map_,
      PolynomialTransferMapTest::null_reference_trajectory_);

// ***********
// test cases
// ***********

TEST_F(PolynomialTransferMapTest, PhaseSpaceVectorTransport) {
  // test delta-only transport (reference trajectory == transported vector)
  MAUS::PolynomialTransferMap no_delta_map(PolynomialTransferMapTest::reflection_polynomial_map_,
                                 PolynomialTransferMapTest::phase_space_vector_);
  MAUS::PhaseSpaceVector transported_phase_space_vector
    = no_delta_map.Transport(phase_space_vector_);
  EXPECT_TRUE(transported_phase_space_vector == phase_space_vector_);

  // test simple reflection transport of a phase space vector
  transported_phase_space_vector
    = reflection_transfer_map_.Transport(phase_space_vector_);
  EXPECT_TRUE(transported_phase_space_vector == reflected_phase_space_vector_);
}

TEST_F(PolynomialTransferMapTest, Constructors) {
  MAUS::PolynomialTransferMap no_delta_map(PolynomialTransferMapTest::reflection_polynomial_map_,
                                 PolynomialTransferMapTest::phase_space_vector_,
                                 PolynomialTransferMapTest::phase_space_vector_);
  MAUS::PhaseSpaceVector transported_phase_space_vector
    = no_delta_map.Transport(phase_space_vector_);
  EXPECT_TRUE(transported_phase_space_vector == phase_space_vector_);

  MAUS::PolynomialTransferMap no_delta_map_copy(no_delta_map);
  transported_phase_space_vector
    = no_delta_map_copy.Transport(phase_space_vector_);
  EXPECT_TRUE(transported_phase_space_vector == phase_space_vector_);
}

TEST_F(PolynomialTransferMapTest, FirstOrderCovarianceMatrixTransport) {
  // test simple reflection transport a covariance matrix
  MAUS::CovarianceMatrix transported_covariance_matrix
    = reflection_transfer_map_.Transport(covariance_matrix_);
  EXPECT_TRUE(transported_covariance_matrix == reflected_covariance_matrix_);
}
