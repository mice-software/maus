// MAUS WARNING: THIS IS LEGACY CODE.
#include "DetModel/KL/KLFiber.hh"

#include "Geant4/G4Material.hh"
#include "Geant4/G4Tubs.hh"
#include "Geant4/G4LogicalVolume.hh"
#include "Geant4/G4PVPlacement.hh"
#include "Geant4/G4PVParameterised.hh"
#include "Geant4/G4AssemblyVolume.hh"

#include "Geant4/G4UserLimits.hh"

#include "Geant4/G4VisAttributes.hh"
#include "Geant4/G4Colour.hh"

#include "Geant4/G4ios.hh"

#include "Geant4/G4ThreeVector.hh"
#include "Geant4/G4RotationMatrix.hh"
#include "Geant4/globals.hh"

#include "DetModel/SciFi/SciFiSD.hh"
#include "Geant4/G4SDManager.hh"

//////////////////////////////////////////////////////////////////////////
//
//

KLFiber::~KLFiber()
{
  if(fSolidFiber)       delete fSolidFiber;
  if(fLogicFiber)       delete fLogicFiber;
}


/////////////////////////////////////////////////////////////////////////////
//
//
KLFiber::KLFiber(MiceModule* mod, G4Material* mater, G4VPhysicalVolume *mlv )
{
  G4double zStart, zStep, z, yStart, yStep, y;

  G4double cellY = mod->dimensions().y();
  G4double cellZ = mod->dimensions().z();
  G4double fiberRad = 0.5*mod->propertyDouble( "FibreDiameter" );
  G4double fiberLength = mod->propertyDouble( "FibreLength" );
  G4double fiberSpacing = mod->propertyDouble( "FibreSpacingY" );
  G4double fiberSpacingZ = mod->propertyDouble( "FibreSpacingZ" );
  G4int    cellNr = mod->propertyInt("Cell");

  G4ThreeVector nullVect = G4ThreeVector(0,0,0);
  G4RotationMatrix rm0;
  G4RotationMatrix rm90;
  rm90.rotateY(90.*deg);

  if (fiberRad > 0) { // We can disable fibers by giving negative radius

    // Individial fiber
    fSolidFiber = new G4Tubs("sFiberKL", 0.0, fiberRad,
                              0.5*fiberLength ,0.0,2*pi) ;
    fLogicFiber = new G4LogicalVolume(fSolidFiber, mater, "lFiberKL");

    G4AssemblyVolume* assemblyCell = new G4AssemblyVolume();
    assemblyCell->AddPlacedVolume( fLogicFiber, nullVect, &rm90 );

    yStep  = fiberSpacing;
    if (fiberSpacingZ < 0){
      zStep  = yStep*0.5*sqrt(3); //triangular, KLOE standard
    } else {
      zStep = fiberSpacingZ; // KLOE light
    }
    zStart = -0.5*cellZ+zStep; // boundaries are glue filled holes
    yStart = -0.5*cellY+0.5*yStep;
    int layer = 0;
    int nbrFibers = 0;
    z = zStart;

    while(z<=fabs(zStart)){
      y = yStart+(layer%2)*0.5*yStep; // offset every second layer
      while(y<=fabs(yStart)){
        G4ThreeVector Tm( 0,y,z);
        assemblyCell->MakeImprint( mlv->GetLogicalVolume(), Tm, &rm0, cellNr );
        nbrFibers++;
        y += yStep;
	}
      if ( (cellNr%10 !=0) && (layer%2 != 0)) {
        // add one extra row at the bottom (not copied over by next cell)
        y = yStart-0.5*yStep;
        G4ThreeVector Tm( 0,y,z);
        assemblyCell->MakeImprint( mlv->GetLogicalVolume(), Tm, &rm0, cellNr );
        nbrFibers++;
      }
      layer++;
      z += zStep;    

}
  }

    G4VisAttributes* visAttInv = new G4VisAttributes(false);
    if (fLogicFiber) fLogicFiber ->SetVisAttributes(visAttInv);
}

