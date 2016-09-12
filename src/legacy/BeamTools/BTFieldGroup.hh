// MAUS WARNING: THIS IS LEGACY CODE.
//BTFieldGroup contains and places groups of fields
//Generic but intended to hold field data for e.g. a module in MICE

#ifndef BTFIELDGROUP_HH
#define BTFIELDGROUP_HH

#define HEP_USE_VECTOR_MODULE

#include "CLHEP/Vector/Rotation.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "BTField.hh"
#include "BTSolenoid.hh"
#include "BTPillBox.hh"
#include "BTMagFieldMap.hh"
#include "Config/MiceModule.hh"
#include <vector>

using namespace::std;

class BTFieldGroup : public BTField
{
public:
	BTFieldGroup();
	BTFieldGroup(const BTFieldGroup& rhs);
	~BTFieldGroup();

	BTFieldGroup& operator=(const BTFieldGroup& rhs);

	BTFieldGroup * Clone() const {return new BTFieldGroup(*this);}

	//Remove field from the group
  //if willClose=true calls Close() at the end; which refreshes the mesh
	void Erase(BTField* field, bool willClose=true);
	//get field value in the local coordinate system
	//GetFieldValue is responsible for bound checking!
	//Returns 0 if outside bound
	void GetFieldValue( const double Point[4], double *EMfield ) const;

	//Default is a magnetic field
	bool DoesFieldChangeEnergy() const;// {for(int i=0; i<_field.size(); i++) if(_fields[i]->DoesFieldChangeEnergy()) return true; return false;}

	//Include a field e.g. solenoid at position and rotation specified
  //If willClose=true, calls Close() at the end; which puts all the fields into a mesh
	void AddField(BTField * newField, const Hep3Vector position, double scaleFactor=1, bool willClose=true);
	void AddField(BTField * newField, const Hep3Vector position, const HepRotation rotation, double scaleFactor=1, bool willClose=true);

	//Hands the phase down to all fields below this one
	RFData SetThePhase(Hep3Vector position, double time, double energy);
	//Asks all members if their phase is set, return false if any are not
	bool   IsPhaseSet() const;
	//Print all fields in the group
	virtual void Print(std::ostream &out) const;
	//Get the relative position/rotation between this field and *field
	//If one or both fields are not recognised, hand it up to the parent
	Hep3Vector  GetLocalPosition(const BTField * field) const;
	HepRotation GetLocalRotation(const BTField * field) const;
	//Gets Point in the local coordinate system of _fields[fieldNumber]
	double * GetLocalCoordinates(const double Point[4], unsigned int fieldNumber) const;
	//Get the (global) scale factor of the field aField
	double GetScaleFactor(const BTField * aField) const;
	double GetScaleFactor(int field)              const {return _scaleFactors[field];}
	void   SetScaleFactor(int field, double scale)      {_scaleFactors[field] = scale;}
	//Get all fields with point possibly in bounding box
	inline std::vector<int> GetFields(const double point[4]) const 
	{return _fieldsToMesh[_mesh->Nearest(point).ToInteger()];}
	//{std::vector<int> a(_fields.size()); for(int i=0; i<a.size(); i++) a[i] = i; return a;}
	//Get the number of fields in the group
	unsigned int GetNumberOfFields() const  {return _fields.size();}
	std::vector<BTField*> GetFields() const {return _fields;}
	//Get the sum of childrens' vector potential
	CLHEP::HepLorentzVector GetVectorPotential(CLHEP::HepLorentzVector position) const;
	void                    GetVectorPotential(const double point[4], double * potential) const 
	{throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Not implemented!", "BTFieldGroup::GetVectorPotential(const double*, double*)"));}
	void                    GetVectorPotentialDifferential(const double point[4], double * potential, int axis) const
	{throw(MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable, "Not implemented!", "BTFieldGroup::GetVectorPotentialDifferential"));}
	CLHEP::HepLorentzVector GetLocalCoordinates(CLHEP::HepLorentzVector position, unsigned int fieldNumber) const;
	//Add all fields to the mesh; update this field's bounding box; set _closed to true; repeat for any parent fields
  //note you have to call Close() before attempting to use the fieldgroup for e.g. GetFieldValue()
	void Close();
	//Get list of vertices of the bounding box for _fields[index] in BTFieldGroup coordinates
	std::vector<CLHEP::Hep3Vector> GetBBVertices(int index);
	//Get and set default grid spacing
	static std::vector<int> GetGridDefault()                      {return _gridDefault;}
	static std::vector<int> SetGridDefault(std::vector<int> grid) {_gridDefault = grid; return _gridDefault;}
	//Get and set my grid spacing
	std::vector<int>        SetGridSize   (std::vector<int> grid) {_gridSize    = grid; return _gridSize;}
	std::vector<int>        GetGridSize   () const                {return _gridSize;}
	ThreeDGrid*             GetGrid       () const                {return _mesh;}

  void SetIsClosed(bool is_closed) {_closed = is_closed;}
  bool GetIsClosed() const {return _closed;}
private:
	std::vector<BTField*>    _fields;
	std::vector<bool>        _isRotated;
	std::vector<HepRotation> _rotations;
	std::vector<HepRotation> _inverseRotations;
	std::vector<Hep3Vector>  _translations;
	std::vector<double>      _scaleFactors;

	std::vector< std::vector<int> > _fieldsToMesh; //_fieldsToMesh[i] = list of _field indices at _mesh->Nearest(point).ToInteger()
	std::vector<int>         _gridSize;
	ThreeDGrid*              _mesh;
	bool                     _closed;

	static std::vector<int>  _gridDefault;
};

class BTFieldAmalgamation : public BTFieldGroup
{
public:
	enum field_type{solenoid};
	BTFieldAmalgamation(double r_max_, double length_, double z_step_, double r_step_, std::string interpolation_, field_type type_=solenoid);
	~BTFieldAmalgamation();
	BTFieldAmalgamation * Clone() const {return new BTFieldAmalgamation(*this);}
	//get field value in the local coordinate system
	//GetFieldValue is responsible for bound checking!
	//Returns 0 if outside bound
	void   GetFieldValue( const double Point[4], double *EMfield ) const;
	//Include a field e.g. solenoid
	void   AddField(BTField * newField, const Hep3Vector position, double scaleFactor=1);
	//Print all fields in the group
	void   Print(std::ostream &out) const;
	//Write amalgamations
	bool   IsAmalgamated() {return amalgamated;}
	void   AmalgamateThis();  //Amalgamates this BTFieldAmalgamation
	static void AmalgamateAll(); //Amalgamates all BTFieldAmalgamations
private:
	static       std::vector<BTFieldAmalgamation*> amalgamations;
	MagFieldMap* fieldMap; //the field map
	bool         amalgamated;
	double       r_max; //for cylindrical stuff
	double       z_length;
	double       r_step;
	double       z_step;
	std::string  interpolation;
	field_type   type;
};



#endif
