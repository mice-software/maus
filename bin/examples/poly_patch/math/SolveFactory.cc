#include <gsl/gsl_sf_pow_int.h>

#include "math/PPSolveFactory.hh"
#include "math/SolveFactory.hh"

SolveFactory::SolveFactory(int polynomial_order,
                           int smoothing_order,
                           int point_dim,
                           int value_dim,
                           std::vector< std::vector<double> > positions,
                           std::vector< std::vector<double> > deriv_positions,
                           std::vector< std::vector<int> >& deriv_indices)
  : polynomial_order_(polynomial_order), smoothing_order_(smoothing_order) {
    n_poly_coeffs_ = SquarePolynomialVector::NumberOfPolynomialCoefficients(point_dim, smoothing_order);
    square_points_ = PPSolveFactory::GetNearbyPointsSquares(point_dim, -1, smoothing_order);
    square_deriv_nearby_points_ = PPSolveFactory::GetNearbyPointsSquares(point_dim, -1, smoothing_order);
    BuildHInvMatrix(positions, deriv_positions, deriv_indices);
    MMatrix<double> A_temp(value_dim, n_poly_coeffs_, 0.);
    square_temp_ = SquarePolynomialVector(point_dim, A_temp);
}

void SolveFactory::BuildHInvMatrix(
                           std::vector< std::vector<double> > positions,
                           std::vector< std::vector<double> > deriv_positions,
                           std::vector< std::vector<int> >& deriv_indices) {
    int nCoeffs = positions.size();
    h_inv_ = MMatrix<double>(n_poly_coeffs_, n_poly_coeffs_, 0.);
    for (int i = 0; i < nCoeffs; ++i) {
        std::vector<double> poly_vec = MakeSquareVector(positions[i]);
        for (int j = 0; j < n_poly_coeffs_; ++j) {
            h_inv_(i+1, j+1) = poly_vec[j];
        }
    }
    for (int i = 0; i < deriv_positions.size(); ++i) {
        std::vector<double> deriv_vec = MakeSquareDerivVector(deriv_positions[i],
                                                              deriv_indices[i],
                                                              smoothing_order_);
        for (int j = 0; j < n_poly_coeffs_; ++j) {
            h_inv_(i+1+nCoeffs, j+1) = deriv_vec[j];
        }
    }
    h_inv_.invert();
}

std::vector<double> SolveFactory::MakeSquareVector(std::vector<double> x) {
    std::vector<double> square_vector(square_points_.size(), 1.);
    for (size_t i = 0; i < square_points_.size(); ++i) {
        std::vector<int>& point = square_points_[i];
        for (size_t j = 0; j < point.size(); ++j)
            square_vector[i] *= gsl_sf_pow_int(x[j], point[j]);
    }
    return square_vector;
}

std::vector<double> SolveFactory::MakeSquareDerivVector(std::vector<double> positions, std::vector<int> deriv_indices, int upper) {
    std::vector<double> deriv_vec(square_deriv_nearby_points_.size(), 1.);
    int square_deriv_nearby_points_size = square_deriv_nearby_points_.size();
    int dim = square_deriv_nearby_points_[0].size();
    for (int i = 0; i < square_deriv_nearby_points_size; ++i) {
        for (int j = 0; j < dim; ++j) {
            int power = square_deriv_nearby_points_[i][j] - deriv_indices[j]; // p_j - q_j
            if (power < 0) {
                deriv_vec[i] = 0.;
                break;
            } else {
                // x^(p_j-q_j)
                deriv_vec[i] *= gsl_sf_pow_int(positions[j], power);
            }
            // p_j*(p_j-1)*(p_j-2)*...*(p_j-q_j)
            for (int k = square_deriv_nearby_points_[i][j]; k > power; --k) {
                deriv_vec[i] *= k;
            }
        }
    }
    return deriv_vec;
}

SquarePolynomialVector* SolveFactory::PolynomialSolve(
         const std::vector< std::vector<double> >& positions,
         const std::vector< std::vector<double> >& values,
         const std::vector< std::vector<double> > &deriv_positions,
         const std::vector< std::vector<double> >& deriv_values,
         const std::vector< std::vector<int> >& deriv_indices) {

    // Algorithm:
    // define G_i = vector of F_i and d^pF/dF^p with values taken from coeffs
    // and derivs respectively
    // define H_ij = vector of f_i and d^pf/df^p)
    // Then use G = H A => A = H^-1 G
    // where A is vector of polynomial coefficients
    // First set up G_i from coeffs and derivs; then calculate H_ij;
    // then do the matrix inversion
    // It is an error to include the same polynomial index more than once in
    // coeffs or derivs or both; any polynomial indices that are missing will be
    // assigned 0 value; polynomial order will be taken as the maximum
    // polynomial order from coeffs and derivs.
    // PointDimension and ValueDimension will be taken from coeffs and derivs;
    // it is an error if these do not all have the same dimensions.
    
    // OPTIMISATION - if we are doing this many times and only changing values,
    // can reuse H
    int nCoeffs = values.size();
    int nDerivs = deriv_values.size();

    if (positions.size() != values.size())
        throw MAUS::Exception(MAUS::Exception::recoverable,
              "Positions misaligned with values",
              "SolveFactory::PolynomialSolve"
        );
    if (deriv_positions.size() != deriv_values.size() ||
        deriv_positions.size() != deriv_indices.size())
        throw MAUS::Exception(MAUS::Exception::recoverable,
              "Derivative positions misaligned with values and indices",
              "SolveFactory::PolynomialSolve"
        );
    int pointDim = 0;
    int valueDim = 0;
    if (positions.size() > 0) {
        pointDim = positions[0].size();
        valueDim = values[0].size();
    } else if (deriv_positions.size() > 0) {
        pointDim = deriv_positions[0].size();
        valueDim = deriv_values[0].size();
    } else {
        throw MAUS::Exception(MAUS::Exception::recoverable,
              "No point data - can't solve!",
              "SolveFactory::PolynomialSolve"
        );
    }

    if (deriv_positions.size()+positions.size() != size_t(n_poly_coeffs_)) {
        throw MAUS::Exception(
              MAUS::Exception::recoverable,
              "Positions and derivatives over or under constrained",
              "SolveFactory::PolynomialSolve"
        );
    }
    for (size_t i = 0; i < positions.size(); ++i) {
        for (size_t j = 0; j < positions[i].size(); ++j) {
        }
    }

    MMatrix<double> A(valueDim, n_poly_coeffs_, 0.);
    for (size_t y_index = 0; y_index < values[0].size(); ++y_index) {
        MVector<double> G(n_poly_coeffs_, 0.);
        // First fill the values
        for (int i = 0; i < nCoeffs && i < n_poly_coeffs_; ++i) {
            G(i+1) = values[i][y_index];
        }
        // Now fill the derivatives
        for (int i = 0; i < nDerivs; ++i) {
            G(i+nCoeffs+1) = deriv_values[i][y_index];
        }
        MVector<double> A_vec = h_inv_*G;
        for (int j = 0; j < n_poly_coeffs_; ++j) {
            A(y_index+1, j+1) = A_vec(j+1);
        }
    }
    SquarePolynomialVector* temp = new SquarePolynomialVector(square_temp_);
    temp->SetCoefficients(A);
    return temp;
}
