// This file is a part of MAUS
//
// MAUS is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// MAUS is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with MAUS in the doc folder.  If not, see
// <http://www.gnu.org/licenses/>.

#include "gtest/gtest.h"

#include "G4ViewParameters.hh"

#include "src/common_cpp/Simulation/MAUSVisManager.hh"
#include "src/legacy/Interface/MICERun.hh"

namespace {

// Test disabled - no way to get G4 to stop visualising once it's turned on
// (at least none that I could see)
// tested instead in MapCppSimulationVisualisation
/*
TEST(MAUSVisManagerTest, TestVis) {
    // there is another test at map level that checks we actually make a file
    Json::Value& config = *MICERun::getInstance()->jsonConfiguration;
    config["geant4_visualisation"] = true;
    config["visualisation_viewer"] = "VRML2FILE";
    config["visualisation_theta"] = 90.;
    config["visualisation_phi"] = 90.;
    config["visualisation_zoom"] = 2.;

    MAUS::MAUSVisManager* vis = new MAUS::MAUSVisManager();

    // I never check that isEnabled actually disables stuff. Really I only put
    // the flag in to stop tests producing loads of .wrl files 
    vis->SetIsEnabled( false );
    EXPECT_TRUE(!vis->GetIsEnabled());
    vis->SetIsEnabled( true );
    EXPECT_TRUE(vis->GetIsEnabled());

    // Check that we can run visualisation
    vis->Initialize();
    vis->SetupRun();
    vis->TearDownRun();

    // Switch it off (don't want the other tests making spurious g4_??.wrl
    vis->SetIsEnabled( false );
    config["geant4_visualisation"] = false;
}
*/


}

