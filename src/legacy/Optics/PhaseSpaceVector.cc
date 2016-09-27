#include "CLHEP/Vector/Rotation.h"

#include "PhaseSpaceVector.hh"
#include "Utils/Squeak.hh"
#include "src/legacy/Config/ModuleConverter.hh"
#include "Utils/Exception.hh"

const double      PhaseSpaceVector::c_l         = CLHEP::c_light;
const int         PhaseSpaceVector::nVarNames   = 21;
const std::string PhaseSpaceVector::varName[21] =
{
	"t",
	"e",
	"x",
	"px",
	"y",
	"py",
	"z",
	"pz",
	"p",
	"r",
	"pt",
	"energy",
	"t\'",
	"x\'",
	"y\'",
	"bx",
	"by",
	"bz",
	"ex",
	"ey",
	"ez"
}; //lets see if it likes that


CLHEP::HepVector PhaseSpaceVector::getSixVector() const
{
	CLHEP::HepVector sixVector(6);
	sixVector[0] = t();
	sixVector[1] = E();
	sixVector[2] = x();
	sixVector[3] = Px();
	sixVector[4] = y();
	sixVector[5] = Py();
	return sixVector;
}

CLHEP::HepVector PhaseSpaceVector::getSixVector(std::string momentumVariable) const
{
	CLHEP::HepVector sixVector(6);
	sixVector[0] = t();
	sixVector[1] = get(momentumVariable);
	sixVector[2] = x();
	sixVector[3] = Px();
	sixVector[4] = y();
	sixVector[5] = Py();
	return sixVector;
}

void PhaseSpaceVector::setSixVector(const CLHEP::HepVector sixVector, double mass)
{
	setT(sixVector[0]);
	setE(sixVector[1]);	
	setX(sixVector[2]);
	setPx(sixVector[3]);	
	setY(sixVector[4]);
	setPy(sixVector[5]);	
	setPz( sqrt(E()*E() - mass*mass - Px()*Px() - Py()*Py()) ) ;
}

CLHEP::HepVector PhaseSpaceVector::getCanonicalSixVector() const
{
	CLHEP::HepVector sixVector(6);
	sixVector[0] = t();
	sixVector[1] = E_c();
	sixVector[2] = x();
	sixVector[3] = Px_c();
	sixVector[4] = y();
	sixVector[5] = Py_c();
	return sixVector;
}

void PhaseSpaceVector::setCanonicalSixVector(const CLHEP::HepVector sixVector, CLHEP::HepLorentzVector A, double z, double mass)
{
	setFourPotential(A);
	setT(sixVector[0]);
	setE(sixVector[1] - _q*A.t()*c_l);	
	setX(sixVector[2]);
	setPx(sixVector[3]- _q*A.x()*c_l);	
	setY(sixVector[4]);
	setPy(sixVector[5]- _q*A.y()*c_l);	
	setZ(z);
	setPz( sqrt(E()*E() - mass*mass - Px()*Px() - Py()*Py()) ) ;
}

std::ostream& PhaseSpaceVector::Print(std::ostream& out) const 
{
	out << "x: " << _position << " p: " << _momentum <<  " A: " << _potential << " B: " << m_B << " E: " << _E; 
	return out;
}

std::ostream& operator<<(std::ostream& out, const PhaseSpaceVector& ps) 
{return ps.Print(out);}

bool PhaseSpaceVector::isEqual(const PhaseSpaceVector & rhs) const
{
	for(int i=0; i<4; i++)
		if(_position[i] != rhs.fourPosition()[i] || _momentum[i] != rhs.fourMomentum()[i] || _potential[i] != rhs.fourPotential()[i])
			return false;
	for(int i=0; i<3; i++)
		if( m_B[i] != rhs.BField()[i] || _E[i] != rhs.EField()[i] )
			return false;
	if(_q != rhs.q() )
		return false;
	return true;
}

CLHEP::HepLorentzVector PhaseSpaceVector::fourMomentum_c() const
{
	CLHEP::HepLorentzVector p_c = _momentum + _q*_potential*c_l;
	return p_c;
}

//stupid comment to try to force compiler to recompile
bool          operator==(const PhaseSpaceVector& lhs, const PhaseSpaceVector& rhs) 
{ return lhs.isEqual(rhs);}
bool          operator!=(const PhaseSpaceVector& lhs, const PhaseSpaceVector& rhs) 
{ return !(lhs.isEqual(rhs));}

//convert from string to a variable using varName array of strings
double PhaseSpaceVector::get(std::string name) const  
{
	int f_varName = -1;
	for(unsigned int i=0; i<name.size(); i++)
		name[i] = tolower(name[i]); //convert string to lower case
	for(int i=0; i<nVarNames; i++)
		if(name == varName[i]) f_varName = i;
	//Now a long list
	switch(f_varName)
	{
		case 0: return t();
		case 1: return E();
		case 2: return x();
		case 3: return Px();
		case 4: return y();
		case 5: return Py();
		case 6: return z();
		case 7: return Pz();
		case 8: return P();
		case 9: return r();
		case 10: return Pt();
		case 11: return E();
		case 12: return tPrime();
		case 13: return xPrime();
		case 14: return yPrime();
		case 15: return Bx();
		case 16: return By();
		case 17: return Bz();
		case 18: return Ex();
		case 19: return Ey();
		case 20: return Ez();
		default: 
			MAUS::Squeak::mout(MAUS::Squeak::debug) << "Variable " << name << " not recognised\n";
	}

	return 0.;
}

//convert from string to a variable using varName array of strings
void PhaseSpaceVector::set(double value, std::string name)  
{
	int f_varName = -1;
	for(unsigned int i=0; i<name.size(); i++)
		name[i] = tolower(name[i]); //convert string to lower case
	for(int i=0; i<nVarNames; i++)
		if(name == varName[i]) f_varName = i;
	//Now a long list
	switch(f_varName)
	{
		case 0: setT(value); break;
		case 1: setE(value); break;
		case 2: setX(value); break;
		case 3: setPx(value); break;
		case 4: setY(value); break;
		case 5: setPy(value); break;
		case 6: setZ(value); break;
		case 7: setPz(value); break;
		case 11: setE(value); break;
		case 19: setBx(value); break;
		case 20: setBy(value); break;
		case 21: setBz(value); break;
		case 22: setEx(value); break;
		case 23: setEy(value); break;
		case 24: setEz(value); break;
		default: 
			throw MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Variable "+name+" not recognised", "PhaseSpaceVector::set(double, string)");
	}
}

void PhaseSpaceVector::setConservingMass(double value, std::string name)
{
	for(unsigned int i=0; i<name.size(); i++)
		name[i] = tolower(name[i]); //convert string to lower case
	double fmass = mass();
	set(value, name);
	if(mass()!=fmass) 
	{
		if(name==varName[7] || name==varName[5] || name==varName[3]) setE ( sqrt(P()*P()+fmass*fmass) );
		else if(name==varName[11] || name==varName[1])               setPz( sqrt(E()*E()-fmass*fmass) );
	}

}


PhaseSpaceVector PhaseSpaceVector::interpolate(std::vector<PhaseSpaceVector> psv, std::string variableName, double variable)
{
  if(psv.size() < 1) throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Interpolating PhaseSpaceVector array with length 0", "PhaseSpaceVector::interpolate"));
	int    point = -1;
	int    i     = 0;
	double delta = 0;
	while(i<(int)psv.size())
	{
		if(variable < psv[i](variableName))
		{
			point = i;
			if(i>0) delta = ( variable - psv[i-1](variableName) )/(psv[i](variableName) - psv[i-1](variableName));
			i     = psv.size(); //break
		}
		else    i++;
	}
	if(point == -1)  return psv.back(); //variable > all of psv
	if(point == 0)   return psv[0];     //variable < all of psv
	CLHEP::HepVector sixVector = delta*(psv[point].getSixVector() - psv[point-1].getSixVector()) + psv[point-1].getSixVector();
	return           PhaseSpaceVector(sixVector, psv[0].m());
}

PhaseSpaceVector PhaseSpaceVector::Rotate(double angle) const
{
	Hep3Vector axis(0,0,1);
	return Rotate(axis, angle);
}

PhaseSpaceVector PhaseSpaceVector::Rotate(Hep3Vector axis, double angle) const
{
	CLHEP::HepRotation rot(axis, angle);
	return PhaseSpaceVector(rot*_position, rot*_momentum, rot*m_B, rot*_E=Hep3Vector(0,0,0), rot*_potential);
}

PhaseSpaceVector operator-(const PhaseSpaceVector& lhs, const PhaseSpaceVector& rhs)
{
	HepLorentzVector x = lhs.fourPosition()  - rhs.fourPosition();
	HepLorentzVector p = lhs.fourMomentum()  - rhs.fourMomentum();
	HepLorentzVector A = lhs.fourPotential() - rhs.fourPotential();
	return PhaseSpaceVector(x, p, Hep3Vector(0,0,0), Hep3Vector(0,0,0), A);
}

std::vector<double> PhaseSpaceVector::eventKinematics() const
{
	std::vector<double> kinematics(8);
	kinematics[0] = t();
	kinematics[1] = E();
	kinematics[2] = x();
	kinematics[3] = Px();
	kinematics[4] = y();
	kinematics[5] = Py();
	kinematics[6] = z();
	kinematics[7] = Pz();
	return kinematics;
}

VirtualHit PhaseSpaceVector::virtualHit(int trackID, int stationID)
{
  VirtualHit hit;
  hit.SetPos     ( _position.vect() );
  hit.SetMomentum( _momentum.vect() );
  hit.SetCharge  ( q() );
  hit.SetTime    ( _position.t() );
  hit.SetEnergy  ( _momentum.e() );
  hit.SetMass    ( m() );
  hit.SetPID     ( ModuleConverter::MassToPdgPid(m(), m()/1.e3+1.e-3) );
  hit.SetTrackID ( trackID );
  hit.SetStationNumber ( stationID );
  hit.SetBField(m_B);
  hit.SetEField(_E);
  return hit;
}
