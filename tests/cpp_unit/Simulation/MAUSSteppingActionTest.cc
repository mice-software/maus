/* This file is part of MAUS: http://  micewww.pp.rl.ac.uk:8080/projects/maus
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
 * along with MAUS.  If not, see <http://  www.gnu.org/licenses/>.
 */

#include "gtest/gtest.h"

#include "Geant4/G4Step.hh"
#include "Geant4/G4ParticleTable.hh"
#include "Geant4/G4Material.hh" 
#include "Geant4/G4GRSVolume.hh"
#include "Geant4/G4RotationMatrix.hh"
#include "Geant4/G4ThreeVector.hh"
#include "Geant4/G4PVPlacement.hh"

#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/Simulation/MAUSGeant4Manager.hh"
#include "src/common_cpp/Simulation/MAUSSteppingAction.hh"
#include "src/common_cpp/Simulation/VirtualPlanes.hh"

#include "src/legacy/Interface/MICERun.hh"

using namespace MAUS;

namespace {
class MAUSSteppingActionTest : public ::testing::Test {
 protected:
  MAUSSteppingActionTest() : stepping(MAUSGeant4Manager::GetInstance()->GetStepping()) {
    stepping->SetSteps(new std::vector<Step>());
    G4ParticleDefinition* pd =
                         G4ParticleTable::GetParticleTable()->FindParticle(-13);
    G4DynamicParticle* dyn =
                           new G4DynamicParticle(pd, G4ThreeVector(1., 2., 3.));

    track = new G4Track(dyn, 7., G4ThreeVector(4., 5., 6.));

    step = new G4Step(); // memory leak?
    point = new G4StepPoint();
    point->SetPosition(G4ThreeVector(1., 2., 3.));
    point->SetMass(pd->GetPDGMass());
    point->SetMomentumDirection(G4ThreeVector(0., ::sqrt(0.5), ::sqrt(0.5)));
    point->SetKineticEnergy(100.);
    post = new G4StepPoint();
    post->SetPosition(G4ThreeVector(7., 8., 9.));
    post->SetMass(pd->GetPDGMass());
    post->SetMomentumDirection(G4ThreeVector(::sqrt(1./3.), ::sqrt(1./3.), ::sqrt(1./3.)));
    post->SetKineticEnergy(110.);
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
  stepping->SetWillKeepSteps(true);
  EXPECT_EQ(stepping->GetWillKeepSteps(), true);
  stepping->SetWillKeepSteps(false);
  EXPECT_EQ(stepping->GetWillKeepSteps(), false);
  std::vector<Step>* steps = new std::vector<Step>();
  EXPECT_NO_THROW(stepping->SetSteps(steps));
  EXPECT_EQ(steps, stepping->GetSteps());
}

// test that we set stop and kill if stepping goes more than max n steps
// check that the kill reason is filled with something
TEST_F(MAUSSteppingActionTest, UserSteppingActionMaxNStepsTest) {
  Json::Value& conf = *MICERun::getInstance()->jsonConfiguration;
  int maxNSteps = JsonWrapper::GetProperty
               (conf, "maximum_number_of_steps", JsonWrapper::intValue).asInt();
  MAUSTrackingAction* tracking = MAUSGeant4Manager::GetInstance()->GetTracking();
  tracking->SetWillKeepTracks(true);
  tracking->SetTracks(new std::vector<Track>());
  tracking->PreUserTrackingAction(step->GetTrack());
  stepping->SetSteps(new std::vector<Step>());
  stepping->UserSteppingAction(step);

  ASSERT_EQ(tracking->GetTracks()->size(), 1);
  EXPECT_EQ(tracking->GetTracks()->at(0).GetKillReason().size(), 0);
  for (int i = 0; i < maxNSteps+1; ++i)
    track->IncrementCurrentStepNumber();
  stepping->UserSteppingAction(step);
  EXPECT_GT(tracking->GetTracks()->at(0).GetKillReason().size(), 0);
  EXPECT_EQ(track->GetTrackStatus(), fStopAndKill);

}

// test that we write steps correctly on each step (or not if flag is false)
TEST_F(MAUSSteppingActionTest, UserSteppingActionWriteStepsTest) {
  stepping->SetSteps(new std::vector<Step>());
  stepping->SetWillKeepSteps(false);
  stepping->SetSteps(new std::vector<Step>());
  stepping->UserSteppingAction(step);
  ASSERT_TRUE(stepping->GetSteps() != NULL);
  EXPECT_EQ(stepping->GetSteps()->size(), 0);
  stepping->SetWillKeepSteps(true);
  stepping->UserSteppingAction(step);
  stepping->UserSteppingAction(step);
  EXPECT_EQ(stepping->GetSteps()->size(), 2);
}
// test that we write to json correctly
TEST_F(MAUSSteppingActionTest, StepToMausTest) {
  G4Material material("material_test", 1, 1, 1.);
  post->SetMaterial(&material);

  Step out = stepping->StepToMaus(step, true);
  EXPECT_DOUBLE_EQ(out.GetPosition().x(), point->GetPosition().x());
  EXPECT_DOUBLE_EQ(out.GetPosition().y(), point->GetPosition().y());
  EXPECT_DOUBLE_EQ(out.GetPosition().z(), point->GetPosition().z());
  EXPECT_DOUBLE_EQ(out.GetMomentum().x(), point->GetMomentum().x());
  EXPECT_DOUBLE_EQ(out.GetMomentum().y(), point->GetMomentum().y());
  EXPECT_DOUBLE_EQ(out.GetMomentum().z(), point->GetMomentum().z());
  EXPECT_DOUBLE_EQ(out.GetSpin().x(), point->GetPolarization().x());
  EXPECT_DOUBLE_EQ(out.GetSpin().y(), point->GetPolarization().y());
  EXPECT_DOUBLE_EQ(out.GetSpin().z(), point->GetPolarization().z());
  EXPECT_DOUBLE_EQ(out.GetBField().x(), 0.);
  EXPECT_DOUBLE_EQ(out.GetBField().y(), 0.);
  EXPECT_DOUBLE_EQ(out.GetBField().z(), 0.);
  EXPECT_DOUBLE_EQ(out.GetEField().x(), 0.);
  EXPECT_DOUBLE_EQ(out.GetEField().y(), 0.);
  EXPECT_DOUBLE_EQ(out.GetEField().z(), 0.);
  EXPECT_DOUBLE_EQ(out.GetEnergyDeposited(), 8.);
  EXPECT_DOUBLE_EQ(out.GetEnergy(), point->GetTotalEnergy());
  EXPECT_DOUBLE_EQ(out.GetTime(), point->GetGlobalTime());
  EXPECT_DOUBLE_EQ(out.GetProperTime(), point->GetProperTime());
  EXPECT_DOUBLE_EQ(out.GetPathLength(), track->GetTrackLength());

  EXPECT_EQ(out.GetMaterial(), "");
  EXPECT_EQ(out.GetVolume(), "");

  out = stepping->StepToMaus(step, false);
  EXPECT_DOUBLE_EQ(out.GetTime(), post->GetGlobalTime());
  EXPECT_EQ(out.GetMaterial(), "material_test");
}

// test that we write volume to json correctly; disabled because it throws a
// G4Exception
/*
TEST_F(MAUSSteppingActionTest, StepToJsonVolumeTest) {
  G4RotationMatrix rot;
  G4ThreeVector vec(1., 1., 1.);
 	G4PVPlacement vol(&rot, vec, NULL, "volume_test", NULL, false, 0);
  G4GRSVolume touchable(&vol, rot, vec);

  point->SetTouchableHandle(&touchable);

  Json::Value out = stepping->StepToJson(step, true);
  EXPECT_EQ(out["volume"].asString(), "volume_test");
}
*/

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
                                    (CLHEP::Hep3Vector(0., 0.1, 1.)/::sqrt(1.01));

  step->GetPostStepPoint()->SetGlobalTime(1.);
  step->GetPostStepPoint()->SetPosition(CLHEP::Hep3Vector(3., 4., 8.));
  step->GetPostStepPoint()->SetMass(dyn->GetMass());
  step->GetPostStepPoint()->SetKineticEnergy(110.);
  step->GetPostStepPoint()->SetMomentumDirection
                                    (CLHEP::Hep3Vector(0., 0.1, 1.)/::sqrt(1.01));
  return track;
}

// test that we do virtual stepping correctly
TEST_F(MAUSSteppingActionTest, UserSteppingActionVirtualTest) {
  MiceModule mod;
  mod.setProperty<double>("PlaneTime", 0.);
  mod.setProperty<std::string>("SensitiveDetector", "Virtual");
  mod.setProperty<std::string>("IndependentVariable", "time");
  mod.setProperty<std::string>("MultiplePasses", "NewStation");

  VirtualPlaneManager* vpm = MAUSGeant4Manager::GetInstance()->GetVirtualPlanes();
  vpm->SetVirtualHits(new std::vector<VirtualHit>());
  vpm->ConstructVirtualPlanes(&mod);
  vpm->ConstructVirtualPlanes(&mod);

  G4Step*  step  = new G4Step();
  G4Track* track = SetG4TrackAndStep(step);
  if(track) {}

  stepping->UserSteppingAction(step);
  stepping->UserSteppingAction(step);
  EXPECT_EQ(vpm->GetVirtualHits()->size(), 4);
}

} //namespace end

