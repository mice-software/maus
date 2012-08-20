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
  MAUSPrimaryGeneratorActionTest() : primary(MAUSGeant4Manager::GetInstance()->GetPrimaryGenerator()) {}
  virtual ~MAUSPrimaryGeneratorActionTest() {}
  virtual void SetUp() {}
  virtual void TearDown() {}
  MAUS::MAUSPrimaryGeneratorAction* primary;
};

TEST_F(MAUSPrimaryGeneratorActionTest, PushPopTest) {
    MAUS::MAUSPrimaryGeneratorAction::PGParticle part_in, part_out;
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
    MAUS::MAUSPrimaryGeneratorAction::PGParticle part_in;
    part_in.x = 1.;
    part_in.y = 2.;
    part_in.z = 3.;
    part_in.time = 4.;
    part_in.px = 5.;
    part_in.py = 6.;
    part_in.pz = 7.;
    part_in.energy = 200.;
    part_in.seed = 27;
    part_in.pid = -13;

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

    EXPECT_EQ(part_in.seed, CLHEP::HepRandom::getTheSeed());
    EXPECT_EQ(part_in.pid,  event->GetPrimaryVertex()->GetPrimary()->GetPDGcode());

    for (size_t i=0; i<5; ++i) {
        EXPECT_THROW(primary->GeneratePrimaries(event), Squeal);
    }
 
    delete event;
}

TEST_F(MAUSPrimaryGeneratorActionTest, PGParticleReadWriteTest) {
    MAUSPrimaryGeneratorAction::PGParticle part_in, part_out;
    part_in.x = 1.;
    part_in.y = 2.;
    part_in.z = 3.;
    part_in.time = 4.;
    part_in.px = 5.;
    part_in.py = 6.;
    part_in.pz = 7.;
    part_in.energy = 200.;
    part_in.seed = 27;
    part_in.pid = -13;

    Json::Value val = part_in.WriteJson();
    part_out.ReadJson(val);
    EXPECT_NEAR(part_in.x, part_out.x, 1e-6);
    EXPECT_NEAR(part_in.y, part_out.y, 1e-6);
    EXPECT_NEAR(part_in.z, part_out.z, 1e-6);
    EXPECT_NEAR(part_in.px, part_out.px, 1e-6);
    EXPECT_NEAR(part_in.py, part_out.py, 1e-6);
    EXPECT_NEAR(part_in.pz, part_out.pz, 1e-6);
    EXPECT_NEAR(part_in.time, part_out.time, 1e-6);
    EXPECT_NEAR(part_in.energy, part_out.energy, 1e-6);
    EXPECT_EQ(part_in.pid, part_out.pid);
    EXPECT_EQ(part_in.seed, part_out.seed);

}

TEST_F(MAUSPrimaryGeneratorActionTest, PGParticleFromVirtualHitTest) {
    VirtualHit hit;
    hit.SetPos(CLHEP::Hep3Vector(1.,2.,3.));
    hit.SetTime(4.);
    hit.SetMomentum(CLHEP::Hep3Vector(5.,6.,7.));
    hit.SetEnergy(200.);
    hit.SetPID(-13);
    MAUSPrimaryGeneratorAction::PGParticle part_in(hit);
    EXPECT_NEAR(part_in.x, 1., 1e-6);
    EXPECT_NEAR(part_in.y, 2., 1e-6);
    EXPECT_NEAR(part_in.z, 3., 1e-6);
    EXPECT_NEAR(part_in.px, 5., 1e-6);
    EXPECT_NEAR(part_in.py, 6., 1e-6);
    EXPECT_NEAR(part_in.pz, 7., 1e-6);
    EXPECT_NEAR(part_in.time, 4., 1e-6);
    EXPECT_NEAR(part_in.energy, 200., 1e-6);
    EXPECT_EQ(part_in.pid, -13);
    EXPECT_EQ(part_in.seed, size_t(0));

}

} //namespace end

