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

#include "src/common/Simulation/MAUSGeant4Manager.hh"

namespace {

// I think all I can do here is test that members were initialised to sometihng
TEST(MAUSGeant4ManagerTest, GetSetTest) {
  MAUSGeant4Manager* g4man = MAUSGeant4Manager::GetInstance();
  ASSERT_TRUE(g4man != NULL);

  g4man->SetStoreTracks(true);
  EXPECT_EQ(g4man->GetStoreTracks(), true);

  g4man->SetStoreTracks(false);
  EXPECT_EQ(g4man->GetStoreTracks(), false);

  ASSERT_TRUE(g4man->GetRunManager() != NULL);
  ASSERT_TRUE(g4man->GetStepping() != NULL);
  ASSERT_TRUE(g4man->GetPrimaryGenerator() != NULL);
  ASSERT_TRUE(g4man->GetGeometry() != NULL);
  ASSERT_TRUE(g4man->GetTracking() != NULL);
}

}

