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
                    std::vector<SquarePolynomialVector*> polynomials_);
    PolynomialPatch();
	  ~PolynomialPatch();
    virtual void  F(const double* point, double* value) const;


    inline unsigned int PointDimension() const {return point_dimension_;}
    inline unsigned int ValueDimension() const {return value_dimension_;}
  	PolynomialPatch* Clone() const; //copy function
    SquarePolynomialVector* GetPolynomialVector(const double* point) const;

  private:
    Mesh* grid_points_;
    std::vector<SquarePolynomialVector*> points_;
    unsigned int point_dimension_;
    unsigned int value_dimension_;

    PolynomialPatch(const PolynomialPatch& rhs);
    PolynomialPatch& operator=(const PolynomialPatch& rhs);
};

#endif // PolynomialPatch_hh


