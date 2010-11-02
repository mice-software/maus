#include "EmCalKLFiber.hh"

#include "G4Material.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVParameterised.hh"
#include "G4AssemblyVolume.hh"

#include "G4UserLimits.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4ios.hh"

#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "globals.hh"

#include "DetModel/SciFi/SciFiSD.hh"
#include "G4SDManager.hh"

//////////////////////////////////////////////////////////////////////////
//
//

EmCalKLFiber::~EmCalKLFiber()
{
  miceMemory.addDelete( Memory::EmCalKLFiber ); 
	
  if(fSolidFiber)       delete fSolidFiber;
  if(fLogicFiber)       delete fLogicFiber;
}


/////////////////////////////////////////////////////////////////////////////
//
//
EmCalKLFiber::EmCalKLFiber(MiceModule* mod, G4Material* mater, G4VPhysicalVolume *mlv )
{
  miceMemory.addNew( Memory::EmCalKLFiber ); 
  
  G4double zStart, zStep, z, yStart, yStep, y;

//   G4double cellX = mod->dimensions().x();
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
    fSolidFiber = new G4Tubs("sFiberCAL", 0.0, fiberRad,
                              0.5*fiberLength ,0.0,2*pi) ;
    fLogicFiber = new G4LogicalVolume(fSolidFiber, mater, "lFiberCAL");
    
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
  //   int nbrFibers = 0;
    z = zStart;

    while(z<=fabs(zStart)){
      y = yStart+(layer%2)*0.5*yStep; // offset every second layer
      while(y<=fabs(yStart)){
        G4ThreeVector Tm( 0,y,z);
        assemblyCell->MakeImprint( mlv->GetLogicalVolume(), Tm, &rm0, cellNr );
  //       nbrFibers++;
        y += yStep;
      }
      if ( (cellNr%10 !=0) && (layer%2 != 0)) {
        // add one extra row at the bottom (not copied over by next cell)
        y = yStart-0.5*yStep;
        G4ThreeVector Tm( 0,y,z);
        assemblyCell->MakeImprint( mlv->GetLogicalVolume(), Tm, &rm0, cellNr );
      }
      layer++;
      z += zStep;
    }
  //   G4cout << "Fibers per EMCal cell = " << nbrFibers << G4endl;
  }

//     G4VisAttributes* visAttSciFi =
//                       new G4VisAttributes(true, G4Colour(0.87,0.87,0.45));
    G4VisAttributes* visAttInv = new G4VisAttributes(false);
    if (fLogicFiber) fLogicFiber ->SetVisAttributes(visAttInv);
//     if (fLogicFiber) fLogicFiber->SetVisAttributes(visAttSciFi);

}

