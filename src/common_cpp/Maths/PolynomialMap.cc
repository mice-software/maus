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

/* Author: Chris Rogers (G4MICE)
 * Author: Peter Lane (MAUS update)
 */

#include "Maths/PolynomialMap.hh"

#include <cmath>
#include <iomanip>
#include <limits>
#include <sstream>
#include <vector>

#include "gsl/gsl_sf_gamma.h"

#include "Interface/Interpolator.hh"
#include "Interface/Mesh.hh"
#include "Utils/Exception.hh"
#include "Maths/Matrix.hh"
#include "Maths/SymmetricMatrix.hh"
#include "Maths/Vector.hh"

namespace MAUS {

bool PolynomialMap::print_headers_ = true;

PolynomialMap::PolynomialMap(int numberOfInputVariables,
                                   Matrix<double> polynomialCoefficients)
  :  point_dimension_(numberOfInputVariables), index_key_by_power_(),
    index_key_by_vector_(), coefficient_matrix_(polynomialCoefficients) {
  SetCoefficients(numberOfInputVariables, polynomialCoefficients);
}

PolynomialMap::PolynomialMap(
  std::vector<PolynomialCoefficient>  coefficients)
  :  point_dimension_(0), index_key_by_power_(), index_key_by_vector_(),
    coefficient_matrix_() {
  SetCoefficients(coefficients);
}

PolynomialMap::PolynomialMap(const PolynomialMap & original_instance) {
  point_dimension_ = original_instance.point_dimension_;
  index_key_by_power_ = original_instance.index_key_by_power_;
  index_key_by_vector_ = original_instance.index_key_by_vector_;
  coefficient_matrix_ = original_instance.coefficient_matrix_;
  polynomial_vector_ = original_instance.polynomial_vector_;
  print_headers_ = original_instance.print_headers_;
}

void PolynomialMap::SetCoefficients(int pointDim, Matrix<double> coeff) {
  point_dimension_     = pointDim;
  coefficient_matrix_  = coeff;
  int num_poly_coeff   = coefficient_matrix_.number_of_columns();
  index_key_by_power_  = std::vector< std::vector<int> >();
  index_key_by_vector_ = std::vector< std::vector<int> >();
  polynomial_vector_ = std::vector<PolynomialCoefficient>();
  for (int i = 0; i < num_poly_coeff; ++i)
    index_key_by_power_.push_back(IndexByPower(i, pointDim));
  for (int i = 0; i < num_poly_coeff; ++i)
    index_key_by_vector_.push_back(IndexByVector(i, pointDim));

  for (size_t i = 0; i < coefficient_matrix_.number_of_rows(); ++i)
    for (int j = 0; j < num_poly_coeff; ++j)
      polynomial_vector_.push_back(
          PolynomialMap::PolynomialCoefficient(index_key_by_vector_[j],
          i,
          coefficient_matrix_(i+1, j+1) ) );
}

void PolynomialMap::SetCoefficients(std::vector<PolynomialCoefficient> coeff) {
  point_dimension_        = 0;
  index_key_by_power_  = std::vector< std::vector<int> >();
  index_key_by_vector_ = std::vector< std::vector<int> >();
  polynomial_vector_   = coeff;

  int maxPolyOrder = 0;
  int valueDim     = 0;
  for (size_t i = 0; i < coeff.size(); ++i) {
    int polyOrder = coeff[i].InVariables().size();
    for (size_t j = 0; j < coeff[i].InVariables().size(); ++j)
      if (coeff[i].InVariables()[j] > point_dimension_) {
        point_dimension_ = coeff[i].InVariables()[j];
      }
    if (coeff[i].OutVariable()    > valueDim) valueDim  = coeff[i].OutVariable();
    if (polyOrder > maxPolyOrder) maxPolyOrder = polyOrder;
  }
  point_dimension_++;  // PointDim indexes from 0
  coefficient_matrix_ = Matrix<double>(
        valueDim+1,
        NumberOfPolynomialCoefficients(point_dimension_,
        maxPolyOrder+1),
        0.);

  for (size_t i = 0; i < coefficient_matrix_.number_of_columns(); ++i) {
    index_key_by_power_.push_back(IndexByPower(i, point_dimension_));
  }
  for (size_t i = 0; i < coefficient_matrix_.number_of_columns(); ++i) {
    index_key_by_vector_.push_back(IndexByVector(i, point_dimension_));
    for (size_t j = 0; j < coeff.size();   ++j)
      if (coeff[j].InVariables() == index_key_by_vector_.back()) {
        int dim = coeff[j].OutVariable();
        coefficient_matrix_(dim+1, i+1) = coeff[j].Coefficient();
      }
  }
}

void PolynomialMap::SetCoefficients(Matrix<double> coeff) {
  coefficient_matrix_  = coeff;
  polynomial_vector_ = std::vector<PolynomialCoefficient>();
  for (size_t i = 0; i < coeff.number_of_rows() &&
                     i < coefficient_matrix_.number_of_rows(); ++i) {
    for (size_t j = 0; j < coeff.number_of_columns() &&
                     j < coefficient_matrix_.number_of_columns(); ++j) {
      polynomial_vector_.push_back(
          PolynomialMap::PolynomialCoefficient(index_key_by_vector_[j],
          i,
          coefficient_matrix_(i+1, j+1) ) );
    }
  }
}

Matrix<double> PolynomialMap::GetCoefficientsAsMatrix() const {
  return coefficient_matrix_;
}

std::vector<PolynomialMap::PolynomialCoefficient>
PolynomialMap::GetCoefficientsAsVector() const {
  return polynomial_vector_;
}

PolynomialMap* PolynomialMap::Recentred(double * point) const {
  throw(Exceptions::Exception(Exceptions::nonRecoverable,
        "Recentred not implemented",
        "PolynomialMap::Recentred"));
}

void  PolynomialMap::F(const double* point, double* value) const {
  Vector<double> pointV(PointDimension(), 1.);
  Vector<double> valueV(ValueDimension(), 1.);
  for (size_t i = 0; i < PointDimension(); ++i) {
    pointV(i+1) = point[i];
  }

  F(pointV, valueV);

  for (size_t i = 0; i < ValueDimension(); ++i) {
    value[i]  = valueV(i+1);
  }
}

void  PolynomialMap::F(const Vector<double>& point, Vector<double>& value)
  const {
  // create a vector of point coordinate products that form, together with the
  // coefficents, the polynomial terms
  Vector<double> polynomial_vector(index_key_by_vector_.size(), 1.);
  MakePolynomialVector(point, polynomial_vector);
  value = coefficient_matrix_ * polynomial_vector;
}

unsigned int PolynomialMap::PointDimension() const {
  return point_dimension_;
}

unsigned int PolynomialMap::ValueDimension() const {
  return coefficient_matrix_.number_of_rows();
}

unsigned int PolynomialMap::PolynomialOrder() const {
  if (index_key_by_vector_.size() > 0) {
    return index_key_by_vector_.back().size();
  } else {
    return 0;
  }
}

PolynomialMap * PolynomialMap::Inverse() const {
  // FIXME(plane1@hawk.iit.edu): implement
  throw(Exceptions::Exception(Exceptions::recoverable,
               "Not implemented.",
               "PolynomialMap::Inverse()"));
}

PolynomialMap PolynomialMap::Inverse(int max_order) const {
  // FIXME(plane1@hawk.iit.edu): implement
  throw(Exceptions::Exception(Exceptions::recoverable,
               "Not implemented.",
               "PolynomialMap::Inverse(int)"));
}

PolynomialMap * PolynomialMap::Clone() const {
  return new PolynomialMap(*this);
}

Vector<double>&  PolynomialMap::MakePolynomialVector(const Vector<double>& point,
                                                     Vector<double>& polyVector)
    const {
  const size_t poly_vec_len = NumberOfPolynomialCoefficients(PointDimension(),
                                                             PolynomialOrder());
  if (polyVector.size() == 0) {
    polyVector = Vector<double>(poly_vec_len);
  } else if (polyVector.size() != poly_vec_len) {
    throw(Exceptions::Exception(Exceptions::recoverable,
                    "Polynomial vector object is not empty "
                    "and is not of correct size.",
                    "PolynomialMap::MakePolynomialVector"));
  }
  for (size_t i = 0; i < index_key_by_vector_.size(); ++i) {
    polyVector[i] = 1.;
    for (size_t j = 0; j < index_key_by_vector_[i].size(); ++j) {
      polyVector[i] *= point[index_key_by_vector_[i][j]];
    }
  }
  return polyVector;
}

double*  PolynomialMap::MakePolynomialVector(const double* point,
                                             double* polyVector)
    const {
  for (size_t i = 0; i < index_key_by_vector_.size(); ++i) {
    polyVector[i] = 1.;
    for (size_t j = 0; j < index_key_by_vector_[i].size(); ++j)
      polyVector[i] *= point[ index_key_by_vector_[i][j] ];
  }
    return polyVector;
}

Vector<double> &  PolynomialMap::UnmakePolynomialVector(
    const Vector<double> & polyVector, Vector<double> & point)
    const {
  const size_t point_len = PointDimension();
  if (point.size() == 0) {
    point = Vector<double>(point_len);
  } else if (point.size() != point_len) {
    throw(Exceptions::Exception(Exceptions::recoverable,
                    "Point object is not empty "
                    "and is not of correct size.",
                    "PolynomialMap::UnmakePolynomialVector"));
  }
  for (size_t index = 0; index < point_len; ++index) {
      point[index] = polyVector[index+1];
  }
  return point;
}

double * PolynomialMap::UnmakePolynomialVector(double const * const polyVector,
                                               double * point)
    const {
  const size_t size = PointDimension();
  for (size_t index = 0; index < size; ++index) {
      point[index] = polyVector[index+1];
  }
  return point;
}

// Turn int index into a std::vector<int> 'a' of length 'd' with values
//    x_1^a_1 x_2^a_2 ... x_d^a_d
// e.g. x_1^4 x_2^3 = {4,3,0,0}
std::vector<int> PolynomialMap::IndexByPower(int index, int nInputVariables) {
    std::vector<int> powerIndex(nInputVariables, 0);
    std::vector<int> vectorIndex = IndexByVector(index, nInputVariables);
    for (size_t i = 0; i < vectorIndex.size(); ++i) {
        powerIndex[vectorIndex[i]]++;
    }
    return powerIndex;
}

// Turn int index into an index for element of polynomial
// e.g. x_1^4 x_2^3 = {1,1,1,1,2,2,2}
std::vector<int> PolynomialMap::IndexByVector(int index, int nInputVariables) {
    if (index == 0) return std::vector<int>();
    std::vector<int> indices(1, -1);
    nInputVariables--;
    for (int i = 0; i < index;   ++i) {
        if (indices.front() == nInputVariables) {
          indices = std::vector<int>(indices.size()+1, 0);
          indices.back()--;
        }
        if (indices.back()  == nInputVariables) {
            int j = indices.size()-1;
            while (indices[j] == nInputVariables) j--;
            for (int k = indices.size()-1; k >= j; k--) {
                indices[k] = indices[j]+1;
            }
        } else {
          indices.back()++;
        }
    }
    return indices;
}

size_t PolynomialMap::NumberOfPolynomialCoefficients(int pointDimension,
                                                     int order) {
    if (order <= 0) return 1;

    int n = 1;
    for (int i = 0; i < order; ++i) {
        n += gsl_sf_choose(pointDimension+i, i+1);
    }
    return n;
}

std::ostream& operator<<(std::ostream& out, const PolynomialMap& pv) {
    if (PolynomialMap::print_headers_) {
        pv.PrintHeader(out, '.', ' ', 14, true);
    }
    out << '\n' << pv.GetCoefficientsAsMatrix();
    return out;
}

template <class Container >
void PolynomialMap::PrintContainer(std::ostream& out,
     const Container& container, char T_separator, char str_separator,
     size_t length, bool pad_at_start) {
  // class Container::iterator it;
  std::stringstream strstr1("");
  std::stringstream strstr2("");
  class Container::const_iterator it1 = container.begin();
  class Container::const_iterator it2 = it1;
  while (it1 != container.end()) {
    it2++;
    if (it1 != container.end() && it2 != container.end()) {
      strstr1 << (*it1) << T_separator;
    } else {
      strstr1 << (*it1);
    }
    it1++;
  }

  if (strstr1.str().size() > length && length > 0) {
    strstr2 << strstr1.str().substr(1, length);
  } else {
    strstr2 << strstr1.str();
  }
  if (!pad_at_start) {
    out << strstr2.str();
  }
  for (size_t i = 0; i < length - strstr2.str().size(); ++i) {
    out << str_separator;
  }
  out << strstr2.str();
}

void PolynomialMap::PrintHeader(std::ostream& out, char int_separator,
                      char str_separator, int length, bool pad_at_start) const {
  if (index_key_by_power_.size() > 0) {
    PrintContainer< std::vector<int> >(out, index_key_by_power_[0],
                          int_separator, str_separator, length-1, pad_at_start);
  }
  for (size_t i = 1; i < index_key_by_power_.size(); ++i) {
    PrintContainer<std::vector<int> >(out, index_key_by_power_[i],
                            int_separator, str_separator, length, pad_at_start);
  }
}

Vector<double> PolynomialMap::Means
       (std::vector<std::vector<double> > values, std::vector<double> weights) {
  if (values.size() < 1) {
    throw(Exceptions::Exception
           (Exceptions::recoverable, "No input values", "PolynomialMap::Means"));
  }
  if (values[0].size() < 1) {
    throw(Exceptions::Exception
            (Exceptions::recoverable, "Dimension < 1", "PolynomialMap::Means"));
  }
  if (weights.size() != values.size()) {
    weights = std::vector<double>(values.size(), 1.);
  }
  size_t npoints     = values.size();
  size_t dim         = values[0].size();
  Vector<double>    means(dim, 0);
  double             totalWeight = 0;
  for (size_t x = 0; x < npoints; x++) {
    totalWeight += weights[x];
  }
  std::vector<double> normalized_weights;
  for (size_t x = 0; x < npoints; x++) {
    normalized_weights.push_back(weights[x] / totalWeight);
  }

  double mean;
  for (size_t i = 0; i < dim; ++i) {
    mean = 0.;
    for (size_t x = 0; x < npoints; ++x) {
      mean += values[x][i] * normalized_weights[x];
    }
    means(i+1) = mean;
  }

  return means;
}

SymmetricMatrix PolynomialMap::Covariances(
    const std::vector<std::vector<double> >&          values,
    const std::vector<double>&                        weights,
    const Vector<double>&                             means) {
  size_t npoints = values.size();
  if (npoints < 1) {
    throw(Exceptions::Exception(Exceptions::recoverable,
                 "No input values",
                 "PolynomialMap::Covariances()"));
  }
  size_t dim = values[0].size();
  if (dim < 1) {
    throw(Exceptions::Exception(Exceptions::recoverable,
                 "Dimension < 1",
                 "PolynomialMap::Covariances()"));
  }
  Vector<double> means_vector(dim);
  if (means.size() != dim) {
    means_vector = Means(values, weights);
  } else {
    means_vector = means;
  }
  std::vector<double> weights_vector;
  if (weights.size() != npoints) {
    weights_vector = std::vector<double>(npoints, 1.);
  } else {
    weights_vector = weights;
  }

  SymmetricMatrix covariances(dim);

  double totalWeight = 0;
  for (size_t x = 0; x < npoints; ++x) {
    totalWeight += weights_vector[x];
  }

  std::vector<double> normalized_weights;
  for (size_t x = 0; x < npoints; ++x) {
    normalized_weights.push_back(weights_vector[x] / totalWeight);
  }

  double sum;
  for (size_t i = 0; i < dim; ++i) {
    for (size_t j = 0; j <= i; ++j) {
      sum = 0.;
      for (size_t x = 0; x < npoints; x++) {
        sum += values[x][i] * values[x][j] * normalized_weights[x];
      }
      covariances.set(i+1, j+1, sum - means_vector[i] * means_vector[j]);
    }
  }
  return covariances;
}

PolynomialMap* PolynomialMap::PolynomialLeastSquaresFit(
    const std::vector< std::vector<double> >& points,
    const std::vector< std::vector<double> >& values,
    unsigned int                              polynomialOrder,
    const VectorMap*                          weightFunction,
    Matrix<double>                            point_errors) {
  if (weightFunction == NULL) {
    // use default value for weights defined in PolnomialMap.hh
    std::vector<double> dummy;
    return PolynomialLeastSquaresFit(points, values, polynomialOrder, dummy, point_errors);
  }

  std::vector<double> weights(points.size());
  for (size_t i = 0; i < points.size(); ++i) {
    weightFunction->F(&points[i][0], &weights[i]);
  }
  return PolynomialLeastSquaresFit(points, values, polynomialOrder, weights, point_errors);
}

PolynomialMap* PolynomialMap::PolynomialLeastSquaresFit(
  const std::vector< std::vector<double> >& points,
  const std::vector< std::vector<double> >& values,
  unsigned int                              polynomialOrder,
  const std::vector<double>&                weights,
  Matrix<double>                            point_errors) {
  // Algorithm: We have F2 = sum_i ( f_k f_l) where f are polynomial terms;
  // FY = sum_i (f_)

  size_t pointDim = points[0].size();
  size_t valueDim = values[0].size();
  size_t nPoints  = points.size();
  size_t nCoeffs  = NumberOfPolynomialCoefficients(pointDim, polynomialOrder);

  if (point_errors.number_of_rows() == 0 &&
      point_errors.number_of_columns() == 0) {
      point_errors = Matrix<double>(nCoeffs, nCoeffs, 0.);
  }
  if (point_errors.number_of_rows() != nCoeffs ||
      point_errors.number_of_columns() != nCoeffs) {
      std::stringstream message;
      message << "The size of the point_errors (" << point_errors.number_of_rows()
              << "x" << point_errors.number_of_columns() << ") "
              << "does not match number of polynomial coefficients (" << nCoeffs
              << "x" << nCoeffs << ")" << std::endl;
      throw(Exceptions::Exception(Exceptions::recoverable,
                      message.str(),
                      "PolynomialMap::PolynomialLeastSquaresFit"));
  }
  // create
  Matrix<double> dummy(valueDim, nCoeffs, 0.);
  for (size_t i = 0; i < valueDim; ++i)
    for (size_t j = 0; j < nCoeffs; ++j)
      dummy(i+1, j+1) = 1;
  PolynomialMap * polynomial_map = new PolynomialMap(pointDim, dummy);

  // Create the design matrix
  std::vector<double> point_poly_vector(nCoeffs, 0);
  Matrix<double> design_matrix(nPoints, nCoeffs, 0.);  // design matrix
  for (size_t row = 0; row < nPoints; ++row) {
    polynomial_map->MakePolynomialVector(&points[row][0],
                                         &point_poly_vector[0]);
    for (size_t column = 0; column < nCoeffs; ++column) {
      design_matrix(row+1, column+1) = point_poly_vector[column];
    }
  }

  // Create the value matrix
  Matrix<double> value_matrix(nPoints, valueDim, 0.);  // value matrix
  for (size_t row = 0; row < nPoints; ++row) {
    for (size_t column = 0; column < valueDim; ++column) {
      value_matrix(row+1, column+1) = values[row][column];
    }
  }

  // Create the weight matrix (diagonal are the per-point/value weights)
  Vector<double> weight_vector(nPoints, 1.);
  Matrix<double> weight_matrix(nPoints, nPoints, 0.);
  if (weights.size() > 0) {
    if (weights.size() != nPoints) {
      std::stringstream message;
      message << "The number of weights (" << weights.size() << ") "
              << "does not equal the number of data points (" << nPoints << ")"
              << std::endl;
      throw(Exceptions::Exception(Exceptions::recoverable,
                  message.str(),
                  "PolynomialMap::PolynomialLeastSquaresFit"));
    }

    weight_vector = Vector<double>(weights);
  }
  for (size_t index = 0; index < nPoints; ++index) {
    weight_matrix(index+1, index+1) = weight_vector[index];
  }

  Matrix<double> design_matrix_transpose = transpose(design_matrix);

  // F2 = A^T A, where A is the design matrix with linearly independent columns
  Matrix<double> F2(nCoeffs, nCoeffs, 0.);
  F2 = design_matrix_transpose * weight_matrix * design_matrix;
  for (size_t i = 1; i < nCoeffs+1; ++i)
    for (size_t j = 1; j < nCoeffs+1; ++j)
      F2(i, j) -= point_errors(i, j)*nPoints;

  // Fy = A^T Y, where A is the design matrix and Y is the value matrix
  Matrix<double> Fy(nCoeffs, valueDim, 0.);
  Fy = design_matrix_transpose * weight_matrix * value_matrix;

  // F2^(-1) = (A^T A)^(-1), where A is the design matrix
  Matrix<double> F2_inverse;
  try {
    F2_inverse = inverse(F2);
  } catch (Exceptions::Exception exc) {
    delete polynomial_map;
    std::stringstream message;
    message << "Could not find least squares fit for data. Nested exception:"
            << std::endl << "\"" << exc.GetMessage() << "\"" << std::endl;
    throw(Exceptions::Exception(Exceptions::recoverable,
                 message.str(),
                 "PolynomialMap::PolynomialLeastSquaresFit"));
  }

  // C = (A^T A)^(-1) A^T Y, where A is the design matrix and Y is the value matrix
  Matrix<double> coefficient_matrix = transpose(F2_inverse * Fy);
  polynomial_map->SetCoefficients(pointDim, coefficient_matrix);
  return polynomial_map;
}

PolynomialMap* PolynomialMap::ConstrainedPolynomialLeastSquaresFit(
  const std::vector< std::vector<double> >&                points,
  const std::vector< std::vector<double> >&                values,
  unsigned int                                             polynomialOrder,
  std::vector< PolynomialMap::PolynomialCoefficient >  coeffs,
  const std::vector<double>&                              weights) {
  // Algorithm: we want g(x) = old_f(x) + new_f(x), where old_f has polynomial
  // terms in poly, new_f has all the rest. Use value - f(point) as input and
  // do LLS forcing old_f(x) polynomial terms to 0
  // Nb: in this constrained case we have to go through the output vector and
  // treat each like a 1D vector
  size_t nPoints    = points.size();
  if (nPoints < 1) {
    throw(Exceptions::Exception(
      Exceptions::recoverable,
      "No data for LLS Fit",
      "PolynomialMap::ConstrainedPolynomialLeastSquaresFit(...)"));
  }
  if (values.size() != nPoints) {
    throw(Exceptions::Exception(
      Exceptions::recoverable,
      "Misaligned array in LLS Fit",
      "PolynomialMap::ConstrainedPolynomialLeastSquaresFit(...)"));
  }
  int pointDim   = points[0].size();
  int valueDim   = values[0].size();
  int nCoeffsNew = NumberOfPolynomialCoefficients(pointDim, polynomialOrder);
  Matrix<double> A(valueDim, nCoeffsNew, 0.);

  std::vector<double> wt(nPoints, 1);
  if (weights.size() > 0) wt = weights;

  // guaranteed to be of right order
  PolynomialMap  newPolyMap1D(pointDim, Matrix<double>(1, nCoeffsNew));
  std::vector< std::vector<double> > tempFx(nPoints, std::vector<double>(nCoeffsNew) );
  for (size_t i = 0; i < nPoints; ++i)
    newPolyMap1D.MakePolynomialVector(&points[i][0], &tempFx[i][0]);

  for (int dim = 0; dim < valueDim ; dim++) {
    std::vector<PolynomialMap::PolynomialCoefficient> oldCoeffs1D;
    for (size_t i = 0; i < coeffs.size(); ++i) {
      if (coeffs[i].OutVariable() == dim &&
          coeffs[i].InVariables().size() < polynomialOrder) {
        oldCoeffs1D.push_back(coeffs[i]);
      }
    }
    PolynomialMap oldPolyMap1D(oldCoeffs1D);
    int              nCoeffsOld = oldCoeffs1D.size();
    int              deltaCoeff = nCoeffsNew - nCoeffsOld;
    if (deltaCoeff <= 0) break;

    // index of variables that need calculation
    std::vector<int> needsCalculation;
    for (int i = 0; i < nCoeffsNew; ++i) {
      bool exists = true;
      for (size_t j = 0; j < oldCoeffs1D.size(); ++j)
        if (IndexByVector(i, pointDim) == oldCoeffs1D[j].InVariables())
          exists = false;
      if (exists) needsCalculation.push_back(i);
    }

    Vector<double> Fy(deltaCoeff, 0);
    Matrix<double> F2(deltaCoeff, deltaCoeff,  0.);

    // optimisation note - this algorithm spends all its time in this loop
    for (size_t i = 0; i < nPoints && needsCalculation.size() > 0; ++i) {
      std::vector<double> oldValue(valueDim);
      oldPolyMap1D.F(&points[i][0], &oldValue[0]);
      for (int k = 0; k < deltaCoeff; k++) {
        Fy(k+1) += (values[i][dim] -  // ynew - yold
              oldValue[dim])*tempFx[i][needsCalculation[k]]*wt[i];
        for (int j = 0; j < deltaCoeff;    ++j)
          F2(j+1, k+1) += tempFx[i][needsCalculation[k]]*
                          tempFx[i][needsCalculation[j]]*wt[i];
      }
    }

    Matrix<double> F2_inverse;
    try {
      F2_inverse = inverse(F2);
    } catch (Exceptions::Exception exc) {
    std::stringstream message;
    message << "Could not find constrained least squares fit for data. "
            << "Nested exception:" << std::endl
            << "\"" << exc.GetMessage() << "\"" << std::endl;
      throw(Exceptions::Exception(Exceptions::recoverable,
            message.str(),
            "PolynomialMap::ConstrainedPolynomialLeastSquaresFit"));
    }
    Vector<double> AVec = F2_inverse * Fy;
    for (int i = 0; i < deltaCoeff; ++i) A(dim+1, needsCalculation[i]+1) = AVec(i+1);
  }

  PolynomialMap tempPoly(coeffs);
  for (size_t i = 0; i < coeffs.size(); ++i)
    for (size_t j = 0; j < tempPoly.index_key_by_vector_.size(); ++j)
      if (tempPoly.index_key_by_vector_[j] == coeffs[i].InVariables()) {
        A(coeffs[i].OutVariable()+1, j+1) = coeffs[i].Coefficient();
      }

  return new PolynomialMap(pointDim, A);
}


PolynomialMap* PolynomialMap::Chi2ConstrainedLeastSquaresFit(
  const std::vector< std::vector<double> >&                xin,
  const std::vector< std::vector<double> >&                xout,
  unsigned int                                             polynomialOrder,
  std::vector< PolynomialMap::PolynomialCoefficient >  coeffs,
  double                                                  chi2Start,
  double                                                  discardStep,
  double *                                                chi2End,
  double                                                  chi2Limit,
  std::vector<double>                                      weights,
  bool                                                    firstIsMean) {
  size_t nParticles   = xin.size();
  size_t dimensionOut = xout[0].size();
  if (weights.size() != nParticles) {
    weights = std::vector<double>(xin.size(), 1.);
  }
  std::vector<double> weights_in = std::vector<double>(weights);
  std::vector<double> amps(nParticles);
  Vector<double> means(dimensionOut, 0);
  if (!firstIsMean) {
     means = Means(xout, weights);
  } else {
    for (size_t i = 0; i < dimensionOut; ++i) {
      means(i+1) = xout[0][i];
    }
  }

  SymmetricMatrix covariances = Covariances(xout, weights, means);
  SymmetricMatrix inverse_covaraiance_matrix;
  try {
    inverse_covaraiance_matrix = inverse(covariances);
  }
  catch (Exceptions::Exception exc) {
    throw(Exceptions::Exception(Exceptions::recoverable,
                 "Failed to find least squares fit for data",
                 "PolynomialMap::Chi2ConstrainedLeastSquaresFit"));
  }

  double totalWeight = 0.;
  double discard       = chi2Start;
  double chi2          = chi2Limit*2.;
  int    nCoefficients = PolynomialMap::NumberOfPolynomialCoefficients(
                            xin[0].size(), polynomialOrder)
                       * xout[0].size();
  int    nGood         = nParticles;

  if (discard <= 0.) {
    // if chi2Start ill-defined, just use maximum chi2 in sample
    for (size_t i = 0; i < nParticles; ++i) {
      Vector<double> xoutVec(dimensionOut, 0);
      for (size_t j = 0; j < dimensionOut; ++j) {
        xoutVec(j+1) = xout[i][j] - means(j+1);
      }
      amps[i] = (transpose(xoutVec) * inverse_covaraiance_matrix * xoutVec)(1);
      if (amps[i] > discard) discard = amps[i];
      totalWeight += weights[i];
    }
  }
  discard /= discardStep; // Set discard to the largest amplitude in the data

  PolynomialMap* map = new PolynomialMap(
    std::vector<PolynomialMap::PolynomialCoefficient>());
  while (nGood >= nCoefficients && chi2 > chi2Limit) {
    chi2               = 0.;
    nGood              = nParticles;
    if (map != NULL) delete map;

    // optimisation note - algorithm spends all its time doing the CPLS Fit
    try {
      map = PolynomialMap::ConstrainedPolynomialLeastSquaresFit(
                xin, xout, polynomialOrder, coeffs, weights);
    }
    catch (Exceptions::Exception exc) {
      map = NULL;
      chi2 = chi2Limit * 2.;
    }

    for (size_t i = 0; i < nParticles && map != NULL; ++i) {
      if (fabs(weights[i]) > 1.e-9) {
        std::vector<double> pout(dimensionOut, 0.);
        map->F(&xin[i][0], &pout[0]);
        Vector<double> residualVec(dimensionOut, 0);
        for (size_t j = 0; j < dimensionOut; ++j) {
          residualVec(j+1) = pout[j] - xout[i][j];
        }
        // watch weights handling here
        chi2 +=   weights[i] * weights[i] / totalWeight/totalWeight
                * (transpose(residualVec) * inverse_covaraiance_matrix *
                residualVec)(1);
      }
    }
    for (size_t i = 0; i < nParticles;   ++i)
      if (amps[i] > discard) {
        totalWeight -= weights[i];
        weights[i] = 0.;
        nGood--;
      }
    discard /= discardStep;
  }
  if (chi2 > chi2Limit || map == NULL) {
    std::stringstream err;
    err << "PolynomialMap::Chi2ConstrainedLeastSquaresFit failed to "
        << "converge. Best fit had <chi2> = " << chi2
        << " compared to limit " << chi2Limit << std::endl;
    throw(Exceptions::Exception(Exceptions::recoverable, err.str(),
          "PolynomialMap::Chi2ConstrainedLeastSquaresFit(...)"));
  }
  if (chi2End != NULL) *chi2End = discard; // save discard at end for future use
  return map;
}

PolynomialMap* PolynomialMap::Chi2SweepingLeastSquaresFit(
    const                                                   VectorMap& vec,
    unsigned int                                            polynomialOrder,
    std::vector< PolynomialMap::PolynomialCoefficient >  coeffs,
    double                                                  chi2Max,
    std::vector<double>&                                    delta,
    double                                                  deltaFactor,
    int                                                     maxNumberOfSteps) {
  // build particles in shell
  // try to do chi2 fit
  // if chi2 fit works, increase shell size
  // else increase polynomial order, revise fit
  PolynomialMap*   map1 = NULL;
  PolynomialMap*   map2 = NULL;
  int    step = 0;
  for (size_t i_order = 1; i_order <= polynomialOrder; i_order++) {
    std::vector<std::vector<double> > in;
    std::vector<std::vector<double> > out;
    double chi2 = -1;
    while (chi2 < chi2Max && step < maxNumberOfSteps) {
      in.push_back(std::vector<double>(delta.size(), 0.));
      step++;
      std::vector<std::vector<double> > in_mod = PointBox(delta, i_order+1);
      for (size_t i = 0; i < in_mod.size(); ++i) in.push_back(in_mod[i]);
      vec.FAppend(in, out);
      if (map2 != NULL && map1 != NULL) delete  map2;
      if (map1 != NULL) map2 = map1;
      map1 = ConstrainedPolynomialLeastSquaresFit(in, out, i_order, coeffs);
      chi2  = map1->GetAvgChi2OfDifference(in, out);
      if (chi2 < chi2Max)
        for (size_t i = 0; i < delta.size(); ++i) delta[i] *= deltaFactor;
    }
    delete map1;
    map1 = NULL;
  }
  for (size_t i = 0; i < delta.size(); ++i) delta[i] /= deltaFactor;
  return map2;
}

PolynomialMap * PolynomialMap::Chi2SweepingLeastSquaresFitVariableWalls(
    const VectorMap& vec, unsigned int polynomialOrder,
    std::vector< PolynomialMap::PolynomialCoefficient > coeffs,
    double chi2Max, std::vector<double>& delta,
    double deltaFactor, int maxNumberOfSteps,
    std::vector<double> max_delta) {
  PolynomialMap * map1
    = Chi2SweepingLeastSquaresFit(vec, polynomialOrder, coeffs, chi2Max,
                                  delta, deltaFactor, maxNumberOfSteps);
  if (map1 == NULL) return NULL;
  PolynomialMap* map2 = NULL;
  int    step = 0;
  for (size_t i_order = map1->PolynomialOrder(); i_order <= polynomialOrder;
       i_order++) {
    for (size_t i = 0; i < delta.size(); ++i) {
      double chi2 = -1;
      while (chi2 < chi2Max && step < maxNumberOfSteps &&
             delta[i]*deltaFactor < max_delta[i]) {
        if (chi2 < chi2Max)
          delta[i] *= deltaFactor;
        step++;
        std::vector<std::vector<double> > in = PointBox(delta, i_order+1);
        in.push_back(std::vector<double>(delta.size(), 0.));
        std::vector<std::vector<double> > out;
        vec.FAppend(in, out);
        if (map2 != NULL && map1 != NULL) delete  map2;
        if (map1 != NULL) map2 = map1;
        map1 = ConstrainedPolynomialLeastSquaresFit(in, out, i_order, coeffs);
        chi2  = map1->GetAvgChi2OfDifference(in, out);
        if (chi2 > chi2Max)
          delta[i] /= deltaFactor;
      }
    }
    delete map1;
    map1 = NULL;
  }
  return map2;
}


void PolynomialMap::PolynomialCoefficient::SpaceTransform
                       (std::vector<int> space_in, std::vector<int> space_out) {
  std::map<int, int> mapping; // probably optimise this
  for (size_t i = 0; i < space_out.size(); ++i)
    for (size_t j = 0; j < space_in.size(); ++j) {
      // mapping[space_in_index] returns space_out_index
      if (space_out[i] == space_in[j]) mapping[j] = i;
    }
  std::vector<int> in_variables(_inVarByVec);
  for (size_t con = 0; con < in_variables.size(); con++) {
    if (mapping.find(in_variables[con]) != mapping.end()) {
      in_variables[con] = mapping[in_variables[con]];
    } else { throw(Exceptions::Exception(Exceptions::recoverable,
                    "Input variable not found in space transform",
                    "PolynomialMap::PolynomialCoefficient::SpaceTransform"));
    }
  }
  if (mapping.find(_outVar) != mapping.end()) {
    _outVar = mapping[_outVar];
  } else {
    throw(Exceptions::Exception(Exceptions::recoverable,
                 "Output variable not found in space transform",
                 "PolynomialMap::PolynomialCoefficient::SpaceTransform"));
  }
  _inVarByVec = in_variables;
}

// Return chi2 of some set of output variables compared with the polynomial
// vector of some set of input variables
double PolynomialMap::GetAvgChi2OfDifference(
    std::vector< std::vector<double> > in,
    std::vector< std::vector<double> > out) {
  std::vector< std::vector<double> > out_p;
  if (in.size() < 1) {
    throw(Exceptions::Exception(Exceptions::recoverable,
                 "No data in input",
                 "PolynomialMap::GetAvgChi2OfDifference(...)"));
  }
  if (in.size() != out.size()) {
    throw(Exceptions::Exception(Exceptions::recoverable,
     "Input data and output data misaligned for calculation of chi2 difference",
     "PolynomialMap::GetAvgChi2OfDifference(...)"));
  }
  for (size_t i = 0; i < in.size(); ++i) {
    if (in[i].size() != PointDimension() || out[i].size() != ValueDimension())
      throw(Exceptions::Exception(Exceptions::recoverable,
                   "Bad input data for calculation of chi2 difference",
                   "PolynomialMap::GetAvgChi2OfDifference(...)"));
  }
  this->FAppend(in, out_p);

  std::vector< std::vector<double> > diff(in.size());
  for (size_t i = 0; i < out_p.size(); ++i) {
    diff[i] = std::vector<double>( ValueDimension() );
    for (size_t j = 0; j < ValueDimension(); ++j)
      diff[i][j] = out[i][j]-out_p[i][j];
  }

  double chi2 = 0;
  SymmetricMatrix cov_inv = Covariances(diff,
                                        std::vector<double>(diff.size(), 1.),
                                        Vector<double>(diff.size(), 0.));
  for (size_t i = 0; i < diff.size(); ++i) {
    Vector<double> diff_mv(diff[i]);
    chi2 += (transpose(diff_mv) * cov_inv * diff_mv)(1);
  }
  chi2 /= static_cast<double>(diff.size());
  return chi2;
}


// Make a shell of points on the outside of a hypercube with dimension same as
// delta length, corners of hypercube are at delta[0], delta[1], ... delta[n]
// and -delta[0], ..., -delta[n] number of points is at least
// 2*NumberOfPolynomialCoefficients(i_order, delta.size())-1 so that it will
// always define a polynomial of order i_order (one point +ve, one -ve hence
// factor 2) Require evenly spaced points, sometimes this means I make more
// points on the shell
static const double PI = atan(1)*4.;
std::vector< std::vector<double> > PolynomialMap::PointBox(
  std::vector<double>  delta,
  int                  i_order) {
  int min_size         = 3*NumberOfPolynomialCoefficients(i_order, delta.size());
  int n_points_per_dim = 2;
  int dim              = delta.size();
  while (::pow(n_points_per_dim, dim)-::pow(n_points_per_dim-2, dim)
         <= min_size) {
    n_points_per_dim++;
  }

  std::vector< std::vector<double> > pos;
  std::vector<int>    grid_size(dim, n_points_per_dim);
  std::vector<double> grid_min(delta);
  std::vector<double> grid_spacing(delta);
  for (size_t i = 0; i < grid_spacing.size(); ++i)
    grid_spacing[i] *= 2./static_cast<double>(n_points_per_dim-1);
  for (size_t i = 0; i < grid_min.size(); ++i)
    grid_min[i] *= -1.;
  NDGrid grid(dim, &grid_size[0], &grid_spacing[0], &grid_min[0]);
  for (Mesh::Iterator it = grid.Begin(); it < grid.End(); it++) {
    std::vector<int> place = it.State();
    for (size_t i = 0; i < place.size(); ++i)
      // If I am on the outside of the grid
      if (place[i] == 1 || place[i] == n_points_per_dim) {
        pos.push_back(it.Position());
        break; // end inner i loop to detect if it is on outside of grid
      }
  }
  return pos;
}

// Algorithm - take the PointBox output and scale so that length is 1 in
// scale_matrix coordinate system
std::vector< std::vector<double> > PolynomialMap::PointShell(
  Matrix<double>  scale_matrix,
  int             i_order) {
  size_t point_dim = scale_matrix.number_of_rows();
  Matrix<double> scale_inv = inverse(scale_matrix);
  std::vector<std::vector<double> > point_box
    = PointBox(std::vector<double>(point_dim, 1.), i_order);
  for (size_t i = 0; i < point_box.size(); ++i) {
    Vector<double> point(&point_box[i][0], point_dim);
    double scale  = (transpose(point) * scale_inv * point)(1);
    point /= ::pow(scale, static_cast<double>(point_dim));
    for (size_t j = 0; j < point_dim; ++j) {
      point_box[i][j]=point(j+1);
    }
  }
  return point_box;
}

Vector<double> generate_polynomial_2D(const PolynomialMap & map,
                                      const size_t point_variable_index,
                                      const size_t value_variable_index,
                                      const double point_variable_min,
                                      const double point_variable_max,
                                      const double point_variable_increment) {
  const size_t point_dimension = map.PointDimension();
  const size_t value_dimension = map.ValueDimension();
  Vector<double> point(point_dimension, 0.);
  Vector<double> value(value_dimension);

  size_t plot_size = size_t((point_variable_max - point_variable_min)
                            / point_variable_increment + 1);
  Vector<double> plot(plot_size);
  size_t plot_index = 0;
  for (double point_variable_value = point_variable_min;
       point_variable_value <= point_variable_max;
       point_variable_value += point_variable_increment) {
    point[point_variable_index] = point_variable_value;
    map.F(point, value);
    plot[plot_index++] = value[value_variable_index];
  }

  return plot;
}

}  // namespace MAUS
