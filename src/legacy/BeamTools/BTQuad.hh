// MAUS WARNING: THIS IS LEGACY CODE.
// @(#) $Id: BTQuad.hh,v 1.2 2006-09-17 09:43:31 rogers Exp $  $Name:  $
//
// BTQuad - implement a Quadrupole field
//
// Chris Rogers (based on implementation by TJR)
//

#ifndef BTQUAD_HH
#define BTQUAD_HH 1

#include "BTField.hh"
#include <iostream>

class BTQuad : public BTField
{
public:
	//Basic constructor
	//Picture is of a box with spherical (hyperbolic) pole tips inside
	//Max half width is the distance from the quad centre to the box edge
	//Pole tip radius is the distance from the quad centre to the pole tip
	BTQuad(double Length, double FieldGradient, double PoleTipRadius, double MaxHalfWidth);
	//Copy constructor
	BTQuad(const BTQuad& rhs);
	//Creates an empty quad
	BTQuad();
	//Destructor
	~BTQuad() {;} //no pointers
	//Makes a copy
	BTQuad * Clone() const {return new BTQuad(*this);}

	//For G4
	void GetFieldValue(const double point[4], double* Bfield) const;
	//Output usual parameters
	void Print(std::ostream & out) const;

	//Settings
	void SetLength(double length)            {_length = length;}
	void SetPoleTipRadius(double radius)     {_radius = radius;}
	void SetFieldGradient(double gradient)   {_fieldGradient = gradient;}
	void SetMaxHalfWidth(double width)       {_maxHalfWidthSquared = width*width;}
	void SetFieldAtPoleTip(double field)     {_fieldGradient = field/_radius;}
	void SetMaxZFactor(double MaxZFactor)    {_maxZ = MaxZFactor*_length;}
	void SetFringeFactors(double a[6])       {for(int i=0; i<6; i++) _a[i] = a[i];}
	void SetFringeFactor(double a, int i)    { _a[i] = a;}
	void UseFringeField(bool UseFringeField) {_useFringeFields = UseFringeField;}

	static void SetStaticMaxZFactor(double StaticMaxZFactor)   {_staticMaxZ = StaticMaxZFactor;}
	static void SetStaticFringeFactors(double A[6])            {for(int i=0; i<6; i++) _staticA[i] = A[i];}
	static void SetStaticUseFringeFields(bool UseFringeFields) {_staticUseFringeFields = UseFringeFields;}

	CLHEP::HepLorentzVector GetVectorPotential(CLHEP::HepLorentzVector Position) const;
 
private:
	//Constant field region
	double _length, _fieldGradient, _radius, _maxHalfWidthSquared, _maxZ;
	static double _staticMaxZ; //default
	//Fringe fields based on the Enge model
	bool   _useFringeFields;
	static bool   _staticUseFringeFields; //default
	static double _staticA[6];
	double _a[6];
	//Fringe field function
	double Enge(const double z) const;
	double EngePrime(const double z) const;

};

#endif
