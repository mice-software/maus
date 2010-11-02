//BTFieldGroup contains and places groups of fields
//Generic but intended to hold field data for e.g. a module in MICE

#ifndef BTCOMBINEDFUNCTION_HH
#define BTCOMBINEDFUNCTION_HH

#include <vector>
#include <iostream>
#include "BTField.hh"
#include "BTMultipole.hh"


class BTCombinedFunction : public BTField
{
public:
	//length is the length along the midpoint
	//to make a straight multipole, set radiusOfCurvature = 0.
	BTCombinedFunction(double By, double fieldIndex, double length, double radiusOfCurvature, double height, double width, int maxPole, 
	                   std::string curvature="");
	BTCombinedFunction(double By, double fieldIndex, double length, double radiusOfCurvature, double height, double width, int maxPole,
	                   double effectiveWidth, double endLength, int endPole, std::vector<double> engeParameters, std::string curvature="");
	~BTCombinedFunction() {;}

	void Print(std::ostream &out) const;

	//Physics methods
	void GetFieldValue( const double Point[4], double *EMfield ) const
	{
		for(unsigned int i=0; i<_fields.size(); i++)
		{ 
			double field[6] = {0,0,0,0,0,0};
			_fields[i].GetFieldValue(Point, field);
			for(int i=0; i<6; i++) EMfield[i] += field[i];
		}
	}
	BTCombinedFunction * Clone() const {return new BTCombinedFunction(*this);}
	void SetMomentumBased();

	CLHEP::HepLorentzVector GetVectorPotential(CLHEP::HepLorentzVector Point) const;
	CLHEP::Hep3Vector       GetExternalPoint() const;

	void   GetReferenceTrajectory(std::vector<double>& x, std::vector<double>& z, std::vector<double>& angle, std::vector<double>& s); 

private:
	std::vector<BTMultipole>   _fields;
	double                     _by;
	double                     _fieldIndex;
	double                     _length;
	double                     _radiusOfCurvature;
	double                     _height;
	double                     _width;

	double GetFieldAtPoleTip(int pole);// {if(pole == 1) return _by; else return 0;}
};

#endif
