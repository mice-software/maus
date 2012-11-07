// MAUS WARNING: THIS IS LEGACY CODE.
//BTField is the parent of all fields in the beamtools package
//Abstract Base Class
//Has simply a GetFieldValue method


#ifndef MICEMAGNETICFIELD_HH
#define MICEMAGNETICFIELD_HH

#include "BeamTools/BTFieldConstructor.hh"
#include "Geant4/G4MagneticField.hh"

class MiceMagneticField : public G4MagneticField
{
public:
	MiceMagneticField(BTFieldConstructor * theField) : _magField(theField->GetMagneticField())
  {};

  MiceMagneticField()	{};
	
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
