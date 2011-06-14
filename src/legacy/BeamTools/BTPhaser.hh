// MAUS WARNING: THIS IS LEGACY CODE.
#include "BTField.hh"
#include "Interface/RFData.hh"
#include <iostream>
#include "Interface/MICERun.hh"

#ifndef BTPHASER_HH
#define BTPHASER_HH

class BTPhaser
{
public:

  class FieldForPhasing {
   public:
    std::string name;
    CLHEP::Hep3Vector  plane_position;
    CLHEP::HepRotation rotation;
    double radius;
  };

	~BTPhaser();

  void SetFieldForPhasing(FieldForPhasing* field) {_fields.push_back(field);}
  std::vector<FieldForPhasing*> GetFieldsForPhasing() {return _fields;}

	//Call this method once the global field has been set
	bool        SetThePhase(Hep3Vector Position, double time);
	int         NumberOfCavities()        {return _fields.size();}

	//IsPhaseSet() tells Simulation whether to try to phase cavities
  //set to true if the last attempt to phase a cavity was successful
	bool IsPhaseSet();

	//IsRefPart() tells BeamTools how to generate e.g. RF fields 
	bool IsRefPart()               {return _firingRefs;}
	bool IsRefPart(bool isRef)     {_firingRefs = isRef; return _firingRefs;}

	void        SetPhaseTolerance(double tolerance)  {_phaseTolerance = tolerance;}
	double      GetPhaseTolerance()                  {return _phaseTolerance;}

	std::vector<RFData*> GetRFData();
  static BTPhaser* GetInstance();

private:
  static BTPhaser * _instance;
	BTPhaser();
	bool      _allPhasesSet;
	double    _phaseTolerance;
	bool      _firingRefs;
	std::vector<RFData*>     _rfData;
  std::vector<FieldForPhasing*> _fields;
};



#endif
