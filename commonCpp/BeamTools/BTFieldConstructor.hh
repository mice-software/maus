//BTFieldConstructor constructs the MICE fields
//At the moment only Mice Stage 6 is implemented but in principle
//easy to implement other Mice Stages
//
//Also handles misc tasks such as writing out the field map.
//
//Also don't use this class for a constant field.
//In this case we use the GEANT4 constant field class
//for more processor efficient stepping algorithm
//

#ifndef BTMAGNETICFIELDCONSTRUCTOR_HH
#define BTMAGNETICFIELDCONSTRUCTOR_HH

#include "Config/MagnetParameters.hh"
#include "Config/RFParameters.hh"

#include "BTFieldGroup.hh"
#include "BTPhaser.hh"

#include <iostream>

class MiceModule;

class BTFieldConstructor : public BTFieldGroup
{
public:
	BTFieldConstructor(MiceModule * rootModule);
	BTFieldConstructor();
	~BTFieldConstructor() {;}

	void BuildFields(MiceModule * rootModule);
	bool DoesFieldChangeEnergy() {return BTFieldGroup::DoesFieldChangeEnergy();}
	//Return true if a module has electromagnetic field, false if no field or magnetic
	bool ModuleHasElectromagneticField(const MiceModule * theModule);
	//Return true if BTFields are used in the geometry
	static bool HasBTFields() {return (MagnetParameters::getInstance())->FieldMode()=="Full";}
	//Return true if has rf
	bool HasRF() {return DoesFieldChangeEnergy();}

	BTField * GetMagneticField()        {return _magneticField;}
	BTField * GetElectroMagneticField() {return _electroMagneticField;}
	//Set default values from datacards - can be called statically
	static void SetDefaults();
	//Construct MiceModule fields
	BTField * GetField            (const MiceModule * theModule);
	BTField * GetSolenoid         (const MiceModule * theModule);
	BTField * GetFastSolenoid     (const MiceModule * theModule);
	BTField * GetMagFieldMap      (const MiceModule * theModule);
	BTField * GetRFCavity         (const MiceModule * theModule); //Pill box or field map
	BTField * GetDipole           (const MiceModule * theModule);
	BTField * GetQuadrupole       (const MiceModule * theModule); //needs sorting
	BTField * GetMultipole        (const MiceModule * theModule);
	BTField * GetCombinedFunction (const MiceModule * theModule);
	BTField * GetConstantField    (const MiceModule * theModule);
	BTField * GetFieldAmalgamation(const MiceModule * theModule);

	static std::vector< std::string > GetFieldTypes() {return std::vector<std::string>(_fieldTypes, _fieldTypes+_numberOfFieldTypes);}
        //Set name parameter of newField from theModule - if theModule is NULL or name parameter is not defined, throw
	void   SetName(BTField* newField, const MiceModule* theModule) const;

private:
	BTFieldGroup * _magneticField;
	BTFieldGroup * _electroMagneticField;
	//Tell the Phaser where to go to set the phase
	void SetPhaser() {BTPhaser::SetGlobalField(this, _needsPhases);}
	bool                           _needsPhases;
	static       std::string       _defaultSolenoidMode;
	static const std::string       _fieldTypes[];
	static const int               _numberOfFieldTypes; 
	std::vector<const MiceModule*> _fieldMapsForWriting;
	int                            _amalgamatedFields;
	static       int               _fieldNameIndex;

	//WriteFieldMaps - needs to be done after all fields are loaded
	void WriteFieldMaps();


};

#endif
