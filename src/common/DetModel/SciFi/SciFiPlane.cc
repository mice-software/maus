/*
SciFiPlane - Specific parameterisation of the SciFi station to be called by the generic MICE detector construction code
*/
#include "G4Material.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVParameterised.hh"

#include "G4UserLimits.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4ios.hh"

#include "SciFiPlane.hh"

#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "globals.hh"

#include "SciFiSD.hh"
#include "DoubletFiberParam.hh"
#include "G4SDManager.hh"

SciFiPlane::SciFiPlane( MiceModule* mod, G4Material* mater, G4VPhysicalVolume *mlv )
{
  miceMemory.addNew( Memory::SciFiPlane ); 
	
  G4double tr = mod->dimensions().x();
  G4double fd = mod->propertyDouble( "FibreDiameter" );
  G4double fp = mod->propertyDouble( "Pitch" );
  G4double cd = mod->propertyDouble( "CoreDiameter" );
  G4double ar = mod->propertyDouble( "ActiveRadius" );

  G4double doubletThickness = mod->dimensions().y();

  G4String doubletName = mod->fullName() + "Doublet";
  G4String coreName = mod->fullName() + "DoubletCores";
  G4String clad1Name = mod->fullName() + "DoubletClad1";
  G4String clad2Name = mod->fullName() + "DoubletClad2";

  //G4RotationMatrix* trot =new G4RotationMatrix( mod->rotation() );

  solidDoublet = new G4Tubs( doubletName, 0.0, tr, doubletThickness / 2.0, 0.0 * deg, 360.0 * deg );

  logicDoublet = new G4LogicalVolume( solidDoublet, mater, doubletName, 0, 0, 0 );

  physiDoublet = placeCore = new G4PVPlacement( 0, mod->position(), logicDoublet, doubletName, mlv->GetLogicalVolume(), false, 0 );

  G4double tlen = 1.0 * mm;

  G4int num = (G4int)floor( 2. * ar / ( 0.5 * fp ) );

  // Beginning of the fiber core definitions

  solidCore = new G4Tubs( coreName, 0.0, cd / 2., tlen, 0.0 * deg, 360.0 * deg );

  logicCore = new G4LogicalVolume( solidCore, mater, coreName, 0, 0, 0 );

  G4VPVParameterisation* coreParam = new DoubletFiberParam( ar, ar, cd, 0.0, fd, fp / fd );
   
  physiCore = new G4PVParameterised( coreName, logicCore, physiDoublet, kUndefined, num, coreParam );
}

SciFiPlane::~SciFiPlane()
{
  	miceMemory.addDelete( Memory::SciFiPlane ); 
	
	delete solidDoublet;
	delete logicDoublet;
	delete physiDoublet;

	delete solidCore;
	delete logicCore;
	delete physiCore;
}
