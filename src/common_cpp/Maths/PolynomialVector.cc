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

#include "Maths/PolynomialVector.hh"

#include <math.h>

#include <iomanip>
#include <sstream>
#include <vector>

#include "gsl/gsl_sf_gamma.h"

#include "Interface/Squeal.hh"
#include "Maths/Vector.hh"

namespace MAUS {

PolynomialVector::PolynomialVector(const Vector<double>& point) const {
  for (size_t i = 0; i < index_key_by_vector_.size(); ++i) {
    for (size_t j = 0; j < index_key_by_vector_[i].size(); ++j) {
      polyVector(i+1) *= point(index_key_by_vector_[i][j]+1);
    }
  }
  return polyVector;
}

PolynomialVector::PolynomialVector(const double* point) const {
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
std::vector<int> PolynomialVector::IndexByPower(int index,
                                                int nInputVariables) {
  std::vector<int> powerIndex(nInputVariables, 0);
  std::vector<int> vectorIndex = IndexByVector(index, nInputVariables);
  for (size_t i = 0; i < vectorIndex.size(); ++i) {
      powerIndex[vectorIndex[i]]++;
  }
  return powerIndex;
}

// Turn int index into an index for element of polynomial
// e.g. x_1^4 x_2^3 = {1,1,1,1,2,2,2}
std::vector<int> PolynomialVector::IndexByVector (int index,
                                                  int nInputVariables) {
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

std::ostream& operator<<(std::ostream& out, const PolynomialVector& pv) {
    if (PolynomialVector::print_headers_) {
        pv.PrintHeader(out, '.', ' ', 14, true);
    }
    out << '\n' << pv.GetCoefficientsAsMatrix();
    return out;
}

// ###### PolynomialVectorMap functions ######

PolynomialVectorMap::PolynomialVectorMap(const size_t dimension,
                                         const size_t order)
    : point_dimension_(dimension), polynomial_order_(order) {
  value_dimension_ = 0;
  for (int i = 0; i < order; ++i) {
    value_dimension_
      += BinomialCoefficient::Choose(i + dimension - 1, dimension - 1);
  }

  index_key_by_power_  = std::vector< std::vector<int> >();
  for (int i = 0; i < nPCoeff; ++i)
    index_key_by_power_.push_back(IndexByPower(i, pointDim));

  index_key_by_vector_ = std::vector< std::vector<int> >();
  for (int i = 0; i < nPCoeff; ++i)
    index_key_by_vector_.push_back(IndexByVector(i, pointDim));
}

void PolynomialVectorMap::F(const double* point, double* value) const {
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

void PolynomialVectorMap::F(const Vector<double>& point, Vector<double>& value)
  const {
  // create a vector of point coordinate products that form, together with the
  // coefficents, the polynomial terms
  Vector<double> polynomial_vector(index_key_by_vector_.size(), 1.);
  MakePolyVector(point, polynomial_vector);
  value = coefficient_matrix_ * polynomial_vector;
}

unsigned int PolynomialVectorMap::PointDimension() const {
  return point_dimension_;
}

unsigned int PolynomialVectorMap::ValueDimension() const {
  return coefficient_matrix_.number_of_rows();
}

unsigned int PolynomialVectorMap::PolynomialOrder() const {
  if (index_key_by_vector_.size() > 0) {
    return index_key_by_vector_.back().size()+1;
  } else {
    return 0;
  }
}

PolynomialVectorMap * PolynomialVectorMap::Clone() const {
  return new PolynomialVectorMap(*this);
}

}  // namespace MAUS
