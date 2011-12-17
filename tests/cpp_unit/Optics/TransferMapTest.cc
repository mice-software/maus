// Copyright 2010 Chris Rogers
//
//This file is a part of G4MICE
//
//G4MICE is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//G4MICE is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with G4MICE in the doc folder.  If not, see 
//<http://www.gnu.org/licenses/>.


#include <limits>
#include <math.h>
#include <iostream>
#include <sstream>
#include <fstream>

#include "Interface/Matrix.hh"
#include "Optics/CovarianceMatrix.hh"
#include "Optics/PhaseSpaceVector.hh"
#include "Optics/TransferMap.hh"

#include "gtest/gtest.h" 

class TransferMapTest : public ::testing::Test
{
public:
  TransferMapTest()
  {
  }

protected:

  static const double reflection_polynomial_coefficient_matrix_data_[6][7];
  static const double null_reference_trajectory_data_[6];
  static const double phase_space_vector_data_[6];
  static const double reflected_phase_space_vector_data_[6];
  static const double covariance_matrix_data_[6][6];
  static const double reflected_covariance_matrix_data_[6][6];

  static const MAUS::Matrix<double> reflection_polynomial_coefficient_matrix_;
  static const MAUS::PolynomialVector reflection_polynomial_map_;
  static const MAUS::PhaseSpaceVector null_reference_trajectory_;
  static const MAUS::PhaseSpaceVector phase_space_vector_;
  static const MAUS::PhaseSpaceVector reflected_phase_space_vector_;
  static const MAUS::CovarianceMatrix covariance_matrix_;
  static const MAUS::CovarianceMatrix reflected_covariance_matrix_;
  static const MAUS::TransferMap reflection_transfer_map_;
};

//************************************************//
//TransferMapTest static const initializations//
//************************************************//
const double TransferMapTest::reflection_polynomial_coefficient_matrix_data_
[6][7] = {
  {0., 0., 0., 0., 0., 0., -1.},
  {0., 0., 0., 0., 0., -1.,0.},
  {0., 0., 0., 0., -1.,0., 0.},
  {0., 0., 0., -1.,0., 0., 0.},
  {0., 0., -1.,0., 0., 0., 0.},
  {0., -1.,0., 0., 0., 0., 0.}
};
const double TransferMapTest::null_reference_trajectory_data_[6] = {
  0., 0., 0., 0., 0., 0.
};
const double TransferMapTest::phase_space_vector_data_[6] = {
  0., 1., 2., 3., 4., 5.
};
const double TransferMapTest::reflected_phase_space_vector_data_[6] = {
  -5., -4., -3., -2., -1., 0.
};
const double TransferMapTest::covariance_matrix_data_[6][6] = {
  {0.,  1.,   2.,   3.,   4.,   5.},
  {1.,  6.,   7.,   8.,   9.,   10.},
  {2.,  7.,   11.,  12.,  13.,  14.},
  {3.,  8.,   12.,  15.,  16.,  17.},
  {4.,  9.,   13.,  16.,  18.,  19.},
  {5.,  10.,  14.,  17.,  19.,  20.}
};
const double TransferMapTest::reflected_covariance_matrix_data_[6][6] = {
  {20., 19.,  17.,  14.,  10.,  5.},
  {19., 18.,  16.,  13.,  9.,   4.},
  {17., 16.,  15.,  12.,  8.,   3.},
  {14., 13.,  12.,  11.,  7.,   2.},
  {10., 9.,   8.,   7.,   6.,   1.},
  {5.,  4.,   3.,   2.,   1.,   0.}
};

const MAUS::Matrix<double>
TransferMapTest::reflection_polynomial_coefficient_matrix_
  = MAUS::Matrix<double>(6, 7,
      (double*) TransferMapTest::
        reflection_polynomial_coefficient_matrix_data_);

const MAUS::PolynomialVector TransferMapTest::reflection_polynomial_map_
  = MAUS::PolynomialVector(6,
      TransferMapTest::reflection_polynomial_coefficient_matrix_);

const MAUS::PhaseSpaceVector TransferMapTest::null_reference_trajectory_
  = MAUS::PhaseSpaceVector((double*) TransferMapTest::
      null_reference_trajectory_data_);

const MAUS::PhaseSpaceVector TransferMapTest::phase_space_vector_
  = MAUS::PhaseSpaceVector((double*) TransferMapTest::phase_space_vector_data_);

const MAUS::PhaseSpaceVector TransferMapTest::reflected_phase_space_vector_
  = MAUS::PhaseSpaceVector(
    (double*) TransferMapTest::reflected_phase_space_vector_data_);

const MAUS::CovarianceMatrix TransferMapTest::covariance_matrix_
  = MAUS::CovarianceMatrix((double*) TransferMapTest::covariance_matrix_data_);

const MAUS::CovarianceMatrix TransferMapTest::reflected_covariance_matrix_
  = MAUS::CovarianceMatrix((double*) TransferMapTest::
      reflected_covariance_matrix_data_);

const MAUS::TransferMap TransferMapTest::reflection_transfer_map_
  = MAUS::TransferMap(
      TransferMapTest::reflection_polynomial_map_,
      TransferMapTest::null_reference_trajectory_);

//***********
//test cases
//***********

TEST_F(TransferMapTest, PhaseSpaceVectorTransport) {
  //test delta-only transport (reference trajectory == transported vector)
  MAUS::TransferMap no_delta_map(TransferMapTest::reflection_polynomial_map_,
                                 TransferMapTest::phase_space_vector_);
  MAUS::PhaseSpaceVector transported_phase_space_vector
    = no_delta_map * phase_space_vector_;
  EXPECT_TRUE(transported_phase_space_vector == phase_space_vector_);

  //test simple reflection transport of a phase space vector
  transported_phase_space_vector
    = reflection_transfer_map_ * phase_space_vector_;
  EXPECT_TRUE(transported_phase_space_vector == reflected_phase_space_vector_);
}

TEST_F(TransferMapTest, FirstOrderCovarianceMatrixTransport) {
  //test simple reflection transport a covariance matrix
  MAUS::CovarianceMatrix transported_covariance_matrix
    = reflection_transfer_map_ * covariance_matrix_;
  EXPECT_TRUE(transported_covariance_matrix == reflected_covariance_matrix_);
}
