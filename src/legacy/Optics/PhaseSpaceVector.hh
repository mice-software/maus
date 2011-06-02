#ifndef PHASESPACEVECTOR_H
#define PHASESPACEVECTOR_H 1

#include "src/legacy/Interface/MCHit.hh"
#include "src/legacy/Interface/VirtualHit.hh"

#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Units/PhysicalConstants.h"

#include <iostream>

using CLHEP::HepLorentzVector;
using CLHEP::Hep3Vector;

class PhaseSpaceVector
{
public:
	PhaseSpaceVector() : _momentum(HepLorentzVector()), _position(HepLorentzVector()), 
                             _potential(HepLorentzVector()), m_B(Hep3Vector()), _E(Hep3Vector()), _q(CLHEP::eplus)
	{;}
	PhaseSpaceVector(HepLorentzVector Position, HepLorentzVector Momentum, 
                         HepLorentzVector Potential=HepLorentzVector(0,0,0,0), double q=CLHEP::eplus) 
           : _momentum(Momentum), _position(Position), _potential(Potential),
	     m_B(Hep3Vector()), _E(Hep3Vector()), _q(q)
	{;}
	PhaseSpaceVector(HepLorentzVector Position, HepLorentzVector Momentum, Hep3Vector B,
	                 Hep3Vector E=Hep3Vector(0,0,0), 
                         HepLorentzVector Potential=HepLorentzVector(0,0,0,0), double q=CLHEP::eplus) 
           : _momentum(Momentum), _position(Position), _potential(Potential), m_B(B), _E(E), _q(q)
	{;}

	PhaseSpaceVector(CLHEP::HepVector sixVector, double mass) : _momentum(HepLorentzVector()), _position(HepLorentzVector()), 
                             _potential(HepLorentzVector()), m_B(Hep3Vector()), _E(Hep3Vector()), _q(CLHEP::eplus)
	{setSixVector(sixVector, mass);}

	PhaseSpaceVector(MCHit* hit) : _momentum(HepLorentzVector()), _position(HepLorentzVector()), 
                             _potential(HepLorentzVector()), m_B(Hep3Vector()), _E(Hep3Vector()), _q(hit->charge())
	{_momentum = HepLorentzVector(hit->momentum(), hit->energy()); _position = HepLorentzVector(hit->position(), hit->time());}

	~PhaseSpaceVector() {;}

	//Kinetic momenta
	double Px() const {return _momentum.x();}
	double Py() const {return _momentum.y();}
	double Pz() const {return _momentum.z();}
	double E()  const {return _momentum.t();}
	//Canonical momenta
	double Px_c() const {return _momentum.x()+_potential.x()*_q*c_l;}
	double Py_c() const {return _momentum.y()+_potential.y()*_q*c_l;}
	double Pz_c() const {return _momentum.z()+_potential.z()*_q*c_l;}
	double E_c()  const {return _momentum.t()+_potential.t()*_q*c_l;}
	//Position
	double x() const {return _position.x();}
	double y() const {return _position.y();}
	double z() const {return _position.z();}
	double t() const {return _position.t();}
	//subscript operator 0-2 = position, 3=time, 4-6=momenta, 7=energy
	double& operator[](int i)       { if(i<4) return _position[i]; else return _momentum[i-4];}
	double  operator[](int i) const { if(i<4) return _position[i]; else return _momentum[i-4];}
	//1-3 = position, 4 = time, 5-7=momenta, 7=energy
	double& operator()(int i)       { if(i<5) return _position[i-1]; else return _momentum[i-5];}
	double  operator()(int i) const { if(i<5) return _position[i-1]; else return _momentum[i-5];}
	//return std::vector<double>(t,E,x,px,y,py)
	std::vector<double> eventKinematics() const;
	//List of strings given in PSV.cc
	double operator()(std::string variable) const {return get(variable);}
	double get(std::string variable) const;
	//Trace space
	double xPrime() const {if(Pz()!=0) return Px()/Pz(); else return 0;}
	double yPrime() const {if(Pz()!=0) return Py()/Pz(); else return 0;}
	double tPrime() const {if(Pz()!=0) return  E()/Pz(); else return 0;}
	//Four vectors
	CLHEP::HepLorentzVector fourMomentum()   const {return _momentum;}
	CLHEP::HepLorentzVector fourPosition()   const {return _position;}
	CLHEP::HepLorentzVector fourPotential()  const {return _potential;}
	//Canonical momentum
	CLHEP::HepLorentzVector fourMomentum_c() const; 
	void dummy();

	//Fields
	Hep3Vector   BField() const {return m_B;}
	Hep3Vector   EField() const {return _E;}
	//Field components
	double Bx() const {return m_B.x();}
	double By() const {return m_B.y();}
	double Bz() const {return m_B.z();}
	double Ex() const {return _E.x();}
	double Ey() const {return _E.y();}
	double Ez() const {return _E.z();}
	//Vector potential
	double Ax() const {return _potential.x();}
	double Ay() const {return _potential.y();}
	double Az() const {return _potential.z();}
	double At() const {return _potential.t();}
	//mass, radius, kinetic 3-momentum, kinetic transverse momentum, charge
	double m()  const {return _momentum.m();}
	double r()  const {return _position.perp();}
	double P()  const {return _momentum.rho();}
	double Pt() const {return _momentum.perp();}
	double q()  const {return _q;}
	//long hand
	double mass()   const {return m();}
	double radius() const {return r();}
	double charge() const {return q();}
	//Einstein Coefficients
	double beta()  const {return P()/E();}
	double gamma() const {return E()/m();}
	//Set <variable>
	void set(double value, std::string name);
	void set(std::string name, double value) {set(value, name);}
	//As above but alter *either* energy *or* pz so that mass is conserved
	void setConservingMass(double value, std::string name);
	void setConservingMass(std::string name, double value) {setConservingMass(value, name);}
	//Set position
	void setX(double x) {_position.setX(x);}
	void setY(double y) {_position.setY(y);}
	void setZ(double z) {_position.setZ(z);}
	void setT(double t) {_position.setT(t);}
	//Set momentum
	void setPx(double px) {_momentum.setX(px);}
	void setPy(double py) {_momentum.setY(py);}
	void setPz(double pz) {_momentum.setZ(pz);} //BUG Set E to keep m constant?
	void setE(double E)   {_momentum.setT(E); } //BUG Set Pz to keep m constant?
	//Set potential
	void setAx(double Ax)   {_potential.setX(Ax);}
	void setAy(double Ay)   {_potential.setY(Ay);}
	void setAz(double Az)   {_potential.setZ(Az);}
	void setAt(double At)   {_potential.setT(At); }
	//Set 4-vectors
	void setFourPosition(HepLorentzVector U)  {_position  = U;}
	void setFourMomentum(HepLorentzVector P)  {_momentum  = P;}
	void setFourPotential(HepLorentzVector A) {_potential = A;}
	//Set 4-canonical momentum assuming vector potential is already set
	void setFourPotential_c(HepLorentzVector P_c) {_momentum = P_c - _q*_potential*c_l;}
	//Set 4-vectors (nb I checked that CLHEP::HepLorentzVector(x,y,z,t) etc is correct)
	void setFourPosition(double t, double x, double y, double z)  
		{_position = CLHEP::HepLorentzVector(x,y,z,t);}
	void setFourMomentum(double E, double px, double py, double pz)  
		{_momentum  = CLHEP::HepLorentzVector(px,py,pz,E);}
	void setFourPotential(double V, double Ax, double Ay, double Az) 
		{_potential = CLHEP::HepLorentzVector(Ax,Ay,Az,V);}
	//Set 4-canonical momentum assuming vector potential is already set
	void setFourPotential_c(double pt, double px, double py, double pz) 
		{setFourPotential_c(CLHEP::HepLorentzVector(px,py,pz,pt));}
	//Set field 3-vectors
	void setB(Hep3Vector B) {m_B = B;}
	void setE(Hep3Vector E) {_E = E;}
	//Set field components
	void setBx(double bx) {m_B.setX(bx);}
	void setBy(double by) {m_B.setY(by);}
	void setBz(double bz) {m_B.setZ(bz);}
	void setEx(double ex) {_E.setX(ex);}
	void setEy(double ey) {_E.setY(ey);}
	void setEz(double ez) {_E.setZ(ez);}
	//Angular Momentum
	double lCan() {return x()*Py_c() - y()*Px_c();}
	double lKin() {return x()*Py()   - y()*Px();}
	//Charge
	void setQ(double q)   {_q = q;}  
	//six vector goes t,E,x,px,y,py
	CLHEP::HepVector getSixVector() const;
	CLHEP::HepVector getSixVector(std::string momentumVariable) const;
	void             setSixVector(const CLHEP::HepVector sixVector, double mass);
	//six vector goes t,E,x,px,y,py
	CLHEP::HepVector getCanonicalSixVector() const;
	void             setCanonicalSixVector(const CLHEP::HepVector sixVector, CLHEP::HepLorentzVector A, 
                                               double z, double mass);
	void             setCanonicalSixVector(const CLHEP::HepVector sixVector, double z, double mass)
	                 {setCanonicalSixVector(sixVector,_potential, z, mass); }
	//utilities
	std::ostream&    Print(std::ostream& out) const; 
	bool             isEqual(const PhaseSpaceVector &rhs) const;
	//return a linear interpolation of the psv 6 vector (keep mass constant) based on the value of variable
	//of type variable name. If out of bounds, return value of nearest psv. psv is assumed to be ordered ito variable
	static PhaseSpaceVector interpolate(std::vector<PhaseSpaceVector> psv, std::string variableName, double variable);
	//return the psv after rotation by angle
	PhaseSpaceVector        Rotate(double angle) const; //default axis to (0,0,1)
	PhaseSpaceVector        Rotate(Hep3Vector axis, double angle) const;
  //convert to a hit
  VirtualHit virtualHit(int trackId, int stationId);

	static std::vector<std::string> listOfVariables() {return std::vector<std::string>(varName, varName+nVarNames);}


private:
	CLHEP::HepLorentzVector _momentum, _position, _potential;	
	CLHEP::Hep3Vector   m_B, _E;
	//charge
	double _q;
	static const double       c_l; //speed of light
	static const std::string  varName[]; //list of variables as strings; PhaseSpaceVector(std::string) converts to doubles
	static const int          nVarNames; //number of names in varName
};

std::ostream& operator<<(std::ostream& out, const PhaseSpaceVector& ps);
//Gives dodgy looking linker error
bool          operator==(const PhaseSpaceVector& lhs, const PhaseSpaceVector& rhs);// { return lhs.isEqual(rhs);}
bool          operator!=(const PhaseSpaceVector& lhs, const PhaseSpaceVector& rhs);// { return !(lhs == rhs);}

//x_1 = x_2 - x_3; p_1 = p_2 - p_3; A_1 = A_2 - A_3; B = 0; E = 0
PhaseSpaceVector operator-(const PhaseSpaceVector& lhs, const PhaseSpaceVector& rhs);

#endif



