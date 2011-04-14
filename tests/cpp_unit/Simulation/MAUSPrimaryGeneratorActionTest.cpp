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

#include <limits>

#include "gtest/gtest.h"

#include "G4Event.hh"

#include "src/common/Simulation/MAUSGeant4Manager.hh"
#include "src/common/Simulation/MAUSPrimaryGeneratorAction.hh"

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
    part_in.y = 1.;
    part_in.z = 1.;
    part_in.time = 1.;
    part_in.px = 1.;
    part_in.py = 1.;
    part_in.pz = 1.;
    part_in.energy = 200.;
    part_in.seed = 1;
    part_in.pid = -13;

    primary->Push(part_in);
    primary->Push(part_in);

    part_in.energy = 105.; //non-physical
    primary->Push(part_in);

    part_in.energy = 200.;
    part_in.pid = 0; //non-physical?
    primary->Push(part_in);

    part_in.seed = -1; //non-physical
    primary->Push(part_in);

    part_in.seed = std::numeric_limits<unsigned int>::max()+1; 
    primary->Push(part_in);

    G4Event* event = new G4Event();
    for (size_t i=0; i<2; ++i)
        primary->GeneratePrimaries(event);
    for (size_t i=0; i<5; ++i)
        EXPECT_THROW(primary->GeneratePrimaries(event), Squeal);
    delete event;
}

} //namespace end

