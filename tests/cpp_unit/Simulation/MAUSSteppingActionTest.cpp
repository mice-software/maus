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
#include "G4ParticleTable.hh"

#include "src/cpp_core/Utils/JsonWrapper.hh"
#include "src/cpp_core/Simulation/MAUSGeant4Manager.hh"
#include "src/cpp_core/Simulation/MAUSSteppingAction.hh"

#include "src/common/Interface/MICERun.hh"

namespace {
class MAUSSteppingActionTest : public ::testing::Test {
 protected:
  MAUSSteppingActionTest() : stepping(MAUSGeant4Manager::GetInstance()->GetStepping()) {
    G4ParticleDefinition* pd =
                         G4ParticleTable::GetParticleTable()->FindParticle(-13);
    G4DynamicParticle* dyn =
                        new G4DynamicParticle(pd, G4ThreeVector(1., 2., 3.));
    track = new G4Track(dyn, 7., G4ThreeVector(4., 5., 6.));

    step = new G4Step(); // memory leak?
    point = new G4StepPoint();
    post = new G4StepPoint();
    post->SetGlobalTime(-10.);
    track->SetStep(step);
    step->SetTrack(track);
    step->SetPreStepPoint(point);
    step->SetPostStepPoint(post);

    track->AddTrackLength(10.);
    step->SetTotalEnergyDeposit(8.);

    point->SetGlobalTime(1.);
    point->SetProperTime(2.);
    point->SetPosition(G4ThreeVector(2.,3.,4.));
    point->SetMomentumDirection(G4ThreeVector(1.,2.,3.));
    point->SetKineticEnergy(9.);
  }
  virtual ~MAUSSteppingActionTest() {}
  virtual void SetUp() {}
  virtual void TearDown() {}

  MAUS::MAUSSteppingAction* stepping;
  G4Track* track;
  G4Step* step;
  G4StepPoint* point;
  G4StepPoint* post;

};

// test get and set work correctly
TEST_F(MAUSSteppingActionTest, GetSetTest) {
  stepping->SetKeepTracks(true);
  EXPECT_EQ(stepping->GetKeepTracks(), true);
  stepping->SetKeepTracks(false);
  EXPECT_EQ(stepping->GetKeepTracks(), false);

  stepping->SetTracks(Json::Value("Hello"));
  EXPECT_EQ(Json::Value("Hello"), stepping->GetTracks());
  stepping->SetTracks(Json::Value());
}

// test that we set stop and kill if stepping goes more than max n steps
TEST_F(MAUSSteppingActionTest, UserSteppingActionMaxNStepsTest) {
  Json::Value& conf = *MICERun::getInstance()->jsonConfiguration;
  int maxNSteps = JsonWrapper::GetProperty
                      (conf, "maximum_number_of_steps", JsonWrapper::intValue).asInt();
  stepping->SetTracks(JsonWrapper::StringToJson("{\"track0\":{}}"));
  stepping->UserSteppingAction(step);
  EXPECT_TRUE(stepping->GetTracks()["track0"].type() == Json::objectValue);
  EXPECT_TRUE(stepping->GetTracks()["track0"]["KillReason"].type() ==
                                                             Json::nullValue);
  for (size_t i = 0; i < maxNSteps+1; ++i)
    track->IncrementCurrentStepNumber();
  stepping->UserSteppingAction(step);
  EXPECT_EQ(stepping->GetTracks()["track0"]["KillReason"].type(),
                                                             Json::stringValue);
  EXPECT_EQ(track->GetTrackStatus(), fStopAndKill);

}

// test that we write steps correctly on each step (or not if flag is false)
TEST_F(MAUSSteppingActionTest, UserSteppingActionWriteStepsTest) {
  stepping->SetTracks(JsonWrapper::StringToJson("{\"track0\":{}}"));
  stepping->SetKeepTracks(false);
  stepping->SetTracks(Json::Value());
  stepping->UserSteppingAction(step);
  EXPECT_EQ(stepping->GetTracks()["track0"]["steps"].type(), Json::nullValue);
  stepping->SetKeepTracks(true);
  stepping->UserSteppingAction(step);
  stepping->UserSteppingAction(step);
  ASSERT_EQ(stepping->GetTracks()["track0"]["steps"].type(), Json::arrayValue);
  // we want 3 steps - 2 post steps plus the first one
  EXPECT_EQ(stepping->GetTracks()["track0"]["steps"].size(), 3);
}

// test that we write to json correctly
TEST_F(MAUSSteppingActionTest, StepToJsonTest) {
  Json::Value out = stepping->StepPointToJson(step, true);
  EXPECT_DOUBLE_EQ(out["position"]["x"].asDouble(), 2.);
  EXPECT_DOUBLE_EQ(out["position"]["y"].asDouble(), 3.);
  EXPECT_DOUBLE_EQ(out["position"]["z"].asDouble(), 4.);
  EXPECT_DOUBLE_EQ(out["momentum"]["x"].asDouble(), 9.);
  EXPECT_DOUBLE_EQ(out["momentum"]["y"].asDouble(), 18.);
  EXPECT_DOUBLE_EQ(out["momentum"]["z"].asDouble(), 27.);
  EXPECT_DOUBLE_EQ(out["energy_deposited"].asDouble(), 8.);
  EXPECT_DOUBLE_EQ(out["energy"].asDouble(), 9.);
  EXPECT_DOUBLE_EQ(out["time"].asDouble(), 1.);
  EXPECT_DOUBLE_EQ(out["proper_time"].asDouble(), 2.);
  EXPECT_DOUBLE_EQ(out["path_length"].asDouble(), 10.);

  out = stepping->StepPointToJson(step, false);
  EXPECT_DOUBLE_EQ(out["time"].asDouble(), -10.);
}

G4Track* SetG4TrackAndStep(G4Step* step) {
  G4ParticleDefinition* pd =
                       G4ParticleTable::GetParticleTable()->FindParticle(-13);
  G4DynamicParticle* dyn =
                      new G4DynamicParticle(pd, G4ThreeVector(1., 2., 3.));
  G4Track* track = new G4Track(dyn, 7., G4ThreeVector(4., 5., 6.));

  track->SetStep(step);
  step->SetTrack(track);
  step->SetPreStepPoint(new G4StepPoint());
  step->SetPostStepPoint(new G4StepPoint());
  track->SetTrackID(10);

  step->GetPreStepPoint()->SetGlobalTime(-1.);
  step->GetPreStepPoint()->SetPosition(CLHEP::Hep3Vector(2., 3., 4.));
  step->GetPreStepPoint()->SetMass(dyn->GetMass());
  step->GetPreStepPoint()->SetKineticEnergy(100.);
  step->GetPreStepPoint()->SetMomentumDirection
                                    (CLHEP::Hep3Vector(0., 0.1, 1.)/sqrt(1.01));

  step->GetPostStepPoint()->SetGlobalTime(1.);
  step->GetPostStepPoint()->SetPosition(CLHEP::Hep3Vector(3., 4., 8.));
  step->GetPostStepPoint()->SetMass(dyn->GetMass());
  step->GetPostStepPoint()->SetKineticEnergy(110.);
  step->GetPostStepPoint()->SetMomentumDirection
                                    (CLHEP::Hep3Vector(0., 0.1, 1.)/sqrt(1.01));
  return track;
}

// test that we do virtual stepping correctly
TEST_F(MAUSSteppingActionTest, UserSteppingActionVirtualTest) {
  MiceModule mod;
  mod.setProperty<double>("PlaneTime", 0.);
  mod.setProperty<std::string>("SensitiveDetector", "Virtual");
  mod.setProperty<std::string>("IndependentVariable", "time");
  mod.setProperty<std::string>("MultiplePasses", "NewStation");
  VirtualPlaneManager::ConstructVirtualPlanes(NULL, &mod);
  VirtualPlaneManager::ConstructVirtualPlanes(NULL, &mod);

  G4Step*  step  = new G4Step();
  G4Track* track = SetG4TrackAndStep(step);

  stepping->UserSteppingAction(step);
  stepping->UserSteppingAction(step);
  Json::FastWriter writer;
  EXPECT_EQ(stepping->GetTracks()["virtual_hits"].size(), 4);
}

} //namespace end

