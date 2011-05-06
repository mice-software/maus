/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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
 * along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "gtest/gtest.h"

#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4ParticleTable.hh"

#include "CLHEP/Vector/Rotation.h"
#include "CLHEP/Vector/ThreeVector.h"

#include "src/common/Simulation/VirtualPlanes.hh"
#include "src/common/Config/MiceModule.hh"
#include "src/common/BeamTools/BTConstantField.hh"
#include "src/common/Interface/Squeal.hh"
#include "src/common/Interface/VirtualHit.hh"

// FORCE G4 setup first so that we can make G4 particle table ok
#include "tests/cpp_unit/Simulation/MAUSGeant4ManagerTest.cpp"

namespace {
class VirtualPlaneTest : public ::testing::Test {
 protected:
  VirtualPlaneTest() : pos(1., -2., 6.) {
    rot.set(CLHEP::Hep3Vector(-1., -1., -1.), 235./360.*2.*CLHEP::pi);
    vp_z = VirtualPlane::BuildVirtualPlane(rot, pos, 100., true, 5.,
                                      BTTracker::z, VirtualPlane::ignore, true);
    vp_u = VirtualPlane::BuildVirtualPlane(rot, pos, 100., true, 5.,
                                      BTTracker::u, VirtualPlane::ignore, true);
    vp_t = VirtualPlane::BuildVirtualPlane(rot, pos, 100., true, 5.,
                                      BTTracker::t, VirtualPlane::ignore, true);
    vp_tau = VirtualPlane::BuildVirtualPlane(rot, pos, 100., false, 5.,
                        BTTracker::tau_field, VirtualPlane::new_station, false);
  }
  virtual ~VirtualPlaneTest() {}
  virtual void SetUp() {}
  virtual void TearDown() {}

  MiceModule mod;
  CLHEP::HepRotation rot;
  CLHEP::Hep3Vector pos;
  VirtualPlane vp_z, vp_u, vp_t, vp_tau;
};

TEST_F(VirtualPlaneTest, ConstructorTest) {  // tests most Get functions also
  EXPECT_EQ(vp_tau.GetMultipassAlgorithm(), VirtualPlane::new_station);
  EXPECT_EQ(vp_tau.GetPlaneIndependentVariableType(), BTTracker::tau_field);
  EXPECT_NEAR(vp_tau.GetPlaneIndependentVariable(), 5., 1e-9);
  EXPECT_NEAR(vp_tau.GetRadialExtent(), 100., 1e-9);
  EXPECT_EQ(vp_tau.GetGlobalCoordinates(), false);
  EXPECT_EQ(vp_tau.GetAllowBackwards(), false);
  for (size_t i = 0; i <3; ++i)
    EXPECT_NEAR(vp_tau.GetPosition()[i], pos[i], 1e-9);
  EXPECT_NEAR(vp_tau.GetRotation().phiX(), rot.phiX(), 1e-9);
  EXPECT_NEAR(vp_tau.GetRotation().phiY(), rot.phiY(), 1e-9);
  EXPECT_NEAR(vp_tau.GetRotation().phiZ(), rot.phiZ(), 1e-9);
  EXPECT_THROW(VirtualPlane::BuildVirtualPlane(rot, pos, 1., true, 5.,
               BTTracker::tau_potential, VirtualPlane::ignore, true), Squeal);
}

TEST_F(VirtualPlaneTest, GetIndependentVariableZTest) {
  G4StepPoint point1;
  CLHEP::Hep3Vector xyz(1, 2, 3);
  point1.SetPosition(xyz);
  point1.SetGlobalTime(4.);
  point1.SetProperTime(5.);
  EXPECT_NEAR(vp_z.GetIndependentVariable(&point1), 3., 1e-3);
  EXPECT_NEAR(vp_t.GetIndependentVariable(&point1), 4., 1e-3);
  EXPECT_NEAR(vp_tau.GetIndependentVariable(&point1), 5., 1e-3);

  CLHEP::Hep3Vector particle_vec = xyz-pos;  // vector from plane to point
  // BUG just a bit worried that this is rot.inverse()
  CLHEP::Hep3Vector plane_normal = rot.inverse()*CLHEP::Hep3Vector(0, 0, 1);

  EXPECT_NEAR(vp_u.GetIndependentVariable(&point1), plane_normal.dot(particle_vec), 1e-3);
}

TEST_F(VirtualPlaneTest, SteppingOverTest) {
  G4Step step;
  step.SetPreStepPoint(new G4StepPoint());  // deleted by step
  step.SetPostStepPoint(new G4StepPoint());
  step.GetPreStepPoint()->SetGlobalTime(4.);
  step.GetPostStepPoint()->SetGlobalTime(6.);
  EXPECT_TRUE(vp_t.SteppingOver(&step));
  step.GetPreStepPoint()->SetGlobalTime(6.);
  step.GetPostStepPoint()->SetGlobalTime(4.);
  EXPECT_TRUE(vp_t.SteppingOver(&step));
  vp_t = VirtualPlane::BuildVirtualPlane(rot, pos, 1., true, 5., BTTracker::t,
                                                  VirtualPlane::ignore, false);
  EXPECT_FALSE(vp_t.SteppingOver(&step));
  step.GetPreStepPoint()->SetGlobalTime(3.);
  step.GetPostStepPoint()->SetGlobalTime(4.);
  EXPECT_FALSE(vp_t.SteppingOver(&step));
  step.GetPreStepPoint()->SetGlobalTime(6.);
  step.GetPostStepPoint()->SetGlobalTime(7.);
  EXPECT_FALSE(vp_t.SteppingOver(&step));
  step.GetPreStepPoint()->SetGlobalTime(5.);
  step.GetPostStepPoint()->SetGlobalTime(6.);
  EXPECT_TRUE(vp_t.SteppingOver(&step));
  step.GetPreStepPoint()->SetGlobalTime(4.);  // don't record these ones twice
  step.GetPostStepPoint()->SetGlobalTime(5.);
  EXPECT_FALSE(vp_t.SteppingOver(&step));
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

  step->GetPreStepPoint()->SetGlobalTime(1.);
  step->GetPreStepPoint()->SetPosition(CLHEP::Hep3Vector(2., 3., 4.));
  step->GetPreStepPoint()->SetMass(dyn->GetMass());
  step->GetPreStepPoint()->SetKineticEnergy(100.);
  step->GetPreStepPoint()->SetMomentumDirection
                                    (CLHEP::Hep3Vector(0., 0.1, 1.)/sqrt(1.01));

  step->GetPostStepPoint()->SetGlobalTime(2.);
  step->GetPostStepPoint()->SetPosition(CLHEP::Hep3Vector(3., 4., 8.));
  step->GetPostStepPoint()->SetMass(dyn->GetMass());
  step->GetPostStepPoint()->SetKineticEnergy(110.);
  step->GetPostStepPoint()->SetMomentumDirection
                                    (CLHEP::Hep3Vector(0., 0.1, 1.)/sqrt(1.01));
  return track;
}

TEST_F(VirtualPlaneTest, BuildNewHitTest) {  // sorry this is a long one...
  VirtualPlane vp_z_local = VirtualPlane::BuildVirtualPlane(rot, pos, 100.,
                          false, 5., BTTracker::z, VirtualPlane::ignore, true);

  BTConstantField field(10., 100., CLHEP::Hep3Vector(0, 0, 0.001)); // 1 Tesla
  VirtualPlaneManager::ConstructVirtualPlanes(&field, MICERun::getInstance()->miceModule);
  G4Step*  step  = new G4Step();
  G4Track* track = SetG4TrackAndStep(step);
  VirtualHit hit = vp_z.BuildNewHit(step, 99);
  double     mass = step->GetPreStepPoint()->GetMass();

  EXPECT_NEAR(hit.GetPos().x(), 2.25, 1e-2);
  EXPECT_NEAR(hit.GetPos().y(), 3.25, 1e-2);
  EXPECT_NEAR(hit.GetPos().z(), 5.,  1e-6);
  CLHEP::Hep3Vector p = hit.GetMomentum();
  EXPECT_NEAR(hit.GetEnergy(), 102.5+mass,  1e-1);
  double p_tot = sqrt(hit.GetEnergy()*hit.GetEnergy()-mass*mass);
  // transport through B-field should conserve ptrans and pz
  EXPECT_NEAR(sqrt(p.x()*p.x()+p.y()*p.y()), p_tot*0.1/sqrt(1.01), 1e-2);
  EXPECT_NEAR(p.z(), p_tot*1./sqrt(1.01),  1e-6);
  EXPECT_EQ(hit.GetStationNumber(), 99);
  EXPECT_EQ(hit.GetTrackID(), 10);
  EXPECT_NEAR(hit.GetTime(), 1.25, 1e-3);  // not quite as v_z b4/after is
                                           // different - but near enough
  EXPECT_EQ(hit.GetPID(), -13);
  EXPECT_EQ(hit.GetMass(), mass);
  EXPECT_NEAR(hit.GetBField().x(), 0, 1e-9);
  EXPECT_NEAR(hit.GetBField().y(), 0, 1e-9);
  EXPECT_NEAR(hit.GetBField().z(), 1e-3, 1e-9);
  EXPECT_NEAR(hit.GetEField().x(), 0, 1e-9);
  EXPECT_NEAR(hit.GetEField().y(), 0, 1e-9);
  EXPECT_NEAR(hit.GetEField().z(), 0, 1e-9);

  step->GetPreStepPoint()->SetPosition(CLHEP::Hep3Vector(2e6, 3., 4.));
  EXPECT_THROW(vp_z.BuildNewHit(step, 99), Squeal);  // outside radial cut
  step->GetPreStepPoint()->SetPosition(CLHEP::Hep3Vector(2, 3., 4.));

  VirtualHit hit_l = vp_z_local.BuildNewHit(step, 99);

  CLHEP::Hep3Vector h_pos = rot*(hit.GetPos() - pos);
  CLHEP::Hep3Vector h_mom = rot*hit.GetMomentum();
  CLHEP::Hep3Vector h_b   = rot*hit.GetBField();
  EXPECT_NEAR(hit_l.GetPos().x(), h_pos.x(), 1e-6);
  EXPECT_NEAR(hit_l.GetPos().y(), h_pos.y(), 1e-6);
  EXPECT_NEAR(hit_l.GetPos().z(), h_pos.z(), 1e-6);

  EXPECT_NEAR(hit_l.GetMomentum().x(), h_mom.x(), 1e-6);
  EXPECT_NEAR(hit_l.GetMomentum().y(), h_mom.y(), 1e-6);
  EXPECT_NEAR(hit_l.GetMomentum().z(), h_mom.z(), 1e-6);

  EXPECT_NEAR(hit_l.GetBField().x(), h_b.x(), 1e-6);
  EXPECT_NEAR(hit_l.GetBField().y(), h_b.y(), 1e-6);
  EXPECT_NEAR(hit_l.GetBField().z(), h_b.z(), 1e-6);
}

TEST_F(VirtualPlaneTest, ComparePositionTest) {
  VirtualPlane vp_z_1 = VirtualPlane::BuildVirtualPlane(rot, pos, 100.,
                          false, 5., BTTracker::z, VirtualPlane::ignore, true);
  VirtualPlane vp_z_2 = VirtualPlane::BuildVirtualPlane(rot, pos, 100.,
                          false, 5.1, BTTracker::z, VirtualPlane::ignore, true);

  EXPECT_TRUE(VirtualPlane::ComparePosition(&vp_z_1, &vp_z_2));
}

TEST_F(VirtualPlaneTest, InRadialCutTest) {
  VirtualPlane vp_z_no_cut = VirtualPlane::BuildVirtualPlane(rot, pos, 0.,
                          false, 5., BTTracker::z, VirtualPlane::ignore, true);
  // position in local coords
  CLHEP::Hep3Vector pos_in(99./sqrt(2), 99./sqrt(2), 0.);
  CLHEP::Hep3Vector pos_out(100.1/sqrt(2), 100.1/sqrt(2), 0.);
  pos_in  = rot.inverse()*pos_in+pos;
  pos_out = rot.inverse()*pos_out+pos;
  EXPECT_TRUE(vp_z.InRadialCut(pos_in));
  EXPECT_TRUE(!vp_z.InRadialCut(pos_out));
  EXPECT_TRUE(vp_z_no_cut.InRadialCut(pos_out));
}

class VirtualPlaneManagerTest : public ::testing::Test {
 protected:
  VirtualPlaneManagerTest() : mod() {
    mod.addPropertyString("SensitiveDetector", "Virtual");
    mod.addPropertyHep3Vector("Position", "0 0 1 m");
    mod.addPropertyHep3Vector("Rotation", "0. 45. 0. degree");
  }
  virtual ~VirtualPlaneManagerTest() {}
  virtual void SetUp() {}
  virtual void TearDown() {}

  MiceModule mod;
};

TEST_F(VirtualPlaneManagerTest, GetInstanceTest) {
  VirtualPlaneManager* vpm = VirtualPlaneManager::GetInstance();
  ASSERT_TRUE(vpm != NULL);
  ASSERT_EQ(vpm, VirtualPlaneManager::GetInstance());
  delete vpm;
}

TEST_F(VirtualPlaneManagerTest, GetSetFieldTest) {
  delete VirtualPlaneManager::GetInstance();
  BTFieldGroup* group = NULL;
  EXPECT_EQ(VirtualPlaneManager::GetInstance()->GetField(), group);
  group = new BTFieldGroup();
  VirtualPlaneManager::GetInstance()->SetField(group);
  EXPECT_EQ(VirtualPlaneManager::GetInstance()->GetField(), group);
  delete group;
}

TEST_F(VirtualPlaneManagerTest, ConstructVirtualPlanes) {  // also GetPlanes()
  delete VirtualPlaneManager::GetInstance();
  MiceModule mod1, mod2, mod3;
  mod1.addPropertyString("SensitiveDetector", "Envelope");
  VirtualPlaneManager::ConstructVirtualPlanes(NULL, &mod1);
  EXPECT_EQ(VirtualPlaneManager::GetStationNumberFromModule(&mod1), 1);

  mod2.addPropertyString("SensitiveDetector", "Virtual");
  VirtualPlaneManager::ConstructVirtualPlanes(NULL, &mod2);
  EXPECT_EQ(VirtualPlaneManager::GetStationNumberFromModule(&mod2), 2);

  mod3.addPropertyString("SensitiveDetector", "");
  VirtualPlaneManager::ConstructVirtualPlanes(NULL, &mod3);
  EXPECT_THROW(VirtualPlaneManager::GetStationNumberFromModule(&mod3), Squeal);

  EXPECT_EQ(VirtualPlaneManager::GetInstance()->GetPlanes().size(), 2);
  delete VirtualPlaneManager::GetInstance();

  VirtualPlaneManager::ConstructVirtualPlanes(NULL, &mod2);
  double point[] = {0, 0, 0, 0};
  double f[] = {0, 0, 0, 0, 0, 0};
  VirtualPlaneManager::GetField()->GetFieldValue(point, f);
  EXPECT_NEAR(f[0]*f[0]+f[1]*f[1]+f[2]*f[2], 0., 1e-9);

  BTFieldGroup* fg = new BTFieldGroup();
  VirtualPlaneManager::ConstructVirtualPlanes(fg, &mod2);
  EXPECT_EQ(fg, VirtualPlaneManager::GetField());
  delete fg;
}

void __test_indep(std::string indep_string,
                      BTTracker::var indep_enum,
                      double indep_var,
                      MiceModule mod) {
  mod.setProperty<std::string>("IndependentVariable", indep_string);
  delete VirtualPlaneManager::GetInstance();
  VirtualPlaneManager* vpm = VirtualPlaneManager::GetInstance();
  VirtualPlaneManager::ConstructVirtualPlanes(NULL, &mod);
  EXPECT_EQ(vpm->GetPlanes().back()->GetPlaneIndependentVariableType(),
            indep_enum);
  EXPECT_NEAR
     (vpm->GetPlanes().back()->GetPlaneIndependentVariable(), indep_var, 1e-9);
}


TEST_F(VirtualPlaneManagerTest, ConstructFromModule_IndepVariableTest) {
  VirtualPlaneManager* vpm = VirtualPlaneManager::GetInstance();
  VirtualPlaneManager::ConstructVirtualPlanes(NULL, &mod);

  __test_indep("z", BTTracker::z, 1000., mod);
  __test_indep("u", BTTracker::u, 0., mod);

  // throw unless PlaneTime is set
  EXPECT_THROW(__test_indep("t", BTTracker::t, 0., mod), Squeal);
  EXPECT_THROW(__test_indep("tau", BTTracker::t, 0., mod), Squeal);
  EXPECT_THROW(__test_indep("time", BTTracker::t, 0., mod), Squeal);
  mod.setProperty<double>("PlaneTime", -1.);
  __test_indep("t", BTTracker::t, -1., mod);
  __test_indep("time", BTTracker::t, -1., mod);
  __test_indep("tau", BTTracker::tau_field, -1., mod);
}

void __test_multipass(std::string mp_string,
                      VirtualPlane::multipass_handler mp_enum,
                      MiceModule mod) {
  mod.setProperty<std::string>("MultiplePasses", mp_string);
  delete VirtualPlaneManager::GetInstance();
  VirtualPlaneManager* vpm = VirtualPlaneManager::GetInstance();
  VirtualPlaneManager::ConstructVirtualPlanes(NULL, &mod);
  EXPECT_EQ(vpm->GetPlanes().back()->GetMultipassAlgorithm(), mp_enum);
}

TEST_F(VirtualPlaneManagerTest, ConstructFromModule_MultiplePassesTest) {
  __test_multipass("SameStation",  VirtualPlane::same_station, mod);
  __test_multipass("NewStation",  VirtualPlane::new_station, mod);
  __test_multipass("Ignore",  VirtualPlane::ignore, mod);
  EXPECT_THROW(__test_multipass("X",  VirtualPlane::ignore, mod), Squeal);
}

TEST_F(VirtualPlaneManagerTest, ConstructFromModule_OtherStuffTest) {
  mod.setProperty<double>("RadialExtent", 1);
  mod.setProperty<bool>("GlobalCoordinates", false);
  mod.setProperty<bool>("AllowBackwards", false);
  delete VirtualPlaneManager::GetInstance();
  VirtualPlaneManager::ConstructVirtualPlanes(NULL, &mod);
  VirtualPlane* vp =  VirtualPlaneManager::GetInstance()->GetPlanes().back();
  EXPECT_NEAR(vp->GetRadialExtent(), 1., 1e-9);
  EXPECT_TRUE(!vp->GetGlobalCoordinates());
  EXPECT_TRUE(!vp->GetAllowBackwards());
}

// Also tests GetNumberOfHits
TEST_F(VirtualPlaneManagerTest, VirtualPlanesSteppingActionTest) {
  MiceModule mod_0;
  mod_0.addPropertyString("SensitiveDetector", "Virtual");
  mod_0.addPropertyHep3Vector("Position", "0 0 0.0 mm");
  MiceModule mod_6;
  mod_6.addPropertyString("SensitiveDetector", "Virtual");
  mod_6.addPropertyHep3Vector("Position", "0 0 6.0 mm");
  for (size_t i = 0; i < 3; ++i)
    VirtualPlaneManager::ConstructVirtualPlanes(NULL, &mod_6);
  VirtualPlaneManager::ConstructVirtualPlanes(NULL, &mod_0);  // two copies

  VirtualPlaneManager* vpm = VirtualPlaneManager::GetInstance();
  G4Step*  step  = new G4Step();
  SetG4TrackAndStep(step);

  std::vector<VirtualHit> hits = vpm->VirtualPlanesSteppingAction(step);
  EXPECT_EQ(vpm->GetNumberOfHits(1), 0);
  for (size_t i = 2; i <= 4; ++i)
    EXPECT_EQ(vpm->GetNumberOfHits(i), 1) << "Failed on station " << i;
  EXPECT_EQ(hits.size(), 3);
  for (size_t i = 0; i < hits.size(); ++i)
    EXPECT_EQ(hits[i].GetStationNumber(), i+2);
  EXPECT_EQ(vpm->GetNumberOfHits(5), 0);
  EXPECT_THROW(vpm->GetNumberOfHits(0), Squeal);
  EXPECT_THROW(vpm->GetNumberOfHits(6), Squeal);

  delete step;
}

// Also tests StartOfEvent
TEST_F(VirtualPlaneManagerTest, VirtualPlanesSteppingActionMultipassTest) {
  delete VirtualPlaneManager::GetInstance();
  VirtualPlaneManager* vpm = VirtualPlaneManager::GetInstance();
  MiceModule mod_ignore;
  mod_ignore.addPropertyString("SensitiveDetector", "Virtual");
  mod_ignore.addPropertyHep3Vector("Position", "0 0 5.8 mm");
  mod_ignore.addPropertyString("MultiplePasses", "Ignore");
  VirtualPlaneManager::ConstructVirtualPlanes(NULL, &mod_ignore);

  MiceModule mod_same;
  mod_same.addPropertyString("SensitiveDetector", "Virtual");
  mod_same.addPropertyHep3Vector("Position", "0 0 5.9 mm");
  mod_same.addPropertyString("MultiplePasses", "SameStation");
  VirtualPlaneManager::ConstructVirtualPlanes(NULL, &mod_same);

  MiceModule mod_new;
  mod_new.addPropertyString("SensitiveDetector", "Virtual");
  mod_new.addPropertyHep3Vector("Position", "0 0 6.0 mm");
  mod_new.addPropertyString("MultiplePasses", "NewStation");
  VirtualPlaneManager::ConstructVirtualPlanes(NULL, &mod_new);

  G4Step*  step  = new G4Step();
  SetG4TrackAndStep(step);

  std::vector<VirtualHit> hits1 = vpm->VirtualPlanesSteppingAction(step);
  std::vector<VirtualHit> hits2 = vpm->VirtualPlanesSteppingAction(step);
  std::vector<VirtualHit> hits3 = vpm->VirtualPlanesSteppingAction(step);

  EXPECT_EQ(vpm->GetNumberOfHits(1), 1);
  EXPECT_EQ(vpm->GetNumberOfHits(2), 3);
  EXPECT_EQ(vpm->GetNumberOfHits(3), 3);

  ASSERT_EQ(hits1.size(), 3);
  for (size_t i = 0; i < 3; ++i)
    EXPECT_EQ(hits1[i].GetStationNumber(), i+1);

  ASSERT_EQ(hits2.size(), 2);
  EXPECT_EQ(hits2[0].GetStationNumber(), 2);
  EXPECT_EQ(hits2[1].GetStationNumber(), 6);

  ASSERT_EQ(hits3.size(), 2);
  EXPECT_EQ(hits3[0].GetStationNumber(), 2);
  EXPECT_EQ(hits3[1].GetStationNumber(), 9);

  VirtualPlaneManager::StartOfEvent();

  std::vector<VirtualHit> hits4 = vpm->VirtualPlanesSteppingAction(step);

  ASSERT_EQ(hits4.size(), 3);
  for (size_t i = 0; i < 3; ++i) {
    EXPECT_EQ(hits4[i].GetStationNumber(), i+1);
    EXPECT_EQ(vpm->GetNumberOfHits(i+1), 1);
  }

  delete step;
}

TEST_F(VirtualPlaneManagerTest, GetModuleFromStationNumberTest) {
  delete VirtualPlaneManager::GetInstance();
  MiceModule mod_alt;
  mod_alt.addPropertyString("SensitiveDetector", "Virtual");
  mod_alt.addPropertyHep3Vector("Position", "0 0 2 m");
  VirtualPlaneManager* vpm = VirtualPlaneManager::GetInstance();
  VirtualPlaneManager::ConstructVirtualPlanes(NULL, &mod);
  EXPECT_THROW(vpm->GetModuleFromStationNumber(0), Squeal);
  EXPECT_EQ(vpm->GetModuleFromStationNumber(1), &mod);
  EXPECT_EQ(vpm->GetModuleFromStationNumber(2), &mod);
  VirtualPlaneManager::ConstructVirtualPlanes(NULL, &mod_alt);
  EXPECT_THROW(vpm->GetModuleFromStationNumber(0), Squeal);
  EXPECT_EQ(vpm->GetModuleFromStationNumber(1), &mod);
  EXPECT_EQ(vpm->GetModuleFromStationNumber(2), &mod_alt);
  EXPECT_EQ(vpm->GetModuleFromStationNumber(3), &mod);
  EXPECT_EQ(vpm->GetModuleFromStationNumber(4), &mod_alt);
}

TEST_F(VirtualPlaneManagerTest, GetStationNumberFromModuleTest) {
  delete VirtualPlaneManager::GetInstance();
  VirtualPlaneManager* vpm = VirtualPlaneManager::GetInstance();
  MiceModule mod_alt;
  mod_alt.addPropertyString("SensitiveDetector", "Virtual");
  mod_alt.addPropertyHep3Vector("Position", "0 0 2 m");

  VirtualPlaneManager::ConstructVirtualPlanes(NULL, &mod);
  EXPECT_EQ(vpm->GetStationNumberFromModule(&mod), 1);
  EXPECT_THROW(vpm->GetStationNumberFromModule(&mod_alt), Squeal);
  VirtualPlaneManager::ConstructVirtualPlanes(NULL, &mod_alt);
  EXPECT_EQ(vpm->GetStationNumberFromModule(&mod), 1);
  EXPECT_EQ(vpm->GetStationNumberFromModule(&mod_alt), 2);
}

}  // namespace

