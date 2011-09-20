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

#include <G4VPhysicalVolume.hh>
#include <G4VSolid.hh>
#include <G4Material.hh>
#include <G4Element.hh>
#include <G4LogicalVolume.hh>
#include <G4ThreeVector.hh>
#include <G4PVPlacement.hh>
#include <globals.hh>
#include <G4SDManager.hh>
#include <G4UserLimits.hh>
#include <G4Tubs.hh>
#include <G4PVParameterised.hh>

#include <limits>

#include "src/legacy/Config/MiceModule.hh"

#include "src/common_cpp/DetModel/SciFi/SciFiPlane.hh"

#include "gtest/gtest.h"
#include "CLHEP/Random/Random.h"

namespace {
class SciFiPlaneTest : public ::testing::Test {
 protected:
  SciFiPlaneTest()  {
  G4double innerRadiusOfTheTube = 0.*cm;
  G4double outerRadiusOfTheTube = 30.*cm;
  G4double hightOfTheTube = 1.*m;
  G4double startAngleOfTheTube = 0.*deg;
  G4double spanningAngleOfTheTube = 360.*deg;

  G4Tubs* tracker_tube = new G4Tubs("tracker_tube",
                             innerRadiusOfTheTube,
                             outerRadiusOfTheTube,
                             hightOfTheTube,
                             startAngleOfTheTube,
                             spanningAngleOfTheTube);

  G4LogicalVolume* tracker_log
          = new G4LogicalVolume(tracker_tube, mat, "tracker_log");



  tracker = new G4PVPlacement(0, G4ThreeVector(),
                              "name", tracker_log,
                              0, false, 0, false);
  }
  virtual ~SciFiPlaneTest() {}
  virtual void SetUp()    {}
  virtual void TearDown() {}

  G4PVPlacement* tracker;
  G4Material* mat;
};

TEST_F(SciFiPlaneTest, Test_num_fibres_in_all_planes) {
  std::string filename;
  filename = "Stage6.dat";

  for ( int trackerNo = 0; trackerNo < 2; trackerNo++ ) {
    for ( int stationNo = 1; stationNo < 6; stationNo++ ) {
      for ( int planeNo = 0; planeNo < 3; planeNo++ ) {
        // finding the module here is time consuming...
        // but if it is done outside the for loops, we hit a seg fault (eventually).
        std::vector<const MiceModule*> modules;
        MiceModule*      _module;
        _module = new MiceModule(filename);
        modules = _module->findModulesByPropertyString("SensitiveDetector", "SciFi");
        const MiceModule* this_plane = NULL;
        for ( unsigned int j = 0; !this_plane && j < modules.size(); ++j ) {
          // find the right module
          if ( modules[j]->propertyExists("Tracker", "int") &&
               modules[j]->propertyExists("Station", "int") &&
               modules[j]->propertyExists("Plane", "int") &&
               modules[j]->propertyInt("Tracker") == trackerNo &&
               modules[j]->propertyInt("Station") == stationNo &&
               modules[j]->propertyInt("Plane") == planeNo )
            // save the module
            this_plane = modules[j];
        }
        assert(this_plane != NULL);

        // arguments are the plane module, material and mother physical volume
        MiceModule* aplane = const_cast < MiceModule* > (this_plane);
        SciFiPlane(aplane, mat, tracker);

        // plane 0 (view v), plane 1 (view x), plane 2 (view w)
        if ( trackerNo == 0 && stationNo == 5 && planeNo == 1 )
          EXPECT_EQ(SciFiPlane::SciFi_numFibres, 215*7);
        if ( planeNo == 0 || planeNo == 2 )
          EXPECT_EQ(SciFiPlane::SciFi_numFibres, 214*7);
        if ( planeNo == 1 && !(trackerNo == 0 && stationNo == 5) )
          EXPECT_EQ(SciFiPlane::SciFi_numFibres, 212*7);

        modules.clear();
        delete _module;
        delete this_plane;
        delete aplane;
      }
    }
  }
}

} // namespace
