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

#include <math.h>

#include <iomanip>
#include <sstream>
#include <vector>

#include "gsl/gsl_sf_gamma.h"

#include "Interface/Interpolator.hh"
#include "Interface/Mesh.hh"
#include "Interface/Squeal.hh"
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

void PolynomialMap::SetCoefficients(int pointDim, Matrix<double> coeff) {
  int nPCoeff      = coeff.number_of_columns();
  point_dimension_        = pointDim;
  index_key_by_power_  = std::vector< std::vector<int> >();
  index_key_by_vector_ = std::vector< std::vector<int> >();
  polynomial_vector_ = std::vector<PolynomialCoefficient>();
  for (int i = 0; i < nPCoeff; ++i)
    index_key_by_power_.push_back(IndexByPower(i, pointDim));
  for (int i = 0; i < nPCoeff; ++i)
    index_key_by_vector_.push_back(IndexByVector(i, pointDim));

  for (size_t i = 0; i < coefficient_matrix_.number_of_rows(); ++i)
    for (int j = 0; j < nPCoeff; ++j)
      polynomial_vector_.push_back(
          PolynomialMap::PolynomialCoefficient(index_key_by_vector_[j],
          i,
          coefficient_matrix_(i+1, j+1) ) );
}

void PolynomialMap::SetCoefficients
                                    (std::vector<PolynomialCoefficient> coeff) {
  point_dimension_        = 0;
  index_key_by_power_  = std::vector< std::vector<int> >();
  index_key_by_vector_ = std::vector< std::vector<int> >();
  polynomial_vector_      = coeff;

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
  for (size_t i = 0; i < coeff.number_of_rows() &&
                     i < coefficient_matrix_.number_of_rows(); ++i) {
    for (size_t j = 0; j < coeff.number_of_columns() &&
                     j < coefficient_matrix_.number_of_columns(); ++j) {
      coefficient_matrix_(i+1, j+1) = coeff(i+1, j+1);
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
  throw(Squeal(Squeal::nonRecoverable, "Recentred not implemented",
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
  MakePolyVector(point, polynomial_vector);
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
    return index_key_by_vector_.back().size()+1;
  } else {
    return 0;
  }
}

PolynomialMap * PolynomialMap::Clone() const {
  return new PolynomialMap(*this);
}


Vector<double>&  PolynomialMap::MakePolyVector(const Vector<double>& point,
                                                  Vector<double>& polyVector)
                                                 const {
    for (size_t i = 0; i < index_key_by_vector_.size(); ++i) {
        for (size_t j = 0; j < index_key_by_vector_[i].size(); ++j) {
            polyVector(i+1) *= point(index_key_by_vector_[i][j]+1);
        }
    }
    return polyVector;
}

double*  PolynomialMap::MakePolyVector(const double* point,
                                          double* polyVector) const {
    for (size_t i = 0; i < index_key_by_vector_.size(); ++i) {
        polyVector[i] = 1.;
        for (size_t j = 0; j < index_key_by_vector_[i].size(); ++j)
            polyVector[i] *= point[ index_key_by_vector_[i][j] ];
    }
    return polyVector;
}

// Turn int index into a std::vector<int> 'a' of length 'd' with values
//    x_1^a_1 x_2^a_2 ... x_d^a_d
// e.g. x_1^4 x_2^3 = {4,3,0,0}
std::vector<int> PolynomialMap::IndexByPower
                                              (int index, int nInputVariables) {
    std::vector<int> powerIndex(nInputVariables, 0);
    std::vector<int> vectorIndex = IndexByVector(index, nInputVariables);
    for (size_t i = 0; i < vectorIndex.size(); ++i) {
        powerIndex[vectorIndex[i]]++;
    }
    return powerIndex;
}

// Turn int index into an index for element of polynomial
// e.g. x_1^4 x_2^3 = {1,1,1,1,2,2,2}
std::vector<int> PolynomialMap::IndexByVector
                                              (int index, int nInputVariables) {
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
    int n = 0;
    if (order <= 0) return 0;
    for (int i = 1; i < order; ++i) {
        n += gsl_sf_choose(pointDimension+i-1, i);
    }
    return n+1;
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
    throw(Squeal
           (Squeal::recoverable, "No input values", "PolynomialMap::Means"));
  }
  if (values[0].size() < 1) {
    throw(Squeal
            (Squeal::recoverable, "Dimension < 1", "PolynomialMap::Means"));
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
    throw(Squeal(Squeal::recoverable,
                 "No input values",
                 "PolynomialMap::Covariances()"));
  }
  size_t dim = values[0].size();
  if (dim < 1) {
    throw(Squeal(Squeal::recoverable,
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
    } else { throw(Squeal(Squeal::recoverable,
                    "Input variable not found in space transform",
                    "PolynomialMap::PolynomialCoefficient::SpaceTransform"));
    }
  }
  if (mapping.find(_outVar) != mapping.end()) {
    _outVar = mapping[_outVar];
  } else {
    throw(Squeal(Squeal::recoverable,
                 "Output variable not found in space transform",
                 "PolynomialMap::PolynomialCoefficient::SpaceTransform"));
  }
  _inVarByVec = in_variables;
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
  int max_point = n_points_per_dim/2;
  if ( 2*(n_points_per_dim/2) != n_points_per_dim ) // odd n_points
    max_point = (n_points_per_dim-1)/2;

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

}  // namespace MAUS
