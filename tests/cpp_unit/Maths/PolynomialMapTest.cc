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
#include "Utils/Squeak.hh"
#include "Interface/Differentiator.hh"
#include "Interface/Interpolator.hh"
#include "Maths/Matrix.hh"
#include "Maths/PolynomialMap.hh"
#include "Maths/Vector.hh"

// NOTE: tests PolynomialMap, Differentiator and
//       PolynomialInterpolator classes.
// Three classes that are very closely related anyway...

using MAUS::PolynomialMap;
using MAUS::Matrix;
using MAUS::Vector;

class PolynomialMapTest : public ::testing::Test {
 public:
  PolynomialMapTest()
      : polynomial_map_(NULL), cloned_map_(NULL), extracted_coeff_map_(NULL) {}

  bool verify_mapping(const PolynomialMap* polynomial_map,
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

  // size of point is 3
  static void weight_function(const double * point, double * weight) {
    int hit_count = 0;
    for (int index = 0; index < 3; ++index) {
      if (point[index] == 92.) {
        ++hit_count;
      }
    }
    if (hit_count == 3) {
      *weight = 0.0;
    } else {
      *weight = 1.0;
    }
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
    polynomial_map_ = new PolynomialMap(2, coefficient_matrix);

    cloned_map_ = polynomial_map_->Clone();

    std::vector<PolynomialMap::PolynomialCoefficient> coeffs
      = polynomial_map_->GetCoefficientsAsVector();
    extracted_coeff_map_ = new PolynomialMap(coeffs);
  }

  virtual void TearDown() {
    delete polynomial_map_;
    delete cloned_map_;
    delete extracted_coeff_map_;
  }

  PolynomialMap const * polynomial_map_;
  PolynomialMap const * cloned_map_;
  PolynomialMap const * extracted_coeff_map_;
};

TEST_F(PolynomialMapTest, Constructor) {
  ASSERT_EQ(polynomial_map_->PointDimension(), (size_t) 2);
  ASSERT_EQ(polynomial_map_->ValueDimension(), (size_t) 3);

  ASSERT_EQ(extracted_coeff_map_->PointDimension(), (size_t) 2);
  ASSERT_EQ(extracted_coeff_map_->ValueDimension(), (size_t) 3);
}

TEST_F(PolynomialMapTest, Clone) {
  delete polynomial_map_;
  polynomial_map_ = NULL;
  ASSERT_EQ(cloned_map_->PointDimension(), (size_t) 2);
  ASSERT_EQ(cloned_map_->ValueDimension(), (size_t) 3);
}

TEST_F(PolynomialMapTest, MapEvaluation) {
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
      << PolynomialMap::NumberOfPolynomialCoefficients(3, i) << std::endl;
  }

  // FIXME(plane1@iit.edu): No actual test here. Just some debug output.
  for (int i = 0; i < 20; i++) {
    std::vector<int> indexP = PolynomialMap::IndexByPower(i, 3);
    std::vector<int> indexV = PolynomialMap::IndexByVector(i, 3);

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

TEST_F(PolynomialMapTest, IterableEquality) {
  std::vector<int> a;
  std::vector<int> b;
  ASSERT_TRUE(PolynomialMap::IterableEquality(
    a, b, a.begin(), a.end(), b.begin(), b.end()));
  a.push_back(-2);
  ASSERT_FALSE(PolynomialMap::IterableEquality(
    a, b, a.begin(), a.end(), b.begin(), b.end()));
  b.push_back(-2);

  ASSERT_TRUE(PolynomialMap::IterableEquality(
    a, b, a.begin(), a.end(), b.begin(), b.end()));
  b.push_back(-2);
  ASSERT_FALSE(PolynomialMap::IterableEquality(
    a, b, a.begin(), a.end(), b.begin(), b.end()));
}

TEST_F(PolynomialMapTest, PointBox) {
  double deltaA[] = {2., 3., 4., 5.};
  std::vector<double> delta(deltaA, deltaA+4);
  std::vector<std::vector<double> > ps = PolynomialMap::PointBox(delta, 6);
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

TEST_F(PolynomialMapTest, GetAvgChi2OfDifference) {
  double mat_data[] = {1, 2, 7, 13, 5, 0,
                       4, 3, 11, 3, 7, 9,
                       1, 2, 7, 13, 8, 2};
  Matrix<double>  mat(3, 6, mat_data);
  PolynomialMap pvec(2, mat);
  std::vector< std::vector<double> > in;
  std::vector< std::vector<double> > out;
  std::vector< std::vector<double> > out_neg;

  // empty in vector
  bool testpass = true;
  try {
    pvec.GetAvgChi2OfDifference(in, out);
    testpass = false;
  } catch (Exceptions::Exception exc) {}
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

  // in/out size mismatch
  in.pop_back();
  testpass = true;
  try {
    pvec.GetAvgChi2OfDifference(in, out);
    testpass = false;
  } catch (Exceptions::Exception exc) {}
  ASSERT_TRUE(testpass);

  // in[i] size != point dimension
  out.pop_back();
  in[0].pop_back();
  testpass = true;
  try {
    pvec.GetAvgChi2OfDifference(in, out);
    testpass = false;
  } catch (Exceptions::Exception exc) {}
  ASSERT_TRUE(testpass);

  // out[i] size != value dimension
  in.push_back(std::vector<double>(2, 0.0));
  in[0].push_back(0.0);
  out[0].pop_back();
  testpass = true;
  try {
    pvec.GetAvgChi2OfDifference(in, out);
    testpass = false;
  } catch (Exceptions::Exception exc) {}
  ASSERT_TRUE(testpass);
}

TEST_F(PolynomialMapTest, Means) {
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

  Vector<double> v1 = PolynomialMap::Means(values, std::vector<double>());
  Vector<double> v2 = PolynomialMap::Means(values, weights1);
  Vector<double> v3 = PolynomialMap::Means(values, weights2);

  for (size_t i = 1; i <= 3; i++) {
    EXPECT_TRUE(fabs(v1(i)-static_cast<double>(i*i)/3.) < 1e-6);
    EXPECT_TRUE(fabs(v2(i)-static_cast<double>(i*i)/3.) < 1e-6);
    EXPECT_TRUE(fabs(v3(i)) < 1e-6);
  }
}

TEST_F(PolynomialMapTest, Covariances) {
  std::vector<std::vector<double> > values;
  values.push_back(std::vector<double>());
  for (size_t i = 0; i < 3; i++) values.back().push_back(1);
  values.push_back(std::vector<double>());
  for (size_t i = 0; i < 3; i++) values.back().push_back(1);
  values.push_back(std::vector<double>());
  for (size_t i = 0; i < 3; i++) values.back().push_back(-1);
  values.push_back(std::vector<double>());
  for (size_t i = 0; i < 3; i++) values.back().push_back(-1);

  // empty weights -> PolynomialMap defaults to weights being all 1.
  std::vector<double> weights0;
  // empty means -> PolynomialMap defaults to means calculated from values
  Vector<double> means0;
  std::vector<double> weights1(4, 1.);
  Vector<double> means1(3, 0.);
  std::vector<double> weights2(4, 1.);
  weights2[0] = 0.;
  weights2[1] = 0.;
  Vector<double> means2(3, -1.);

  Matrix<double> m1 = PolynomialMap::Covariances(values, weights0, means0);
  Matrix<double> m2 = PolynomialMap::Covariances(values, weights1, means1);
  Matrix<double> m3 = PolynomialMap::Covariances(values, weights2, means2);

  EXPECT_TRUE(fabs(determinant(m1-Matrix<double>(3, 3, 1.))) < 1e-6);
  EXPECT_TRUE(fabs(determinant(m2-Matrix<double>(3, 3, 1.))) < 1e-6);
  EXPECT_TRUE(fabs(determinant(m3)) < 1e-6);
}


TEST_F(PolynomialMapTest, LeastSquaresFittingErrorHandling) {
  // Check that the error matrix term does *something* (anything) and handles
  // correctly bad input (throws)
  // Maths is checked in python tests... as it requires high stats
  Squeak::mout(Squeak::debug) << "PolynomialLeastSquaresTest" << std::endl;
  int               nX   = 4;
  int               nY   = 4;
  int               nZ   = 4;
  Matrix<double> mat(3, 4, 0.);
  mat(1, 1) = 1.;
  mat(2, 1) = 4.;
  mat(3, 1) = 1.;
  mat(1, 2) = 2.;
  mat(2, 2) = 3.;
  mat(3, 2) = 2.;
  mat(1, 3) = 7.;
  mat(2, 3) = 11.;
  mat(3, 3) = 7.;
  mat(1, 4) = 13.;
  mat(2, 4) = 3.;
  mat(3, 4) = 13.;

  VectorMap* weightFunction = NULL;

  PolynomialMap* vecF = new PolynomialMap(3, mat);
  std::vector< std::vector<double> > points(nX*nY*nZ, std::vector<double>(3));
  std::vector< std::vector<double> > values(nX*nY*nZ, std::vector<double>(3));
  std::vector<double>                weights(nX*nY*nZ, 1);
  size_t n_coeffs = vecF->NumberOfPolynomialCoefficients();
  Matrix<double> errs1a(n_coeffs, n_coeffs, 0.);
  Matrix<double> errs1b(n_coeffs, n_coeffs, 1.);
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
  // null weightFunction just tests branching to
  // PolynomialLeastSquaresFit(points, values, 1)
  bool testpass = true;
  PolynomialMap* pVec1a = PolynomialMap::PolynomialLeastSquaresFit(
                                    points, values, 1, weightFunction, errs1a);
  PolynomialMap* pVec1b = PolynomialMap::PolynomialLeastSquaresFit(
                                    points, values, 1, weightFunction, errs1b);
  Matrix<double> coeff1a = pVec1a->GetCoefficientsAsMatrix();
  Matrix<double> coeff1b = pVec1b->GetCoefficientsAsMatrix();
  for (size_t i = 1; i <= coeff1a.number_of_rows(); i++)
    for (size_t j = 1; j <= coeff1b.number_of_columns(); j++) {
      if (fabs(coeff1a(i, j) - mat(i, j)) > 1e-6)
        testpass = false;
      if (fabs(coeff1b(i, j) - mat(i, j)) < 1e-6)
        testpass = false;
    }
  delete pVec1b;
  delete pVec1a;
  EXPECT_TRUE(testpass);

  Matrix<double> errs2(n_coeffs, n_coeffs-1, 0.);
  EXPECT_THROW(PolynomialMap::PolynomialLeastSquaresFit(
        points, values, 1, weightFunction, errs2), Exceptions::Exception);
  Matrix<double> errs3(n_coeffs-1, n_coeffs, 0.);
  EXPECT_THROW(PolynomialMap::PolynomialLeastSquaresFit(
        points, values, 1, weightFunction, errs3), Exceptions::Exception);
}

TEST_F(PolynomialMapTest, LeastSquaresFitting) {
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

  VectorMap* weightFunction = NULL;

  PolynomialMap* vecF = new PolynomialMap(3, Matrix<double>(mat));
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
  // null weightFunction just tests branching to
  // PolynomialLeastSquaresFit(points, values, 1)
  PolynomialMap* pVec
    = PolynomialMap::PolynomialLeastSquaresFit(
        points, values, 1, weightFunction);

  // polynomial order should be 1
  EXPECT_EQ(pVec->PolynomialOrder(), static_cast<size_t>(1));

  // with polynomial order 1, the number of poly. coeff. is just the number of
  // variables plus one for the constant term
  EXPECT_EQ(pVec->NumberOfPolynomialCoefficients(), static_cast<size_t>(4));

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
  points.push_back(std::vector<double>(3, 92.));
  values.push_back(std::vector<double>(3, 17.));
  weights.push_back(0.);
  pVec = PolynomialMap::PolynomialLeastSquaresFit(points, values,
                                                     1, weights);

  // polynomial order should be 1
  EXPECT_EQ(pVec->PolynomialOrder(), static_cast<size_t>(1));

  // with polynomial order 1, the number of poly. coeff. is just the number of
  // variables plus one for the constant term
  EXPECT_EQ(pVec->NumberOfPolynomialCoefficients(), static_cast<size_t>(4));

  recCoeff = MAUS::CLHEP::HepMatrix(pVec->GetCoefficientsAsMatrix());
  Squeak::mout(Squeak::debug) << "Weighted Input" << mat << "Weighted Output"
                              << recCoeff << std::endl;
  for (int i = 0; i < recCoeff.num_row(); i++)
    for (int j = 0; j < recCoeff.num_col(); j++)
      if (fabs(recCoeff[i][j] - mat[i][j]) > 1e-6) testpass = false;
  delete pVec;
  EXPECT_TRUE(testpass);

  // same test as above but using VectorMap instead of std::vector<double>
  // for the weights
  weightFunction
    = new Function(&weight_function, points.size(), points.size());
  testpass = true;
  pVec = NULL;
  pVec = PolynomialMap::PolynomialLeastSquaresFit(points, values,
                                                     1, weightFunction);

  // polynomial order should be 1
  EXPECT_EQ(pVec->PolynomialOrder(), static_cast<size_t>(1));

  // with polynomial order 1, the number of poly. coeff. is just the number of
  // variables plus one for the constant term
  EXPECT_EQ(pVec->NumberOfPolynomialCoefficients(), static_cast<size_t>(4));

  recCoeff = MAUS::CLHEP::HepMatrix(pVec->GetCoefficientsAsMatrix());
  for (int i = 0; i < recCoeff.num_row(); i++)
    for (int j = 0; j < recCoeff.num_col(); j++)
      if (fabs(recCoeff[i][j] - mat[i][j]) > 1e-6) testpass = false;
  if (pVec != NULL) {
    delete pVec;
  }
  delete weightFunction;
  EXPECT_TRUE(testpass);

  testpass = true;
  // now take some of the input values, try for a constrained fit
  PolynomialMap Constraint(2, Matrix<double>(mat.sub(1, 2, 1, 3)));
  std::vector<PolynomialMap::PolynomialCoefficient> coeffs
    = Constraint.GetCoefficientsAsVector();
  for (int i = 0; i < 2; i++)
    coeffs.erase(coeffs.begin());
  PolynomialMap* constraintPVec = new PolynomialMap(coeffs);
  pVec = PolynomialMap::ConstrainedPolynomialLeastSquaresFit(
    points, values, 1, constraintPVec->GetCoefficientsAsVector(), weights);

  // polynomial order should be 1
  EXPECT_EQ(pVec->PolynomialOrder(), static_cast<size_t>(1));

  // with polynomial order 1, the number of poly. coeff. is just the number of
  // variables plus one for the constant term
  EXPECT_EQ(pVec->NumberOfPolynomialCoefficients(), static_cast<size_t>(4));

  recCoeff = MAUS::CLHEP::HepMatrix(pVec->GetCoefficientsAsMatrix());
  Squeak::mout(Squeak::debug) << "Constrained Input\n" << *constraintPVec
                              << "Constrained Output\n" << *pVec << std::endl;
  for (int i = 0; i < recCoeff.num_row(); i++)
    for (int j = 0; j < recCoeff.num_col(); j++)
      if (fabs(recCoeff[i][j] - mat[i][j]) > 1e-6) testpass = false;
  delete pVec;
  EXPECT_TRUE(testpass);

  // bad points size
  testpass = true;
  try {
    const std::vector< std::vector<double> > bad_points;
    PolynomialMap::ConstrainedPolynomialLeastSquaresFit(
      bad_points, values,
      1, constraintPVec->GetCoefficientsAsVector(), weights);
    testpass = false;
  } catch (Exceptions::Exception exc) {}
  ASSERT_TRUE(testpass);

  // bad values size
  testpass = true;
  try {
    const std::vector< std::vector<double> > bad_values;
    PolynomialMap::ConstrainedPolynomialLeastSquaresFit(
      points, bad_values,
      1, constraintPVec->GetCoefficientsAsVector(), weights);
    testpass = false;
  } catch (Exceptions::Exception exc) {}
  ASSERT_TRUE(testpass);

  testpass = true;
  // should return a copy of
  double mat2[] = {1., 2., 7., 13., 200., 500., 800., 1100., 1400., 1700.,
                   1., 3., 8., 14., 300., 600., 900., 1200., 1500., 1800.,
                   1., 4., 9., 15., 400., 700., 1000., 1300., 1600., 1900.};
  Squeak::mout(Squeak::debug) << "Chi2SweepingLeastSquaresFit" << std::endl;
  PolynomialMap* testF
    = new PolynomialMap(3, Matrix<double>(3, 10, mat2));
  std::vector<double> delta(3);
  delta[0] = 1e-50;
  delta[1] = 1e-50;
  delta[2] = 1e-50;
  // check we can make a polynomial vector at all
  pVec  = PolynomialMap::Chi2SweepingLeastSquaresFit(
            *testF, 5, std::vector< PolynomialMap::PolynomialCoefficient >(),
            1e-40, delta, 10., 100);

  if (pVec == NULL) {
    testpass = false;
    Squeak::mout(Squeak::debug) << "Failed to make PolynomialMap when "
                                << "PolynomialMap expected " << pVec
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
  pVec  = PolynomialMap::Chi2SweepingLeastSquaresFit(
    *testF, 1, std::vector< PolynomialMap::PolynomialCoefficient >(),
    1e-40, delta, 10., 100);

  if (pVec == NULL) {
    testpass = false;
    Squeak::mout(Squeak::debug) << "Failed to make PolynomialMap when "
                                << "PolynomialMap expected " << pVec
                                << std::endl;
  } else {
    // polynomial order should be 1
    EXPECT_EQ(pVec->PolynomialOrder(), static_cast<size_t>(1));

    // with polynomial order 1, the number of poly. coeff. is just the number of
    // variables plus one for the constant term
    EXPECT_EQ(pVec->NumberOfPolynomialCoefficients(), static_cast<size_t>(4));

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
  pVec  = PolynomialMap::Chi2SweepingLeastSquaresFit(
    *testF, 5, std::vector< PolynomialMap::PolynomialCoefficient >(),
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
  PolynomialMap* testF2
    = new PolynomialMap(3, Matrix<double>(3, 10, mat3));
  Squeak::mout(Squeak::debug) << "Chi2SweepingLeastSquaresFitVariableWalls"
                              << std::endl;
  // delta.push_back(0.);
  delta[0] = 1e-50;
  delta[1] = 1e-50;
  delta[2] = 1e-50;
  pVec  = PolynomialMap::Chi2SweepingLeastSquaresFitVariableWalls(
    *testF2, 2, std::vector< PolynomialMap::PolynomialCoefficient >(),
    1e-20, delta, 10., 100, deltaMax);

  // polynomial order should be 2
  EXPECT_EQ(pVec->PolynomialOrder(), static_cast<size_t>(2));

  // with point size 3 and polynomial order 2,
  // the number of poly. coeff. is (3+2)!/(3!2!) = 10
  EXPECT_EQ(pVec->NumberOfPolynomialCoefficients(), static_cast<size_t>(10));

  Squeak::mout(Squeak::debug) << "delta variable walls: ";
  for (size_t i = 0; i < delta.size(); i++) {
    Squeak::mout(Squeak::debug) << delta[i] << " ";
  }
  Squeak::mout(Squeak::debug) << std::endl;
  delta[0] = 1e-50;
  delta[1] = 1e-50;
  delta[2] = 1e-50;
  PolynomialMap* pVec2 = PolynomialMap::Chi2SweepingLeastSquaresFit(
    *testF2, 2, std::vector< PolynomialMap::PolynomialCoefficient >(),
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
    // polynomial order should be 2
    EXPECT_EQ(pVec2->PolynomialOrder(), static_cast<size_t>(2));

    // with point size 3 and polynomial order 2,
    // the number of poly. coeff. is (3+2)!/(3!2!) = 10
    EXPECT_EQ(pVec2->NumberOfPolynomialCoefficients(), static_cast<size_t>(10));

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
  pVec  = PolynomialMap::Chi2SweepingLeastSquaresFitVariableWalls(
    *testF2, 5, std::vector< PolynomialMap::PolynomialCoefficient >(),
    1e-60, delta, 10., 100, deltaMax);

  if (pVec != NULL)
    testpass = false;
  Squeak::mout(Squeak::debug) << "Should be NULL " << pVec << std::endl;

  delete testF;
  Squeak::mout(Squeak::debug) << "PolynomialLeastSquaresTest " << *testF2
                              << testpass << std::endl;
  EXPECT_TRUE(testpass);
}

TEST_F(PolynomialMapTest, SpaceTransform) {
  int myints[] = {1, 2};
  std::vector<int> inVariablesByVector(
    myints, myints + sizeof(myints) / sizeof(int));
  PolynomialMap::PolynomialCoefficient coefficient
    = PolynomialMap::PolynomialCoefficient(inVariablesByVector, 0, 1.1);
  int space_in_array[] = {0, 2, 3, 5, 6};
  std::vector<int> space_in(
    space_in_array, space_in_array + sizeof(space_in_array) / sizeof(int));
  int space_out_array[] = {4, 7, 1, 2, 3, 0};
  std::vector<int> space_out(
    space_out_array, space_out_array + sizeof(space_out_array) / sizeof(int));
  coefficient.SpaceTransform(space_in, space_out);

  std::vector<int> transformedInVariables = coefficient.InVariables();
  EXPECT_EQ(3, transformedInVariables[0]);
  EXPECT_EQ(4, transformedInVariables[1]);
  EXPECT_EQ(5, coefficient.OutVariable());
  EXPECT_EQ(1.1, coefficient.Coefficient());

  // in variable not in space mapping
  bool testpass = true;
  try {
    coefficient.SpaceTransform(space_in, space_out);
    testpass = false;
  } catch (Exceptions::Exception exc) {}
  ASSERT_TRUE(testpass);

  // out variable not in space mapping
  space_in[3] = 7;
  space_in[4] = 1;
  testpass = true;
  try {
    coefficient.SpaceTransform(space_in, space_out);
    testpass = false;
  } catch (Exceptions::Exception exc) {}
  ASSERT_TRUE(testpass);
}

/*
TEST_F(PolynomialMapTest, MakePolynomialVector) {
  const double point_data[2] = {1.0, 2.0};
  Vector<double> point(point_data, 2);
  Vector<double> polynomial_vector;
  test.polynomial_map_->MakePolynomialVector(point, polynomial_vector);
  for (size_t index = 0; index < 2; ++index) {
    EXPECT_NEAR(point[index], point_data[index], 1.e-6);
  }
}
*/

TEST_F(PolynomialMapTest, SetCoefficients) {
  Matrix<double> coefficient_matrix(3, 6, 0.);  // initialize all elements to zero
  PolynomialMap test_map(2, coefficient_matrix);
  const double coefficients[18] = {
    1., 2., 3., 4., 5., 6.,
    0., 1., 4., 9., 16., 25.,
    -1., -2., -3., -4., -5., -6.,
  };
  coefficient_matrix = Matrix<double>(3, 6, coefficients);
  test_map.SetCoefficients(coefficient_matrix);
  const Matrix<double> test_coefficient_matrix
    = test_map.GetCoefficientsAsMatrix();
  const Matrix<double> master_coefficient_matrix
    = polynomial_map_->GetCoefficientsAsMatrix();
  for (size_t row = 0; row < 3; ++row) {
    for (size_t column = 0; column < 6; ++column) {
      EXPECT_NEAR(test_coefficient_matrix(row+1, column+1),
                  master_coefficient_matrix(row+1, column+1),
                  1.e-6);
    }
  }
}

TEST_F(PolynomialMapTest, UnmakePolynomialVector) {
  const double point_data[2] = {1.0, 2.0};
  Vector<double> point(point_data, 2);
  Vector<double> polynomial_vector;
  polynomial_map_->MakePolynomialVector(point, polynomial_vector);
  polynomial_map_->UnmakePolynomialVector(polynomial_vector, point);
  for (size_t index = 0; index < 2; ++index) {
    EXPECT_NEAR(point[index], point_data[index], 1.e-6);
  }

  Vector<double> empty_point;
  polynomial_map_->UnmakePolynomialVector(polynomial_vector, empty_point);
  for (size_t index = 0; index < 2; ++index) {
    EXPECT_NEAR(empty_point[index], point_data[index], 1.e-6);
  }

  Vector<double> wrong_size_point(4);
  bool testpass = true;
  try {
    polynomial_map_->UnmakePolynomialVector(polynomial_vector, wrong_size_point);
    testpass = false;
  } catch (Exceptions::Exception exc) {}
  ASSERT_TRUE(testpass);

  double polynomial_vector_data[polynomial_vector.size()];
  polynomial_map_->MakePolynomialVector(point_data, polynomial_vector_data);
  double point_test[2];
  polynomial_map_->UnmakePolynomialVector(polynomial_vector_data, point_test);
  for (size_t index = 0; index < 2; ++index) {
    EXPECT_NEAR(point_test[index], point_data[index], 1.e-6);
  }
}

TEST_F(PolynomialMapTest, generate_polynomial_2) {
  const double y0[3] = {1., 0., -1.};
  const double y1[3] = {7., 10., -7.};
  for (size_t value_var_index = 0; value_var_index < 3; ++value_var_index) {
    Vector<double> polynomial = generate_polynomial_2D(*polynomial_map_,
                                                      0,    // point var index
                                                      value_var_index,
                                                      0.,   // point var min
                                                      1.,   // point var max
                                                      1.);  // point var delta
    const double y[2] = {y0[value_var_index], y1[value_var_index]};
    for (size_t index = 0; index < 2; ++index) {
      EXPECT_NEAR(y[index], polynomial[index], 1.e-6);
    }
  }
}

TEST_F(PolynomialMapTest, RecentredNotSupported) {
  bool testpass = true;
  try {
    double * point = NULL;
    polynomial_map_->Recentred(point);
    testpass = false;
  } catch (Exceptions::Exception exc) {}
  ASSERT_TRUE(testpass);
}

TEST_F(PolynomialMapTest, InverseNotSupported) {
  bool testpass = true;
  try {
    polynomial_map_->Inverse();
    testpass = false;
  } catch (Exceptions::Exception exc) {}
  ASSERT_TRUE(testpass);
}
