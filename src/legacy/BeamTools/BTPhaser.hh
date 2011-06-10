// MAUS WARNING: THIS IS LEGACY CODE.
#include "BTField.hh"
#include "Interface/RFData.hh"
#include <iostream>

#ifndef BTPHASER_HH
#define BTPHASER_HH

class BTPhaser
{
public:

  class ItemForPhasing {
   public:
    std::string name;
    CLHEP::Hep3Vector  plane_position;
    CLHEP::HepRotation rotation;
    double radius;
  };

	~BTPhaser();

  std::vector<ItemForPhasing*> GetItemsForPhasing() {return _cavities;}

	//Call this method once the global field has been set
	bool        SetThePhase(Hep3Vector Position, double time);
	int         NumberOfCavities()        {return _cavities.size();}

	//IsPhaseSet() tells Simulation whether to try to phase cavities
  //set to true if the last attempt to phase a cavity was successful
	void        IsPhaseSet(bool set) {_allPhasesSet = set;}
	bool IsPhaseSet()         {return _allPhasesSet;}

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
  std::vector<ItemForPhasing*> _cavities;
};



#endif
