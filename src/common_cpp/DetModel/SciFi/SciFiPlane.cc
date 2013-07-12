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

#include "Geant4/G4Material.hh"
#include "Geant4/G4Tubs.hh"
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

#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Vector/Rotation.h"
#include "src/common_cpp/DataStructure/ThreeVector.hh"

#include "DetModel/SciFi/SciFiPlane.hh"
#include "DetModel/SciFi/SciFiSD.hh"
#include "DetModel/SciFi/DoubletFiberParam.hh"

// SciFiPlane - Specific parameterisation of the SciFi station
// to be called by the generic MICE detector construction code.
//
// Geometry layout to be documented!
//

SciFiPlane::SciFiPlane(MiceModule* mod,
                       G4Material* mater,
                       G4VPhysicalVolume *mlv) {
  G4double tr = mod->dimensions().x();
  G4double fd = mod->propertyDouble("FibreDiameter");
  G4double fp = mod->propertyDouble("Pitch");
  G4double cd = mod->propertyDouble("CoreDiameter");
  G4double ar = mod->propertyDouble("ActiveRadius");

  G4double doubletThickness = mod->dimensions().y();

  G4String doubletName = mod->fullName() + "Doublet";
  G4String coreName    = mod->fullName() + "DoubletCores";

  // Rotation of the fibre wrt the station body.
  G4RotationMatrix* pRot = new G4RotationMatrix(mod->relativeRotation(mod->mother()));

  CLHEP::HepRotation xflip;
  const Hep3Vector rowx(1., 0., 0.);
  const Hep3Vector rowy(0., -1., 0.);
  const Hep3Vector rowz(0., 0., -1.);
  xflip.setRows(rowx, rowy, rowz);

  // this is the rotation of the fibre array
  (*pRot) = (*pRot)*xflip;
  // This is a fibre
  solidDoublet = new G4Tubs(doubletName, 0.0,
                            tr, doubletThickness / 2.0,
                            0.0 * deg, 360.0 * deg);

  logicDoublet = new G4LogicalVolume(solidDoublet, mater, doubletName, 0, 0, 0);

  // Note from G4PVPlacement.hh:
  // Initialise a single volume, positioned in a frame which is rotated by
  // "*pRot" and traslated by tlate, relative to the coordinate system of the
  // mother volume "mlv->GetLogicalVolume()".
  physiDoublet = placeCore = new G4PVPlacement(pRot, mod->position(),
                                               logicDoublet, doubletName,
                                               mlv->GetLogicalVolume(),
                                               false, 0);

  // lenght of the tube of fibre
  G4double tlen = 1.0 * mm;

  // the number of fibres to be simulated
  _numFibres = (G4int)floor(2. * ar / (0.5 * fp));

  // Beginning of the fiber core definitions
  solidCore = new G4Tubs(coreName, 0.0, cd / 2., tlen, 0.0 * deg, 360.0 * deg);

  logicCore = new G4LogicalVolume(solidCore, mater, coreName, 0, 0, 0);

  G4VPVParameterisation* coreParam = new DoubletFiberParam(ar, ar, cd,
                                                           0.0, fd, fp / fd);

  physiCore = new G4PVParameterised(coreName, logicCore, physiDoublet,
                                    kUndefined, _numFibres, coreParam);
}

SciFiPlane::~SciFiPlane() {
  delete physiDoublet;
  delete physiCore;

  delete solidDoublet;
  delete solidCore;

  delete logicDoublet;
  delete logicCore;
}
