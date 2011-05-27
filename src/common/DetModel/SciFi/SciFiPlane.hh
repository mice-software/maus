/*
** Modified example 1 from the GEANT4 distribution to simulate the
** MICE scintillating fiber tracker for the MICE proposal
** August 21, 2002
*/
#ifndef SciFiPlane_h
#define SciFiPlane_h 1

#include "G4Tubs.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"
#include "Config/MiceModule.hh"

class G4Tubs;
class G4LogicalVolume;
class G4VPhysicalVolume;

/**
* A SciFiPlane describes a plane (or projection, view, doublet layer) of scintillating fibre
* detector. This class is used in the GEANT4 simulation of the Sci Fi tracker to describe
* physical arrangement of all the fibres in a doublet layer, including the description of
* the cladding and core of each scintillating fibre.
**/

class SciFiPlane
{

public:

        //! Constructor taking information from the SciFiTrackerGeom class

	SciFiPlane( MiceModule*, G4Material*, G4VPhysicalVolume* );

        //! Default destructor
	~SciFiPlane(); 

       // Return the physical volume of the core

       G4VPhysicalVolume*		physicalCore() const	{ return physiCore; };

       G4LogicalVolume*			logicalCore() const	{ return logicCore; };

       G4PVPlacement*			placementCore() const	{ return placeCore; };

private:

	G4Tubs*            solidDoublet;
	G4LogicalVolume*   logicDoublet; 
	G4VPhysicalVolume* physiDoublet; 

	G4Tubs*            solidCore;
	G4LogicalVolume*   logicCore; 
	G4VPhysicalVolume* physiCore; 

	G4Tubs*            solidCladding1;
	G4LogicalVolume*   logicCladding1; 
	G4VPhysicalVolume* physiCladding1; 

	G4Tubs*            solidCladding2;
	G4LogicalVolume*   logicCladding2; 
	G4VPhysicalVolume* physiCladding2; 

        G4PVPlacement*	   placeCore;
};

#endif
