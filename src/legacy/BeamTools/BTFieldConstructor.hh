// MAUS WARNING: THIS IS LEGACY CODE.
// Copyright 2006-2011 Chris Rogers
//
// This file is a part of G4MICE
//
// G4MICE is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// G4MICE is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with G4MICE in the doc folder.  If not, see 
// <http://www.gnu.org/licenses/>.

/// BTFieldConstructor constructs a field map from MiceModules
///
/// BTFieldConstructor uses MiceModules to build a set of field maps. Field maps
/// are identified by BTFieldConstructor as those MiceModules that contain the
/// PropertyString "FieldType", with value belonging to one of the field types.
/// For documentation on the parameter definitions in MiceModules, please see
/// MiceModule documentation (in, e.g. $MICESRC/doc).
///
/// BTFieldConstructor makes a loop over all MiceModules with "FieldType"
/// property and calls GetField - which decides the type of field and then
/// calls a function to interpret the MiceModule and construct a field of the
/// appropriate type.
///
/// Slight subtleties:
/// * G4 likes to have a MagneticField object and an ElectromagneticField 
///   object, so in fact we construct a magnetic FieldGroup and an
///   electromagnetic FieldGroup (which contains the magnetic FieldGroup).
/// * Some defaults are set up here.
/// * BTFieldGroup hides a grid used to "encourage" random access for field
///   lookups, which is also set up here. 


#ifndef BTFIELDCONSTRUCTOR_HH
#define BTFIELDCONSTRUCTOR_HH

#include "Config/MagnetParameters.hh"
#include "Config/RFParameters.hh"

#include "BeamTools/BTFieldGroup.hh"
#include "BeamTools/BTPhaser.hh"
#include "BeamTools/BTMultipole.hh"  // want EndFieldModel subclass

#include <iostream>

class MiceModule;

class BTFieldConstructor : public BTFieldGroup
{
public:
  /// Set up the fields using root module
	BTFieldConstructor(MiceModule * rootModule);
  /// Set up some defaults
	BTFieldConstructor();
  /// Destructor
	~BTFieldConstructor();
  /// Build the fields from root module
	void BuildFields(MiceModule * rootModule);
  /// Returns true if any child field changes energy - used to tell whether we
  /// need an electric field by geant4
	bool DoesFieldChangeEnergy() {return BTFieldGroup::DoesFieldChangeEnergy();}
  /// Returns true if the module is an RF cavity
  ///
  /// Detected by looking for a CavityMode property string (probably not the 
  /// most elegant solution)
	bool ModuleHasElectromagneticField(const MiceModule * theModule);
	/// Return true if BTFields are used in the geometry
  ///
  /// It is possible to ask G4MICE to use GEANT4 fields, which I guess might be
  /// desirable for some tests.
	static bool HasBTFields() {
    return (MagnetParameters::getInstance())->FieldMode()=="Full";
  }
	/// Return true if DoesFieldChangeEnergy is true - one assumes that means
  /// there are RF cavities which need phasing.
	bool HasRF() {return DoesFieldChangeEnergy();}
  /// Return the magnetic field component of the field map.
	BTField * GetMagneticField()        {return _magneticField;}
  /// Return the full field map (magnetic field is a child of electromagnetic
  /// field).
	BTField * GetElectroMagneticField() {return _electroMagneticField;}
	/// Set default values from datacards
	static void SetDefaults();
	/// Look for field type in theModule and build the appropriate field.
  ///
  /// If GetField doesn't recognise the field type, it gives a warning (but
  /// doesn't throw)
	BTField * GetField            (const MiceModule * theModule);
  /// Get a solenoidal field map
	BTField * GetSolenoid         (const MiceModule * theModule);
  /// Get a derivatives solenoid field map - found from a field expansion
	BTField * GetDerivativesSolenoid     (const MiceModule * theModule);
  /// Read a magnetic field map
	BTField * GetMagFieldMap      (const MiceModule * theModule);
  /// Read a sector magnetic field map
	BTField * GetSectorMagFieldMap      (const MiceModule * theModule);
  /// Generate an RF cavity (TM010 mode only) - pill box and RF field map
	BTField * GetRFCavity         (const MiceModule * theModule);
  /// Return a BTMultipole with parameters specific to dipoles
	BTField * GetDipole           (const MiceModule * theModule);
  /// Return a BTMultipole with parameters specific to quadrupoles
	BTField * GetQuadrupole       (const MiceModule * theModule);
  /// Return a BTMultipole object
	BTField * GetMultipole        (const MiceModule * theModule);
  /// Return a CombinedFunction magnet
	BTField * GetCombinedFunction (const MiceModule * theModule);
  /// Return a ConstantField in a cylindrical or cuboidal region
	BTField * GetConstantField    (const MiceModule * theModule);
  /// Return a collection of coaxial solenoidal fields, merged into a single
  /// field map (for speed purposes)
	BTField * GetFieldAmalgamation(const MiceModule * theModule);
  /// Return a model for the end field, presumably for a multipole of some sort
  BTMultipole::EndFieldModel* GetEndFieldModel(const MiceModule* theModule,
                                                                      int pole);
  /// Return a vector of possible field types
	static std::vector< std::string > GetFieldTypes() {
    return std::vector<std::string>(_fieldTypes, 
                                              _fieldTypes+_numberOfFieldTypes);
  }
  /// Set name parameter of newField from theModule - if theModule is NULL or
  /// name parameter is not defined, throw.
	void   SetName(BTField* newField, const MiceModule* theModule) const;

private:
	BTFieldGroup * _magneticField;
	BTFieldGroup * _electroMagneticField;
	//Tell the Phaser where to go to set the phase
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
