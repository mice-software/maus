//BTField is the parent of all fields in the beamtools package
//Abstract Base Class
//Has simply a GetFieldValue method


#ifndef MICEMAGNETICFIELD_HH
#define MICEMAGNETICFIELD_HH

#include "BeamTools/BTFieldConstructor.hh"
#include "G4MagneticField.hh"

#include "Interface/Memory.hh" 

class MiceMagneticField : public G4MagneticField
{
public:
	MiceMagneticField(BTFieldConstructor * theField) : _magField(theField->GetMagneticField())
	{ miceMemory.addNew( Memory::MiceMagneticField); };

	MiceMagneticField()	{ miceMemory.addDelete( Memory::MiceMagneticField ); }; 
	
	//BUG - should be inlined!
	void GetFieldValue(const double Point[4], double * bfield) const
	{
		_magField->GetFieldValue(Point, bfield);
	}

	G4bool DoesFieldChangeEnergy() const {return false;}
	void   Print(std::ostream &out) const {out << "MiceElectroMagneticField ";}

private:
	BTField* _magField;
};

#endif
