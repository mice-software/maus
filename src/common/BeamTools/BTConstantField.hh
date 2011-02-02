#ifndef BTCONSTANTFIELD_HH
#define BTCONSTANTFIELD_HH

#include "BTField.hh"
#include "CLHEP/Vector/ThreeVector.h"

class BTConstantField: public BTField
{
public:
	//First constructor for cylinders, second for cuboids
	BTConstantField(double length, double radius, CLHEP::Hep3Vector field); 
	BTConstantField(double length, double width,  double height, CLHEP::Hep3Vector field);
	//Modified Copy for inheritance
	BTConstantField * Clone() const {return new BTConstantField(*this);}
	//Put _field into bfield if inside the volume
	void GetFieldValue(const double Point[4], double * bfield) const;
	//Print out member data
	void Print(std::ostream & out) const; 
private:
	//Return true if Point is inside the volume
	bool IsInsideRectangularVolume(const double Point[4]) const;
	bool IsInsideCylindricalVolume(const double Point[4]) const; 
	//Member data
	bool   _isCylindrical;
	double _length, _x, _y, _rSquared;
	double _field[6];
	
};

#endif

