#include "math/MMatrix.hh"
#include "math/PolynomialVectorFactory.hh"
#include "math/SquarePolynomialVector.hh"

#ifndef SolveFactory_hh
#define SolveFactory_hh

class SolveFactory : public PolynomialVectorFactory {
  public:
    SolveFactory(int polynomial_order,
                 int smoothing_order,
                 int point_dim,
                 int value_dim,
                 std::vector< std::vector<double> > positions,
                 std::vector< std::vector<double> > deriv_positions,
                 std::vector< std::vector<int> >& deriv_indices);
    ~SolveFactory() {}
    SquarePolynomialVector* PolynomialSolve(
             const std::vector< std::vector<double> >& positions,
             const std::vector< std::vector<double> >& values,
             const std::vector< std::vector<double> > &deriv_positions,
             const std::vector< std::vector<double> >& deriv_values,
             const std::vector< std::vector<int> >& deriv_indices);

    std::vector<double> MakeSquareVector(std::vector<double> x);
    std::vector<double> MakeSquareDerivVector(std::vector<double> positions, std::vector<int> deriv_indices, int upper);

  private:
    void BuildHInvMatrix(std::vector< std::vector<double> > positions,
                         std::vector< std::vector<double> > deriv_positions,
                         std::vector< std::vector<int> >& deriv_indices);

    int polynomial_order_;
    int smoothing_order_;
    int n_poly_coeffs_;
    std::vector< std::vector<int> > square_points_;
    std::vector< std::vector<int> > square_deriv_nearby_points_;

    SquarePolynomialVector square_temp_;
    MMatrix<double> h_inv_;

};

#endif // SolveFactory_hh


