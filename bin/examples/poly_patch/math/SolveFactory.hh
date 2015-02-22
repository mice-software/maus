#include "math/PolynomialVectorFactory.hh"
#include "math/PolynomialVector.hh"

#ifndef SolveFactory_hh
#define SolveFactory_hh

class SolveFactory : public PolynomialVectorFactory {
  public:
    SolveFactory(int polynomial_order, int smoothing_order, bool use_squares);
    ~SolveFactory() {}
    PolynomialVector* PolynomialSolve(
             const std::vector< std::vector<double> >& positions,
             const std::vector< std::vector<double> >& values,
             const std::vector< std::vector<double> > &deriv_positions,
             const std::vector< std::vector<double> >& deriv_values,
             const std::vector< std::vector<int> >& deriv_indices);

    static std::vector<double> MakeSquareVector(std::vector<double> x, int lower, int upper);
    static std::vector<double> MakeSquareDerivVector(std::vector<double> positions, std::vector<int> deriv_indices, int upper);

  private:
    int polynomial_order_;
    int smoothing_order_;
    bool squares_;
};

#endif // SolveFactory_hh


