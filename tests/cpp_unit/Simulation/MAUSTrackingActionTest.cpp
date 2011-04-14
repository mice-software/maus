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

#include "G4Step.hh"

#include "src/common/Interface/JsonWrapper.hh"
#include "src/common/Interface/MICERun.hh"

#include "src/common/Simulation/MAUSGeant4Manager.hh"
#include "src/common/Simulation/MAUSTrackingAction.hh"

namespace {
class MAUSTrackingActionTest : public ::testing::Test {
 protected:
  MAUSTrackingActionTest() : 
                    tracking(MAUSGeant4Manager::GetInstance()->GetTracking()) {
    G4ParticleDefinition* pd = 
                         G4ParticleTable::GetParticleTable()->FindParticle(-13);
    G4DynamicParticle* dyn_start =
                        new G4DynamicParticle(pd, G4ThreeVector(1., 2., 3.));
    G4DynamicParticle* dyn_end =
                        new G4DynamicParticle(pd, G4ThreeVector(12., 13., 14.));
    start = new G4Track(dyn_start, 7., G4ThreeVector(4., 5., 6.));
    end = new G4Track(dyn_end, 8., G4ThreeVector(9., 10., 11.));
  }
  virtual ~MAUSTrackingActionTest() {}
  virtual void SetUp() {}
  virtual void TearDown() {}
  MAUS::MAUSTrackingAction* tracking;
  G4Track *start, *end;

};

TEST_F(MAUSTrackingActionTest, doesNothing) {
  EXPECT_TRUE(false) << "Need to write MAUSTrackingActionTest" << std::endl;

}

} // namespace

