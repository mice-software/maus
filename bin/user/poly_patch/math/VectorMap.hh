// MAUS WARNING: THIS IS LEGACY CODE.
//Interpolator definitions
//A robust framework for arbitrary interpolation
//Thinking about field maps but should be extensible to other interpolation types
//designed to permit implementation of interpolations that e.g. see Maxwell's laws 
//or e.g. have dynamically sized grids with minimal reworking of user code 
//and optimisable for speed

#ifndef INTERPOLATOR_HH
#define INTERPOLATOR_HH

#include <math.h>
#include <vector>

#include "gsl/gsl_sf_gamma.h"

#include "math/Mesh.hh"

///// VectorMap  /////

//Abstract base class
//Maps from arbitrary length array of doubles to arbitrary length array of double e.g. map position vector to field vector
class VectorMap
{
public:
	//return map value; vectors NOT checked for size
	virtual void  F    (const double*   point,       double* value)              const = 0;
	virtual void  F    (const Mesh::Iterator& point, double* value)              const //overload if mesh::pointdimension != vectormap::pointdimension 
	{double* PointA = new double[this->PointDimension()]; point.Position(PointA); F(PointA, value); delete PointA;}
  //Compare the length of in and out; if in is longer, append new items in out until they are the same length NOT TESTED YET
  virtual void FAppend(const std::vector< std::vector<double> >& point_vec, std::vector< std::vector<double> >& value_vec) const;
	//Checks for self-consistency
	inline virtual bool  CheckPoint(const std::vector<double>& point) const {return (point.size() == PointDimension());}
	inline virtual bool  CheckValue(const std::vector<double>& value) const {return (value.size() == ValueDimension());}
	//Tell me the required dimension of the input point and output value
	virtual unsigned int PointDimension()         const = 0; //would like to make static! But can't inherit static functions
	virtual unsigned int ValueDimension()         const = 0;
	//Read and write operations
	virtual              VectorMap* Clone()       const = 0; //copy function
	virtual             ~VectorMap() {;}

	//If VectorMap uses a Mesh return it else return NULL
	virtual Mesh*        GetMesh()                const {return NULL;}
private:
};


#endif
