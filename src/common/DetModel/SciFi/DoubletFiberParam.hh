// MAUS WARNING: THIS IS LEGACY CODE.
/*
** Modified example 1 from the GEANT4 distribution to simulate the
** MICE scintillating fiber tracker for the MICE proposal
** Ed McKigney - August 21, 2002
*/
#ifndef DoubletFiberParam_H
#define DoubletFiberParam_H 1

#include "G4RotationMatrix.hh"
#include "globals.hh"
#include "G4VPVParameterisation.hh"

class G4VPhysicalVolume;
class G4Tubs;

/** DoubletFiberParam stores physical parameters of the Scintillating Fibre doublets (or planes)
*   which are used to describe the Scintillating Fibre tracker in GEANT4
**/

class DoubletFiberParam : public G4VPVParameterisation
{
public:
	//! Constructor taking the parameters that define the doublet planes
	DoubletFiberParam(G4double, G4double, G4double, G4double, G4double, 
			  G4double);

	//! Calculate the transformations needed to instantiate an instance of the SciFi doublet in a certain G4 physical volume
	void ComputeTransformation (const G4int copyNo,
				    G4VPhysicalVolume* physVol) const;

	//! Calculate the dimensions of the fibres that make up a doublet layer
	void ComputeDimensions (G4Tubs &, const G4int ,
				const G4VPhysicalVolume*) const;

	// return active radius
	G4double		getActiveRadius() const { return activeRadius; };
	G4double		getSensitiveRadius() const { return sensitiveRadius; };
	G4double		getInnerDiameter() const { return innerDiameter; };
	G4double		getOuterDiameter() const { return outerDiameter; };
	G4double		getFiberDiameter() const { return fiberDiameter; };
	G4double		getFiberPitch() const { return fiberPitch; };

private:
	G4double activeRadius;
	G4double sensitiveRadius;
	G4double innerDiameter;
	G4double outerDiameter;
	G4double fiberDiameter;
	G4double fiberPitch;

	G4RotationMatrix* coreRotation;
};

#endif
