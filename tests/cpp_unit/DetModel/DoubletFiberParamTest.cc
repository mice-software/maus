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

#include "src/legacy/Config/MiceModule.hh"
#include "src/common_cpp/DetModel/SciFi/DoubletFiberParam.hh"

#include "gtest/gtest.h"

namespace {
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
  }
  virtual ~DoubletFiberParamTest() {}
  virtual void SetUp()    {}
  virtual void TearDown() {}
  double note135_core_diameter;
  double note135_fiber_diameter;
  double note135_fiber_pitch;
};

TEST_F(DoubletFiberParamTest, test_fiber_parameters) {
  std::string filename = "Stage6.dat";
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
  G4double pSensitiveRadius = this_plane->propertyDouble("ActiveRadius");
  G4double pActiveRadius    = this_plane->propertyDouble("ActiveRadius");
  G4double pOuterDiameter   = this_plane->propertyDouble("CoreDiameter");
  G4double pInnerDiameter   = 0.0;
  G4double pFiberDiameter   = this_plane->propertyDouble("FibreDiameter");
  G4double pFiberPitch = ( this_plane->propertyDouble("Pitch") ) /
                         ( this_plane->propertyDouble("FibreDiameter") );

  // .. feed them to DoubletFiberParam()
  G4double fetched_outer_diameter = DoubletFiberParam(pSensitiveRadius,
                                                      pActiveRadius,
                                                      pOuterDiameter,
                                                      pInnerDiameter,
                                                      pFiberDiameter,
                                                      pFiberPitch).getOuterDiameter();
  G4double fetched_fiber_diameter = DoubletFiberParam(pSensitiveRadius,
                                                      pActiveRadius,
                                                      pOuterDiameter,
                                                      pInnerDiameter,
                                                      pFiberDiameter,
                                                      pFiberPitch).getFiberDiameter();
  G4double fetched_fiber_pitch = DoubletFiberParam(pSensitiveRadius,
                                                      pActiveRadius,
                                                      pOuterDiameter,
                                                      pInnerDiameter,
                                                      pFiberDiameter,
                                                      pFiberPitch).getFiberPitch();

  // ... compare with MICE Note 135
  EXPECT_EQ(note135_core_diameter, fetched_outer_diameter);
  EXPECT_EQ(note135_fiber_diameter, fetched_fiber_diameter);
  EXPECT_EQ(note135_fiber_pitch/note135_fiber_diameter, fetched_fiber_pitch);
}

} // namespace
