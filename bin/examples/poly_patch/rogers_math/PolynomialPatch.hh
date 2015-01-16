/** Patch together several polynomial vectors; make a lookup table
 */

#include "rogers_math/PolynomialVector.hh"
#include "rogers_math/VectorMap.hh"
#include "rogers_math/Mesh.hh"

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
                                int polynomial_order,
                                int smoothing_order);
    inline unsigned int PointDimension() const {return point_dimension_;}
    inline unsigned int ValueDimension() const {return value_dimension_;}
  	PolynomialPatch* Clone() const; //copy function
    static std::vector<std::vector<int> > GetNearbyPoints(int point_dimension, int value_dimension, int npoints);
    PolynomialVector* GetPolynomialVector(const double* point) const;

private:

    static void NearbyPointsRecursive(std::vector<int> check, size_t k, std::vector<std::vector<int> >& nearby_points);
    Mesh* grid_points_;
    std::vector<PolynomialVector*> points_;
    unsigned int point_dimension_;
    unsigned int value_dimension_;

    PolynomialPatch(const PolynomialPatch& rhs);
    PolynomialPatch& operator=(const PolynomialPatch& rhs);
};


