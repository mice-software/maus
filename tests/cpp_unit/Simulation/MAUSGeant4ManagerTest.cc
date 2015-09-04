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
#include "src/common_cpp/Globals/GlobalsManager.hh"
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
    pos["x"] = pos["y"] = pos["z"] = 1.;

    // read of json value is dealt with elsewhere
    (*conf)["simulation_reference_particle"]["particle_id"] = Json::Value(13);
    (*conf)["simulation_reference_particle"]["position"] = pos;
    (*conf)["simulation_reference_particle"]["momentum"] = pos;
    (*conf)["simulation_reference_particle"]["energy"] = 200.;
    (*conf)["simulation_reference_particle"]["time"] = -2.;
    (*conf)["simulation_reference_particle"]["random_seed"] = Json::Int(2);
    EXPECT_EQ(MAUSGeant4Manager::GetInstance()->GetReferenceParticle().pid, 13);
    // check that we set mass shell condition (details elsewhere)
    EXPECT_EQ(MAUSGeant4Manager::GetInstance()->GetReferenceParticle().px,
              MAUSGeant4Manager::GetInstance()->GetReferenceParticle().pz);
    EXPECT_GT(MAUSGeant4Manager::GetInstance()->GetReferenceParticle().px,
              MAUSGeant4Manager::GetInstance()->GetReferenceParticle().x);
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
    MAUSGeant4Manager* g4manager = MAUSGeant4Manager::GetInstance();
    g4manager->GetPhysicsList()->BeginOfReferenceParticleAction();
    // test that track is set ok
    MCEvent* event = g4manager->RunParticle(part_in);
    ASSERT_EQ(event->GetTracks()->size(), 1);
    Track track = event->GetTracks()->at(0);
    EXPECT_NEAR(track.GetInitialPosition().x(), 1., 1e-9);
    EXPECT_NEAR(track.GetInitialPosition().y(), 2., 1e-9);
    EXPECT_NEAR(track.GetInitialPosition().z(), 3., 1e-9);
    delete event;

    // test that tracks can be switched on and off
    g4manager->GetTracking()->SetWillKeepTracks(false);
    g4manager->GetStepping()->SetWillKeepSteps(false);
    event = g4manager->RunParticle(part_in);
    ASSERT_FALSE(event->GetTracks() == NULL);
    EXPECT_EQ(event->GetTracks()->size(), 0);
    MAUSGeant4Manager::GetInstance()->GetTracking()->SetWillKeepTracks(true);
    delete event;
    event = MAUSGeant4Manager::GetInstance()->RunParticle(part_in);
    ASSERT_FALSE(event->GetTracks() == NULL);
    ASSERT_EQ(event->GetTracks()->size(), 1);
    ASSERT_FALSE(event->GetTracks()->at(0).GetSteps() == NULL);
    EXPECT_EQ(event->GetTracks()->at(0).GetSteps()->size(), 0);
    delete event;

    // test that steps can be switched on and off
    MAUSGeant4Manager::GetInstance()->GetStepping()->SetWillKeepSteps(false);
    event = MAUSGeant4Manager::GetInstance()->RunParticle(part_in);
    ASSERT_FALSE(event->GetTracks()->at(0).GetSteps() == NULL);
    EXPECT_EQ(event->GetTracks()->at(0).GetSteps()->size(), 0);
    delete event;
    MAUSGeant4Manager::GetInstance()->GetStepping()->SetWillKeepSteps(true);
    event = MAUSGeant4Manager::GetInstance()->RunParticle(part_in);
    ASSERT_TRUE(event->GetTracks()->at(0).GetSteps() != NULL);
    EXPECT_GT(event->GetTracks()->at(0).GetSteps()->size(), 0);
    delete event;

    // Not such a good test as we don't have any virtual planes in the geometry
    // test that virtuals can be switched on and off
    g4manager->GetVirtualPlanes()->SetWillUseVirtualPlanes(false);
    event = MAUSGeant4Manager::GetInstance()->RunParticle(part_in);
    EXPECT_EQ(event->GetVirtualHits()->size(), 0);
    delete event;
    g4manager->GetVirtualPlanes()->SetWillUseVirtualPlanes(true);
    event = MAUSGeant4Manager::GetInstance()->RunParticle(part_in);
    EXPECT_EQ(event->GetVirtualHits()->size(), 0);
    delete event;
    g4manager->GetPhysicsList()->BeginOfRunAction();
}

TEST(MAUSGeant4ManagerTest, RunParticleCppTest) {
    MAUSGeant4Manager* g4manager = MAUSGeant4Manager::GetInstance();

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

    MAUS::Primary* prim = part_in.WriteCpp();
    g4manager->GetStepping()->SetWillKeepSteps(false);
    MCEvent* event = g4manager->RunParticle(*prim);
    EXPECT_NEAR(event->GetPrimary()->GetPosition().x(), 1., 1e-9);
    EXPECT_NEAR(event->GetPrimary()->GetPosition().y(), 2., 1e-9);
    EXPECT_NEAR(event->GetPrimary()->GetPosition().z(), 3., 1e-9);
    delete event;
    delete prim;
}

TEST(MAUSGeant4ManagerTest, RunManyParticlesTest) {
    std::string pg_string = 
      std::string("{\"primary\":{")+
      std::string("\"position\":{\"x\":1.0, \"y\":2.0, \"z\":3.0}, ")+
      std::string("\"momentum\":{\"x\":0.0, \"y\":0.0, \"z\":1.0}, ")+
      std::string("\"spin\":{\"x\":0.0, \"y\":0.0, \"z\":1.0}, ")+
      std::string("\"particle_id\":-13, \"energy\":226.0, ")+
      std::string("\"time\":0.0, \"random_seed\":10}}");
    std::string pg_array_string = "["+pg_string+","+pg_string+","+pg_string+"]";
    Json::Value pg = JsonWrapper::StringToJson(pg_array_string);
    MAUSGeant4Manager::GetInstance()->GetStepping()->SetWillKeepSteps(false);
    Json::Value out = MAUSGeant4Manager::GetInstance()->RunManyParticles(pg);
    for (int i = 0; i < int(out.size()); ++i) {
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

TEST(MAUSGeant4ManagerTest, RunManyParticlesCppTest) {
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
    MAUS::Primary* prim = part_in.WriteCpp();
    MCEvent* an_event = new MCEvent();
    an_event->SetPrimary(prim);
    std::vector<MCEvent*>* events = new std::vector<MCEvent*>();
    events->push_back(an_event);
    std::vector<MCEvent*>* out = MAUSGeant4Manager::GetInstance()->
                                                       RunManyParticles(events);
    ASSERT_EQ(out->size(), 1);
    ASSERT_EQ(out, events);
    delete events->at(0);
    delete events;
}

double get_energy(VirtualHit virtual_hit) {
    double m =virtual_hit.GetMass();
    double p =virtual_hit.GetMomentum().mag();
    return sqrt(m*m+p*p);
}

/*
Rogers - this test makes a segv somewhere in physics list
I have to reinitialise the physics model because the OpticsModel tests do...
*/
// #define MAUSGeant4ManagerTest_ScatteringOffMaterialTest
#ifdef MAUSGeant4ManagerTest_ScatteringOffMaterialTest

TEST(MAUSGeant4ManagerTest, ScatteringOffMaterialTest) {
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
    simulator->GetPhysicsList()->Setup();
    simulator->GetPhysicsList()->BeginOfRunAction(); // force physics list
    MAUS::VirtualPlaneManager*  old_virtual_planes
                = new MAUS::VirtualPlaneManager(*simulator->GetVirtualPlanes());
    MAUS::VirtualPlaneManager * const virtual_planes
                                                = new MAUS::VirtualPlaneManager;
    MAUS::VirtualPlane end_plane = MAUS::VirtualPlane::BuildVirtualPlane(
        CLHEP::HepRotation(), CLHEP::Hep3Vector(0., 0., 2000.), -1, true,
        2000., BTTracker::z, MAUS::VirtualPlane::ignore, false);
    virtual_planes->AddPlane(new MAUS::VirtualPlane(end_plane), NULL);
    virtual_planes->SetWillUseVirtualPlanes(true);
    simulator->SetVirtualPlanes(virtual_planes);
    simulator->GetStepping()->SetWillKeepSteps(false);

    // mu+, mu-, e+, e0, pi-, pi+, p, 4He, K+, K-
    int pid_list[] = {-13, 13, -11, 11, -211, 211, 2212, 1000020040, 321, -321};
    // could add neutrons, antiprotons (though not for MICE)
    for (size_t pid_index = 0; pid_index < 10; ++pid_index) {
        part_in.z = 1000.;
        part_in.pid = pid_list[pid_index];
        std::vector<VirtualHit>* vhits;
        // full physics and vacuum
        MCEvent* event = simulator->RunParticle(part_in);
        vhits = event->GetVirtualHits();
        ASSERT_EQ(vhits->size(), 1);
        EXPECT_NEAR(0., vhits->at(0).GetMomentum().x(), 1.0e-3)
          << "Failed with pid " << part_in.pid;
        EXPECT_NEAR(0., vhits->at(0).GetMomentum().y(), 1.0e-3)
          << "Failed with pid " << part_in.pid;
        EXPECT_NEAR(5000., get_energy(vhits->at(0)), 1.0e-3)
          << "Failed with pid " << part_in.pid;
        delete event;

        // move now into lH2
        part_in.z = 0.;
        event = simulator->RunParticle(part_in);
        vhits = event->GetVirtualHits();
        ASSERT_EQ(vhits->size(), 1);
        EXPECT_GE(fabs(vhits->at(0).GetMomentum().x()), 1.0e-3)
          << "Failed with pid " << part_in.pid;
        EXPECT_GE(fabs(vhits->at(0).GetMomentum().y()), 1.0e-3)
          << "Failed with pid " << part_in.pid;
        EXPECT_GE(fabs(5000.-get_energy(vhits->at(0))), 1.0e-3)
          << "Failed with pid " << part_in.pid;
        delete event;

        // reference physics (mean dedx and no stochastics)
        simulator->GetPhysicsList()->BeginOfReferenceParticleAction();
        event = simulator->RunParticle(part_in);
        vhits = event->GetVirtualHits();
        ASSERT_EQ(vhits->size(), 1);
        EXPECT_NEAR(0., vhits->at(0).GetMomentum().x(), 1.0e-3)
          << "Failed with pid " << part_in.pid;
        EXPECT_NEAR(0., vhits->at(0).GetMomentum().y(), 1.0e-3)
          << "Failed with pid " << part_in.pid;
        EXPECT_GE(fabs(5000.-get_energy(vhits->at(0))), 1.0e-3)
          << "Failed with pid " << part_in.pid;
        delete event;

        // full physics and lh2
        simulator->GetPhysicsList()->BeginOfRunAction();
        event = simulator->RunParticle(part_in);
        vhits = event->GetVirtualHits();
        ASSERT_EQ(vhits->size(), 1);
        EXPECT_GE(fabs(vhits->at(0).GetMomentum().x()), 1.0e-3)
          << "Failed with pid " << part_in.pid;
        EXPECT_GE(fabs(vhits->at(0).GetMomentum().y()), 1.0e-3)
          << "Failed with pid " << part_in.pid;
        EXPECT_GE(fabs(5000.-get_energy(vhits->at(0))), 1.0e-3)
          << "Failed with pid " << part_in.pid;
        delete event;
    }
    simulator->SetVirtualPlanes(old_virtual_planes);
}

#endif

}
