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


/*
TEST_F( MAUSPhysicsListTest, GetPhysicsListTest) {
    MAUS::MAUSPhysicsList* list1 = MAUS::MAUSPhysicsList::GetMAUSPhysicsList();
    delete list1;
    Json::Value& dc = *MICERun::getInstance()->jsonConfiguration;
    dc["physics_model"] = "not a physics list";
    EXPECT_THROW(delete MAUS::MAUSPhysicsList::GetMAUSPhysicsList(), Squeal);
    dc["physics_model"] = "QGSP_BERT";
}

TEST_F( MAUSPhysicsListTest, ModelsTest) {
    EXPECT_EQ(list->ScatteringModel("MCs"), MAUS::MAUSPhysicsList::mcs);
    EXPECT_EQ(list->ScatteringModel("noNe"), MAUS::MAUSPhysicsList::no_scat);
    EXPECT_THROW(list->ScatteringModel("bob"), Squeal);

    EXPECT_EQ(list->EnergyLossModel("IoNisation"), MAUS::MAUSPhysicsList::dedx);
    EXPECT_EQ(list->EnergyLossModel("eStrag"), MAUS::MAUSPhysicsList::energyStraggling);
    EXPECT_EQ(list->EnergyLossModel("noNe"), MAUS::MAUSPhysicsList::no_eloss);
    EXPECT_THROW(list->EnergyLossModel("bob"), Squeal);

    EXPECT_EQ(list->HadronicModel("AlL"), MAUS::MAUSPhysicsList::all_hadronic);
    EXPECT_EQ(list->HadronicModel("noNe"), MAUS::MAUSPhysicsList::no_hadronic);
    EXPECT_THROW(list->HadronicModel("bob"), Squeal);
}
*/

#include "src/common_cpp/Utils/JsonWrapper.hh"
CLHEP::Hep3Vector mc_track_to_momentum(Json::Value track) {
  double p0 = track["tracks"]["track_1"]["final_momentum"]["x"].asDouble();
  double p1 = track["tracks"]["track_1"]["final_momentum"]["y"].asDouble();
  double p2 = track["tracks"]["track_1"]["final_momentum"]["z"].asDouble();
  CLHEP::Hep3Vector momentum(p0, p1, p2);
  return momentum;
}

void test_processes(CLHEP::Hep3Vector& p_mu_1, CLHEP::Hep3Vector& p_mu_2, CLHEP::Hep3Vector& p_pi) {
  MAUSGeant4Manager* mgm = MAUSGeant4Manager::GetInstance();
  bool keepTracks = mgm->GetTracking()->GetWillKeepTracks();
  mgm->GetTracking()->SetWillKeepTracks(true);
  MAUS::MAUSPrimaryGeneratorAction::PGParticle prim;
  prim.pz = 1.;
  prim.energy = 300.;
  prim.pid = -13;
  prim.seed = 0;
  Json::Value out_mu_1 = mgm->RunParticle(prim);
  prim.seed = 1;
  Json::Value out_mu_2 = mgm->RunParticle(prim);
  prim.seed = 2;
  prim.pid = 211;
  Json::Value out_pi = mgm->RunParticle(prim);
  p_mu_1 =  mc_track_to_momentum(out_mu_1);
  p_mu_2 =  mc_track_to_momentum(out_mu_2);
  p_pi =  mc_track_to_momentum(out_pi);
  mgm->GetTracking()->SetWillKeepTracks(keepTracks);
}

TEST_F( MAUSPhysicsListTest, BeginOfReferenceParticleActionTest) {
  double PI_MASS2 = 139.570*139.570; // charged pion mass 2
  MAUS::MAUSPhysicsList* list = 
                            MAUSGeant4Manager::GetInstance()->GetPhysicsList();
  Json::Value& dc = *MICERun::getInstance()->jsonConfiguration;
  dc["reference_energy_loss_model"] = "none";
  CLHEP::Hep3Vector p_mu_1, p_mu_2, p_pi;
  list->Setup();
  list->BeginOfReferenceParticleAction();
  test_processes(p_mu_1, p_mu_2, p_pi);
  EXPECT_LT(fabs(p_pi.mag2() + PI_MASS2) - 300.*300., 1e-1) << p_pi;
  EXPECT_LT(fabs(p_pi[0]), 1.e-3) << p_pi;

  dc["reference_energy_loss_model"] = "ionisation";
  list->Setup();
  list->BeginOfReferenceParticleAction();
  test_processes(p_mu_1, p_mu_2, p_pi);
  EXPECT_GT(fabs(p_pi.mag2() + PI_MASS2 - 300.*300.), 1.e-1) << p_pi;
  EXPECT_LT(fabs(p_mu_1.mag2() - p_mu_2.mag2()), 1.e-1) << p_mu_1 << p_mu_2;
  EXPECT_LT(fabs(p_pi[0]), 1.e-3) << p_pi;
}

TEST_F( MAUSPhysicsListTest, BeginOfParticleAction_ELossTest) {
  double PI_MASS2 = 139.570*139.570; // charged pion mass 2
  MAUS::MAUSPhysicsList* list = 
                            MAUSGeant4Manager::GetInstance()->GetPhysicsList();
  Json::Value& dc = *MICERun::getInstance()->jsonConfiguration;
  dc["hadronic_model"] = "none";
  dc["scattering_model"] = "none";
  dc["energy_loss_model"] = "none";
  CLHEP::Hep3Vector p_mu_1, p_mu_2, p_pi;
  list->Setup();
  list->BeginOfParticleAction();
  test_processes(p_mu_1, p_mu_2, p_pi);
  EXPECT_LT(fabs(p_pi.mag2() + PI_MASS2) - 300.*300., 1e-1) << p_pi;
  EXPECT_LT(fabs(p_pi[0]), 1.e-3) << p_pi;

  dc["energy_loss_model"] = "ionisation";
  list->Setup();
  list->BeginOfRunAction();
  test_processes(p_mu_1, p_mu_2, p_pi);
  EXPECT_GT(fabs(p_pi.mag2() + PI_MASS2 - 300.*300.), 1.e-1) << p_pi;
  EXPECT_LT(fabs(p_mu_1.mag2() - p_mu_2.mag2()), 1.e-3) << p_mu_1 << p_mu_2;
  EXPECT_LT(fabs(p_pi[0]), 1.e-3) << p_pi;

  dc["energy_loss_model"] = "estrag";
  list->Setup();
  list->BeginOfRunAction();
  test_processes(p_mu_1, p_mu_2, p_pi);
  EXPECT_GT(fabs(p_pi.mag2() + PI_MASS2 - 300.*300.), 1.e-1) << p_pi;
  EXPECT_GT(fabs(p_mu_1.mag2() - p_mu_2.mag2()), 1.e-3) << p_mu_1 << p_mu_2;
  EXPECT_LT(fabs(p_pi[0]), 1.e-3) << p_pi;
}


}

