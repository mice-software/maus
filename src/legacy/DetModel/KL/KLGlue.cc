// MAUS WARNING: THIS IS LEGACY CODE.
#include "DetModel/KL/KLGlue.hh"

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

KLGlue::~KLGlue()
{
  if(fSolidGlueStrip)       delete fSolidGlueStrip;
  if(fLogicGlueStrip)       delete fLogicGlueStrip;

  if(fSolidHole)       delete fSolidHole;
  if(fLogicHole)       delete fLogicHole;
}


/////////////////////////////////////////////////////////////////////////////
//
//
KLGlue::KLGlue(MiceModule* mod, G4Material* mater, G4VPhysicalVolume *mlv )
{
  G4double zStart, zStep, z, yStart, yStep, y;

  G4double cellY = mod->dimensions().y();
  G4double cellZ = mod->dimensions().z();
  G4double fiberRad = 0.5*mod->propertyDouble( "FibreDiameter" );
  G4double fiberLength = mod->propertyDouble( "FibreLength" );
  G4double fiberSpacing = mod->propertyDouble( "FibreSpacingY" );
  G4double fiberSpacingZ = mod->propertyDouble( "FibreSpacingZ" );
  G4double glueThickness = mod->propertyDouble( "GlueThickness" );
  G4int    cellNr = mod->propertyInt("Cell");

  G4ThreeVector nullVect = G4ThreeVector(0,0,0);
  G4RotationMatrix rm0;
  G4RotationMatrix rm90;
  rm90.rotateY(90.*deg);

  // The strip of glue between lead foils, center of fiber = center of strip
  G4double glueStripW = 0.5*fiberSpacing-fiberRad;
  fSolidGlueStrip = new G4Box("sGlueStripKL", 0.5*fiberLength, 0.5*glueStripW, 0.5*glueThickness);
  fLogicGlueStrip = new G4LogicalVolume(fSolidGlueStrip, mater, "lGlueStripKL");
  G4ThreeVector upVect = G4ThreeVector(0,fiberRad+0.5*glueStripW,0);
  G4ThreeVector dnVect = G4ThreeVector(0,-fiberRad-0.5*glueStripW,0);

  // The space to put the fiber inside
  G4double holeRad = 0.5*glueThickness+fiberRad;
  fSolidHole = new G4Tubs("sHoleKL", fiberRad, holeRad,
                            0.5*fiberLength ,0.0,2*pi) ;
  fLogicHole = new G4LogicalVolume(fSolidHole, mater, "lHoleKL");

  G4AssemblyVolume* assemblyCell = new G4AssemblyVolume();
  assemblyCell->AddPlacedVolume( fLogicGlueStrip, upVect, &rm0 );
  assemblyCell->AddPlacedVolume( fLogicGlueStrip, dnVect, &rm0 );
  assemblyCell->AddPlacedVolume( fLogicHole, nullVect, &rm90 );

  G4AssemblyVolume* assemblyCell2 = new G4AssemblyVolume();
  assemblyCell->AddPlacedVolume( fLogicGlueStrip, upVect, &rm0 );
  assemblyCell->AddPlacedVolume( fLogicHole, nullVect, &rm90 );

  yStep  = fiberSpacing;
  if (fiberSpacingZ < 0){
    zStep  = yStep*0.5*sqrt(3); //triangular, KLOE standard
  } else {
    zStep = fiberSpacingZ; // KLOE light
  }
  zStart = -0.5*cellZ+zStep; // boundaries are glue filled holes
  yStart = -0.5*cellY+0.5*yStep;
  int layer = 0;
  z = zStart;

  while(z<=fabs(zStart)){
    y = yStart+(layer%2)*0.5*yStep; // offset every second layer
    while(y<=fabs(yStart)){
      G4ThreeVector Tm( 0,y,z);
      assemblyCell->MakeImprint( mlv->GetLogicalVolume(), Tm, &rm0, cellNr );
      y += yStep;
    }
    if ( (cellNr%10 !=0) && (layer%2 != 0)) {
      // add one extra row at the bottom (not copied over by next cell)
      y = yStart-0.5*yStep;
      G4ThreeVector Tm( 0,y,z);
      assemblyCell2->MakeImprint( mlv->GetLogicalVolume(), Tm, &rm0, cellNr );
    }
    layer++;
    z += zStep;
  }

  G4VisAttributes* visAttInv = new G4VisAttributes(false);
  if (fLogicGlueStrip)  fLogicGlueStrip  ->SetVisAttributes(visAttInv);
  if (fLogicHole)  fLogicHole  ->SetVisAttributes(visAttInv);

}

