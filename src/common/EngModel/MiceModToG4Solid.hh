#ifndef MiceModToG4Solid_hh
#define MiceModToG4Solid_hh 1

#include "Config/MiceModule.hh"


//class G4VSolid;
//class MiceModule;

//Interface between MiceModule solids and GEANT4
//Note that when adding new volumes, you also need to update 
//Config/*/ModuleTextFileIO.*

namespace MiceModToG4Solid
{
	//Build a G4Solid if you don't know the solid type
	G4VSolid * buildSolid    ( MiceModule* );

	//If you know the solid type, you may prefer to call these methods
	//One for each supported G4Solid
	G4VSolid * buildWedge    ( MiceModule* );
	G4VSolid * buildBox      ( MiceModule* );
	G4VSolid * buildCylinder ( MiceModule* );
	G4VSolid * buildTube     ( MiceModule* );
	G4VSolid * buildSphere   ( MiceModule* );
	G4VSolid * buildPolycone ( MiceModule* );
	G4VSolid * buildTorus    ( MiceModule* );
	G4VSolid * buildBoolean  ( MiceModule* );
	G4VSolid * buildMultipole( MiceModule* );
	G4VSolid * buildEllipticalCone ( MiceModule* );

	G4VSolid * addUnion        (G4VSolid* solid, MiceModule * mod);
	G4VSolid * addIntersection (G4VSolid* solid, MiceModule * mod);
	G4VSolid * addSubtraction  (G4VSolid* solid, MiceModule * mod);
	MiceModule* getModule      (MiceModule* mod, std::string modName);


}

#endif

