/** Patch together several polynomial vectors; make a lookup table
 */

#include "math/PolynomialVector.hh"
#include "math/VectorMap.hh"
#include "math/Mesh.hh"

#ifndef PolynomialPatch_hh
#define PolynomialPatch_hh

class PolynomialPatch : public VectorMap {
public:
    PolynomialPatch(Mesh* grid_points_,
                    std::vector<PolynomialVector*> polynomials_);
    PolynomialPatch();
	  ~PolynomialPatch();
    virtual void  F(const double* point, double* value) const;

    static PolynomialPatch* LeastSquaresFit(
                                Mesh* points,
                                std::vector<std::vector<double> > values,
                                size_t polynomial_order);
    static PolynomialPatch* Solve(
                                Mesh* points,
                                std::vector<std::vector<double> > values,
                                int poly_patch_order,
                                int smoothing_order,
                                bool squares);

    static void SolverGetDerivs(int poly_patch_order,
                                      int smoothing_order,
                                      int value_dim,
                                      Mesh::Iterator it,
                                      Mesh* points,
                                      std::vector<PolynomialVector*> polynomials,
                                      std::vector< std::vector<double> > &deriv_points,
                                      std::vector< std::vector<double> >& deriv_values,
                                      std::vector< std::vector<int> >& deriv_indices);

    static void SolverGetDerivs2(int poly_patch_order,
                                      int smoothing_order,
                                      int value_dim,
                                      Mesh::Iterator it,
                                      std::vector<PolynomialVector*> polynomials,
                                      std::vector< std::vector<double> > &deriv_points,
                                      std::vector< std::vector<double> >& deriv_values,
                                      std::vector< std::vector<int> >& deriv_indices);

    static void SolverGetValues(int poly_patch_order,
                                      int value_dim,
                                      Mesh::Iterator it,
                                      Mesh* points,
                                      std::vector<std::vector<double> > values,
                                      std::vector< std::vector<double> >& this_points,
                                      std::vector< std::vector<double> >& this_values);

    inline unsigned int PointDimension() const {return point_dimension_;}
    inline unsigned int ValueDimension() const {return value_dimension_;}
  	PolynomialPatch* Clone() const; //copy function

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
    PolynomialVector* GetPolynomialVector(const double* point) const;

private:

    static void NearbyPointsRecursive(std::vector<int> check, size_t check_index, size_t poly_power, std::vector<std::vector<int> >& nearby_points);
    Mesh* grid_points_;
    std::vector<PolynomialVector*> points_;
    unsigned int point_dimension_;
    unsigned int value_dimension_;

    PolynomialPatch(const PolynomialPatch& rhs);
    PolynomialPatch& operator=(const PolynomialPatch& rhs);
};

#endif // PolynomialPatch_hh


