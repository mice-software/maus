/* This file is part of MAUS: http://   micewww.pp.rl.ac.uk:8080/projects/maus
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
 * along with MAUS.  If not, see <http://   www.gnu.org/licenses/>.
 */

/* Author: Peter Lane
 */

#include "src/common_cpp/Optics/CovarianceMatrix.hh"

#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include "TMatrixDSym.h"

#include "Utils/Exception.hh"
#include "Maths/Matrix.hh"
#include "Maths/SymmetricMatrix.hh"

namespace MAUS {

using MAUS::Matrix;
using ::CLHEP::c_light;

// ############################
//  Free Functions
// ############################

// ****************************
//  Conversion Functions
// ****************************

CovarianceMatrix rotate(const CovarianceMatrix& covariances,
                        const double angle) {
  double fcos = ::cos(angle);
  double fsin = ::sin(angle);
  double rotation_array[36] = {
    1.,    0.,    0.,    0.,    0.,    0.,
    0.,    1.,    0.,    0.,    0.,    0.,
    0.,    0.,    fcos,  0.,    fsin,  0.,
    0.,    0.,    0.,    fcos,  0.,    fsin,
    0.,    0.,    -fsin, 0.,    fcos,  0.,
    0.,    0.,    0.,    -fsin, 0.,    fcos
  };
  const Matrix<double> rotation(6, 6, rotation_array);
  const Matrix<double> rotation_transpose = transpose(rotation);

  // the orthogonal similarity transform of a symmetric matrix is also symmetric
  CovarianceMatrix rotated_covariances(
    rotation * covariances * rotation_transpose);

  return rotated_covariances;
}

// ############################
//  CovarianceMatrix
// ############################

// ****************************
//  Constructors
// ****************************

CovarianceMatrix::CovarianceMatrix() : SymmetricMatrix(6)
{ }

CovarianceMatrix::CovarianceMatrix(const CovarianceMatrix& original_instance)
    : SymmetricMatrix((SymmetricMatrix) original_instance)
{ }

CovarianceMatrix::CovarianceMatrix(const Matrix<double>& matrix)
    : SymmetricMatrix() {
  if (   (matrix.number_of_rows() < 6)
      || (matrix.number_of_columns() < 6)) {
    throw(Exceptions::Exception(Exceptions::recoverable,
                 "Attempted to construct with a Matrix<double> containing "
                 "fewer than six rows and/or columns",
                 "CovarianceMatrix::CovarianceMatrix(Matrix<double>)"));
  }
  build_matrix(6);
  for (size_t row = 1; row <= 6; ++row) {
    for (size_t column = 1; column <= row; ++column) {
      set(row, column, matrix(row, column));
    }
  }
}

CovarianceMatrix::CovarianceMatrix(const SymmetricMatrix& symmetric_matrix)
    : SymmetricMatrix() {
  if (   (symmetric_matrix.number_of_rows() < 6)
      || (symmetric_matrix.number_of_columns() < 6)) {
    throw(Exceptions::Exception(Exceptions::recoverable,
          "Attempted to construct with a SymmetricMatrix containing "
          "fewer than six rows/columns",
          "CovarianceMatrix::CovarianceMatrix(SymmetricMatrix<double>)"));
  }
  build_matrix(6);
  for (size_t row = 1; row <= 6; ++row) {
    for (size_t column = 1; column <= row; ++column) {
      set(row, column, symmetric_matrix(row, column));
    }
  }
}

CovarianceMatrix::CovarianceMatrix(const ::CLHEP::HepSymMatrix& hep_sym_matrix)
    : SymmetricMatrix() {
  if (   (hep_sym_matrix.num_row() < 6)
      || (hep_sym_matrix.num_col() < 6)) {
    throw(Exceptions::Exception(Exceptions::recoverable,
          "Attempted to construct with a HepSymMatrix containing fewer "
          "than six rows/columns",
          "CovarianceMatrix::CovarianceMatrix(CLHEP::HepSymMatrix<double>)"));
  }
  build_matrix(6);
  double element;
  for (size_t row = 1; row <= 6; ++row) {
    for (size_t column = 1; column <= row; ++column) {
      element = hep_sym_matrix(row, column);
      Matrix<double>::operator()(row, column) = element;
      Matrix<double>::operator()(column, row) = element;
    }
  }
}

CovarianceMatrix::CovarianceMatrix(const TMatrixDSym& root_sym_matrix)
    : SymmetricMatrix(root_sym_matrix) {
  if (   (root_sym_matrix.GetNrows() < 6)
      || (root_sym_matrix.GetNcols() < 6)) {
    throw(Exceptions::Exception(Exceptions::recoverable,
          "Attempted to construct with a TMatrixDSym containing fewer "
          "than six rows/columns",
          "CovarianceMatrix::CovarianceMatrix(TMatrixDSym<double>)"));
  }
  const double * data = root_sym_matrix.GetMatrixArray();
  build_matrix(6, data);
}

CovarianceMatrix::CovarianceMatrix(double const * const array_matrix)
    : SymmetricMatrix(6, array_matrix)
{ }

/* xboa uses an equation for calculating canonical angular momentum that is
 * different from the standard. This in turn affects Ltwiddle_t. Furthermore,
 * xboa uses a different definition of kappa and sigma_y_Px than G. Penn's
 * paper (this might be an error). It also appears that xboa uses total
 * momentum instead of longitudinal momentum in calculations of the covariances.
 */
const CovarianceMatrix CovarianceMatrix::CreateFromPennParameters(
    double mass,
    double momentum,
    double charge,
    double Bz,
    double Ltwiddle_t,
    double emittance_t,
    double beta_t,
    double alpha_t,
    double emittance_l,
    double beta_l,
    double alpha_l,
    double dispersion_x,
    double dispersion_prime_x,
    double dispersion_y,
    double dispersion_prime_y) {

  // *** calculate some intermediate values ***
  double energy     = ::sqrt(momentum * momentum + mass * mass);
  // this differs from G. Penn's paper
  double kappa      = c_light * Bz / (2. * momentum);
  double gamma_t    = (1 + alpha_t*alpha_t
                       + (beta_t*kappa - Ltwiddle_t)*(beta_t*kappa-Ltwiddle_t))
                    / beta_t;
  double gamma_l    = (1+alpha_l*alpha_l+beta_l*beta_l*kappa*kappa)/beta_l;

  // *** calculate the lower triangle covariances <A B> = sigma_A_B ***
  double sigma_t_t  =  emittance_l * mass * beta_l / momentum;

  double sigma_E_t  = -emittance_l * mass * alpha_l;
  double sigma_E_E  =  emittance_l * mass * gamma_l * momentum;

  double sigma_x_t  =  0.;
  double sigma_x_E  =  dispersion_x * sigma_E_E / energy;
  double sigma_x_x  =  emittance_t * beta_t * mass / momentum;

  double sigma_Px_t =  0.;
  double sigma_Px_E =  dispersion_prime_x * sigma_E_E / energy;
  double sigma_Px_x = -mass * emittance_t * alpha_t;
  double sigma_Px_Px=  mass * momentum * emittance_t * gamma_t;

  double sigma_y_t  =  0.;
  double sigma_y_E  = dispersion_y * sigma_E_E / energy;
  double sigma_y_x  =   0.;
  // this differes from G. Penn's paper
  double sigma_y_Px =  mass * emittance_t
                    * (beta_t*kappa - Ltwiddle_t) * charge;
  double sigma_y_y  =  emittance_t * beta_t * mass / momentum;

  double sigma_Py_t =  0.;
  double sigma_Py_E =  dispersion_prime_y * sigma_E_E / energy;
  double sigma_Py_x = -sigma_y_Px;
  double sigma_Py_Px=  0.;
  double sigma_Py_y =  sigma_Px_x;
  double sigma_Py_Py=  sigma_Px_Px;

  double covariances[36] = {
    sigma_t_t,  0.,         0.,         0.,           0.,         0.,
    sigma_E_t,  sigma_E_E,  0.,         0.,           0.,         0.,
    sigma_x_t,  sigma_x_E,  sigma_x_x,  0.,           0.,         0.,
    sigma_Px_t, sigma_Px_E, sigma_Px_x, sigma_Px_Px,  0.,         0.,
    sigma_y_t,  sigma_y_E,  sigma_y_x,  sigma_y_Px,   sigma_y_y,  0.,
    sigma_Py_t, sigma_Py_E, sigma_Py_x, sigma_Py_Px,  sigma_Py_y, sigma_Py_Py
  };

  return CovarianceMatrix(covariances);
}

const CovarianceMatrix CovarianceMatrix::CreateFromTwissParameters(
      double mass,
      double momentum,
      double emittance_x,
      double beta_x,
      double alpha_x,
      double emittance_y,
      double beta_y,
      double alpha_y,
      double emittance_l,
      double beta_l,
      double alpha_l,
      double dispersion_x,
      double dispersion_prime_x,
      double dispersion_y,
      double dispersion_prime_y) {
  // *** calculate some intermediate values ***
  double energy  = ::sqrt(momentum * momentum + mass * mass);
  double gamma_x = (1+alpha_x*alpha_x)/beta_x;
  double gamma_y = (1+alpha_y*alpha_y)/beta_y;
  double gamma_l = (1+alpha_l*alpha_l)/beta_l;

  // *** calculate the lower triangle covariances <A B> = sigma_A_B ***
  double sigma_t_t  = emittance_l * mass * beta_l / momentum;

  double sigma_E_t  = -emittance_l * mass * alpha_l;
  double sigma_E_E  = emittance_l * mass * gamma_l * momentum;

  double sigma_x_t  = 0.;
  double sigma_x_E  = dispersion_x * sigma_E_E / energy;
  // FIXME(plane1@hawk.iit.edu) Shouldn't this be just beta_x emittance_x?
  double sigma_x_x  = emittance_x * mass * beta_x / momentum;

  double sigma_Px_t = 0.;
  double sigma_Px_E = dispersion_prime_x * sigma_E_E / energy;
  // FIXME(plane1@hawk.iit.edu) Shouldn't this be just -alpha_x emittance_x ?
  double sigma_Px_x = -emittance_x * mass * alpha_x;
  // FIXME(plane1@hawk.iit.edu) Shouldn't this be just gamma_x emittance_x ?
  double sigma_Px_Px= emittance_x * mass * momentum * gamma_x;

  double sigma_y_t  = 0.;
  double sigma_y_E  = dispersion_y * sigma_E_E / energy;
  double sigma_y_x  = 0.;
  double sigma_y_Px = 0.;
  // FIXME(plane1@hawk.iit.edu) Shouldn't this be just beta_y emittance_y?
  double sigma_y_y  = emittance_y * mass * beta_y / momentum;

  double sigma_Py_t = 0.;
  double sigma_Py_E = dispersion_prime_y * sigma_E_E / energy;
  double sigma_Py_x = 0.;
  double sigma_Py_Px= 0.;
  // FIXME(plane1@hawk.iit.edu) Shouldn't this be just -alpha_y emittance_y ?
  double sigma_Py_y = -emittance_y * mass * alpha_y;
  // FIXME(plane1@hawk.iit.edu) Shouldn't this be just gamma_y emittance_y ?
  double sigma_Py_Py= emittance_y * mass * momentum * gamma_y;

  double covariances[36] = {
    sigma_t_t,  0.,         0.,         0.,           0.,         0.,
    sigma_E_t,  sigma_E_E,  0.,         0.,           0.,         0.,
    sigma_x_t,  sigma_x_E,  sigma_x_x,  0.,           0.,         0.,
    sigma_Px_t, sigma_Px_E, sigma_Px_x, sigma_Px_Px,  0.,         0.,
    sigma_y_t,  sigma_y_E,  sigma_y_x,  sigma_y_Px,   sigma_y_y,  0.,
    sigma_Py_t, sigma_Py_E, sigma_Py_x, sigma_Py_Px,  sigma_Py_y, sigma_Py_Py
  };

  return CovarianceMatrix(covariances);
}

bool CovarianceMatrix::IsPositiveDefinite() const {
  size_t min_row = 1;
  size_t min_column = 1;
  size_t rows = size();
  for (size_t length = 1; length <= rows; length++) {
    // Sylvester Criterion - all the leading principle minors must be positive
    if (determinant(submatrix(min_row, length, min_column, length)) <= 0) {
      return false;
    }
  }

  return true;
}
}  // namespace MAUS
