//BTFieldGroup contains and places groups of fields
//Generic but intended to hold field data for e.g. a module in MICE

#ifndef BTMULTIPOLE_HH
#define BTMULTIPOLE_HH

#include <vector>
#include <iostream>
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/Rotation.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "Interface/RFData.hh"
#include "BTField.hh"

using CLHEP::Hep3Vector;

class BTMultipole : public BTField
{
public:
	//length is the length along the midpoint
	//to make a straight multipole, set radiusOfCurvature = 0.
	BTMultipole(int pole, double fieldAtPoleTip, double length, double radiusOfCurvature, double height, double width, std::string curvature="");
	BTMultipole(int pole, double fieldAtPoleTip, double length, double radiusVariable, double height, double width,
	            double effectiveWidth, double endLength, int endPole, std::vector<double> engeParameters, std::string curvature="");

	//This is ugly alternate "constructor"
	static BTMultipole DipoleFromMomentum(double momentum, double length, double angle, double height, double width);
	~BTMultipole() {;}

	//GetFieldValue at a Point relative to multipole start
	void GetFieldValue       ( const double Point[4], double *EMfield ) const;
	void GetFieldValueRotated( Hep3Vector Point, double *EMfield ) const; //get field value in rotated coordinate system
	void HardEdgedFieldValue (Hep3Vector Point, double *EMfield )  const;
	void EngeFieldValue      (Hep3Vector Point, double *EMfield )  const;

	//transform to a rotated coordinate system z=0 is multipole end, with +z outside pole and -z inside pole
	Hep3Vector TransformToRotated             (const double point[3]) const; 
	Hep3Vector TransformToRotatedConstant     (const double point[3]) const; //transform value[3] to rotated coordinates
	Hep3Vector TransformToRotatedStraightEnds (const double point[3]) const; //transform value[3] to rotated coordinates
	Hep3Vector TransformToRotatedMomentumBased(const double point[3]) const; //transform value[3] to rotated coordinates

	void       TransformToRectangular             (double * point) const; //transform point in rotated coordinates to rectangular coordinates
	void       TransformToRectangular             (const double point[3], double* value) const; //transform value at Point to rectangular coordinates
	void       TransformToRectangularConstant     (const double point[3], double* value) const;
	void       TransformToRectangularStraightEnds (const double point[3], double* value) const;
	void       TransformToRectangularMomentumBased(const double point[3], double* value) const;
	//Vector potential NOT IMPLEMENTED FOR MULTIPOLES (yet, cf Zgoubi docs for scalar potential to implement)
	CLHEP::Hep3Vector       GetExternalPoint() const;
	CLHEP::HepLorentzVector GetVectorPotential(CLHEP::HepLorentzVector Point) const
	{return CLHEP::HepLorentzVector(0,0,0,0);}

	//Utility methods
	void       Print(std::ostream &out) const;
	BTMultipole * Clone() const {return new BTMultipole(*this);}

	//return integral (By(dx, z)) dz / (By(z=centre)/dx)
	//which is the effective length if field gradient is constant along whole quad
	double QuadEffectiveLength(double dx) const;

	double Enge(double x, int derivative) const; //calculate Enge or a derivative
	double GetMagnitude() {return _magnitude;}
	void   GetReferenceTrajectory(std::vector<double>& x, std::vector<double>& z, std::vector<double>& angle, std::vector<double>& s); 
	void   SetReferenceTrajectory(std::vector<double>  x, std::vector<double>  z, std::vector<double>  angle, std::vector<double>  s); 
	enum   endFieldModel {hardEdged, enge};
	enum   curvatureModel{straight, constant, straightEnds, momentumBased};
	curvatureModel GetCurvatureModel() {return _curvature;}

private:
	static int EvolveReferenceTrajectory(double s, const double y[], double f[], void *params);
	static int EvolveIntegralB          (double s, const double y[], double f[], void *params);
	double     IntegralB                (double dxOffset) const;

	void   SetupDipoleMagnitude        (double momentum, double angleInRadians);

	void   SetupReferenceTrajectory();
	bool   isInside(const double Point[4]) const;
	//Analytical enge derivatives up to d^4 Enge/dx^4; numerical for higher derivatives
	double EngeN(double x, int n) const;
	//helper functions to get engeN... 
	//using  enge = 1/(1+exp(a_0 x+a_1 x^2+...)) = 1/(1+exp(h)) = 1/g 
	//hN = d^n h/ dx^n; n<=4
	double hN(double x, int n) const;
	//gN = d^n g/ dh^n; n<=4
	double gN(double x, int n) const;
	//Radius of curvature as a function of |s|-endLength; constant for now
	double RadiusOfCurvature(double sRelativeToEnd) const;

	int     _pole; //lowest order term is a multipole of what order?
	double  _magnitude; //constant of proportionality
	double  _effectiveWidth;
	double  _length;
	double  _rCurv;
	double  _height;
	double  _width;
	double  _endLength;
	std::vector<double> _engeParameters;
	endFieldModel       _endField;
	curvatureModel      _curvature;
	int     _endFieldPole; //pole number to go up to in end field
	//Geometry info used for _curvature==straightEnds
	Hep3Vector  _endPoint;    //point in middle at far end of multipole
	HepRotation _endRotation; //rotation to angle of far end of multipole
	//Geometry info used for _curvature==momentumBased
	double      _momentum; //momentum of reference particle

	std::vector<double> _angle; //angle of tangent to reference trajectory relative to (x,y,z)=(0,0,1)
	std::vector<double> _refX;  //x position of reference trajectory
	std::vector<double> _refZ;  //z position of reference trajectory
	std::vector<double> _refS;  //s position of reference trajectory

	static const BTMultipole * staticMultipole; //staticMultipole used during integration for numerical determination of integral(B)dl, integral(dB/dx)dl
	static double              staticDX;        //staticDX used during numerical determination of integral(dB/dx)dl
	static double _absoluteError; //parameters used for calculating (numerically) reference trajectory, effective length, etc
	static double _relativeError;
	static double _stepSize;
};

#endif
