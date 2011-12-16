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

class TransferMapTestData
{
public:
  TransferMapTestData()
  {
    double element = 0.;
    for (int row=0; row<6; ++row)
    {
      for (int column=0; column<=row; ++column)
      {
        CovarianceMatrix_[row][column] = element;
        
        if (row == (size_t) 1)
        {
          PhaseSpaceVector_[column] = element;
          ReflectedPhaseSpaceVector_[6-column] = -element;
        }
        
        if (column == (row+1))
        {
          ReflectionPolynomialCoefficientMatrix_[row][column] = -1.;
        }
        else
        {
          ReflectionPolynomialCoefficientMatrix_[row][column] = 0.;
        }

        element  += 1.0;
      }
    }
  }

  static double ReflectionPolynomialCoefficientMatrix_[6][7];
  static double CovarianceMatrix_[6][6];
  static double PhaseSpaceVector_[6];
  static double ReflectedPhaseSpaceVector_[6];
};
double TransferMapTestData::ReflectionPolynomialCoefficientMatrix_[6][7] = {
  {0., 0., 0., 0., 0., 0., 0.},
  {0., 0., 0., 0., 0., 0., 0.},
  {0., 0., 0., 0., 0., 0., 0.},
  {0., 0., 0., 0., 0., 0., 0.},
  {0., 0., 0., 0., 0., 0., 0.},
  {0., 0., 0., 0., 0., 0., 0.}
};
double TransferMapTestData::CovarianceMatrix_[6][6] = {
  {0., 0., 0., 0., 0., 0.},
  {0., 0., 0., 0., 0., 0.},
  {0., 0., 0., 0., 0., 0.},
  {0., 0., 0., 0., 0., 0.},
  {0., 0., 0., 0., 0., 0.},
  {0., 0., 0., 0., 0., 0.}
};
double TransferMapTestData::PhaseSpaceVector_[6] = {
  0., 0., 0., 0., 0., 0.
};
double TransferMapTestData::ReflectedPhaseSpaceVector_[6] = {
  0., 0., 0., 0., 0., 0.
};

class TransferMapTest : public ::testing::Test
{
public:
  TransferMapTest()
  {
  }

protected:

  static const MAUS::Matrix<double> reflection_polynomial_coefficient_matrix_;
  static const MAUS::PolynomialVector reflection_polynomial_map_;
  static const MAUS::CovarianceMatrix covariance_matrix_;
  static const MAUS::PhaseSpaceVector phase_space_vector_;
  static const MAUS::PhaseSpaceVector reflected_phase_space_vector_;
  static const MAUS::TransferMap reflection_transfer_map_;
};

//************************************************//
//TransferMapTest static const initializations//
//************************************************//

const MAUS::Matrix<double>
TransferMapTest::reflection_polynomial_coefficient_matrix_
  = MAUS::Matrix<double>(6, 7,
      (double*) TransferMapTestData::ReflectionPolynomialCoefficientMatrix_);

const MAUS::PolynomialVector TransferMapTest::reflection_polynomial_map_
  = MAUS::PolynomialVector(6,
      TransferMapTest::reflection_polynomial_coefficient_matrix_);

const MAUS::CovarianceMatrix TransferMapTest::covariance_matrix_
  = MAUS::CovarianceMatrix((double*) TransferMapTestData::CovarianceMatrix_);

const MAUS::PhaseSpaceVector TransferMapTest::phase_space_vector_
  = MAUS::PhaseSpaceVector((double*) TransferMapTestData::PhaseSpaceVector_);

const MAUS::PhaseSpaceVector TransferMapTest::reflected_phase_space_vector_
  = MAUS::PhaseSpaceVector(
    (double*) TransferMapTestData::ReflectedPhaseSpaceVector_);

const MAUS::TransferMap TransferMapTest::reflection_transfer_map_
  = MAUS::TransferMap(
      TransferMapTest::reflection_polynomial_map_,
      TransferMapTest::phase_space_vector_);

//***********
//test cases
//***********

TEST_F(TransferMapTest, CopyConstructor) {
  MAUS::PhaseSpaceVector transported_phase_space_vector
    = reflection_transfer_map_ * phase_space_vector_;
  EXPECT_TRUE(transported_phase_space_vector == reflected_phase_space_vector_);
}
