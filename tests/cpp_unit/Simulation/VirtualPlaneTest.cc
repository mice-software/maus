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

#include "G4StepPoint.hh"

#include "CLHEP/Vector/Rotation.h"
#include "CLHEP/Vector/ThreeVector.h"

#include "src/common/Simulation/VirtualPlanes.hh"
#include "src/common/Interface/Squeal.hh"

namespace {
class VirtualPlaneTest : public ::testing::Test {
 protected:
  VirtualPlaneTest() : pos(1., -2., 6.) {
    rot.set(CLHEP::Hep3Vector(-1.,-1.,-1.), 235./360.*2.*CLHEP::pi);
    vp_z = VirtualPlane::BuildVirtualPlane(rot, pos, 1., true, 5., BTTracker::z,
                                                    VirtualPlane::ignore, true);
    vp_u = VirtualPlane::BuildVirtualPlane(rot, pos, 1., true, 5., BTTracker::u,
                                                    VirtualPlane::ignore, true);
    vp_t = VirtualPlane::BuildVirtualPlane(rot, pos, 1., true, 5., BTTracker::t,
                                                    VirtualPlane::ignore, true);
    vp_tau = VirtualPlane::BuildVirtualPlane(rot, pos, 1., true, 5.,
                              BTTracker::tau_field, VirtualPlane::ignore, true);
  }
  virtual ~VirtualPlaneTest() {}
  virtual void SetUp() {}
  virtual void TearDown() {}

  CLHEP::HepRotation rot;
  CLHEP::Hep3Vector pos;
  VirtualPlane vp_z, vp_u, vp_t, vp_tau;
};

TEST_F(VirtualPlaneTest, ConstructorTest) {
  EXPECT_EQ(vp_z.GetMultipassAlgorithm(), VirtualPlane::ignore);
  EXPECT_EQ(vp_t.GetMultipassAlgorithm(), VirtualPlane::ignore);
  EXPECT_EQ(vp_tau.GetMultipassAlgorithm(), VirtualPlane::ignore);
  EXPECT_EQ(vp_u.GetMultipassAlgorithm(), VirtualPlane::ignore);
  EXPECT_THROW( VirtualPlane::BuildVirtualPlane(rot, pos, 1., true, 5.,
                 BTTracker::tau_potential, VirtualPlane::ignore, true), Squeal);
}

TEST_F(VirtualPlaneTest, GetIndependentVariableZTest) {
  G4StepPoint point1;
  CLHEP::Hep3Vector xyz(1,2,3);
  point1.SetPosition(xyz);
  point1.SetGlobalTime(4.);
  point1.SetProperTime(5.);
  EXPECT_NEAR(vp_z.GetIndependentVariable(&point1), 3., 1e-3);
  EXPECT_NEAR(vp_t.GetIndependentVariable(&point1), 4., 1e-3);
  EXPECT_NEAR(vp_tau.GetIndependentVariable(&point1), 5., 1e-3);

  CLHEP::Hep3Vector particle_vec = xyz-pos; //vector from plane to point
  CLHEP::Hep3Vector plane_normal = rot.inverse()*CLHEP::Hep3Vector(0,0,1); //BUG just a bit worried that this is rot.inverse()
  
  EXPECT_NEAR(vp_u.GetIndependentVariable(&point1), plane_normal.dot(particle_vec), 1e-3);
}

TEST_F(VirtualPlaneTest, SteppingOverTest) {
  G4StepPoint point[] = {G4StepPoint, G4StepPoint, G4StepPoint};
  for (int i = 0; i < 3; ++i) {
    G4Step step
    CLHEP::Hep3Vector xyz(1,2,3);
    point[1.SetPosition(xyz*(i+1.));
    point1.SetGlobalTime(scale*(i+1.));
    point1.SetProperTime(scale*(i+1.));
  }
  EXPECT_TRUE(vp_z.SteppingOver(&point[0], ));

  CLHEP::Hep3Vector particle_vec = xyz-pos; //vector from plane to point
  CLHEP::Hep3Vector plane_normal = rot.inverse()*CLHEP::Hep3Vector(0,0,1); //BUG just a bit worried that this is rot.inverse()
  EXPECT_NEAR(vp_u.GetIndependentVariable(&point1), plane_normal.dot(particle_vec), 1e-3);

}

}


