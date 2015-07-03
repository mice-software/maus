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
// <http://www.gnu.org/licenses/>



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
    std::vector<Track>* tracks = new std::vector<Track>();
    MAUSGeant4Manager::GetInstance()->GetTracking()->SetTracks(tracks);
    tracking->PreUserTrackingAction(start);
    Track track = MAUSGeant4Manager::GetInstance()->GetTracking()
                                                           ->GetTracks()->at(0);
    EXPECT_DOUBLE_EQ(track.GetInitialPosition().x(), 4.);
    EXPECT_DOUBLE_EQ(track.GetInitialPosition().y(), 5.);
    EXPECT_DOUBLE_EQ(track.GetInitialPosition().z(), 6.);
    EXPECT_NEAR(track.GetInitialMomentum().x(), 1., 1e-6);
    EXPECT_NEAR(track.GetInitialMomentum().y(), 2., 1e-6);
    EXPECT_NEAR(track.GetInitialMomentum().z(), 3., 1e-6);
    EXPECT_EQ(track.GetParticleId(), -13);
    EXPECT_EQ(track.GetTrackId(), 3);
    EXPECT_EQ(track.GetParentTrackId(), 12);

    start->SetTrackID(2);
    tracking->PreUserTrackingAction(start);
    tracks = MAUSGeant4Manager::GetInstance()->GetTracking()->GetTracks();
    EXPECT_EQ(tracks->size(), 2);
}

TEST_F(MAUSTrackingActionTest, PostUserTrackingActionTest) {
    std::vector<Track>* tracks = new std::vector<Track>();
    MAUSGeant4Manager::GetInstance()->GetTracking()->SetTracks(tracks);
    tracking->PreUserTrackingAction(end);
    tracking->PostUserTrackingAction(end);
    Track track = MAUSGeant4Manager::GetInstance()->GetTracking()
                                                           ->GetTracks()->at(0);
    EXPECT_NEAR(track.GetFinalPosition().x(),  9., 1e-6);
    EXPECT_NEAR(track.GetFinalPosition().y(), 10., 1e-6);
    EXPECT_NEAR(track.GetFinalPosition().z(), 11., 1e-6);
    EXPECT_NEAR(track.GetFinalMomentum().x(), 12., 1e-6);
    EXPECT_NEAR(track.GetFinalMomentum().y(), 13., 1e-6);
    EXPECT_NEAR(track.GetFinalMomentum().z(), 14., 1e-6);
 
    end->SetTrackID(2);
    tracking->PreUserTrackingAction(end);
    tracking->PostUserTrackingAction(end);
    tracks = MAUSGeant4Manager::GetInstance()->GetTracking()->GetTracks();
    EXPECT_EQ(tracks->size(), 2);

    end->SetTrackID(4);
    EXPECT_THROW(tracking->PostUserTrackingAction(end), MAUS::Exception);
}

} // namespace

