/* This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
 *
 * MAUS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MAUS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
// C++
#include <stdio.h>
#include <iostream>

// CLHEP
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/Rotation.h"
#include "CLHEP/Geometry/Transform3D.h"

// GEANT 4
#include "Geant4/G4NistManager.hh"
#include "Geant4/G4Material.hh"
#include "Geant4/G4Tubs.hh"
#include "Geant4/G4Box.hh"
#include "Geant4/G4Trap.hh"
#include "Geant4/G4LogicalVolume.hh"
#include "Geant4/G4PVPlacement.hh"
#include "Geant4/G4PVParameterised.hh"
#include "Geant4/G4UserLimits.hh"
#include "Geant4/G4VisAttributes.hh"
#include "Geant4/G4Colour.hh"
#include "Geant4/G4ios.hh"
#include "Geant4/G4ThreeVector.hh"
#include "Geant4/G4RotationMatrix.hh"
#include "Geant4/globals.hh"
#include "Geant4/G4SDManager.hh"

#include "DetModel/EMR/EMRBar.hh"
#include "DetModel/EMR/EMRSD.hh"
#include "DetModel/EMR/BarParameterisation.hh"

EMRBar::EMRBar(MiceModule* mod,
               G4Material* mater,
               G4VPhysicalVolume *mlv) {


  // --------- Material definition ---------
  G4NistManager* nistMan = G4NistManager::Instance();
  nistMan->SetVerbose(2);
  // nistMan->ListMaterials("all";)

  G4Material* Air = nistMan->FindOrBuildMaterial("G4_Galactic");
  G4Material* Polystyrene = nistMan->FindOrBuildMaterial("G4_POLYSTYRENE");

  G4String symbol;             // a=mass of a mole;
  G4double a, z, density;      // z=mean number of protons;
  G4int ncomponents, natoms;

  G4Element* H = new G4Element("Hydrogen", symbol="H", z = 1., a = 1.01*g/mole);
  G4Element* C = new G4Element("Carbon"  , symbol="C", z = 6., a = 12.01*g/mole);
  G4Element* O = new G4Element("Oxygen"  , symbol="O", z = 8., a = 15.9994*g/mole);

  G4Material* Glue = new G4Material("EpoxyGlue", density = 1.36*g/cm3, ncomponents = 3);
  Glue->AddElement(C, natoms = 261);
  Glue->AddElement(H, natoms = 304);
  Glue->AddElement(O, natoms = 40);

  G4Material* Acr = new G4Material("Acrylic", density = 1.18*g/cm3, ncomponents = 3);
  Acr->AddElement(C, natoms = 5);
  Acr->AddElement(O, natoms = 2);
  Acr->AddElement(H, natoms = 8);

  // Print all the materials defined.
  //
  // G4cout << G4endl << "The materials defined are : " << G4endl << G4endl;
  // G4cout << *(G4Material::GetMaterialTable()) << G4endl;

  // --------- Sizes of the principal geometrical components (solids)  ---------

  G4double fNbOfBars = mod->propertyDouble("NbOfBars");
  // std::cerr << "NUMBER OF BARS " << fNbOfBars << std::endl;
  G4double fBarWidth = mod->propertyDouble("BarWidth");
  G4double fBarHeight = mod->propertyDouble("BarHeight");
  G4double fBarLength = mod->propertyDouble("BarLength");
  G4double fHoleRad = mod->propertyDouble("HoleRad");
  G4double fFiberCladdingExtRadius = mod->propertyDouble("FiberCladdingExtRadius");
  G4double fGap = mod->propertyDouble("Gap");

  G4double fFiberCoreRadius = 0.95*fFiberCladdingExtRadius;
  G4Material* BarMater = Polystyrene;
  G4Material* HoleMater = Glue;
  G4Material* FiberCladdingExtMater = Acr;
  G4Material* FiberCoreMater = Polystyrene;
  G4double fCalorimeterLength = 1.2*fBarLength;
//   G4double fWorldLength = 1.2*fCalorimeterLength;
  // ----------------------------------------------------------------------------

  // ------------------------------
  // Calorimeter
  // ------------------------------
  G4double HalfCaloLength = 0.5*fCalorimeterLength;
  G4RotationMatrix *rotation = new G4RotationMatrix(mod->rotation());

  solidCalorimeter = new G4Box("calorimeter",
                               HalfCaloLength,
                               HalfCaloLength,
                               HalfCaloLength);

  logicCalorimeter = new G4LogicalVolume(solidCalorimeter, Air, "Calorimeter", 0, 0, 0);
  physiCalorimeter = new G4PVPlacement(rotation,
                                       mod->position(),
                                       logicCalorimeter,    // its logical volume
                                       "Calorimeter",       // its name
                                       mlv->GetLogicalVolume(), // its mother  volume
                                       false,               // no boolean operations
                                       0);                  // copy number

  // ----------------------------------------------------------------------------

  G4double pDz    = 0.5*fBarHeight; // Half-length along the z-axis

  G4double pTheta = 0.0;            // Polar angle of the line joining
                                    // the centres of the faces at -/+pDz

  G4double pPhi   = 0.0;            // Azimuthal angle of the line joining
                                    // the centres of the faces at -/+pDz

  G4double pDy1   = 0.5*fBarWidth;  // Half-length along y of the face at -pDz

  G4double pDx1   = 0.5*fBarLength; // Half-length along x of the side at
                                    // y=-pDy1 of the face at -pDz

  G4double pDx2   = 0.5*fBarLength; // Half-length along x of the side at
                                    // y=+pDy1 of the face at -pDz

  G4double pAlp1  = 0.0;     // Angle with respect to the y axis from the centre of the side
                             // at y=-pDy1 to the centre at y=+pDy1 of the face at -pDz

  G4double pDy2   = 0.01*cm; // Half-length along y of the face at +pDz

  G4double pDx3   = pDx1;    // Half-length along x of the side at y=-pDy2 of the face at +pDz

  G4double pDx4   = pDx1;    // Half-length along x of the side at y=+pDy2 of the face at +pDz

  G4double pAlp2  = pAlp1;   // Angle with respect to the y axis from the centre of the side
                             // at y=-pDy2 to the centre at y=+pDy2 of the face at +pDz

  solidBar = new G4Trap("bar", pDz, pTheta, pPhi, pDy1, pDx1,
                        pDx2, pAlp1, pDy2, pDx3, pDx4, pAlp2);

  logicBar = new G4LogicalVolume(solidBar, BarMater, "Bar", 0, 0, 0);

  barParam = new BarParameterisation(fBarWidth,
                                     fBarHeight,
                                     fNbOfBars,
                                     fGap);

  // Creates all of the bars at this stage
  // dummy value : kZAxis -- modified by parameterised volume
  physiBar = new G4PVParameterised("EMRBar",         // their name
                                   logicBar,         // their logical volume
                                   logicCalorimeter, // Mother logical volume
                                   kZAxis,           // Are placed along this axis
                                   fNbOfBars,        // Number of bars
                                   barParam);        // The parametrisation

  fAddWLSFiber = mod->propertyBool("AddWLSFiber");

  if (fAddWLSFiber) {
    // ------------------------------
    // Hole
    // ------------------------------
    G4RotationMatrix roty = G4RotationMatrix();
    roty.rotateY(90*deg);
    G4ThreeVector positionH = G4ThreeVector(0., 0., 0.05*mm);
    G4Transform3D transformH = G4Transform3D(roty, positionH);

    solidHole = new G4Tubs("Hole", 0.*mm, fHoleRad/2, fBarLength/2, 0.*deg, 360.*deg);
    logicHole = new G4LogicalVolume(solidHole, HoleMater, "Hole", 0, 0, 0);
    physiHole = new G4PVPlacement(transformH, // at (x,y,z)
                                  logicHole,  // its logical volume
                                  "Hole",     // its name
                                  logicBar,   // its mother  volume
                                  false,      // no boolean operations
                                  0);         // copy number

    // ------------------------------
    // Fiber Cladding
    // ------------------------------
    solidFiberCladding = new G4Tubs("FiberCladding",
                                    0.*mm,
                                    fFiberCladdingExtRadius/2,
                                    fBarLength/2,
                                    0.*deg,
                                    360.*deg);

    logicFiberCladding = new G4LogicalVolume(solidFiberCladding,
                                             FiberCladdingExtMater,
                                            "FiberCladding",
                                             0, 0, 0);

    physiFiberCladding =
    new G4PVPlacement(0,                     // no rotation
                      G4ThreeVector((fHoleRad-fFiberCladdingExtRadius)/2, 0., 0.),
                      logicFiberCladding,    // its logical volume
                      "FiberCladding",       // its name
                      logicHole,             // its mother  volume
                      false,                 // no boolean operations
                      0);                    // copy number

    // ------------------------------
    // Fiber Core
    // ------------------------------
    solidFiberCore = new G4Tubs("FiberCladding",
                                0.*mm,
                                fFiberCoreRadius/2,
                                fBarLength/2,
                                0.*deg,
                                360.*deg);

    logicFiberCore = new G4LogicalVolume(solidFiberCore,
                                         FiberCoreMater,
                                         "FiberCore",
                                         0, 0, 0);

    physiFiberCore = new G4PVPlacement(0,                  // no rotation
                                       G4ThreeVector(),
                                       logicFiberCore,     // its logical volume
                                       "FiberCore",        // its name
                                       logicFiberCladding, // its mother  volume
                                       false,              // no boolean operations
                                       0);                 // copy number
    // ------------------------------
  }// if (fAddWLSFiber)
}

EMRBar::~EMRBar() {
  //  delete physiCore;
}



