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

TEST(MAUSVisManagerTest, TestVis) {
    // there is another test at map level that checks we actually make a file
    MAUS::MAUSVisManager* vis = new MAUS::MAUSVisManager ();
    vis->Initialize();
    Json::Value& config = *MICERun::getInstance()->jsonConfiguration;
    config["geant4_visualisation"] = true;
    config["visualisation_viewer"] = "VRML2FILE";
    config["visualisation_theta"] = 90.;
    config["visualisation_phi"] = 90.;
    config["visualisation_zoom"] = 2.;
    vis->SetupRun();
    vis->TearDownRun();
}

}

