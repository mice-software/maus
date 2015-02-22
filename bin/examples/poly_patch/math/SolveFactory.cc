#include "math/PolynomialPatch.hh"
#include "math/SolveFactory.hh"

SolveFactory::SolveFactory(int polynomial_order, int smoothing_order, bool use_squares)
  : polynomial_order_(polynomial_order), smoothing_order_(smoothing_order), squares_(use_squares) {
}

std::vector<double> SolveFactory::MakeSquareVector(std::vector<double> x,
                                                   int lower,
                                                   int upper) {
    std::vector< std::vector<int> > nearby_points = PolynomialPatch::GetNearbyPointsSquares(x.size(), lower, upper);
    std::vector<double> square_vector(nearby_points.size(), 1.);
    for (size_t i = 0; i < nearby_points.size(); ++i) {
        std::vector<int> point = nearby_points[i];
        for (size_t j = 0; j < point.size(); ++j)
            for (size_t k = 0; k < point[j]; ++k)
                square_vector[i] *= x[j];
    }
    return square_vector;
}

std::vector<double> SolveFactory::MakeSquareDerivVector(std::vector<double> positions, std::vector<int> deriv_indices, int upper) {
    std::vector< std::vector<int> > nearby_points = PolynomialPatch::GetNearbyPointsSquares(positions.size(), -1, upper);
    std::vector<double> deriv_vec(nearby_points.size(), 1.);
    for (size_t i = 0; i < nearby_points.size(); ++i) {
        for (int j = 0; j < nearby_points[i].size(); ++j) {
            int power = nearby_points[i][j] - deriv_indices[j]; // p_j - q_j
            if (power < 0) {
                deriv_vec[i] = 0.;
            } else {
                // x^(p_j-q_j)
                for (int k = 0; k < power; ++k) {
                    deriv_vec[i] *= positions[j];
                }
            }
            // p_j*(p_j-1)*(p_j-2)*...*(p_j-q_j)
            for (int k = nearby_points[i][j]; k > power; --k) {
                deriv_vec[i] *= k;
            }
        }
    }
    return deriv_vec;
}

MMatrix<double> convert_A_square_to_A_triangle(int point_dim, int square_order, MMatrix<double> A_square) {
    int triangle_order = square_order*point_dim+1;
    std::vector< std::vector<int> > triangle_points = PolynomialPatch::GetNearbyPointsTriangles(point_dim, 0, triangle_order);
    std::vector< std::vector<int> > square_points = PolynomialPatch::GetNearbyPointsSquares(point_dim, -1, square_order);
    MMatrix<double> A_triangle(A_square.num_row(), triangle_points.size(), 0.);
    for (size_t i = 0; i < triangle_points.size(); ++i) {
        for (size_t j = 0; j < square_points.size(); ++j) {
            bool is_equal = true;
            for (size_t k = 0; k < square_points[j].size() && is_equal; ++k)
                is_equal &= square_points[j][k] == triangle_points[i][k];
            if (is_equal) {
                for (size_t k = 0; k < A_square.num_row(); ++k)
                    A_triangle(k+1, i+1) = A_square(k+1, j+1);
                break;
            }
        }
    }
    return A_triangle;
}

PolynomialVector* SolveFactory::PolynomialSolve(
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

    int nPolyCoeffs = 1;
    if (squares_) {
        for (int i = 0; i < pointDim; ++i)
            nPolyCoeffs *= smoothing_order_+1;
    } else {
       nPolyCoeffs = PolynomialVector::NumberOfPolynomialCoefficients(pointDim, smoothing_order_);
    }
    if (deriv_positions.size()+positions.size() != size_t(nPolyCoeffs)) {
        std::cerr << "size " << deriv_positions.size()+positions.size() << " n_coeffs: " << nPolyCoeffs << std::endl;
        throw MAUS::Exception(
              MAUS::Exception::recoverable,
              "Positions and derivatives over or under constrained",
              "SolveFactory::PolynomialSolve"
        );
    }
    for (size_t i = 0; i < positions.size(); ++i) {
        for (size_t j = 0; j < positions[i].size(); ++j) {
            //std::cerr << positions[i][j] << " ";
        }
        //std::cerr << std::endl;
    }

    MMatrix<double> A(valueDim, nPolyCoeffs, 0.);
    PolynomialVector* temp = new PolynomialVector(pointDim, A);
    for (size_t y_index = 0; y_index < values[0].size(); ++y_index) {
        MVector<double> G(nPolyCoeffs, 0.);
        MMatrix<double> H(nPolyCoeffs, nPolyCoeffs, 0.);
        // First fill the values
        for (int i = 0; i < nCoeffs && i < nPolyCoeffs; ++i) {
            G(i+1) = values[i][y_index];
            std::vector<double> poly_vec(nPolyCoeffs, 0.);
            if (squares_) {
                poly_vec = MakeSquareVector(positions[i], -1, smoothing_order_);
            } else {
                temp->MakePolyVector(&positions[i][0], &poly_vec[0]);
            }
            for (int j = 0; j < nPolyCoeffs; ++j) {
                H(i+1, j+1) = poly_vec[j];
            }
        }
        // Now fill the derivatives
        for (int i = 0; i < nDerivs; ++i) {
            G(i+nCoeffs+1) = deriv_values[i][y_index];
            std::vector<double> deriv_vec;
            if (squares_) {
                deriv_vec = MakeSquareDerivVector(deriv_positions[i], deriv_indices[i], smoothing_order_);
            } else {
                throw("Not implemented");
            }
            for (int j = 0; j < nPolyCoeffs; ++j) {
                H(i+1+nCoeffs, j+1) = deriv_vec[j];
            }
        }
        try {
            std::cerr << "\nH Matrix\n" << H << "\n" << std::flush << H.determinant() << std::endl;
            std::cerr << "\nG Matrix\n" << G << "\n" << std::endl;
            H.invert();
        } catch (MAUS::Exception exc) {
            delete temp;
            throw exc;
        }
        MVector<double> A_vec = H*G;
        // ERROR - mismatch between Square coefficients and triangular coefficients 
        for (int j = 0; j < nPolyCoeffs; ++j) {
            A(y_index+1, j+1) = A_vec(j+1);
        }
    }
    std::cerr << "\nA Square\n" << A << "\n" << std::endl;
    A = convert_A_square_to_A_triangle(pointDim, smoothing_order_, A);
    std::cerr << "\nA Triangle\n" << A << "\n" << std::endl;
    delete temp;
    temp = new PolynomialVector(pointDim, A);
    std::cerr << "Allocate poly vector " << temp << std::endl;
    return temp;
}
