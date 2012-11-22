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

#include "Geant4/G4RunManager.hh"
#include "Geant4/G4SDManager.hh"

#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/Simulation/MAUSPhysicsList.hh"
#include "src/common_cpp/Simulation/MAUSGeant4Manager.hh"

using namespace MAUS;

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
    ASSERT_TRUE(g4man->GetEventAction() != NULL);
}

TEST(MAUSGeant4ManagerTest, GetReferenceParticleTest) {
    Json::Value* conf = MAUS::Globals::GetInstance()->GetConfigurationCards();
    Json::Value pos(Json::objectValue);
    pos["x"] = pos["y"] = pos["z"] = 0.;

    // read of json value is dealt with elsewhere
    (*conf)["simulation_reference_particle"]["particle_id"] = Json::Value(111);
    (*conf)["simulation_reference_particle"]["position"] = pos;
    (*conf)["simulation_reference_particle"]["momentum"] = pos;
    (*conf)["simulation_reference_particle"]["energy"] = -1.;
    (*conf)["simulation_reference_particle"]["time"] = -2.;
    (*conf)["simulation_reference_particle"]["random_seed"] = Json::Int(2);
    EXPECT_EQ(MAUSGeant4Manager::GetInstance()->GetReferenceParticle().pid, 111);
}

TEST(MAUSGeant4ManagerTest, SetPhasesTest) {
    MAUSGeant4Manager::GetInstance()->SetPhases();  // just check it runs
}

TEST(MAUSGeant4ManagerTest, RunParticlePGTest) {
    MAUS::MAUSPrimaryGeneratorAction::PGParticle part_in;
    part_in.x = 1.;
    part_in.y = 2.;
    part_in.z = 3.;
    part_in.time = 4.;
    part_in.px = 5.;
    part_in.py = 6.;
    part_in.pz = 100.;
    part_in.energy = 200.;
    part_in.seed = 10;
    part_in.pid = -11; // e- so no decays etc

    // test that track is set ok
    Json::Value val = MAUSGeant4Manager::GetInstance()->RunParticle(part_in);
    ASSERT_TRUE(val["tracks"].isArray());
    ASSERT_TRUE(val["tracks"][Json::Value::UInt(0)].isObject());
    Json::Value track = val["tracks"][Json::Value::UInt(0)];
    EXPECT_NEAR(track["initial_position"]["x"].asDouble(), 1., 1e-9);
    EXPECT_NEAR(track["initial_position"]["y"].asDouble(), 2., 1e-9);
    EXPECT_NEAR(track["initial_position"]["z"].asDouble(), 3., 1e-9);

    // test that tracks can be switched on and off
    MAUSGeant4Manager::GetInstance()->GetTracking()->SetWillKeepTracks(false);
    val = MAUSGeant4Manager::GetInstance()->RunParticle(part_in);
    EXPECT_TRUE(val["tracks"].isNull());
    MAUSGeant4Manager::GetInstance()->GetTracking()->SetWillKeepTracks(true);
    val = MAUSGeant4Manager::GetInstance()->RunParticle(part_in);
    EXPECT_EQ(val["tracks"].type(), Json::arrayValue);

    // test that steps can be switched on and off
    MAUSGeant4Manager::GetInstance()->GetStepping()->SetWillKeepSteps(false);
    val = MAUSGeant4Manager::GetInstance()->RunParticle(part_in);
    EXPECT_TRUE(val["tracks"][Json::Value::UInt(0)]["steps"].isNull());
    MAUSGeant4Manager::GetInstance()->GetStepping()->SetWillKeepSteps(true);
    val = MAUSGeant4Manager::GetInstance()->RunParticle(part_in);
    EXPECT_EQ(val["tracks"][Json::Value::UInt(0)]["steps"].type(), Json::arrayValue);

    // test that virtuals can be switched on and off
    MAUSGeant4Manager::GetInstance()->
                             GetVirtualPlanes()->SetWillUseVirtualPlanes(false);
    val = MAUSGeant4Manager::GetInstance()->RunParticle(part_in);
    EXPECT_TRUE(val["virtual_hits"].isNull());
    MAUSGeant4Manager::GetInstance()->
                             GetVirtualPlanes()->SetWillUseVirtualPlanes(true);
    val = MAUSGeant4Manager::GetInstance()->RunParticle(part_in);
    EXPECT_EQ(val["virtual_hits"].type(), Json::arrayValue);
}

TEST(MAUSGeant4ManagerTest, RunParticleJsonTest) {
    std::string pg_string =
      "{\"primary\":{\"position\":{\"x\":1.0, \"y\":2.0, \"z\":3.0}, \"momentum\":{\"x\":0.0, \"y\":0.0, \"z\":1.0}, \"particle_id\":-13, \"energy\":226.0, \"time\":0.0, \"random_seed\":10}}";
    Json::Value pg = JsonWrapper::StringToJson(pg_string);
    MAUSGeant4Manager::GetInstance()->GetStepping()->SetWillKeepSteps(false);
    Json::Value out = MAUSGeant4Manager::GetInstance()->RunParticle(pg);
    Json::Value track = out["tracks"][Json::Value::UInt(0)];
    ASSERT_TRUE(track["initial_position"]["x"].isDouble());
    EXPECT_NEAR(track["initial_position"]["x"].asDouble(), 1., 1e-9);
    EXPECT_NEAR(track["initial_position"]["y"].asDouble(), 2., 1e-9);
    EXPECT_NEAR(track["initial_position"]["z"].asDouble(), 3., 1e-9);

    ASSERT_TRUE(out["primary"]["position"]["x"].isDouble());
    EXPECT_NEAR(out["primary"]["position"]["x"].asDouble(), 1., 1e-9);
    EXPECT_NEAR(out["primary"]["position"]["y"].asDouble(), 2., 1e-9);
    EXPECT_NEAR(out["primary"]["position"]["z"].asDouble(), 3., 1e-9);
}

TEST(MAUSGeant4ManagerTest, RunManyParticlesTest) {
    std::string pg_string =
      "{\"primary\":{\"position\":{\"x\":1.0, \"y\":2.0, \"z\":3.0}, \"momentum\":{\"x\":0.0, \"y\":0.0, \"z\":1.0}, \"particle_id\":-13, \"energy\":226.0, \"time\":0.0, \"random_seed\":10}}";
    std::string pg_array_string = "["+pg_string+","+pg_string+","+pg_string+"]";
    Json::Value pg = JsonWrapper::StringToJson(pg_array_string);
    MAUSGeant4Manager::GetInstance()->GetStepping()->SetWillKeepSteps(false);
    Json::Value out = MAUSGeant4Manager::GetInstance()->RunManyParticles(pg);
    for (size_t i = 0; i < out.size(); ++i) {
      Json::Value track = out[i]["tracks"][Json::Value::UInt(0)];
      ASSERT_TRUE(out.isArray());
      ASSERT_TRUE(out[i].isObject());
      ASSERT_TRUE(out[i]["tracks"].isArray());
      ASSERT_TRUE(out[i]["tracks"][Json::Value::UInt(0)].isObject());
      ASSERT_TRUE(track["initial_position"]["x"].isDouble());
      EXPECT_NEAR(track["initial_position"]["x"].asDouble(), 1., 1e-9);
      EXPECT_NEAR(track["initial_position"]["y"].asDouble(), 2., 1e-9);
      EXPECT_NEAR(track["initial_position"]["z"].asDouble(), 3., 1e-9);

      ASSERT_TRUE(out[i]["primary"].isObject());
      ASSERT_TRUE(out[i]["primary"]["position"]["x"].isDouble());
      EXPECT_NEAR(out[i]["primary"]["position"]["x"].asDouble(), 1., 1e-9);
      EXPECT_NEAR(out[i]["primary"]["position"]["y"].asDouble(), 2., 1e-9);
      EXPECT_NEAR(out[i]["primary"]["position"]["z"].asDouble(), 3., 1e-9);
    }
}

#include "src/legacy/Interface/Squeak.hh"
double get_energy(Json::Value virtual_hit) {
    double m =virtual_hit["mass"].asDouble();
    double px =virtual_hit["momentum"]["x"].asDouble();
    double py =virtual_hit["momentum"]["y"].asDouble();
    double pz =virtual_hit["momentum"]["z"].asDouble();
    return sqrt(m*m+px*px+py*py+pz*pz);
}

TEST(MAUSGeant4ManagerTest, ScatteringOffMaterialTest) {
    Squeak::setOutputs(0);
    Squeak::setStandardOutputs(0);
    MAUS::MAUSPrimaryGeneratorAction::PGParticle part_in;
    part_in.x = 0.;
    part_in.y = 0.;
    part_in.z = 1000.;
    part_in.time = 0.;
    part_in.px = 0.;
    part_in.py = 0.;
    part_in.pz = 1.; // just a direction
    part_in.energy = 5000.;
    part_in.seed = 10;
    part_in.pid = -13;

    MAUS::MAUSGeant4Manager * const simulator
                                       = MAUS::MAUSGeant4Manager::GetInstance();
    MAUS::VirtualPlaneManager const * const old_virtual_planes
                                                = simulator->GetVirtualPlanes();
    MAUS::VirtualPlaneManager * const virtual_planes
                                                = new MAUS::VirtualPlaneManager;
    MAUS::VirtualPlane end_plane = MAUS::VirtualPlane::BuildVirtualPlane(
        CLHEP::HepRotation(), CLHEP::Hep3Vector(0., 0., 2000.), -1, true,
        2000., BTTracker::z, MAUS::VirtualPlane::ignore, false);
    virtual_planes->AddPlane(new MAUS::VirtualPlane(end_plane), NULL);
    simulator->SetVirtualPlanes(virtual_planes);
    simulator->GetStepping()->SetWillKeepSteps(false);

    // mu+, mu-, e+, e0, pi-, pi+, p, 4He, K+, K-
    int pid_list[] = {-13, 13, -11, 11, -211, 211, 2212, 1000020040, 321, -321};
    // could add neutrons, antiprotons (though not for MICE)
    for (size_t pid_index = 0; pid_index < 10; ++pid_index) {
        part_in.pid = pid_list[pid_index];
        Json::Value hits(Json::arrayValue);
        hits.append(simulator->RunParticle(part_in)["virtual_hits"]);
        // move now into lH2
        part_in.z = 0.;
        hits.append(simulator->RunParticle(part_in)["virtual_hits"]);
        simulator->GetPhysicsList()->BeginOfReferenceParticleAction();
        hits.append(simulator->RunParticle(part_in)["virtual_hits"]);
        simulator->GetPhysicsList()->BeginOfRunAction();
        hits.append(simulator->RunParticle(part_in)["virtual_hits"]);
        part_in.z = 1000.;
        for (size_t i = 0; i < 4; ++i)
            ASSERT_EQ(1u, hits[i].size());

        // full physics and vacuum
        EXPECT_NEAR(0., hits[0u][0u]["momentum"]["x"].asDouble(), 1.0e-3)
          << "Failed with pid " << part_in.pid;
        EXPECT_NEAR(0., hits[0u][0u]["momentum"]["y"].asDouble(), 1.0e-3)
          << "Failed with pid " << part_in.pid;
        EXPECT_NEAR(5000., get_energy(hits[0u][0u]), 1.0e-3)
          << "Failed with pid " << part_in.pid;

        // full physics and lh2
        EXPECT_GE(fabs(0.-hits[1u][0u]["momentum"]["x"].asDouble()), 1.0e-3)
          << "Failed with pid " << part_in.pid;
        EXPECT_GE(fabs(0.-hits[1u][0u]["momentum"]["y"].asDouble()), 1.0e-3)
          << "Failed with pid " << part_in.pid;
        EXPECT_GE(fabs(5000.-get_energy(hits[1u][0u])), 1.0e-3)
          << "Failed with pid " << part_in.pid;

        // reference physics (mean dedx and no stochastics)
        EXPECT_NEAR(0., hits[2u][0u]["momentum"]["x"].asDouble(), 1.0e-3)
          << "Failed with pid " << part_in.pid;
        EXPECT_NEAR(0., hits[2u][0u]["momentum"]["y"].asDouble(), 1.0e-3)
          << "Failed with pid " << part_in.pid;
        EXPECT_GE(fabs(5000.-get_energy(hits[2u][0u])), 1.0e-3)
          << "Failed with pid " << part_in.pid;

        // full physics and lh2
        EXPECT_GE(fabs(0.-hits[3u][0u]["momentum"]["x"].asDouble()), 1.0e-3)
          << "Failed with pid " << part_in.pid;
        EXPECT_GE(fabs(0.-hits[3u][0u]["momentum"]["y"].asDouble()), 1.0e-3)
          << "Failed with pid " << part_in.pid;
        EXPECT_GE(fabs(5000.-get_energy(hits[3u][0u])), 1.0e-3)
          << "Failed with pid " << part_in.pid;
    }
    simulator->SetVirtualPlanes(
        const_cast<MAUS::VirtualPlaneManager *>(old_virtual_planes));
    delete virtual_planes;
}
}
