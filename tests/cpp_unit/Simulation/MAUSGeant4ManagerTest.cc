// Copyright 2011 Chris Rogers
//
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

#include <G4RunManager.hh>
#include <G4SDManager.hh>

#include "src/common_cpp/Simulation/MAUSGeant4Manager.hh"

namespace {

// I think all I can do here is test that members were initialised to something
TEST(MAUSGeant4ManagerTest, GetSetTest) {
    MAUSGeant4Manager* g4man = MAUSGeant4Manager::GetInstance();
    ASSERT_TRUE(g4man != NULL);

    ASSERT_TRUE(g4man->GetRunManager() != NULL);
    ASSERT_TRUE(g4man->GetStepping() != NULL);
    ASSERT_TRUE(g4man->GetPrimaryGenerator() != NULL);
    ASSERT_TRUE(g4man->GetGeometry() != NULL);
    ASSERT_TRUE(g4man->GetTracking() != NULL);
    ASSERT_TRUE(g4man->GetPhysicsList() != NULL);
    ASSERT_TRUE(g4man->GetVirtualPlanes() != NULL);
}

TEST(MAUSGeant4ManagerTest, GetReferenceParticleTest) {
    Json::Value* conf = MICERun::getInstance()->jsonConfiguration;
    Json::Value pos(Json::objectValue);
    pos["x"] = pos["y"] = pos["z"] = 0.;

    // read of json value is dealt with elsewhere
    (*conf)["simulation_reference_particle"]["particle_id"] = Json::Value(111);
    (*conf)["simulation_reference_particle"]["position"] = pos;
    (*conf)["simulation_reference_particle"]["momentum"] = pos;
    (*conf)["simulation_reference_particle"]["energy"] = -1.;
    (*conf)["simulation_reference_particle"]["time"] = -2.;
    (*conf)["simulation_reference_particle"]["random_seed"] = -2;
    EXPECT_EQ(MAUSGeant4Manager::GetInstance()->GetReferenceParticle().pid, 111);
}

TEST(MAUSGeant4ManagerTest, SetPhasesTest) {
    EXPECT_TRUE(false);
}


TEST(MAUSGeant4ManagerTest, RunParticleTest) {
    MAUS::MAUSPrimaryGeneratorAction::PGParticle part_in;
    part_in.x = 1.;
    part_in.y = 2.;
    part_in.z = 3.;
    part_in.time = 4.;
    part_in.px = 5.;
    part_in.py = 6.;
    part_in.pz = 7.;
    part_in.energy = 200.;
    part_in.seed = 10;
    part_in.pid = -13;

    Json::Value val = MAUSGeant4Manager::GetInstance()->RunParticle(part_in);
    ASSERT_TRUE(val["tracks"][Json::UInt(0)].isObject());
    Json::Value track = val["tracks"][Json::UInt(0)];
    EXPECT_NEAR(track["initial_position"]["x"].asDouble(), 1., 1e-9);
    EXPECT_NEAR(track["initial_position"]["y"].asDouble(), 2., 1e-9);
    EXPECT_NEAR(track["initial_position"]["z"].asDouble(), 3., 1e-9);
}

}

