/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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

#include "gtest/gtest.h"

#include "Geant4/G4Region.hh"
#include "Geant4/G4RegionStore.hh"

#include "src/common_cpp/Simulation/MAUSGeant4Manager.hh"
#include "src/common_cpp/Utils/Globals.hh"
// #define wrapper for friend declarations
#define TESTS_CPP_UNIT_SIMULATION_DETECTORCONSTRUCTORTEST_CC
#include "src/common_cpp/Simulation/DetectorConstruction.hh"
#undef TESTS_CPP_UNIT_SIMULATION_DETECTORCONSTRUCTORTEST_CC

// Comment out functions due to memory problem/instability in G4

namespace MAUS {
namespace Simulation {
// Detector construction is pretty untestable because it is all wrapped up in
// G4 global stuff
class DetectorConstructionTest : public ::testing::Test {
  public:
    DetectorConstructionTest() {
        dc = Globals::GetInstance()->GetGeant4Manager()->GetGeometry();
        mod_path = std::string(getenv("MAUS_ROOT_DIR"))+
                   "/tests/cpp_unit/Simulation/TestGeometries/";
        p.z = -0.;
        p.pz = 1.;
        p.energy = 1.e6;
        p.pid = -13;
    }

    ~DetectorConstructionTest() {
        MiceModule mod("Test.dat");
        dc->SetMiceModules(mod);
    }

    void SetUp() {}
    void TearDown() {}

    MAUSPrimaryGeneratorAction::PGParticle p;
    DetectorConstruction* dc;
    std::string mod_path;
  private:
};

void SetStepperType(DetectorConstruction* dc, std::string type) {
    Json::Value& cards = *(Globals::GetInstance()->GetConfigurationCards());
    cards["stepping_algorithm"] = Json::Value(type);
    dc->SetDatacardVariables(cards);
}

TEST_F(DetectorConstructionTest, SetSteppingAlgorithmTest) {
    MiceModule modEM(mod_path+"EMFieldTest.dat");
    MiceModule modMag(mod_path+"MagFieldTest.dat");
    std::string models[] = {"ClassicalRK4", "Classic", "SimpleHeum",
              "ImplicitEuler", "SimpleRunge", "ExplicitEuler", "CashKarpRKF45"};
    // I just check the default - it is too slow to test everything
    for (int i = 0; i < 1; ++i) {
        SetStepperType(dc, models[i]);
        dc->SetMiceModules(modEM);
    }
    SetStepperType(dc, "error");
    EXPECT_THROW(dc->SetMiceModules(modMag), Exceptions::Exception);
    for (int i = 0; i < 1; ++i) {
        SetStepperType(dc, models[i]);
        dc->SetMiceModules(modMag);
    }
    SetStepperType(dc, "error");
    EXPECT_THROW(dc->SetMiceModules(modMag), Exceptions::Exception);
    SetStepperType(dc, "ClassicalRK4");
}


// Some instability in Geant4? I disable these tests by default because they
// seem to not be quite stable
// Could be some lurking darkness, I think just that Geant4 doesn't do
// reinitialisation properly
// #define FULL_GEANT4_TEST
#ifdef FULL_GEANT4_TEST

TEST_F(DetectorConstructionTest, RootVolumeTest) {
    // I dont test anywhere that the volume name is updated. I dont know how
    MiceModule mod(mod_path+"RootVolumeTest.dat");
    dc->SetMiceModules(mod);
    // Check that root volume gets correct size - tracks end on boundary edge
    // Check that user limits were set - track has (size/step_size)+1 steps
    Json::Value out = Globals::GetInstance()->GetGeant4Manager()->RunParticle(p);
    double z_end = out["tracks"][Json::Value::UInt(0)]["final_position"]["z"].asDouble();
    EXPECT_DOUBLE_EQ(z_end, 3000.);
    EXPECT_EQ(out["tracks"][Json::Value::UInt(0)]["steps"].size(), size_t(4));
    p.pz = 0.;
    p.px = 1.;
    out = Globals::GetInstance()->GetGeant4Manager()->RunParticle(p);
    double x_end = out["tracks"][Json::Value::UInt(0)]["final_position"]["x"].asDouble();
    EXPECT_DOUBLE_EQ(x_end, 1000.);
    EXPECT_EQ(out["tracks"][Json::Value::UInt(0)]["steps"].size(), size_t(2));
    p.px = 0.;
    p.py = 1.;
    out = Globals::GetInstance()->GetGeant4Manager()->RunParticle(p);
    double y_end = out["tracks"][Json::Value::UInt(0)]["final_position"]["y"].asDouble();
    EXPECT_DOUBLE_EQ(y_end, 2000.);
    EXPECT_EQ(out["tracks"][Json::Value::UInt(0)]["steps"].size(), size_t(3));
    // check that no energy deposited in vacuum
    Json::Value steps = out["tracks"][Json::Value::UInt(0)]["steps"];
    double edep = steps[Json::Value::UInt(0)]["energy_deposited"].asDouble();
    EXPECT_LT(edep, 1e-9);
}

TEST_F(DetectorConstructionTest, RootVolumeTestMaterial) {
    MiceModule mod(mod_path+"RootVolumeTestMaterial.dat");
    dc->SetMiceModules(mod);
    // roughly minimum ionising
    // eloss = 11 MeV in 350 mm
    p.energy = 226.;
    Json::Value out = Globals::GetInstance()->GetGeant4Manager()->RunParticle(p);
    Json::Value steps = out["tracks"][Json::Value::UInt(0)]["steps"];
    double edep = steps[Json::Value::UInt(0)]["energy_deposited"].asDouble();
    double path = steps[Json::Value::UInt(0)]["path_length"].asDouble();
    // actual value should be around 0.03; I don't want to check the physics
    // just that it is reasonable (i.e. a material is applied)
    // Note we also check Galactic has no energy loss in RootVolumeTest
    EXPECT_GT(edep/path, 0.01);
    EXPECT_LT(edep/path, 0.1);
}

TEST_F(DetectorConstructionTest, NormalVolumePlacementTest) {
    // Check rotation and translations
    MiceModule mod(mod_path+"VolumeTestPlacement.dat");
    dc->SetMiceModules(mod);
    Json::Value out = Globals::GetInstance()->GetGeant4Manager()->RunParticle(p);
    Json::Value steps = out["tracks"][Json::Value::UInt(0)]["steps"];
    EXPECT_EQ(steps.size(), size_t(4));
    EXPECT_DOUBLE_EQ(steps[Json::Value::UInt(0)]["position"]["z"].asDouble(),
                    0.);
    EXPECT_DOUBLE_EQ(steps[Json::Value::UInt(1)]["position"]["z"].asDouble(),
                     1.-1./20.);
    EXPECT_DOUBLE_EQ(steps[Json::Value::UInt(2)]["position"]["z"].asDouble(),
                     1.+1./20.);
    EXPECT_DOUBLE_EQ(steps[Json::Value::UInt(3)]["position"]["z"].asDouble(),
                     1.+sqrt(2.));
}

TEST_F(DetectorConstructionTest, NormalVolumeRegionTest) {
    // Check rotation and translations
    MiceModule mod(mod_path+"VolumeTestRegions.dat");
    dc->SetMiceModules(mod);
    G4Region* region1 = G4RegionStore::GetInstance()->GetRegion("Region1");
    EXPECT_EQ(region1->GetNumberOfRootVolumes(), size_t(2));
    G4Region* region2 = G4RegionStore::GetInstance()->GetRegion("Region2");
    EXPECT_EQ(region2->GetNumberOfRootVolumes(), size_t(1));
}


TEST_F(DetectorConstructionTest, NormalVolumeMaterialTest) {
    // Check material
    MiceModule mod(mod_path+"VolumeTestMaterial.dat");
    dc->SetMiceModules(mod);
    p.energy = 226.;
    Json::Value out = Globals::GetInstance()->GetGeant4Manager()->RunParticle(p);
    Json::Value steps = out["tracks"][Json::Value::UInt(0)]["steps"];
    double edep = steps[Json::Value::UInt(2)]["energy_deposited"].asDouble();
    double path = steps[Json::Value::UInt(2)]["path_length"].asDouble()-
                  steps[Json::Value::UInt(1)]["path_length"].asDouble();
    EXPECT_GT(edep/path, 0.01);
    EXPECT_LT(edep/path, 0.1);
}


TEST_F(DetectorConstructionTest, NormalVolumeUserLimitsTest) {
    // Check material
    MiceModule mod(mod_path+"VolumeTestUserLimits.dat");
    dc->SetMiceModules(mod);

    // start and end of track same - due to kinetic energy threshold
    // beware kinetic_energy_threshold datacard = 0.1
    p.energy = sqrt(5.*5.+105.658*105.658);  // KE = 0.12
    Json::Value out;
    out = Globals::GetInstance()->GetGeant4Manager()->RunParticle(p);
    Json::Value start = out["tracks"][Json::Value::UInt(0)]["initial_position"];
    Json::Value final = out["tracks"][Json::Value::UInt(0)]["final_position"];
    EXPECT_DOUBLE_EQ(start["z"].asDouble(), final["z"].asDouble());

    // two steps (three step points) ending at track max time (40 ps)
    p.energy = sqrt(15.*15.+105.658*105.658);  // KE = 1.05 on time limiter
    out = Globals::GetInstance()->GetGeant4Manager()->RunParticle(p);
    double time = out["tracks"][Json::Value::UInt(0)]["steps"]
                     [Json::Value::UInt(2)]["time"].asDouble();
    EXPECT_DOUBLE_EQ(time, 0.04);

    // two steps (three step points) each 1 mm, ending at track max (2 mm)
    p.energy = sqrt(1e6*1e6+105.658*105.658);  // on step length limiter
    out = Globals::GetInstance()->GetGeant4Manager()->RunParticle(p);
    double dz1 = out["tracks"][Json::Value::UInt(0)]["steps"]
                    [Json::Value::UInt(1)]["position"]["z"].asDouble();
    EXPECT_DOUBLE_EQ(dz1, 1.);
    double dz2 = out["tracks"][Json::Value::UInt(0)]["steps"]
                    [Json::Value::UInt(2)]["position"]["z"].asDouble();
    EXPECT_DOUBLE_EQ(dz2, 2.);
    double dzTrack = out["tracks"][Json::Value::UInt(0)]["final_position"]
                        ["z"].asDouble();
    EXPECT_DOUBLE_EQ(dzTrack, 2.);
}

TEST_F(DetectorConstructionTest, NormalVolumeVisTest) {
    // Check that vis doesnt blow up (what else to do?)
    MiceModule mod(mod_path+"VolumeTestVisAtt.dat");
    dc->SetMiceModules(mod);
}

TEST_F(DetectorConstructionTest, NormalVolumeNoneRecursionTest) {
    // Check that recursion through MiceModules does throw an exception
    MiceModule mod(mod_path+"VolumeTestNone.dat");
    bool cout_alive = Squeak::coutIsActive();
    EXPECT_THROW(dc->SetMiceModules(mod), Exceptions::Exception);
    EXPECT_EQ(Squeak::coutIsActive(), cout_alive);
}

TEST_F(DetectorConstructionTest, BuildSensitiveDetectorTest) {
    MiceModule mod(mod_path+"SDTest.dat");
    dc->SetMiceModules(mod);
    EXPECT_EQ(dc->GetSDSize(), 3);
    for (int i = 0; i < dc->GetSDSize(); ++i)
        EXPECT_EQ(dc->GetSDHits(i).size(), size_t(0));
    MAUSPrimaryGeneratorAction::PGParticle p;
    p.z = -1000.;
    p.pz = 1.;
    p.energy = 1.e6;
    p.pid = -13;
    Json::Value out = Globals::GetInstance()->GetGeant4Manager()->RunParticle(p);
    for (int i = 0; i < dc->GetSDSize(); ++i) {
        EXPECT_GT(dc->GetSDHits(i).size(), size_t(0));
    }
    dc->ClearSDHits();
    for (int i = 0; i < dc->GetSDSize(); ++i) {
        EXPECT_EQ(dc->GetSDHits(i).size(), size_t(0));
    }
    EXPECT_THROW(dc->GetSDHits(dc->GetSDSize()), Exceptions::Exception);

    MiceModule mod_error(mod_path+"SDErrorTest.dat");
    EXPECT_THROW(dc->SetMiceModules(mod_error), Exceptions::Exception);
}

TEST_F(DetectorConstructionTest, SetDatacardVariablesTest) {
    // nothing terribly productive to test
}

TEST_F(DetectorConstructionTest, BuildG4DetectorVolumeTest) {
    MiceModule mod(mod_path+"G4DetectorTest.dat");
    dc->SetMiceModules(mod);
    MiceModule modError(mod_path+"G4DetectorTestError.dat");
    EXPECT_THROW(dc->SetMiceModules(modError), Exceptions::Exception);
}
#endif
}
}


