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

#include "json/json.h"

#include "Geant4/G4Step.hh"
#include "Geant4/G4StepPoint.hh"
#include "Geant4/G4ParticleTable.hh"

#include "CLHEP/Vector/Rotation.h"
#include "CLHEP/Vector/ThreeVector.h"

#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/Globals/GlobalsManager.hh"
#include "src/common_cpp/Simulation/VirtualPlanes.hh"
#include "src/legacy/Config/MiceModule.hh"
#include "src/legacy/BeamTools/BTConstantField.hh"
#include "src/legacy/BeamTools/BTFieldConstructor.hh"
#include "src/legacy/Interface/Squeal.hh"
#include "src/legacy/Interface/VirtualHit.hh"

namespace MAUS {

/////////////////////// VIRTUALPLANE /////////////////////////////////////////

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
  virtual ~VirtualPlaneTest() {
  }

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
                                  (CLHEP::Hep3Vector(0., 0.1, 1.)/::sqrt(1.01));

  step->GetPostStepPoint()->SetGlobalTime(2.);
  step->GetPostStepPoint()->SetPosition(CLHEP::Hep3Vector(3., 4., 8.));
  step->GetPostStepPoint()->SetMass(dyn->GetMass());
  step->GetPostStepPoint()->SetKineticEnergy(110.);
  step->GetPostStepPoint()->SetMomentumDirection
                                  (CLHEP::Hep3Vector(0., 0.1, 1.)/::sqrt(1.01));
  return track;
}

TEST_F(VirtualPlaneTest, BuildNewHitTest) {  // sorry this is a long one...
  VirtualPlane vp_z_local = VirtualPlane::BuildVirtualPlane(rot, pos, 100.,
                          false, 5., BTTracker::z, VirtualPlane::ignore, true);
  std::string mod_name = std::string(getenv("MAUS_ROOT_DIR"))+
                         std::string("/tests/cpp_unit/Simulation/")+
                         std::string("TestGeometries/MagFieldTest.dat");
  MiceModule* mod_orig = Globals::GetMonteCarloMiceModules();
  MiceModule* test_mod = new MiceModule(mod_name);
  GlobalsManager::SetMonteCarloMiceModules(test_mod);

  VirtualPlaneManager vpm;
  vpm.ConstructVirtualPlanes(
                     MAUS::Globals::GetInstance()->GetMonteCarloMiceModules());
  G4Step*  step  = new G4Step();
  SetG4TrackAndStep(step);
  VirtualHit hit = vp_z.BuildNewHit(step, 99);
  double     mass = step->GetPreStepPoint()->GetMass();

  EXPECT_NEAR(hit.GetPos().x(), 2.25, 1e-2);
  EXPECT_NEAR(hit.GetPos().y(), 3.25, 1e-2);
  EXPECT_NEAR(hit.GetPos().z(), 5.,  1e-6);
  CLHEP::Hep3Vector p = hit.GetMomentum();
  EXPECT_NEAR(hit.GetEnergy(), 102.5+mass,  1e-1);
  double p_tot = ::sqrt(hit.GetEnergy()*hit.GetEnergy()-mass*mass);
  // transport through B-field should conserve ptrans and pz
  EXPECT_NEAR(::sqrt(p.x()*p.x()+p.y()*p.y()), p_tot*0.1/::sqrt(1.01), 1e-2);
  EXPECT_NEAR(p.z(), p_tot*1./::sqrt(1.01),  1e-6);
  EXPECT_EQ(hit.GetStationNumber(), 99);
  EXPECT_EQ(hit.GetTrackID(), 10);
  EXPECT_NEAR(hit.GetTime(), 1.25, 1e-3);  // not quite as v_z b4/after is
                                           // different - but near enough
  EXPECT_EQ(hit.GetPID(), -13);
  EXPECT_EQ(hit.GetMass(), mass);
  double point[4] = {hit.GetPos().x(), hit.GetPos().y(), hit.GetPos().z(),
                     hit.GetTime()};
  double field[6] = {0., 0., 0., 0., 0., 0.};
  Globals::GetMCFieldConstructor()->GetFieldValue(point, field);
  EXPECT_NEAR(hit.GetBField().x(), field[0], 1e-9);
  EXPECT_NEAR(hit.GetBField().y(), field[1], 1e-9);
  EXPECT_NEAR(hit.GetBField().z(), field[2], 1e-9);
  EXPECT_NEAR(hit.GetEField().x(), field[3], 1e-9);
  EXPECT_NEAR(hit.GetEField().y(), field[4], 1e-9);
  EXPECT_NEAR(hit.GetEField().z(), field[5], 1e-9);

  step->GetPreStepPoint()->SetPosition(CLHEP::Hep3Vector(2e6, 3.e6, 4.));
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
  GlobalsManager::SetMonteCarloMiceModules(mod_orig);
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
  CLHEP::Hep3Vector pos_in(99./::sqrt(2), 99./::sqrt(2), 0.);
  CLHEP::Hep3Vector pos_out(100.1/::sqrt(2), 100.1/::sqrt(2), 0.);
  pos_in  = rot.inverse()*pos_in+pos;
  pos_out = rot.inverse()*pos_out+pos;
  EXPECT_TRUE(vp_z.InRadialCut(pos_in));
  EXPECT_TRUE(!vp_z.InRadialCut(pos_out));
  EXPECT_TRUE(vp_z_no_cut.InRadialCut(pos_out));
}

/////////////////////// VIRTUALPLANE END //////////////////////////////////
//
//
//
//
//
//
/////////////////////// VIRTUALPLANE MANAGER //////////////////////////////////

class VirtualPlaneManagerTest : public ::testing::Test {
 protected:
  VirtualPlaneManagerTest() : mod(), vpm() {
    mod.addPropertyString("SensitiveDetector", "Virtual");
    mod.addPropertyHep3Vector("Position", "0 0 1 m");
    mod.addPropertyHep3Vector("Rotation", "0. 45. 0. degree");
  }
  virtual ~VirtualPlaneManagerTest() {}
  virtual void SetUp() {}
  virtual void TearDown() {}

  MiceModule mod;
  VirtualPlaneManager vpm;
};

TEST_F(VirtualPlaneManagerTest, GetSetHitsTest) {
  Json::Value not_array(Json::objectValue);
  EXPECT_THROW(vpm.SetVirtualHits(not_array), Squeal);

  Json::Value array(Json::arrayValue);
  array.append(Json::Value("hello"));
  EXPECT_NO_THROW(vpm.SetVirtualHits(array));
  EXPECT_EQ(array[Json::UInt(0)], vpm.GetVirtualHits()[Json::UInt(0)]);
}


TEST_F(VirtualPlaneManagerTest, ConstructVirtualPlanes) {  // also GetPlanes()
  MiceModule mod1, mod2, mod3;
  mod1.addPropertyString("SensitiveDetector", "Envelope");
  vpm.ConstructVirtualPlanes(&mod1);
  EXPECT_EQ(vpm.GetStationNumberFromModule(&mod1), 1);

  mod2.addPropertyString("SensitiveDetector", "Virtual");
  vpm.ConstructVirtualPlanes(&mod2);
  EXPECT_EQ(vpm.GetStationNumberFromModule(&mod2), 2);

  mod3.addPropertyString("SensitiveDetector", "");
  vpm.ConstructVirtualPlanes(&mod3);
  EXPECT_THROW(vpm.GetStationNumberFromModule(&mod3), Squeal);
  EXPECT_EQ(vpm.GetPlanes().size(), (size_t) 2);
}

void __test_indep(std::string indep_string,
                      BTTracker::var indep_enum,
                      double indep_var,
                      MiceModule& mod, VirtualPlaneManager& vpm) {
  mod.setProperty<std::string>("IndependentVariable", indep_string);
  vpm.ConstructVirtualPlanes(&mod);
  EXPECT_EQ(vpm.GetPlanes().back()->GetPlaneIndependentVariableType(),
            indep_enum) << "Failed with indie " << indep_string;
  EXPECT_NEAR
     (vpm.GetPlanes().back()->GetPlaneIndependentVariable(), indep_var, 1e-9)
      << "Failed with indie " << indep_string;
}


TEST_F(VirtualPlaneManagerTest, ConstructFromModule_IndepVariableTest) {
  // throw unless PlaneTime is set
  size_t vpm_size = vpm.GetPlanes().size();
  EXPECT_THROW(__test_indep("t", BTTracker::t, -4., mod, vpm), Squeal);
  EXPECT_THROW(__test_indep("tau", BTTracker::t, -4., mod, vpm), Squeal);
  EXPECT_THROW(__test_indep("time", BTTracker::t, 4., mod, vpm), Squeal);
  EXPECT_EQ(vpm_size, vpm.GetPlanes().size());  // check no planes alloc'd

  // have to construct in order of indep variable magnitude
  mod.setProperty<double>("PlaneTime", -3.);
  __test_indep("t", BTTracker::t, -3., mod, vpm);
  mod.setProperty<double>("PlaneTime", -2.);
  __test_indep("time", BTTracker::t, -2., mod, vpm);
  mod.setProperty<double>("PlaneTime", -1.);
  __test_indep("tau", BTTracker::tau_field, -1., mod, vpm);

  __test_indep("u", BTTracker::u, 0., mod, vpm);

  __test_indep("z", BTTracker::z, 1000., mod, vpm);
}

void __test_multipass(std::string mp_string,
                      VirtualPlane::multipass_handler mp_enum,
                      MiceModule mod, VirtualPlaneManager& vpm) {
  mod.setProperty<std::string>("MultiplePasses", mp_string);
  vpm.ConstructVirtualPlanes(&mod);
  EXPECT_EQ(vpm.GetPlanes().back()->GetMultipassAlgorithm(), mp_enum);
}

TEST_F(VirtualPlaneManagerTest, ConstructFromModule_MultiplePassesTest) {
  __test_multipass("SameStation",  VirtualPlane::same_station, mod, vpm);
  __test_multipass("NewStation",  VirtualPlane::new_station, mod, vpm);
  __test_multipass("Ignore",  VirtualPlane::ignore, mod, vpm);
  EXPECT_THROW(__test_multipass("X",  VirtualPlane::ignore, mod, vpm), Squeal);
}

TEST_F(VirtualPlaneManagerTest, ConstructFromModule_OtherStuffTest) {
  mod.setProperty<double>("RadialExtent", 1);
  mod.setProperty<bool>("GlobalCoordinates", false);
  mod.setProperty<bool>("AllowBackwards", false);
  vpm.ConstructVirtualPlanes(&mod);
  VirtualPlane* vp =  vpm.GetPlanes().back();
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
  MiceModule mod_10;
  mod_10.addPropertyString("SensitiveDetector", "Virtual");
  mod_10.addPropertyHep3Vector("Position", "0 0 10.0 mm");
  vpm.ConstructVirtualPlanes(&mod_0);
  for (size_t i = 0; i < 3; ++i)
    vpm.ConstructVirtualPlanes(&mod_6);
  vpm.ConstructVirtualPlanes(&mod_10);  // two copies

  G4Step*  step  = new G4Step();
  SetG4TrackAndStep(step);  // prestep is at z=4 poststep at z=8

  vpm.VirtualPlanesSteppingAction(step);
  EXPECT_THROW(vpm.GetNumberOfHits(0), Squeal);
  EXPECT_EQ(vpm.GetNumberOfHits(1), 0);
  for (size_t i = 2; i <= 4; ++i)
    EXPECT_EQ(vpm.GetNumberOfHits(i), 1) << "Failed on station " << i;
  EXPECT_EQ(vpm.GetNumberOfHits(5), 0);
  EXPECT_THROW(vpm.GetNumberOfHits(6), Squeal);

  Json::Value json = vpm.GetVirtualHits();
  ASSERT_EQ(json.size(), (Json::UInt) 3);
  for (size_t i = 0; i < json.size(); ++i)
    EXPECT_EQ(json[i]["station_id"].asInt(), (Json::Int) i+2);

  delete step;
}

TEST_F(VirtualPlaneManagerTest, VirtualPlanesSteppingActionBackwardsTest) {
  G4Step*  step  = new G4Step();
  SetG4TrackAndStep(step);  // prestep is at z=4 poststep at z=8

  MiceModule mod_6;
  mod_6.addPropertyString("SensitiveDetector", "Virtual");
  mod_6.addPropertyHep3Vector("Position", "0 0 6.0 mm");
  mod_6.addPropertyBool("AllowBackwards", false);
  MiceModule mod_7;
  mod_7.addPropertyString("SensitiveDetector", "Virtual");
  mod_7.addPropertyHep3Vector("Position", "0 0 7.0 mm");
  mod_7.addPropertyBool("AllowBackwards", true);
  vpm.ConstructVirtualPlanes(&mod_6);
  vpm.ConstructVirtualPlanes(&mod_7);

  vpm.VirtualPlanesSteppingAction(step);
  EXPECT_EQ(vpm.GetNumberOfHits(1), 1);
  EXPECT_EQ(vpm.GetNumberOfHits(2), 1);
  vpm.SetVirtualHits(Json::Value(Json::arrayValue));
  vpm.StartOfEvent();

  step->GetPreStepPoint()->SetPosition(G4ThreeVector(0., 0., 8.));
  step->GetPostStepPoint()->SetPosition(G4ThreeVector(0., 0., 4.));
  step->GetPreStepPoint()->SetMomentumDirection(G4ThreeVector(0., 0., -1.));
  step->GetPostStepPoint()->SetMomentumDirection(G4ThreeVector(0., 0., -1.));
  vpm.VirtualPlanesSteppingAction(step);
  EXPECT_EQ(vpm.GetNumberOfHits(1), 0);
  EXPECT_EQ(vpm.GetNumberOfHits(2), 1);

  delete step;
}

// Also tests StartOfEvent
TEST_F(VirtualPlaneManagerTest, VirtualPlanesSteppingActionMultipassTest) {
  // Remember we are sorting by z value
  MiceModule mod_ignore;
  mod_ignore.addPropertyString("SensitiveDetector", "Virtual");
  mod_ignore.addPropertyHep3Vector("Position", "0 0 5.8 mm");
  mod_ignore.addPropertyString("MultiplePasses", "Ignore");
  vpm.ConstructVirtualPlanes(&mod_ignore);

  MiceModule mod_same;
  mod_same.addPropertyString("SensitiveDetector", "Virtual");
  mod_same.addPropertyHep3Vector("Position", "0 0 5.9 mm");
  mod_same.addPropertyString("MultiplePasses", "SameStation");
  vpm.ConstructVirtualPlanes(&mod_same);

  MiceModule mod_new;
  mod_new.addPropertyString("SensitiveDetector", "Virtual");
  mod_new.addPropertyHep3Vector("Position", "0 0 6.0 mm");
  mod_new.addPropertyString("MultiplePasses", "NewStation");
  vpm.ConstructVirtualPlanes(&mod_new);

  G4Step*  step  = new G4Step();
  SetG4TrackAndStep(step);

  vpm.VirtualPlanesSteppingAction(step);
  Json::Value json1 = vpm.GetVirtualHits();
  vpm.VirtualPlanesSteppingAction(step);
  Json::Value json2 = vpm.GetVirtualHits();
  vpm.VirtualPlanesSteppingAction(step);
  Json::Value json3 = vpm.GetVirtualHits();

  EXPECT_EQ(vpm.GetNumberOfHits(1), 1);
  EXPECT_EQ(vpm.GetNumberOfHits(2), 3);
  EXPECT_EQ(vpm.GetNumberOfHits(3), 3);  // this is the primary station number

  ASSERT_EQ(json1.size(), (Json::UInt) 3);
  for (size_t i = 0; i < 3; ++i)
    EXPECT_EQ(json1[i]["station_id"].asInt(), (Json::Int) i+1);

  ASSERT_EQ(json2.size(), (Json::UInt) 5);
  EXPECT_EQ(json2[3]["station_id"].asInt(), 2);
  EXPECT_EQ(json2[4]["station_id"].asInt(), 6);

  ASSERT_EQ(json3.size(), (Json::UInt) 7);
  EXPECT_EQ(json3[5]["station_id"].asInt(), 2);
  EXPECT_EQ(json3[6]["station_id"].asInt(), 9);

  vpm.StartOfEvent();

  vpm.VirtualPlanesSteppingAction(step);
  Json::Value json4 = vpm.GetVirtualHits();

  ASSERT_EQ(json4.size(), (Json::UInt) 3);
  for (size_t i = 0; i < 3; ++i) {
    EXPECT_EQ(json4[i]["station_id"].asInt(), (Json::Int) i+1);
    EXPECT_EQ(vpm.GetNumberOfHits(i+1), 1);
  }

  delete step;
}

TEST_F(VirtualPlaneManagerTest, GetModuleFromStationNumberTest) {
  MiceModule mod_alt;
  mod_alt.addPropertyString("SensitiveDetector", "Virtual");
  mod_alt.addPropertyHep3Vector("Position", "0 0 2 m");
  vpm.ConstructVirtualPlanes(&mod);
  EXPECT_THROW(vpm.GetModuleFromStationNumber(0), Squeal);
  EXPECT_EQ(vpm.GetModuleFromStationNumber(1), &mod);
  EXPECT_EQ(vpm.GetModuleFromStationNumber(2), &mod);
  vpm.ConstructVirtualPlanes(&mod_alt);
  EXPECT_THROW(vpm.GetModuleFromStationNumber(0), Squeal);
  EXPECT_EQ(vpm.GetModuleFromStationNumber(1), &mod);
  EXPECT_EQ(vpm.GetModuleFromStationNumber(2), &mod_alt);
  EXPECT_EQ(vpm.GetModuleFromStationNumber(3), &mod);
  EXPECT_EQ(vpm.GetModuleFromStationNumber(4), &mod_alt);
}

TEST_F(VirtualPlaneManagerTest, GetStationNumberFromModuleTest) {
  MiceModule mod_alt;
  mod_alt.addPropertyString("SensitiveDetector", "Virtual");
  mod_alt.addPropertyHep3Vector("Position", "0 0 2 m");

  vpm.ConstructVirtualPlanes(&mod);
  EXPECT_EQ(vpm.GetStationNumberFromModule(&mod), 1);
  EXPECT_THROW(vpm.GetStationNumberFromModule(&mod_alt), Squeal);
  vpm.ConstructVirtualPlanes(&mod_alt);
  EXPECT_EQ(vpm.GetStationNumberFromModule(&mod), 1);
  EXPECT_EQ(vpm.GetStationNumberFromModule(&mod_alt), 2);
}

TEST_F(VirtualPlaneManagerTest, RemovePlaneTest) {
  MiceModule mod_a[5];
  for (size_t i = 0; i < 5; ++i) {
    std::stringstream pos;
    pos << "0 0 " << i << " m";
    mod_a[i].addPropertyString("SensitiveDetector", "Virtual");
    mod_a[i].addPropertyHep3Vector("Position", pos.str());
    vpm.ConstructVirtualPlanes(&mod_a[i]);
  }
  std::set<int> set_1;
  set_1.insert(2);
  vpm.RemovePlanes(set_1);
  EXPECT_NO_THROW(vpm.GetStationNumberFromModule(&mod_a[0]));
  EXPECT_THROW(vpm.GetStationNumberFromModule(&mod_a[1]), Squeal);
  EXPECT_NO_THROW(vpm.GetStationNumberFromModule(&mod_a[2]));
  EXPECT_NO_THROW(vpm.GetStationNumberFromModule(&mod_a[2]));
  EXPECT_NO_THROW(vpm.GetStationNumberFromModule(&mod_a[4]));

  std::set<int> set_2;
  set_2.insert(1);
  set_2.insert(4);
  vpm.RemovePlanes(set_2);
  EXPECT_THROW(vpm.GetStationNumberFromModule(&mod_a[0]), Squeal);
  EXPECT_THROW(vpm.GetStationNumberFromModule(&mod_a[1]), Squeal);
  EXPECT_NO_THROW(vpm.GetStationNumberFromModule(&mod_a[2]));
  EXPECT_NO_THROW(vpm.GetStationNumberFromModule(&mod_a[3]));
  EXPECT_THROW(vpm.GetStationNumberFromModule(&mod_a[4]), Squeal);
}

TEST_F(VirtualPlaneManagerTest, RemovePlanesTest) {
  MiceModule mod_a[5];
  for (size_t i = 0; i < 5; ++i) {
    std::stringstream pos;
    pos << "0 0 " << i << " m";
    mod_a[i].addPropertyString("SensitiveDetector", "Virtual");
    mod_a[i].addPropertyHep3Vector("Position", pos.str());
    vpm.ConstructVirtualPlanes(&mod_a[i]);
  }
  std::vector<VirtualPlane*> planes = vpm.GetPlanes();
  vpm.RemovePlane(planes[1]);
  EXPECT_NO_THROW(vpm.GetStationNumberFromModule(&mod_a[0]));
  EXPECT_THROW(vpm.GetStationNumberFromModule(&mod_a[1]), Squeal);
  EXPECT_NO_THROW(vpm.GetStationNumberFromModule(&mod_a[2]));
  EXPECT_NO_THROW(vpm.GetStationNumberFromModule(&mod_a[3]));
  EXPECT_NO_THROW(vpm.GetStationNumberFromModule(&mod_a[4]));

  vpm.RemovePlane(planes[0]);
  vpm.RemovePlane(planes[4]);
  EXPECT_THROW(vpm.GetStationNumberFromModule(&mod_a[0]), Squeal);
  EXPECT_THROW(vpm.GetStationNumberFromModule(&mod_a[1]), Squeal);
  EXPECT_NO_THROW(vpm.GetStationNumberFromModule(&mod_a[2]));
  EXPECT_NO_THROW(vpm.GetStationNumberFromModule(&mod_a[3]));
  EXPECT_THROW(vpm.GetStationNumberFromModule(&mod_a[4]), Squeal);
}

TEST_F(VirtualPlaneManagerTest, ReadWriteHitTest) {
  Json::Value val1, val2;
  VirtualHit hit1, hit2;
  hit1.SetTrackID(1);
  hit1.SetStationNumber(2);
  hit1.SetPos(CLHEP::Hep3Vector(10, 11, 12));
  hit1.SetMomentum(CLHEP::Hep3Vector(13, 14, 15));
  hit1.SetTime(3);
  hit1.SetEnergy(4);
  hit1.SetPID(5);
  hit1.SetMass(6);
  hit1.SetCharge(7);
  hit1.SetBField(CLHEP::Hep3Vector(16, 17, 18));
  hit1.SetEField(CLHEP::Hep3Vector(19, 20, 21));
  hit1.SetProperTime(8);
  hit1.SetPathLength(9);
  val1 = vpm.WriteHit(hit1);
  hit2 = vpm.ReadHit(val1);

  EXPECT_EQ(hit1.GetStationNumber(), hit2.GetStationNumber());
  EXPECT_EQ(hit1.GetTrackID(), hit2.GetTrackID());
  EXPECT_EQ(hit1.GetPID(), hit2.GetPID());
  EXPECT_NEAR(hit1.GetMass(), hit2.GetMass(), 1e-12);
  EXPECT_NEAR(hit1.GetCharge(), hit2.GetCharge(), 1e-12);
  EXPECT_NEAR(hit1.GetTime(), hit2.GetTime(), 1e-12);
  EXPECT_NEAR(hit1.GetProperTime(), hit2.GetProperTime(), 1e-12);
  EXPECT_NEAR(hit1.GetPathLength(), hit2.GetPathLength(), 1e-12);

  EXPECT_NEAR(hit1.GetPos().x(), hit2.GetPos().x(), 1e-12);
  EXPECT_NEAR(hit1.GetPos().y(), hit2.GetPos().y(), 1e-12);
  EXPECT_NEAR(hit1.GetPos().z(), hit2.GetPos().z(), 1e-12);

  EXPECT_NEAR(hit1.GetMomentum().x(), hit2.GetMomentum().x(), 1e-12);
  EXPECT_NEAR(hit1.GetMomentum().y(), hit2.GetMomentum().y(), 1e-12);
  EXPECT_NEAR(hit1.GetMomentum().z(), hit2.GetMomentum().z(), 1e-12);

  EXPECT_NEAR(hit1.GetBField().x(), hit2.GetBField().x(), 1e-12);
  EXPECT_NEAR(hit1.GetBField().y(), hit2.GetBField().y(), 1e-12);
  EXPECT_NEAR(hit1.GetBField().z(), hit2.GetBField().z(), 1e-12);
  EXPECT_NEAR(hit1.GetEField().x(), hit2.GetEField().x(), 1e-12);
  EXPECT_NEAR(hit1.GetEField().y(), hit2.GetEField().y(), 1e-12);
  EXPECT_NEAR(hit1.GetEField().z(), hit2.GetEField().z(), 1e-12);
}

/////////////////////// VIRTUALPLANE MANAGER END //////////////////////////////

}  // namespace

