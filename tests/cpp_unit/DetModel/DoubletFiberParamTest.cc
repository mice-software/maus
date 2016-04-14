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
#include <iostream>
#include <fstream>
#include "src/legacy/Config/MiceModule.hh"
#include "src/common_cpp/DetModel/SciFi/DoubletFiberParam.hh"
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

#include "gtest/gtest.h"

namespace MAUS {

class DoubletFiberParamTest : public ::testing::Test {
 protected:
  DoubletFiberParamTest()  {
    // the following numbers are from MICE note 135:
    // "Specification of the scintillating and clear fibre for the MICE
    // scintillating fibre trackers"
    // note135_sensitive_radius
    // note135_active_radius
    // note135_inner_diameter
    note135_core_diameter = 0.308;  // mm
    note135_fiber_diameter = 0.350; // mm
    note135_fiber_pitch = 0.427;    // mm
//    calculated_fiber_half_length = 152.7126; // mm
    calculated_fiber_half_length = 153.76626; // mm
    expected_delta_phi = 6.28319;
//    calculated_X = 47.7375;
    calculated_X = 47.71725;
    calculated_Y = 0;
//    calculated_Z = -0.13867;
    calculated_Z = 0.13867;
    centralFibre = 749.5;
  }
  virtual ~DoubletFiberParamTest() {}
  virtual void SetUp()    {}
  virtual void TearDown() {}
  double note135_core_diameter;
  double note135_fiber_diameter;
  double note135_fiber_pitch;
  double calculated_fiber_half_length;
  double expected_delta_phi;
  double calculated_X;
  double calculated_Y;
  double calculated_Z;
  double centralFibre;
};

TEST_F(DoubletFiberParamTest, test_fiber_parameters) {
  std::string filename = "Stage4.dat";
  std::vector<const MiceModule*> modules;
  MiceModule*      _module;
  _module = new MiceModule(filename);
  modules = _module->findModulesByPropertyString("SensitiveDetector", "SciFi");

  const MiceModule* this_plane = NULL;
  for ( unsigned int j = 0; !this_plane && j < modules.size(); ++j ) {
  // find the module corresponding to this plane
  if ( modules[j]->propertyExists("Tracker", "int") &&
       modules[j]->propertyExists("Station", "int") &&
       modules[j]->propertyExists("Plane", "int") &&
       modules[j]->propertyInt("Tracker") == 0 &&
       modules[j]->propertyInt("Station") == 1 &&
       modules[j]->propertyInt("Plane") == 1 )
      // save the module
    this_plane = modules[j];
  }
  assert(this_plane != NULL);

  // read values from the Mice Module...
  G4double pCentralChannel  = this_plane->propertyDouble("CentralFibre");
  G4double pActiveRadius    = this_plane->propertyDouble("ActiveRadius");
  G4double pOuterDiameter   = this_plane->propertyDouble("CoreDiameter");
  G4double pInnerDiameter   = 0.0;
  G4double pFiberDiameter   = this_plane->propertyDouble("FibreDiameter");
  G4double pFiberPitch      = this_plane->propertyDouble("Pitch");
  G4double cf = pCentralChannel*7.0 + 4; // This is the central fibre!

  // .. feed them to DoubletFiberParam()
  G4double fetched_outer_diameter = DoubletFiberParam(cf,
                                                      pActiveRadius,
                                                      pOuterDiameter,
                                                      pInnerDiameter,
                                                      pFiberDiameter,
                                                      pFiberPitch).getOuterDiameter();
  G4double fetched_fiber_diameter = DoubletFiberParam(cf,
                                                      pActiveRadius,
                                                      pOuterDiameter,
                                                      pInnerDiameter,
                                                      pFiberDiameter,
                                                      pFiberPitch).getFiberDiameter();
  G4double fetched_fiber_pitch = DoubletFiberParam(cf,
                                                      pActiveRadius,
                                                      pOuterDiameter,
                                                      pInnerDiameter,
                                                      pFiberDiameter,
                                                      pFiberPitch).getFiberPitch();

  // ... compare with MICE Note 135
  EXPECT_NEAR(note135_core_diameter, fetched_outer_diameter, 1e-12);
  EXPECT_NEAR(note135_fiber_diameter, fetched_fiber_diameter, 1e-12);
  EXPECT_NEAR(note135_fiber_pitch, fetched_fiber_pitch, 1e-12);
}

TEST_F(DoubletFiberParamTest, test_compute_transformation) {
  std::string filename = "Stage4.dat";
  std::vector<const MiceModule*> modules;
  MiceModule*      _module;
  _module = new MiceModule(filename);
  modules = _module->findModulesByPropertyString("SensitiveDetector", "SciFi");

  const MiceModule* this_plane = NULL;
  for ( unsigned int j = 0; !this_plane && j < modules.size(); ++j ) {
  // find the module corresponding to this plane
  if ( modules[j]->propertyExists("Tracker", "int") &&
       modules[j]->propertyExists("Station", "int") &&
       modules[j]->propertyExists("Plane", "int") &&
       modules[j]->propertyInt("Tracker") == 0 &&
       modules[j]->propertyInt("Station") == 1 &&
       modules[j]->propertyInt("Plane") == 1 )
      // save the module
    this_plane = modules[j];
  }
  assert(this_plane != NULL);

  // read values from the Mice Module...
  G4double pCentralChannel  = this_plane->propertyDouble("CentralFibre");
  G4double pActiveRadius    = this_plane->propertyDouble("ActiveRadius");
  G4double pOuterDiameter   = this_plane->propertyDouble("CoreDiameter");
  G4double pInnerDiameter   = 0.0;
  G4double pFiberDiameter   = this_plane->propertyDouble("FibreDiameter");
  G4double pFiberPitch      = this_plane->propertyDouble("Pitch");
  G4double cf = pCentralChannel*7.0 + 4; // This is the central fibre!

  // Setup up dummy geometry to feed to ComputeDimensions and ComputeTransformation
  G4Tubs* fiberElement = new G4Tubs("testFiber", 0.0, 0.35 * mm, 1.0 * mm, 0.0 * deg, 360.0 * deg);
  int copyNo = 525;
  G4Material* mater = new G4Material("Test", 1, 2, 1);
  G4LogicalVolume* nullLogic = new G4LogicalVolume(fiberElement, mater, "testFiber", 0, 0, 0);
  G4String name = "testFiber";
  G4LogicalVolume* mother = 0;
  G4RotationMatrix* rot = new G4RotationMatrix(CLHEP::HepRotationX(0.0*deg));
  G4VPhysicalVolume* physVol = new G4PVPlacement(rot, G4ThreeVector(0., 0., 0.),
                                                 nullLogic, name, mother, false, 0);

  // Feeding geometries to functions
  DoubletFiberParam(cf,
                    pActiveRadius,
                    pOuterDiameter,
                    pInnerDiameter,
                    pFiberDiameter,
                    pFiberPitch).ComputeTransformation(copyNo, physVol);

  // EXPECT_EQ(0 , physVol->GetRotation());
  EXPECT_NEAR(calculated_X , physVol->GetTranslation().getX() , 0.0001);
  EXPECT_NEAR(calculated_Y , physVol->GetTranslation().getY() , 0.0001);
  EXPECT_NEAR(calculated_Z , physVol->GetTranslation().getZ() , 0.0001);
}

TEST_F(DoubletFiberParamTest, test_compute_dimension) {
  std::string filename = "Stage4.dat";
  std::vector<const MiceModule*> modules;
  MiceModule*      _module;
  _module = new MiceModule(filename);
  modules = _module->findModulesByPropertyString("SensitiveDetector", "SciFi");

  const MiceModule* this_plane = NULL;
  for ( unsigned int j = 0; !this_plane && j < modules.size(); ++j ) {
  // find the module corresponding to this plane
  if ( modules[j]->propertyExists("Tracker", "int") &&
       modules[j]->propertyExists("Station", "int") &&
       modules[j]->propertyExists("Plane", "int") &&
       modules[j]->propertyInt("Tracker") == 0 &&
       modules[j]->propertyInt("Station") == 1 &&
       modules[j]->propertyInt("Plane") == 1 )
      // save the module
    this_plane = modules[j];
  }
  assert(this_plane != NULL);

  // read values from the Mice Module...
  G4double pCentralChannel  = this_plane->propertyDouble("CentralFibre");
  G4double pActiveRadius    = this_plane->propertyDouble("ActiveRadius");
  G4double pOuterDiameter   = this_plane->propertyDouble("CoreDiameter");
  G4double pInnerDiameter   = 0.0;
  G4double pFiberDiameter   = this_plane->propertyDouble("FibreDiameter");
  G4double pFiberPitch      = this_plane->propertyDouble("Pitch");
  G4double cf = pCentralChannel*7.0 + 4; // This is the central fibre!

  // Setup up dummy geometry to feed to ComputeDimensions and ComputeTransformation
  G4Tubs* fiberElement = new G4Tubs("testFiber", 0.0, 0.35 * mm, 1.0 * mm, 0.0 * deg, 360.0 * deg);
  int copyNo = 525;
  G4Material* mater = new G4Material("Test", 1, 2, 1);
  G4LogicalVolume* nullLogic = new G4LogicalVolume(fiberElement, mater, "testFiber", 0, 0, 0);
  G4String name = "testFiber";
  G4LogicalVolume* mother = 0;
  G4RotationMatrix* rot = new G4RotationMatrix(CLHEP::HepRotationX(0.0*deg));
  G4VPhysicalVolume* physVol = new G4PVPlacement(rot, G4ThreeVector(0., 0., 0.),
                                                 nullLogic, name, mother, false, 0);

  DoubletFiberParam(cf,
                    pActiveRadius,
                    pOuterDiameter,
                    pInnerDiameter,
                    pFiberDiameter,
                    pFiberPitch).ComputeDimensions(*fiberElement, copyNo, physVol);

  EXPECT_EQ(0 , fiberElement->GetInnerRadius());
  EXPECT_EQ(note135_core_diameter/2.0 , fiberElement->GetOuterRadius());
  EXPECT_NEAR(calculated_fiber_half_length , fiberElement->GetZHalfLength() , 0.001);
  EXPECT_EQ(0 , fiberElement->GetStartPhiAngle());
  EXPECT_NEAR(expected_delta_phi , fiberElement->GetDeltaPhiAngle() , 0.0001);
}

} // ~namespace MAUS
