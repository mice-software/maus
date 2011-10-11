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
#include "json/json.h"

#include "CLHEP/Vector/ThreeVector.h"

#include "src/legacy/Interface/MICERun.hh"
#include "src/common_cpp/Simulation/MAUSPhysicsList.hh"
#include "src/common_cpp/Simulation/MAUSGeant4Manager.hh"

namespace {

class MAUSPhysicsListTest : public ::testing::Test {
 protected:
  MAUSPhysicsListTest() {}
  virtual ~MAUSPhysicsListTest() {}
};

TEST_F( MAUSPhysicsListTest, ReferenceEnergyLossModelTest) {
  MAUS::MAUSPhysicsList* list = 
                            MAUSGeant4Manager::GetInstance()->GetPhysicsList();
  Json::Value& dc = *MICERun::getInstance()->jsonConfiguration;
  dc["reference_energy_loss_model"] = "noNe";
  list->Setup();
  list->BeginOfReferenceParticleAction();

  dc["reference_energy_loss_model"] = "ioNisation";
  list->Setup();
  list->BeginOfReferenceParticleAction();

  dc["reference_energy_loss_model"] = "eStrag";
  list->Setup();
  EXPECT_THROW(list->BeginOfReferenceParticleAction(), Squeal);

  dc["reference_energy_loss_model"] = "bob";
  list->Setup();
  EXPECT_THROW(list->BeginOfReferenceParticleAction(), Squeal);

  dc["reference_energy_loss_model"] = "ioNisation";
}

TEST_F( MAUSPhysicsListTest, ELossModelTest) {
  MAUS::MAUSPhysicsList* list = 
                            MAUSGeant4Manager::GetInstance()->GetPhysicsList();
  Json::Value& dc = *MICERun::getInstance()->jsonConfiguration;
  dc["energy_loss_model"] = "noNe";
  list->Setup();
  list->BeginOfRunAction();

  dc["energy_loss_model"] = "ioNization";
  list->Setup();
  list->BeginOfRunAction();

  dc["energy_loss_model"] = "eStrag";
  list->Setup();
  list->BeginOfRunAction();

  dc["energy_loss_model"] = "bob";
  list->Setup();
  EXPECT_THROW(list->BeginOfRunAction(), Squeal);

  dc["energy_loss_model"] = "eStrag";

}

TEST_F( MAUSPhysicsListTest, ScatModelTest) {
  MAUS::MAUSPhysicsList* list = 
                            MAUSGeant4Manager::GetInstance()->GetPhysicsList();
  Json::Value& dc = *MICERun::getInstance()->jsonConfiguration;
  dc["multiple_scattering_model"] = "noNe";
  list->Setup();
  list->BeginOfRunAction();

  dc["multiple_scattering_model"] = "mSc";
  list->Setup();
  list->BeginOfRunAction();

  dc["multiple_scattering_modell"] = "mCs";
  list->Setup();
  list->BeginOfRunAction();

  dc["multiple_scattering_model"] = "bob";
  list->Setup();
  EXPECT_THROW(list->BeginOfRunAction(), Squeal);

  dc["multiple_scattering_model"] = "mSc";
}

TEST_F( MAUSPhysicsListTest, HadronModelTest) {
  MAUS::MAUSPhysicsList* list = 
                            MAUSGeant4Manager::GetInstance()->GetPhysicsList();
  Json::Value& dc = *MICERun::getInstance()->jsonConfiguration;
  dc["hadronic_model"] = "noNe";
  list->Setup();
  list->BeginOfRunAction();

  dc["hadronic_model"] = "aLl";
  list->Setup();
  list->BeginOfRunAction();

  dc["hadronic_model"] = "bob";
  list->Setup();
  EXPECT_THROW(list->BeginOfRunAction(), Squeal);

  dc["hadronic_model"] = "aLl";
  list->Setup();
  list->BeginOfRunAction();
}

TEST_F( MAUSPhysicsListTest, PartDecayTest) {
  MAUS::MAUSPhysicsList* list = 
                            MAUSGeant4Manager::GetInstance()->GetPhysicsList();
  Json::Value& dc = *MICERun::getInstance()->jsonConfiguration;

  dc["particle_decay"] = "bob";
  EXPECT_THROW(list->Setup(), Squeal);

  dc["particle_decay"] = Json::Value(false);
  list->Setup();
  list->BeginOfRunAction();

  dc["particle_decay"] = Json::Value(true);
  list->Setup();
  list->BeginOfRunAction();
}

TEST_F( MAUSPhysicsListTest, HalfLifeTest) {
  MAUS::MAUSPhysicsList* list = 
                            MAUSGeant4Manager::GetInstance()->GetPhysicsList();
  Json::Value& dc = *MICERun::getInstance()->jsonConfiguration;

  dc["charged_pion_half_life"] = Json::Value("blah");
  EXPECT_THROW(list->Setup(), Squeal);

  dc["charged_pion_half_life"] = Json::Value(1.5);
  list->Setup();
  list->BeginOfRunAction();

  dc["charged_pion_half_life"] = Json::Value(-1.5);
  list->Setup();
  list->BeginOfRunAction();

  dc["muon_half_life"] = "blah";
  EXPECT_THROW(list->Setup(), Squeal);

  dc["muon_half_life"] = Json::Value(1.5);
  list->Setup();
  list->BeginOfRunAction();

  dc["muon_half_life"] = Json::Value(-1.5);
  list->Setup();
  list->BeginOfRunAction();
}

TEST_F( MAUSPhysicsListTest, ProductionThresholdTest) {
  MAUS::MAUSPhysicsList* list = 
                            MAUSGeant4Manager::GetInstance()->GetPhysicsList();
  Json::Value& dc = *MICERun::getInstance()->jsonConfiguration;

  dc["production_threshold"] = Json::Value("blah");
  EXPECT_THROW(list->Setup(), Squeal);

  dc["production_threshold"] = Json::Value(0.5);
  list->Setup();
  list->BeginOfRunAction();
}

}

