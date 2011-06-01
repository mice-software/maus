// MAUS WARNING: THIS IS LEGACY CODE.
//	BTQuad.cc

#include "BTQuad.hh"

double BTQuad::_staticMaxZ=0;
bool   BTQuad::_staticUseFringeFields=false;
double BTQuad::_staticA[6] = {0,0,0,0,0,0};


//Base constructor
BTQuad::BTQuad(double Length, double FieldGradient, double PoleTipRadius, double MaxHalfWidth)
       : _length(Length), _fieldGradient(FieldGradient),
         _radius(PoleTipRadius), _maxHalfWidthSquared(MaxHalfWidth*MaxHalfWidth),
         _maxZ(_staticMaxZ), _useFringeFields(_staticUseFringeFields)
{
		for(int i=0; i<6; i++) _a[i] = _staticA[i];
}
//Copy constructor
BTQuad::BTQuad(const BTQuad& rhs)	: _length(rhs._length), _fieldGradient(rhs._fieldGradient),
                                    _radius(rhs._radius), _maxHalfWidthSquared(rhs._maxHalfWidthSquared),
                                    _maxZ(rhs._maxZ), _useFringeFields(rhs._useFringeFields)
{
	for(int i=0; i<6; i++) _a[i] = rhs._a[i];
}
//Creates an empty quad
BTQuad::BTQuad()	: _length(0.), _fieldGradient(0.), _radius(0.), _maxHalfWidthSquared(0.),
                    _maxZ(_staticMaxZ), _useFringeFields(_staticUseFringeFields)
{
	for(int i=0; i<6; i++) _a[i] = _staticA[i];
}


// Enge function and its derivative
double BTQuad::Enge(const double z) const
{
	double zParam = (fabs(z) - _length/2.)/_radius;
	double f = _a[0]+zParam*(_a[1]+zParam*(_a[2]+zParam*(_a[3]+zParam*(_a[4]+zParam*_a[5]))));
	return 1.0/(1.0+exp(f));
}

double BTQuad::EngePrime(const double z) const
{
	double zParam = (fabs(z) - _length/2.)/_radius;
	double exp_f = exp(_a[0]+zParam*(_a[1]+zParam*(_a[2]+zParam*(_a[3]+zParam*(_a[4]+zParam*_a[5])))));
	double engePrime = -exp_f/(1.0+exp_f)/(1.0+exp_f)*
	       (_a[1]+zParam*(2.0*_a[2]+zParam*(3.0*_a[3]+zParam*(4.0*_a[4]+zParam*5.0*_a[5]))));
	if(z < 0) engePrime *= -1;
	return engePrime;
}

// Return field for LOCAL coordinates point[] and use LOCAL axes for field[].
// Use gradient = 1.0 T/m.
void BTQuad::GetFieldValue(const double point[4], double *Bfield) const
{
	double rSquared = point[0]*point[0]+point[1]*point[1];
	Bfield[0] = 0.;
	Bfield[1] = 0.;
	Bfield[2] = 0.;
	Bfield[3] = 0.;
	Bfield[4] = 0.;
	Bfield[5] = 0.;

	if(_useFringeFields)
	{
		if(rSquared > _maxHalfWidthSquared || fabs(point[2]) > _maxZ*_radius/2.) return;
		double fringe = Enge(point[2]);
		double fringePrime = EngePrime(point[2]);
		Bfield[0] = _fieldGradient*fringe*point[1];
		Bfield[1] = _fieldGradient*fringe*point[0];
		Bfield[2] = _fieldGradient*fringePrime*point[0]*point[1];
	}
	else
	{
		if(fabs(point[2])>_length/2.) return;
		Bfield[0] = _fieldGradient*point[1];
		Bfield[1] = _fieldGradient*point[0];
		Bfield[2] = 0.0;
	}

}

void BTQuad::Print(std::ostream & out) const
{
	out << "Quadrupole Length: " << _length << " FieldGradient: " << _fieldGradient << " PoleTipRadius: " << _radius
	    << " ContainerRadius: " << sqrt(_maxHalfWidthSquared) << " FringeField: ";
	if(!_useFringeFields) out << "Off";
	else
	{
		out << "On Parameters:";
		for(int i=0; i<6; i++) out << " a" << i << ": " << _a[i];
	}
	BTField::Print(out);
}

CLHEP::HepLorentzVector BTQuad::GetVectorPotential(CLHEP::HepLorentzVector Position) const
{
	CLHEP::HepLorentzVector A(0,0,0,0);
	double x = Position.x();
	double y = Position.y();
	double z = Position.z();
	if( x*x+y*y > _maxHalfWidthSquared || fabs(z) > _maxZ*_radius/2.) return A;
	double pos = x*x - y*y;
	A.setZ(_fieldGradient*pos/2.);	
	return A;
}

