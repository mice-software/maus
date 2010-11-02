#include "BTField.hh"
#include "Interface/RFData.hh"
#include <iostream>

#ifndef BTPHASER_HH
#define BTPHASER_HH

class BTPhaser
{
public:
	~BTPhaser();
	//Call this method first
	static void SetGlobalField(BTField * GlobalField, bool NeedsReferenceParticle)
	{
		_globalField = GlobalField;
		_allPhasesSet = !NeedsReferenceParticle;
		_hasGlobalField = true;
	}
	//Call this method once the global field has been set
	static bool        SetThePhase(Hep3Vector Position, double time, double energy);
	static int         NumberOfCavities()        {return _cavityNames.size();}

	//IsPhaseSet() tells Simulation whether to try to phase cavities
  //set to true if the last attempt to phase a cavity was successful
	static void        IsPhaseSet(bool set) {_allPhasesSet = set;}	
	inline static bool IsPhaseSet()         {return _allPhasesSet;}

	//IsRefPart() tells BeamTools how to generate e.g. RF fields 
	inline static bool IsRefPart()               {return _firingRefs;}
	inline static bool IsRefPart(bool isRef)     {_firingRefs = isRef; return _firingRefs;}
	inline static void Print(std::ostream & out) {if (BTPhaser::_hasGlobalField) BTPhaser::_globalField->Print(out);}
	inline static void AddCavityName(std::string name)      {BTPhaser::_cavityNames.push_back(name);}
	inline static void AddCavityDetectorName(std::string name)     {BTPhaser::_cavityDetectorNames.push_back(name);}
	static BTField*    GetGlobalField()                     {return _globalField;}
	static void        SetPhaseTolerance(double tolerance)  {_phaseTolerance = tolerance;}
	static double      GetPhaseTolerance()                  {return _phaseTolerance;}
	static void        SetEnergyTolerance(double tolerance) {_energyTolerance = tolerance;}
	static double      GetEnergyTolerance()                 {return _energyTolerance;}
	static bool        IsCavityDetector(std::string name);
	static bool        IsCavity(std::string name);

	static std::vector<RFData*> GetRFData();
	static std::string CavityDetectorSuffix() {return "DetMiddle";}

private:
	BTPhaser();
	static bool      _hasGlobalField;
	static bool      _allPhasesSet;
	static double    _phaseTolerance;
	static double    _energyTolerance;
	static bool      _firingRefs;
	static BTField * _globalField;
	static std::vector<std::string> _cavityDetectorNames;
	static std::vector<std::string> _cavityNames;
	static std::vector<RFData*>     _rfData;
};



#endif
