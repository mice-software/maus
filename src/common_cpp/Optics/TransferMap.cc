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

#include "src/common_cpp/Optics/TransferMap.hh"

#include "CLHEP/Units/PhysicalConstants.h"

#include "Maths/PolynomialVector.hh"
#include "src/common_cpp/Optics/CovarianceMatrix.hh"
#include "src/common_cpp/Optics/PhaseSpaceVector.hh"

namespace MAUS {
// ##############################
//  TransferMap public
// ##############################

// ******************************
//  Constructors
// ******************************

TransferMap::TransferMap(const PolynomialVector& polynomial,
                         const PhaseSpaceVector& reference_trajectory_in,
                         const PhaseSpaceVector& reference_trajectory_out)
  : polynomial_(polynomial), reference_trajectory_in_(reference_trajectory_in),
    reference_trajectory_out_(reference_trajectory_out)
{ }

TransferMap::TransferMap(const PolynomialVector& polynomial,
                         const PhaseSpaceVector& reference_trajectory)
  : polynomial_(polynomial), reference_trajectory_in_(reference_trajectory),
    reference_trajectory_out_(reference_trajectory)
{ }

TransferMap::TransferMap(const TransferMap& original_instance)
  : polynomial_(original_instance.polynomial_),
    reference_trajectory_in_(original_instance.reference_trajectory_in_),
    reference_trajectory_out_(original_instance.reference_trajectory_out_)
{ }

/**
 *  Name: operator*(CovarianceMatrix const &)
 */
CovarianceMatrix TransferMap::operator*(const CovarianceMatrix& covariances)
    const {
  Matrix<double> transfer_matrix = CreateTransferMatrix();

  Matrix<double> transfer_matrix_transpose = transpose(transfer_matrix);

  CovarianceMatrix transformed_covariances(
    transfer_matrix * covariances * transfer_matrix_transpose);

  return transformed_covariances;
}

/**
 *  Name: operator*(PhaseSpaceVector const &)
 */
PhaseSpaceVector TransferMap::operator*(const PhaseSpaceVector& input_vector)
    const {
  // subtract off the input reference trajectory to obtain phase space delta
  Vector<double> phase_space_delta(input_vector - reference_trajectory_in_);

  // operate on the phase space delta with the polynomial map
  Vector<double> transformed_delta;
  polynomial_.F(phase_space_delta, transformed_delta);

  // add the transformed phase space delta to the output reference trajectory
  PhaseSpaceVector output_vector
    = PhaseSpaceVector(reference_trajectory_out_ + transformed_delta);

  return output_vector;
}

// ******************************
//  First-order Map Functions
// ******************************

Matrix<double> TransferMap::CreateTransferMatrix() const {
  return polynomial_.GetCoefficientsAsMatrix().submatrix(1, 6, 2, 6);
}

// ##############################
//  Free Functions
// ##############################

// ******************************
//  Streaming
// ******************************

std::ostream& operator<<(std::ostream& out, const TransferMap& tm) {
  out  << "Incomming Reference Trajectory: "
      << tm.reference_trajectory_in_ << std::endl
      << tm.reference_trajectory_out_ << std::endl
      << tm.polynomial_ << std::endl;

  return out;
}
}  // namespace MAUS
