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

#include <limits>

#include "gtest/gtest.h"

#include "Geant4/G4Event.hh"

#include "CLHEP/Random/Random.h"

#include "src/common_cpp/Simulation/MAUSGeant4Manager.hh"
#include "src/common_cpp/Simulation/MAUSPrimaryGeneratorAction.hh"

using namespace MAUS;

namespace {
class MAUSPrimaryGeneratorActionTest : public ::testing::Test {
 protected:
  MAUSPrimaryGeneratorActionTest()
      : primary(MAUSGeant4Manager::GetInstance()->GetPrimaryGenerator()) {
    part_in.x = 1.;
    part_in.y = 2.;
    part_in.z = 3.;
    part_in.time = 4.;
    part_in.px = 5.;
    part_in.py = 6.;
    part_in.pz = 7.;
    part_in.sx = 8.;
    part_in.sy = 9.;
    part_in.sz = 10.;
    part_in.energy = 200.;
    part_in.seed = 10;
    part_in.pid = -13;
  }
  virtual ~MAUSPrimaryGeneratorActionTest() {}
  virtual void SetUp() {}
  virtual void TearDown() {}
  MAUS::MAUSPrimaryGeneratorAction* primary;
  MAUS::MAUSPrimaryGeneratorAction::PGParticle part_in;
};

TEST_F(MAUSPrimaryGeneratorActionTest, PushPopTest) {
    MAUS::MAUSPrimaryGeneratorAction::PGParticle part_out;

    primary->Push(part_in);
    part_out = primary->Pop();
    EXPECT_DOUBLE_EQ(part_in.x, part_out.x);

    primary->Push(part_in);
    part_in.x = 2.;
    primary->Push(part_in);
    part_out = primary->Pop();
    EXPECT_DOUBLE_EQ(1., part_out.x); // test FILO
    part_out = primary->Pop();
    EXPECT_DOUBLE_EQ(2., part_out.x);
}

TEST_F(MAUSPrimaryGeneratorActionTest, GeneratePrimariesTest) {
    primary->Push(part_in);
    primary->Push(part_in);

    part_in.energy = 105.; // non-physical
    primary->Push(part_in);

    part_in.energy = 226.;
    part_in.pid = 0; // non-physical?
    primary->Push(part_in);

    part_in.pid = -13;
    part_in.x = 1e9;  // outside world volume
    primary->Push(part_in);
    part_in.x = 1;  // outside world volume

    part_in.px = 1e-16;  // p too small (causes G4 hang)
    part_in.py = 1e-16;  // p too small (causes G4 hang)
    part_in.pz = 1e-16;  // p too small (causes G4 hang)
    primary->Push(part_in);
    part_in.px = 5.;
    part_in.py = 6.;
    part_in.pz = 7.;

    G4Event* event = new G4Event();
    for (size_t i=0; i<2; ++i) {
        primary->GeneratePrimaries(event);
    }
    double mu_mass = 105.658;
    double p_in    = ::sqrt(200.*200.-mu_mass*mu_mass);
    double p_scale = 
        ::sqrt(part_in.px*part_in.px+part_in.py*part_in.py+part_in.pz*part_in.pz);
    double p_norm  = p_in/p_scale;
    
    EXPECT_NEAR(part_in.x, event->GetPrimaryVertex()->GetX0(), 1e-3);
    EXPECT_NEAR(part_in.y, event->GetPrimaryVertex()->GetY0(), 1e-3);
    EXPECT_NEAR(part_in.z, event->GetPrimaryVertex()->GetZ0(), 1e-3);
    EXPECT_NEAR(part_in.time, event->GetPrimaryVertex()->GetT0(), 1e-3);

    EXPECT_NEAR(part_in.px*p_norm, event->GetPrimaryVertex()->GetPrimary()->GetPx(), 1e-3);
    EXPECT_NEAR(part_in.py*p_norm, event->GetPrimaryVertex()->GetPrimary()->GetPy(), 1e-3);
    EXPECT_NEAR(part_in.pz*p_norm, event->GetPrimaryVertex()->GetPrimary()->GetPz(), 1e-3);


    EXPECT_NEAR(part_in.sx, event->GetPrimaryVertex()->GetPrimary()->GetPolX(), 1e-3);
    EXPECT_NEAR(part_in.sy, event->GetPrimaryVertex()->GetPrimary()->GetPolY(), 1e-3);
    EXPECT_NEAR(part_in.sz, event->GetPrimaryVertex()->GetPrimary()->GetPolZ(), 1e-3);

    EXPECT_EQ(part_in.seed, CLHEP::HepRandom::getTheSeed());
    EXPECT_EQ(part_in.pid,  event->GetPrimaryVertex()->GetPrimary()->GetPDGcode());

    for (size_t i=0; i<6; ++i) {
        EXPECT_THROW(primary->GeneratePrimaries(event), MAUS::Exception);
    }
 
    delete event;
}

TEST_F(MAUSPrimaryGeneratorActionTest, PGParticleReadWriteTest) {
    MAUSPrimaryGeneratorAction::PGParticle part_out;

    Json::Value val = part_in.WriteJson();
    part_out.ReadJson(val);
    EXPECT_NEAR(part_in.x, part_out.x, 1e-6);
    EXPECT_NEAR(part_in.y, part_out.y, 1e-6);
    EXPECT_NEAR(part_in.z, part_out.z, 1e-6);
    EXPECT_NEAR(part_in.px, part_out.px, 1e-6);
    EXPECT_NEAR(part_in.py, part_out.py, 1e-6);
    EXPECT_NEAR(part_in.pz, part_out.pz, 1e-6);
    EXPECT_NEAR(part_in.sx, part_out.sx, 1e-6);
    EXPECT_NEAR(part_in.sy, part_out.sy, 1e-6);
    EXPECT_NEAR(part_in.sz, part_out.sz, 1e-6);
    EXPECT_NEAR(part_in.time, part_out.time, 1e-6);
    EXPECT_NEAR(part_in.energy, part_out.energy, 1e-6);
    EXPECT_EQ(part_in.pid, part_out.pid);
    EXPECT_EQ(part_in.seed, part_out.seed);

    // Test bad (negative assigned to unsigned int) seed value in JSON
    bool passed = false;
    val["random_seed"] = Json::Value(-1);
    try {
      part_out.ReadJson(val);
    } catch (MAUS::Exception exc) {
      passed = true;
    }
    EXPECT_TRUE(passed);
}

TEST_F(MAUSPrimaryGeneratorActionTest, PGParticleFromVirtualHitTest) {
    VirtualHit hit;
    hit.SetPosition(MAUS::ThreeVector(1.,2.,3.));
    hit.SetTime(4.);
    hit.SetMomentum(MAUS::ThreeVector(5.,6.,7.));
    hit.SetSpin(MAUS::ThreeVector(8.,9.,10.));
    hit.SetParticleId(-13);
    MAUSPrimaryGeneratorAction::PGParticle part_virt(hit);
    double test_energy = sqrt(5*5+6*6+7*7+hit.GetMass()*hit.GetMass());
    EXPECT_NEAR(part_virt.x, 1., 1e-6);
    EXPECT_NEAR(part_virt.y, 2., 1e-6);
    EXPECT_NEAR(part_virt.z, 3., 1e-6);
    EXPECT_NEAR(part_virt.px, 5., 1e-6);
    EXPECT_NEAR(part_virt.py, 6., 1e-6);
    EXPECT_NEAR(part_virt.pz, 7., 1e-6);
    EXPECT_NEAR(part_virt.sx, 8., 1e-6);
    EXPECT_NEAR(part_virt.sy, 9., 1e-6);
    EXPECT_NEAR(part_virt.sz, 10., 1e-6);
    EXPECT_NEAR(part_virt.time, 4., 1e-6);
    EXPECT_NEAR(part_virt.energy, test_energy, 1e-6); // SHOULD FAIL
    EXPECT_EQ(part_virt.pid, -13);
    EXPECT_EQ(part_virt.seed, size_t(0));

}

TEST_F(MAUSPrimaryGeneratorActionTest, PGParticleMassShellConditionTest) {
    part_in.MassShellCondition();
    EXPECT_NEAR(part_in.x, 1., 1e-6);
    EXPECT_NEAR(part_in.y, 2., 1e-6);
    EXPECT_NEAR(part_in.z, 3., 1e-6);
    EXPECT_NEAR(part_in.px/part_in.pz, 5./7., 1e-6);
    EXPECT_NEAR(part_in.py/part_in.pz, 6./7., 1e-6);
    EXPECT_NEAR(part_in.sx, 8., 1e-6);
    EXPECT_NEAR(part_in.sy, 9., 1e-6);
    EXPECT_NEAR(part_in.sz, 10., 1e-6);
    EXPECT_NEAR(part_in.time, 4., 1e-6);
    EXPECT_NEAR(part_in.energy, 200., 1e-6);
    EXPECT_EQ(part_in.pid, -13);
    EXPECT_EQ(part_in.seed, size_t(10));
    EXPECT_NEAR(part_in.px/part_in.pz, 5./7., 1e-6);
    EXPECT_NEAR(part_in.py/part_in.pz, 6./7., 1e-6);
    double mass = sqrt(part_in.energy*part_in.energy-
                        part_in.px*part_in.px-
                        part_in.py*part_in.py-
                        part_in.pz*part_in.pz);
    EXPECT_NEAR(mass, 105.658, 1e-3);
    part_in.px = 0.;
    part_in.py = 0.;
    part_in.pz = 0.;
    EXPECT_THROW(part_in.MassShellCondition(), MAUS::Exception);
    part_in.pz = 1.;
    part_in.energy = 100.;
    EXPECT_THROW(part_in.MassShellCondition(), MAUS::Exception);
}

TEST_F(MAUSPrimaryGeneratorActionTest, PGParticlePrimaryTest) {
    Primary prim = part_in.GetPrimary();
    EXPECT_NEAR(part_in.x, prim.GetPosition().x(), 1e-6);
    EXPECT_NEAR(part_in.y, prim.GetPosition().y(), 1e-6);
    EXPECT_NEAR(part_in.z, prim.GetPosition().z(), 1e-6);
    EXPECT_NEAR(part_in.px, prim.GetMomentum().x(), 1e-6);
    EXPECT_NEAR(part_in.py, prim.GetMomentum().y(), 1e-6);
    EXPECT_NEAR(part_in.pz, prim.GetMomentum().z(), 1e-6);
    EXPECT_NEAR(part_in.sx, prim.GetSpin().x(), 1e-6);
    EXPECT_NEAR(part_in.sy, prim.GetSpin().y(), 1e-6);
    EXPECT_NEAR(part_in.sz, prim.GetSpin().z(), 1e-6);
    EXPECT_NEAR(part_in.time, prim.GetTime(), 1e-6);
    EXPECT_NEAR(part_in.energy, prim.GetEnergy(), 1e-6);
    EXPECT_EQ(part_in.pid, prim.GetParticleId());
    EXPECT_EQ(part_in.seed, size_t(prim.GetRandomSeed()));
}
} //namespace end

