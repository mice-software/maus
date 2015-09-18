// MAUS WARNING: THIS IS LEGACY CODE.
//BTFieldGroup contains and places groups of fields
//Generic but intended to hold field data for e.g. a module in MICE

#ifndef BTFIELD_HH
#define BTFIELD_HH

#define HEP_USE_VECTOR_MODULE

#include <vector>
#include <iostream>
#include <limits>

#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/Rotation.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "Interface/RFData.hh"

using CLHEP::Hep3Vector;
using CLHEP::HepRotation;
using CLHEP::degree;

class BTField
{
public:
	BTField() : parentField(NULL), bbMin(std::vector<double>(3,-std::numeric_limits<double>::max()/1e3)), //if not defined, limits are large
	                               bbMax(std::vector<double>(3,+std::numeric_limits<double>::max()/1e3)) {}
	BTField(double bbMinX, double bbMinY, double bbMinZ, double bbMaxX, double bbMaxY, double bbMaxZ) 
	  : parentField(NULL), bbMin(std::vector<double>(3)), bbMax(std::vector<double>(3))
	{
		bbMin[0] = bbMinX; bbMin[1] = bbMinY; bbMin[2] = bbMinZ;
		bbMax[0] = bbMaxX; bbMax[1] = bbMaxY; bbMax[2] = bbMaxZ;
	}
	virtual ~BTField() {;}

	//get field value in the local coordinate system
	//GetFieldValue is responsible for bound checking!
	//Returns 0 if outside bound
	virtual void GetFieldValue( const double Point[4], double *EMfield ) const
	{std::cout << "This method should be overloaded" << std::endl;}
	inline virtual std::vector<double> GetFieldValue2(double x, double y, double z, double t)
	{ double point[4] = {x,y,z,t}; std::vector<double> field(6,0); GetFieldValue(point, &field[0]); return field; } 
	virtual void GetFieldValueNonConst( const double Point[4], double *EMfield ) {;}


	//Default is a magnetic field
	virtual bool DoesFieldChangeEnergy() const {return false;}

	//Set the phase for this field
	//Return value is newPhase - previousPhase
	virtual RFData SetThePhase(Hep3Vector Position, double time, double energy) {return RFData();}
	//Returns whether the phase still needs to be set in this field
	virtual bool   IsPhaseSet() const {return true;}

	virtual void Print(std::ostream &out) const
	{out << " scaleFactor: " << GetScaleFactor(this) << " position: " << GetGlobalPosition() 
	     << " rotationVector: " << GetGlobalRotation().getAxis() << " angle: "  << GetGlobalRotation().delta()/degree << " name: " << _name  << "\n";}

	virtual BTField * Clone() const {return new BTField(*this);}
	virtual BTField * GetParentField() const {return parentField;}
	virtual void SetParentField(BTField * aField) {parentField = aField;} //MUST BE BTFieldGroup!!!

	//Get the position of the field in global coordinates
	virtual Hep3Vector GetGlobalPosition() const
	{
		if (parentField!=NULL)
			return( parentField->GetGlobalPosition() + parentField->GetGlobalRotation()*parentField->GetLocalPosition(this) );
		return Hep3Vector(0,0,0); //top level field
	}

	virtual HepRotation GetGlobalRotation() const
	{
		if (parentField!=NULL) //does not commute
			return( parentField->GetGlobalRotation()*parentField->GetLocalRotation(this) ); 
		return HepRotation(0,0,0); //top level field
	}
	//Return a point outside the field map, along a "reference trajectory" if such a thing exists, as close as possible to the field map
	virtual CLHEP::Hep3Vector GetExternalPoint() const
	{return CLHEP::Hep3Vector(0,0,0);}


	//Should be overloaded by the field group
	virtual double GetScaleFactor(const BTField * aField) const
	{return ( (parentField==NULL) ? 1. : parentField->GetScaleFactor(this) );}
	virtual CLHEP::Hep3Vector GetLocalPosition(const BTField * aField) const 
	{return CLHEP::Hep3Vector(0,0,0);}
	virtual CLHEP::HepRotation GetLocalRotation(const BTField * aField) const 
	{return CLHEP::HepRotation(0,0,0);}


	//Vector potential
	virtual CLHEP::HepLorentzVector GetVectorPotential(CLHEP::HepLorentzVector Point) const
	{
	  double pot[4] = {0,0,0,0};
	  double pos[4] = {Point[0], Point[1], Point[2], Point[3]};
	  GetVectorPotential(pos, pot);
	  return CLHEP::HepLorentzVector(pot[0], pot[1], pot[2], pot[3]);
	}

	virtual void GetVectorPotential(const double point[4], double * potential) const {;}
	virtual void GetVectorPotentialDifferential(const double point[4], double * potential, int axis) const {;}

	//These methods are really meant for testing - 
	//Maxwell tests GetFieldValue complies with Maxwell's laws
	virtual double            Maxwell1(const double point[4], const double delta[4]) const; //div.E
	virtual double            Maxwell2(const double point[4], const double delta[4]) const; //div.B
	virtual CLHEP::Hep3Vector Maxwell3(const double point[4], const double delta[4]) const; //curl E + dB/dt (3-vector)
	virtual CLHEP::Hep3Vector Maxwell4(const double point[4], const double delta[4]) const; //curl B + mu eps dE/dt
	//Field from VectorPotential tests that VectorPotential gives same results as GetFieldValue
	//Use B = Curl(A) and E = -div.phi - dA/dt
	virtual void   FieldFromVectorPotential            (const double point[4], const double delta[4], double * field) const;
	virtual void   FieldFromVectorPotentialDifferential(const double point[4], double * field) const;

	//Bounding box min,max in local coordinates - array of length 3
	const double* BoundingBoxMin() const {return &bbMin[0];}
	const double* BoundingBoxMax() const {return &bbMax[0];}

	std::string   GetName()                 const {return _name;}
	void          SetName(std::string name)       {_name = name;}
protected:
	//Return the translation required to transpose this onto aField's position
	Hep3Vector GetTranslation(BTField * aField) const
	{ return (aField->GetGlobalPosition() - this->GetGlobalPosition()); }

	BTField * parentField; //should be BTFieldGroup but introduces nasty circular reference'
	std::string         _name;
	std::vector<double> bbMin; //3-vector that is lower corner of bounding box
	std::vector<double> bbMax; //3-vector that is upper corner of bounding box
private:


};

std::ostream& operator<<(std::ostream& out, const BTField& field);
#endif
