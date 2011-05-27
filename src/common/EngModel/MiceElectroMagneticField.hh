//BTField is the parent of all fields in the beamtools package
//Abstract Base Class
//Has simply a GetFieldValue method


#ifndef MICEELECTROMAGNETICFIELD_HH
#define MICEELECTROMAGNETICFIELD_HH

#include "BeamTools/BTFieldConstructor.hh"
#include "BeamTools/BTField.hh"
#include "G4ElectroMagneticField.hh"

class MiceElectroMagneticField : public G4ElectroMagneticField
{
public:
	MiceElectroMagneticField(BTFieldConstructor * theField) : _emField(theField->GetElectroMagneticField())
  {};

	
	//BUG - should be inlined!
	inline void GetFieldValue(const double Point[4], double * bfield) const
	{
		_emField->GetFieldValue(Point, bfield);
	}

	void Print(std::ostream &out) const {out << "MiceElectroMagneticField ";}

	G4bool   DoesFieldChangeEnergy() const {return true;}
	BTField* GetField()                    {return (BTField*)_emField;}

private:
	BTField       * _emField;
};

#endif
