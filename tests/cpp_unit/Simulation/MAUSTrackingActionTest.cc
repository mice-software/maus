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

#include "Geant4/G4Step.hh"

#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/Simulation/MAUSGeant4Manager.hh"
#include "src/common_cpp/Simulation/MAUSTrackingAction.hh"

#include "src/legacy/Interface/MICERun.hh"

using namespace MAUS;

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
    start->SetTrackID(3);
    start->SetParentID(12);
    end->SetTrackID(3);
  }
  virtual ~MAUSTrackingActionTest() {}
  virtual void SetUp() {}
  virtual void TearDown() {}
  MAUS::MAUSTrackingAction* tracking;
  G4Track *start, *end;

};

TEST_F(MAUSTrackingActionTest, PreUserTrackingActionTest) {
    MAUSGeant4Manager::GetInstance()->GetTracking()->SetTracks(Json::Value());
    tracking->PreUserTrackingAction(start);
    Json::Value json_track = MAUSGeant4Manager::GetInstance()->GetTracking()
                                            ->GetTracks()[Json::Value::UInt(0)];
    ASSERT_EQ(json_track.type(), Json::objectValue);
    EXPECT_DOUBLE_EQ(json_track["initial_position"]["x"].asDouble(), 4.);
    EXPECT_DOUBLE_EQ(json_track["initial_position"]["y"].asDouble(), 5.);
    EXPECT_DOUBLE_EQ(json_track["initial_position"]["z"].asDouble(), 6.);
    EXPECT_NEAR(json_track["initial_momentum"]["x"].asDouble(), 1., 1e-6);
    EXPECT_NEAR(json_track["initial_momentum"]["y"].asDouble(), 2., 1e-6);
    EXPECT_NEAR(json_track["initial_momentum"]["z"].asDouble(), 3., 1e-6);
    EXPECT_EQ(json_track["particle_id"].asInt(), -13);
    EXPECT_EQ(json_track["track_id"].asInt(), 3);
    EXPECT_EQ(json_track["parent_track_id"].asInt(), 12);

    start->SetTrackID(2);
    tracking->PreUserTrackingAction(start);
    json_track = MAUSGeant4Manager::GetInstance()->GetTracking()->GetTracks();
    ASSERT_EQ(json_track[Json::Value::UInt(0)].type(), Json::objectValue);
    ASSERT_EQ(json_track[1].type(), Json::objectValue);
}

TEST_F(MAUSTrackingActionTest, PostUserTrackingActionTest) {
    tracking->SetTracks(Json::Value());
    tracking->PreUserTrackingAction(end);
    tracking->PostUserTrackingAction(end);
    Json::Value json_track = MAUSGeant4Manager::GetInstance()->GetTracking()
                                            ->GetTracks()[Json::Value::UInt(0)];
    ASSERT_EQ(json_track.type(), Json::objectValue);
    EXPECT_DOUBLE_EQ(json_track["final_position"]["x"].asDouble(), 9.);
    EXPECT_DOUBLE_EQ(json_track["final_position"]["y"].asDouble(), 10.);
    EXPECT_DOUBLE_EQ(json_track["final_position"]["z"].asDouble(), 11.);
    EXPECT_NEAR(json_track["final_momentum"]["x"].asDouble(), 12., 1e-6);
    EXPECT_NEAR(json_track["final_momentum"]["y"].asDouble(), 13., 1e-6);
    EXPECT_NEAR(json_track["final_momentum"]["z"].asDouble(), 14., 1e-6);
 
    end->SetTrackID(2);
    tracking->PreUserTrackingAction(end);
    tracking->PostUserTrackingAction(end);
    json_track = MAUSGeant4Manager::GetInstance()->GetTracking()->GetTracks();
    ASSERT_EQ(json_track[Json::Value::UInt(0)]["final_position"].type(),
                                                             Json::objectValue);
    ASSERT_EQ(json_track[1]["final_position"].type(), Json::objectValue);

    end->SetTrackID(4);
    EXPECT_THROW(tracking->PostUserTrackingAction(end), MAUS::Exception);
}

} // namespace

