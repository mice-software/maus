// @(#) $Id: Q35.hh,v 1.6 2006-11-03 20:01:09 hart Exp $  $Name:  $
//
// Q35 - implement a Q35 quadrupole magnet
//
// Tom Roberts
//

#ifndef Q35_H
#define Q35_H 1

#include "G4LogicalVolume.hh"

#include "Interface/Memory.hh" 

/**	class Q35 implements a Q35 quadrupole magnet
 **/
class Q35 {
	G4Material*	fBeamlineMaterial;
	G4double  	fQ35IronLength;
	G4double  	fQ35IronRadius;
	G4double  	fQ35ApertureRadius;
	G4double  	fQ35PoleTipRadius;
	G4double  	fQ35CoilRadius;
	G4double  	fQ35CoilHalfwidth;
	G4Material*	fQ35IronMaterial;

public:
	/// Constructor. Gets parameter values direct from
	/// BeamlineParameters::getInstance().
	Q35(
	G4Material * BeamlineMaterial,
	G4double IronLength,
	G4double IronRadius,
	G4double ApertureRadius,
	G4double PoleTipRadius,
	G4double CoilRadius,
	G4double CoilHalfwidth,
	G4Material * IronMaterial
	);

	~Q35() 		{ miceMemory.addDelete( Memory::Q35 ); };
	
	/// buildQ35() returns the LogicalVolume of the magnet
	G4LogicalVolume *buildQ35();

};

#endif
