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

/* Author: Chris Rogers
 */

#include <iostream>
#include <iomanip>
#include <sstream>

#include "gtest/gtest.h"
#include "CLHEP/Matrix/Matrix.h"

#include "Config/ModuleConverter.hh"
#include "Interface/Squeak.hh"
#include "Interface/Differentiator.hh"
#include "Maths/Matrix.hh"
#include "Maths/PolynomialVector.hh"
#include "Maths/Vector.hh"

// NOTE: tests PolynomialVector, Differentiator and
//       PolynomialInterpolator classes.
// Three classes that are very closely related anyway...

using MAUS::PolynomialVector;
using MAUS::Matrix;
using MAUS::Vector;

class PolynomialVectorTest : public ::testing::Test {
 public:
  PolynomialVectorTest()
      : polynomial_map_(NULL), cloned_map_(NULL), extracted_coeff_map_(NULL) {}

  bool verify_mapping(const PolynomialVector* polynomial_map,
                      double point[], double answer[]) {
    bool testpass = true;
    Vector<double> valueMV(3, 0);
    Vector<double> pointMV(2, 0);
    for (int i = 1; i <= 2; i++) pointMV(i) = point[i-1];
    Squeak::mout(Squeak::debug) << "PolyVectorFTest" << std::endl;
    polynomial_map->F(pointMV, valueMV);
    Squeak::mout(Squeak::debug) << "PolyVectorFTest" << std::endl;
    for (int i = 0; i < 3; i++) {
      testpass &= fabs(valueMV(i+1) - answer[i]) < 1e-9;
    }
    for (int i = 0; i < 3; i++) {
      Squeak::mout(Squeak::debug) << valueMV(i+1) << " ";
    }
    Squeak::mout(Squeak::debug) << "Hep F(*, *) " << testpass << std::endl;

    std::vector<double> valueVec(3, -2);
    std::vector<double> pointVec(2, 0);
    for (int i = 0; i < 2; i++) pointVec[i] = point[i];
    polynomial_map->F(&pointVec[0], &valueVec[0]);
    for (int i = 0; i < 3; i++) {
      testpass &= fabs(valueVec[i] - answer[i]) < 1e-9;
    }
    for (int i = 0; i < 3; i++) {
      Squeak::mout(Squeak::debug) << valueVec[i] << " ";
    }
    Squeak::mout(Squeak::debug) << "Vec F(*, *) " << testpass << std::endl;

    return testpass;
  }
 protected:

  virtual void SetUp() {
    CLHEP::HepMatrix coeffHep(3, 6);
    for (size_t i = 0; i < 6; i++) {
      coeffHep[0][i] =  i+1;
    }
    for (size_t i = 0; i < 6; i++) {
      coeffHep[1][i] =  i*i;
    }
    for (size_t i = 0; i < 6; i++) {
      coeffHep[2][i] = -static_cast<int>(i)-1;
    }
    Matrix<double> coefficient_matrix(coeffHep);
    polynomial_map_ = new PolynomialVector(2, coefficient_matrix);

    cloned_map_ = polynomial_map_->Clone();

    std::vector<PolynomialVector::PolynomialCoefficient> coeffs
      = polynomial_map_->GetCoefficientsAsVector();
    extracted_coeff_map_ = new PolynomialVector(coeffs);
  }

  virtual void TearDown() {
    delete polynomial_map_;
    delete cloned_map_;
    delete extracted_coeff_map_;
  }

  PolynomialVector const * polynomial_map_;
  PolynomialVector const * cloned_map_;
  PolynomialVector const * extracted_coeff_map_;
};

TEST_F(PolynomialVectorTest, Constructor) {
  ASSERT_EQ(polynomial_map_->PointDimension(), (size_t) 2);
  ASSERT_EQ(polynomial_map_->ValueDimension(), (size_t) 3);

  ASSERT_EQ(extracted_coeff_map_->PointDimension(), (size_t) 2);
  ASSERT_EQ(extracted_coeff_map_->ValueDimension(), (size_t) 3);
}

TEST_F(PolynomialVectorTest, Clone) {
  delete polynomial_map_;
  polynomial_map_ = NULL;
  ASSERT_EQ(cloned_map_->PointDimension(), (size_t) 2);
  ASSERT_EQ(cloned_map_->ValueDimension(), (size_t) 3);
}

TEST_F(PolynomialVectorTest, MapEvaluation) {
  double point1[2]  = {0, 0};
  double answer1[3] = {1, 0, -1};
  ASSERT_TRUE(verify_mapping(cloned_map_, point1, answer1));

  ASSERT_TRUE(verify_mapping(extracted_coeff_map_, point1, answer1));


  double point2[2]  = {3, -2};  // {1, 3, -2, 9, -6, 4}
  double answer2[3] = {31, 80, -31};
  ASSERT_TRUE(verify_mapping(cloned_map_, point2, answer2));

  ASSERT_TRUE(verify_mapping(extracted_coeff_map_, point2, answer2));


  for (int i = 0; i < 5; i++) {
    Squeak::mout(Squeak::debug)
      << "nPCoeffs(3, " << i << ") "
      << PolynomialVector::NumberOfPolynomialCoefficients(3, i) << std::endl;
  }

  // FIXME(plane1@iit.edu): No actual test here. Just some debug output.
  for (int i = 0; i < 20; i++) {
    std::vector<int> indexP = PolynomialVector::IndexByPower(i, 3);
    std::vector<int> indexV = PolynomialVector::IndexByVector(i, 3);

    Squeak::mout(Squeak::debug) << std::setw(5) << i << "   *|* ";
    for (size_t j = 0; j < indexP.size(); j++) {
      Squeak::mout(Squeak::debug) << std::setw(5) << indexP[j] << " ";
    }
    Squeak::mout(Squeak::debug) << "  *|*  ";
    for (size_t j = 0; j < indexV.size(); j++) {
      Squeak::mout(Squeak::debug) << std::setw(5) << indexV[j] << " ";
    }
    Squeak::mout(Squeak::debug) << std::endl;
  }
}

TEST_F(PolynomialVectorTest, IterableEquality) {
  std::vector<int> a;
  std::vector<int> b;
  ASSERT_TRUE(PolynomialVector::IterableEquality(
    a, b, a.begin(), a.end(), b.begin(), b.end()));
  a.push_back(-2);
  ASSERT_FALSE(PolynomialVector::IterableEquality(
    a, b, a.begin(), a.end(), b.begin(), b.end()));
  b.push_back(-2);

  ASSERT_TRUE(PolynomialVector::IterableEquality(
    a, b, a.begin(), a.end(), b.begin(), b.end()));
  b.push_back(-2);
  ASSERT_FALSE(PolynomialVector::IterableEquality(
    a, b, a.begin(), a.end(), b.begin(), b.end()));
}

TEST_F(PolynomialVectorTest, PointBox) {
  double deltaA[] = {2., 3., 4., 5.};
  std::vector<double> delta(deltaA, deltaA+4);
  std::vector<std::vector<double> > ps = PolynomialVector::PointBox(delta, 6);
  for (size_t i = 0; i < ps.size(); i++) {
    bool on_edge = false;
    for (size_t j = 0; j < ps[i].size(); j++) {
      // at least one of ps[i] must be == +- delta for it to be on edge
      on_edge = on_edge || fabs(delta[j]-fabs(ps[i][j])) < 1e-9;
    }
    if (!on_edge) {
      for (size_t j = 0; j < ps[i].size(); j++) {
        Squeak::mout(Squeak::debug) << ps[i][j] << " ";
      }
    }
    ASSERT_TRUE(on_edge);
  }
}

TEST_F(PolynomialVectorTest, GetAvgChi2OfDifference) {
  double mat_data[] = {1, 2, 7, 13, 5, 0,
                       4, 3, 11, 3, 7, 9,
                       1, 2, 7, 13, 8, 2};
  Matrix<double>  mat(3, 6, mat_data);
  PolynomialVector pvec(2, mat);
  std::vector< std::vector<double> > in;
  std::vector< std::vector<double> > out;
  std::vector< std::vector<double> > out_neg;

  bool testpass = true;
  try {
    pvec.GetAvgChi2OfDifference(in, out);
    testpass = false;
  } catch (Squeal squee) {}
  ASSERT_TRUE(testpass);

  for (int i = 0; i < 10; i++) {
    in.push_back(std::vector<double>(2, i));
    in.back()[1] = i*2;
    out    .push_back(std::vector<double>(3, 0));
    out_neg.push_back(std::vector<double>(3, 0));
    pvec.F(&in.back()[0], &out.back()[0]);
    for (int i = 0; i < 3; i++) {
      out_neg.back()[i] = -out.back()[i];
    }
  }

  double avg_chi2 = pvec.GetAvgChi2OfDifference(in, out);
  ASSERT_TRUE(fabs(avg_chi2) < 1e-6);

  avg_chi2 = pvec.GetAvgChi2OfDifference(in, out_neg);
  ASSERT_TRUE(fabs(avg_chi2/1e14 - 4.90542) < 1e-3);  // Hope that is right
}

TEST_F(PolynomialVectorTest, Means) {
  std::vector<std::vector<double> > values;
  values.push_back(std::vector<double>());
  for (size_t i = 1; i <= 3; i++) values.back().push_back(i);
  values.push_back(std::vector<double>());
  for (size_t i = 1; i <= 3; i++) {
    values.back().push_back(-static_cast<double>(i));
  }
  values.push_back(std::vector<double>());
  for (size_t i = 1; i <= 3; i++) values.back().push_back(i*i);
  std::vector<double> weights1(3, 1.);
  std::vector<double> weights2(3, 1.);
  weights2[2] = 0.;

  Vector<double> v1 = PolynomialVector::Means(values, std::vector<double>());
  Vector<double> v2 = PolynomialVector::Means(values, weights1);
  Vector<double> v3 = PolynomialVector::Means(values, weights2);

  for (size_t i = 1; i <= 3; i++) {
    EXPECT_TRUE(fabs(v1(i)-static_cast<double>(i*i)/3.) < 1e-6);
    EXPECT_TRUE(fabs(v2(i)-static_cast<double>(i*i)/3.) < 1e-6);
    EXPECT_TRUE(fabs(v3(i)) < 1e-6);
  }
}

TEST_F(PolynomialVectorTest, Covariances) {
  std::vector<std::vector<double> > values;
  values.push_back(std::vector<double>());
  for (size_t i = 0; i < 3; i++) values.back().push_back(1);
  values.push_back(std::vector<double>());
  for (size_t i = 0; i < 3; i++) values.back().push_back(1);
  values.push_back(std::vector<double>());
  for (size_t i = 0; i < 3; i++) values.back().push_back(-1);
  values.push_back(std::vector<double>());
  for (size_t i = 0; i < 3; i++) values.back().push_back(-1);

  // empty weights -> PolynomialVector defaults to weights being all 1.
  std::vector<double> weights0;
  // empty means -> PolynomialVector defaults to means calculated from values
  Vector<double> means0;
  std::vector<double> weights1(4, 1.);
  Vector<double> means1(3, 0.);
  std::vector<double> weights2(4, 1.);
  weights2[0] = 0.;
  weights2[1] = 0.;
  Vector<double> means2(3, -1.);

  Matrix<double> m1 = PolynomialVector::Covariances(values, weights0, means0);
  Matrix<double> m2 = PolynomialVector::Covariances(values, weights1, means1);
  Matrix<double> m3 = PolynomialVector::Covariances(values, weights2, means2);

  EXPECT_TRUE(fabs(determinant(m1-Matrix<double>(3, 3, 1.))) < 1e-6);
  EXPECT_TRUE(fabs(determinant(m2-Matrix<double>(3, 3, 1.))) < 1e-6);
  EXPECT_TRUE(fabs(determinant(m3)) < 1e-6);
}

TEST_F(PolynomialVectorTest, LeastSquaresFitting) {
  Squeak::mout(Squeak::debug) << "PolynomialLeastSquaresTest" << std::endl;
  int               nX   = 4;
  int               nY   = 4;
  int               nZ   = 4;
  CLHEP::HepMatrix mat(3, 4, 0);
  mat[0][0] = 1.;
  mat[1][0] = 4.;
  mat[2][0] = 1.;
  mat[0][1] = 2.;
  mat[1][1] = 3.;
  mat[2][1] = 2.;
  mat[0][2] = 7.;
  mat[1][2] = 11.;
  mat[2][2] = 7.;
  mat[0][3] = 13.;
  mat[1][3] = 3.;
  mat[2][3] = 13.;

  PolynomialVector* vecF = new PolynomialVector(3, Matrix<double>(mat));
  std::vector< std::vector<double> > points(nX*nY*nZ, std::vector<double>(3));
  std::vector< std::vector<double> > values(nX*nY*nZ, std::vector<double>(3));
  std::vector<double>                weights(nX*nY*nZ, 1);
  for (int i = 0; i < nX; i++)
    for (int j = 0; j < nY; j++)
      for (int k = 0; k < nZ; k++) {
        int a = k+j*nZ + i*nY*nZ;
        points[a][0] = i/static_cast<double>(nX)*105.;
        points[a][1] = j/static_cast<double>(nY)*201.;
        points[a][2] = k/static_cast<double>(nZ)*105.;
        vecF->F(&points[a][0], &values[a][0]);
      }
  delete vecF;
  PolynomialVector* pVec
    = PolynomialVector::PolynomialLeastSquaresFit(points, values, 2);
  CLHEP::HepMatrix recCoeff
    = MAUS::CLHEP::HepMatrix(pVec->GetCoefficientsAsMatrix());

  bool testpass = true;
  Squeak::mout(Squeak::debug) << "Input" << mat << "Output" << recCoeff
                              << std::endl;
  for (int i = 0; i < recCoeff.num_row(); i++)
    for (int j = 0; j < recCoeff.num_col(); j++)
      if (fabs(recCoeff[i][j] - mat[i][j]) > 1e-6) testpass = false;
  delete pVec;
  EXPECT_TRUE(testpass);

  testpass = true;
  // now add an outlier with 0 weight - try weighted fit
  points .push_back(std::vector<double>(3, 92.));
  values .push_back(std::vector<double>(3, 17.));
  weights.push_back(0.);
  pVec = PolynomialVector::PolynomialLeastSquaresFit(points, values,
                                                     2, weights);
  recCoeff = MAUS::CLHEP::HepMatrix(pVec->GetCoefficientsAsMatrix());
  Squeak::mout(Squeak::debug) << "Weighted Input" << mat << "Weighted Output"
                              << recCoeff << std::endl;
  for (int i = 0; i < recCoeff.num_row(); i++)
    for (int j = 0; j < recCoeff.num_col(); j++)
      if (fabs(recCoeff[i][j] - mat[i][j]) > 1e-6) testpass = false;
  delete pVec;
  EXPECT_TRUE(testpass);

  testpass = true;
  // now take some of the input values, try for a constrained fit
  PolynomialVector Constraint(2, Matrix<double>(mat.sub(1, 2, 1, 3)));
  std::vector<PolynomialVector::PolynomialCoefficient> coeffs
    = Constraint.GetCoefficientsAsVector();
  for (int i = 0; i < 2; i++)
    coeffs.erase(coeffs.begin());
  PolynomialVector* constraintPVec = new PolynomialVector(coeffs);
  pVec = PolynomialVector::ConstrainedPolynomialLeastSquaresFit(
    points, values, 2, constraintPVec->GetCoefficientsAsVector(), weights);
  recCoeff = MAUS::CLHEP::HepMatrix(pVec->GetCoefficientsAsMatrix());
  Squeak::mout(Squeak::debug) << "Constrained Input\n" << *constraintPVec
                              << "Constrained Output\n" << *pVec << std::endl;
  for (int i = 0; i < recCoeff.num_row(); i++)
    for (int j = 0; j < recCoeff.num_col(); j++)
      if (fabs(recCoeff[i][j] - mat[i][j]) > 1e-6) testpass = false;
  delete pVec;
  EXPECT_TRUE(testpass);

  testpass = true;
  // should return a copy of
  double mat2[] = {1., 2., 7., 13., 200., 500., 800., 1100., 1400., 1700.,
                   1., 3., 8., 14., 300., 600., 900., 1200., 1500., 1800.,
                   1., 4., 9., 15., 400., 700., 1000., 1300., 1600., 1900.};
  Squeak::mout(Squeak::debug) << "Chi2SweepingLeastSquaresFit" << std::endl;
  PolynomialVector* testF
    = new PolynomialVector(3, Matrix<double>(3, 10, mat2));
  std::vector<double> delta(3);
  delta[0] = 1e-50;
  delta[1] = 1e-50;
  delta[2] = 1e-50;
  // check we can make a polynomial vector at all
  pVec  = PolynomialVector::Chi2SweepingLeastSquaresFit(
            *testF, 5, std::vector< PolynomialVector::PolynomialCoefficient >(),
            1e-40, delta, 10., 100);
  if (pVec == NULL) {
    testpass = false;
    Squeak::mout(Squeak::debug) << "Failed to make PolynomialVector when "
                                << "PolynomialVector expected " << pVec
                                << std::endl;
  } else {
    Matrix<double> o1 = pVec ->GetCoefficientsAsMatrix();
    Matrix<double> o2 = testF->GetCoefficientsAsMatrix();
    testpass &= o1.number_of_rows() == o2.number_of_rows()
             && o1.number_of_columns() == o2.number_of_columns();
    for (size_t i = 1; i <= o2.number_of_rows(); i++)
      for (size_t j = 1; j <= o2.number_of_columns(); j++)
        testpass &= fabs(o1(i, j) - o2(i, j)) < 1e-2;
    Squeak::mout(Squeak::debug) << "Input should be same as output\nInput\n"
                                << *testF
                                << "Output\n" << *pVec << " testpass "
                                << testpass << std::endl;
  }

  if (pVec != NULL) delete pVec;
  EXPECT_TRUE(testpass);


  testpass = true;
  // check we can get a decent fit at lower order to a higher order polynomial
  delta[0] = 1e-50;
  delta[1] = 1e-50;
  delta[2] = 1e-50;
  pVec  = PolynomialVector::Chi2SweepingLeastSquaresFit(
    *testF, 2, std::vector< PolynomialVector::PolynomialCoefficient >(),
    1e-40, delta, 10., 100);
  if (pVec == NULL) {
    testpass = false;
    Squeak::mout(Squeak::debug) << "Failed to make PolynomialVector when "
                                << "PolynomialVector expected " << pVec
                                << std::endl;
  } else {
    Matrix<double> o1 = pVec ->GetCoefficientsAsMatrix();
    Matrix<double> o2 = testF->GetCoefficientsAsMatrix();
    testpass &= o1.number_of_rows() == o2.number_of_rows()
             && o1.number_of_columns() == 4;
    for (size_t i = 1; i <= o1.number_of_rows(); i++)
      for (size_t j = 1; j <= o1.number_of_columns(); j++)
        testpass &= fabs(o1(i, j) - o2(i, j)) < 1e-6;
    Squeak::mout(Squeak::debug) << "Input should be same as output\nInput\n"
                                << *testF << "Output\n" << *pVec
                                << " testpass " << testpass << std::endl;
  }
  EXPECT_TRUE(testpass);

  testpass = true;
  // test that I return NULL if I can't converge; for these parameters
  // floating point precision should mean that I don't converge.
  delta[0] = 1e6;
  delta[1] = 1e6;
  delta[2] = 1e6;
  pVec  = PolynomialVector::Chi2SweepingLeastSquaresFit(
    *testF, 5, std::vector< PolynomialVector::PolynomialCoefficient >(),
    1e-60, delta, 10., 100);
  if (pVec != NULL)
    testpass = false;
  Squeak::mout(Squeak::debug) << "Should be NULL " << pVec << " testpass "
                              << testpass << std::endl;
  EXPECT_TRUE(testpass);

  testpass = true;
  // the point of mat3 is that it introduces an effective unit into each
  // dimension; so x_0 has units like 1., x_1 has units like 1e6 and x_2 has
  // units like 1e3 validity region (delta at output) should reflect the units
  double deltaMaxA[] = {1e12, 1e12, 1e12, 1e12};
  std::vector<double> deltaMax(deltaMaxA, deltaMaxA+3);
  double mat3[] = {
    1., 2., 7., 13.e3, 200., 500.e6, 800.e3, 1100.e12, 1400.e9, 1700.e6,
    1., 3., 8.e6, 14.e3, 300., 600.e6, 900.e3, 1200.e12, 1500.e9, 1800.e6,
    1., 4., 9.e6, 15.e3, 400., 700.e6, 1000.e3, 1300.e12, 1600.e9, 1900.e6};
  PolynomialVector* testF2
    = new PolynomialVector(3, Matrix<double>(3, 10, mat3));
  Squeak::mout(Squeak::debug) << "Chi2SweepingLeastSquaresFitVariableWalls"
                              << std::endl;
  // delta.push_back(0.);
  delta[0] = 1e-50;
  delta[1] = 1e-50;
  delta[2] = 1e-50;
  pVec  = PolynomialVector::Chi2SweepingLeastSquaresFitVariableWalls(
    *testF2, 3, std::vector< PolynomialVector::PolynomialCoefficient >(),
    1e-20, delta, 10., 100, deltaMax);
  Squeak::mout(Squeak::debug) << "delta variable walls: ";
  for (size_t i = 0; i < delta.size(); i++) {
    Squeak::mout(Squeak::debug) << delta[i] << " ";
  }
  Squeak::mout(Squeak::debug) << std::endl;
  delta[0] = 1e-50;
  delta[1] = 1e-50;
  delta[2] = 1e-50;
  PolynomialVector* pVec2 = PolynomialVector::Chi2SweepingLeastSquaresFit(
    *testF2, 3, std::vector< PolynomialVector::PolynomialCoefficient >(),
    1e-20, delta, 10., 100);
  Squeak::mout(Squeak::debug) << "delta fixed walls:    ";
  for (size_t i = 0; i < delta.size(); i++) {
    Squeak::mout(Squeak::debug) << delta[i] << " ";
  }
  Squeak::mout(Squeak::debug) << std::endl;
  if (pVec == NULL) {
    Squeak::mout(Squeak::debug) << "Error - "
                                << "Chi2SweepingLeastSquaresFitVariableWalls "
                                << "returns " << pVec << std::endl;
    testpass = false;
  } else {
    Squeak::mout(Squeak::debug) << "Input should be same as output\nInput\n"
                                << *testF2
                                << "Output\n" << *pVec
                                << "For comparison, using normal algorithm\n"
                                << *pVec2 << std::endl;
    Matrix<double> o1 = pVec  ->GetCoefficientsAsMatrix();
    Matrix<double> o2 = testF2->GetCoefficientsAsMatrix();
    testpass &= o1.number_of_rows() == o2.number_of_rows()
             && o1.number_of_columns() == o2.number_of_columns();
    for (size_t i = 1;
         i <= o1.number_of_rows() && i <= o2.number_of_rows();
         i++) {
      for (size_t j = 1;
           j <= o1.number_of_columns() && j <= o2.number_of_columns();
           j++) {
        testpass
          &= fabs(o1(i, j) - o2(i, j))/fabs(o1(i, j) + o2(i, j)) < 1.e-3
          || fabs(o1(i, j) + o2(i, j)) < 1e-9;
      }
    }
  }
  if (pVec != NULL) delete pVec;
  Squeak::mout(Squeak::debug) << "testpass " << testpass << std::endl;
  EXPECT_TRUE(testpass);

  testpass = true;
  // test that I return NULL if I can't converge; for these parameters floating
  // point precision should mean that I don't converge.
  delta[0] = 1e6;
  delta[1] = 1e6;
  delta[2] = 1e6;
  pVec  = PolynomialVector::Chi2SweepingLeastSquaresFitVariableWalls(
    *testF2, 5, std::vector< PolynomialVector::PolynomialCoefficient >(),
    1e-60, delta, 10., 100, deltaMax);
  if (pVec != NULL)
    testpass = false;
  Squeak::mout(Squeak::debug) << "Should be NULL " << pVec << std::endl;

  delete testF;
  Squeak::mout(Squeak::debug) << "PolynomialLeastSquaresTest " << *testF2
                              << testpass << std::endl;
  EXPECT_TRUE(testpass);
}
