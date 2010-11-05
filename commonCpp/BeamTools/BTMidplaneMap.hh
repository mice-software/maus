//BTFieldGroup contains and places groups of fields
//Generic but intended to hold field data for e.g. a module in MICE

#ifndef BTMULTIPOLE_HH
#define BTMULTIPOLE_HH

#include <vector>
#include <iostream>

#include "BTField.hh"

class BTMidplaneMap : public BTField
{
public:
	BTMidplaneMap(std::string filename, std::string format="g4mice");
	~BTMidplaneMap() {;}

	//GetFieldValue at a Point relative to multipole start
	void GetFieldValue( const double Point[4], double *EMfield ) const;

	//Utility methods
  //	void            Print(std::ostream &out) const;
	BTMidplaneMap * Clone() const {return new BTMidplaneMap(*this);}

private:
	void   QuarticInterpolation(const double point[4], double B[3], int axis) const;
	void   BiQuarticCoefficients(double f[16], double dfdx[16], double dfdz[16], double d2fdxdz[16], double* coeffOut) const;

	void   CubicInterpolation(const double point[4], double B[3], int axis) const; //numerical recipes formula
	void   BiCubicCoefficients(double f[4], double dfdx[4], double dfdz[4], double d2fdxdz[4], double* coeffOut) const;

	double Extrapolation(const double point[4], int axis) const {return 0;}

	int       numberXCoords;
	int       numberZCoords;
	double    x0;
	double    z0;
	double    xLength;
	double    zLength;
	double    xzProduct;

	//field and derivatives
	double **** _field; //index 1 is axis (x,z); index 2 is derivative(f, dfdx, dfdy, d2fdxdy); index 3, 4 is x, z position in grid
	static const int _w[16][16];
};

#endif

