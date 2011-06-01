// MAUS WARNING: THIS IS LEGACY CODE.
/*
** Modified example 1 from the GEANT4 distribution to simulate the
** MICE scintillating fiber tracker for the MICE proposal
** Ed McKigney - August 21, 2002
*/
#include "DetModel/SciFi/DoubletFiberParam.hh"

#include "G4VPhysicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"

DoubletFiberParam::DoubletFiberParam(
		G4double pSensitiveRadius,
		G4double pActiveRadius, 
		G4double pOuterDiameter, G4double pInnerDiameter,
		G4double pFiberDiameter, G4double pFiberPitch)
{
	sensitiveRadius = pSensitiveRadius;
	activeRadius = pActiveRadius;
	outerDiameter = pOuterDiameter;
	innerDiameter = pInnerDiameter;
	fiberDiameter = pFiberDiameter;
	fiberPitch = pFiberPitch;

	coreRotation = new G4RotationMatrix(CLHEP::HepRotationX(90.0*deg)); 
}



void DoubletFiberParam::ComputeTransformation
(const G4int copyNo, G4VPhysicalVolume* physVol) const
{
	G4double spacing = sqrt(fiberDiameter*fiberDiameter*
			        (1-fiberPitch*fiberPitch/4));
	G4double xPos = - copyNo*fiberDiameter*fiberPitch/2
		        + (activeRadius-fiberDiameter/2);
	G4double yPos = 0.0;
	G4double zPos = (copyNo%2) ? -0.5*spacing : 0.5*spacing;

	physVol->SetRotation(coreRotation);
	physVol->SetTranslation(G4ThreeVector(xPos, yPos, zPos));
}

void DoubletFiberParam::ComputeDimensions
(G4Tubs& fiberElement, const G4int copyNo,
  const G4VPhysicalVolume* physVol) const
{
	G4double nFiber = 2*floor(activeRadius/(fiberDiameter*fiberPitch/2));

	G4double xPos = 2*activeRadius*(copyNo/nFiber) 
		        - (activeRadius-fiberDiameter/2);

	G4double fiberHalfLen = 0.;

        if( sensitiveRadius > fabs( xPos ) )
          fiberHalfLen = sqrt( sensitiveRadius * sensitiveRadius - xPos * xPos );

	fiberElement.SetInnerRadius(innerDiameter/2);
	fiberElement.SetOuterRadius(outerDiameter/2);
	fiberElement.SetZHalfLength(fiberHalfLen);
	fiberElement.SetStartPhiAngle(0.0*deg);
	fiberElement.SetDeltaPhiAngle(360.0*deg);
}

