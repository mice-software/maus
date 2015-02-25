
#include <vector>

#include "math/Mesh.hh"
#include "math/SquarePolynomialVector.hh"

class PolynomialPatch;

class PPSolveFactory {
  public:
    PPSolveFactory(Mesh* points,
                   std::vector<std::vector<double> > values,
                   int poly_patch_order,
                   int smoothing_order);
    ~PPSolveFactory() {}

    PolynomialPatch* Solve();

    /** Get nearby points in a square pattern
     *
     *  Get nearby points at least poly_order_lower distance away and less than
     *  poly_order_upper distance away; e.g. GetNearbyPointsSquares(2, 4, 6)
     *  will get nearby points on a 2D grid at least 4 grid points and at most
     *  6 grid points, like:
     *
     *  x x x o o x ...
     *  x x x o o x
     *  x x x o o x
     *  o o o o o x
     *  o o o o o x
     *  x x x x x x
     *  ...
     */
    static std::vector<std::vector<int> > GetNearbyPointsSquares(int point_dim, int poly_order_lower, int poly_order_upper);

    static std::vector<std::vector<int> > GetNearbyPointsTriangles(int point_dim, int poly_order_lower, int poly_order_upper);

    PolynomialPatch* LeastSquaresFit(
                                Mesh* points,
                                std::vector<std::vector<double> > values,
                                size_t polynomial_order);


  private:
    void GetPoints();
    void GetValues(Mesh::Iterator it);
    void GetDerivPoints();
    void GetDerivs(Mesh::Iterator it);

    std::vector<double> OutOfBoundsPosition(Mesh::Iterator out_of_bounds_it);
    static void NearbyPointsRecursive(std::vector<int> check, size_t check_index, size_t poly_power, std::vector<std::vector<int> >& nearby_points);

    std::vector<SquarePolynomialVector::PolynomialCoefficient> GetConstraints(size_t pos_dim, size_t value_dim, size_t poly_patch_order);
    SquarePolynomialVector::PolynomialCoefficient GetDeltaIterator(Mesh::Iterator it1, Mesh::Iterator it2, int valueIndex);

    int poly_patch_order_;
    int smoothing_order_;
    int value_dim_;
    Mesh* poly_mesh_;
    Mesh* points_;
    std::vector<std::vector<double> > values_;
    std::vector<SquarePolynomialVector*> polynomials_;

    std::vector< std::vector<double> > this_points_;
    std::vector< std::vector<double> > this_values_;
    std::vector< std::vector<double> > deriv_points_;
    std::vector< std::vector<double> > deriv_values_;
    std::vector< std::vector<int> > deriv_indices_;
    std::vector<int> deriv_index_by_power_;

    std::vector<std::vector<std::vector<int> > > edge_points_;
    std::vector< std::vector<int> > smoothing_points_;
    std::vector<MVector<double> > deriv_pows_;

};

