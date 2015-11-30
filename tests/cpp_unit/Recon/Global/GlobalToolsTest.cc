/* This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
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

#include <cmath>
#include "Geant4/G4NistManager.hh"
#include "gtest/gtest.h"
#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/Recon/Global/GlobalTools.hh"
#include "src/legacy/BeamTools/BTFieldConstructor.hh"
#include "src/common_cpp/Globals/GlobalsManager.hh"
#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/Simulation/MAUSGeant4Manager.hh"
#include "src/common_cpp/Simulation/DetectorConstruction.hh"

namespace MAUS {

class GlobalToolsTest : public ::testing::Test {
 protected:
  GlobalToolsTest()  {}
  virtual ~GlobalToolsTest() {}
  virtual void SetUp()    {}
  virtual void TearDown() {}
};

TEST_F(GlobalToolsTest, GetTrackerPlane) {
  std::vector<double> z_positions =
      GlobalTools::GetTrackerPlaneZPositions("Stage4.dat");
  MAUS::DataStructure::Global::SpacePoint* sp =
      new MAUS::DataStructure::Global::SpacePoint();
  TLorentzVector position(0, 0, 12106.2, 0);
  sp->set_position(position);
  const MAUS::DataStructure::Global::TrackPoint* tp1 =
      new const MAUS::DataStructure::Global::TrackPoint(sp);
  position.SetZ(16773.0);
  sp->set_position(position);
  const MAUS::DataStructure::Global::TrackPoint* tp2 =
      new const MAUS::DataStructure::Global::TrackPoint(sp);
  position.SetZ(19000.0);
  sp->set_position(position);
  const MAUS::DataStructure::Global::TrackPoint* tp3 =
      new const MAUS::DataStructure::Global::TrackPoint(sp);
  std::vector<int> plane1 =
      GlobalTools::GetTrackerPlane(tp1, z_positions);
  std::vector<int> plane2 =
      GlobalTools::GetTrackerPlane(tp2, z_positions);
  std::vector<int> plane3 =
      GlobalTools::GetTrackerPlane(tp3, z_positions);

  delete sp;
  delete tp1;
  delete tp2;
  delete tp3;

  EXPECT_EQ(plane1[0], 0);
  EXPECT_EQ(plane1[1], 2);
  EXPECT_EQ(plane1[2], 1);
  EXPECT_EQ(plane2[0], 1);
  EXPECT_EQ(plane2[1], 4);
  EXPECT_EQ(plane2[2], 2);
  EXPECT_EQ(plane3[0], 99);
  EXPECT_EQ(plane3[1], 0);
  EXPECT_EQ(plane3[2], 0);
}

TEST_F(GlobalToolsTest, GetTrackerPlaneZPositions) {
  std::vector<double> z_positions =
      GlobalTools::GetTrackerPlaneZPositions("Stage4.dat");
  ASSERT_EQ(z_positions.size(), 30);
  double z_reference[] = {11205.7, 11206.3, 11207,
                          11555.6, 11556.3, 11556.9,
                          11855.6, 11856.3, 11856.9,
                          12105.5, 12106.1, 12106.8,
                          12305.5, 12306.1, 12306.8,
                          16021.6, 16022.3, 16022.9,
                          16221.6, 16222.3, 16222.9,
                          16471.6, 16472.3, 16472.9,
                          16771.6, 16772.3, 16772.9,
                          17121.6, 17122.3, 17122.9};
  double epsilon = 0.1;
  for (size_t i = 0; i < z_positions.size(); i++) {
    EXPECT_NEAR(z_positions.at(i), z_reference[i], epsilon);
  }
}

TEST_F(GlobalToolsTest, approx) {
  EXPECT_PRED3(GlobalTools::approx, 1.1, 1.0, 0.15);
  EXPECT_PRED3(GlobalTools::approx, 10.05, 10.06, 0.02);
  // Predicate assertions always expect true
  EXPECT_FALSE(GlobalTools::approx(1.1, 1.0, 0.05));
}

TEST_F(GlobalToolsTest, dEdx) {
  G4NistManager* manager = G4NistManager::Instance();
  G4Material* galactic = manager->FindOrBuildMaterial("G4_Galactic");
  G4Material* hydrogen = manager->FindOrBuildMaterial("G4_H");
  G4Material* polystyrene = manager->FindOrBuildMaterial("G4_POLYSTYRENE");
  G4Material* lead = manager->FindOrBuildMaterial("G4_Pb");
  double mass = 105.65837;
  EXPECT_NEAR(GlobalTools::dEdx(galactic, 130.0, mass), -8.746e-26, 1.0e-28);
  EXPECT_NEAR(GlobalTools::dEdx(galactic, 230.0, mass), -4.354e-26, 1.0e-28);
  EXPECT_NEAR(GlobalTools::dEdx(hydrogen, 130.0, mass), -7.420e-05, 1.0e-7);
  EXPECT_NEAR(GlobalTools::dEdx(hydrogen, 230.0, mass), -3.687e-05, 1.0e-7);
  EXPECT_NEAR(GlobalTools::dEdx(polystyrene, 130.0, mass), -0.4432, 0.001);
  EXPECT_NEAR(GlobalTools::dEdx(polystyrene, 230.0, mass), -0.2232, 0.001);
  EXPECT_NEAR(GlobalTools::dEdx(lead, 130.0, mass), -2.484, 0.01);
  EXPECT_NEAR(GlobalTools::dEdx(lead, 230.0, mass), -1.336, 0.01);
}

TEST_F(GlobalToolsTest, propagate) {
  MAUS::Simulation::DetectorConstruction* dc =
      Globals::GetInstance()->GetGeant4Manager()->GetGeometry();
  std::string mod_path = std::string(getenv("MAUS_ROOT_DIR"))+
      "/tests/cpp_unit/Recon/Global/TestGeometries/";
  MiceModule geometry1(mod_path+"PropagationTest.dat");
  MiceModule geometry2(mod_path+"PropagationTest_NoField.dat");
  MAUS::DataStructure::Global::PID pid = MAUS::DataStructure::Global::kMuPlus;
  double x1[] = {0.0, 0.0, 0.0, 0.0, 184.699, 15.0, 15.0, 150.0};
  double x2[] = {0.0, 0.0, 0.0, 0.0, 184.699, 15.0, 15.0, 150.0};
  double x3[] = {0.0, 0.0, 0.0, 0.0, 184.699, 15.0, 15.0, 150.0};
  double x4[] = {0.0, 0.0, 0.0, 0.0, 184.699, 15.0, 15.0, 150.0};


  dc->SetMiceModules(geometry1);
  BTFieldConstructor* field = MAUS::Globals::GetMCFieldConstructor();
  double epsilon = 0.001;

  // Energy Loss, Magnetic Field
  try {
    GlobalTools::propagate(x1, 2000.0, field, 10.0, pid);
  } catch (Exception exc) {
    std::cerr << exc.what() << std::endl;
  }
  EXPECT_NEAR(x1[1], 10.015, epsilon);
  EXPECT_NEAR(x1[2], -22.047, epsilon);
  EXPECT_NEAR(x1[4], 134.410, epsilon);
  EXPECT_NEAR(x1[5], -11.571, epsilon);
  EXPECT_NEAR(x1[6], 1.203, epsilon);
  EXPECT_NEAR(x1[7], 82.262, epsilon);

  // No Energy Loss, Magnetic Field
  try {
    GlobalTools::propagate(x2, 2000.0, field, 10.0, pid, false);
  } catch (Exception exc) {
    std::cerr << exc.what() << std::endl;
  }
  EXPECT_NEAR(x2[1], -1.807, epsilon);
  EXPECT_NEAR(x2[2], -13.779, epsilon);
  EXPECT_NEAR(x2[4], 184.700, epsilon);
  EXPECT_NEAR(x2[5], -1.896, epsilon);
  EXPECT_NEAR(x2[6], 21.128, epsilon);
  EXPECT_NEAR(x2[7], 150.000, epsilon);

  dc->SetMiceModules(geometry2);
  field = MAUS::Globals::GetMCFieldConstructor();

  // Energy Loss, No Magnetic Field
  try {
    GlobalTools::propagate(x3, 2000.0, field, 10.0, pid);
  } catch (Exception exc) {
    std::cerr << exc.what() << std::endl;
  }
  EXPECT_NEAR(x3[1], 200.000, epsilon);
  EXPECT_NEAR(x3[2], 200.000, epsilon);
  EXPECT_NEAR(x3[4], 134.410, epsilon);
  EXPECT_NEAR(x3[5], 8.226, epsilon);
  EXPECT_NEAR(x3[6], 8.226, epsilon);
  EXPECT_NEAR(x3[7], 82.262, epsilon);

  // No Energy Loss, No Magnetic Field
  try {
    GlobalTools::propagate(x4, 2000.0, field, 10.0, pid, false);
  } catch (Exception exc) {
    std::cerr << exc.what() << std::endl;
  }
  EXPECT_NEAR(x4[1], 200.000, epsilon);
  EXPECT_NEAR(x4[2], 200.000, epsilon);
  EXPECT_NEAR(x4[4], 184.699, epsilon);
  EXPECT_NEAR(x4[5], 15.000, epsilon);
  EXPECT_NEAR(x4[6], 15.000, epsilon);
  EXPECT_NEAR(x4[7], 150.000, epsilon);
}

TEST_F(GlobalToolsTest, changeEnergy) {
  double mass = 100.0;
  double x[] = {0.0, 0.0, 0.0, 0.0, 0.0, 20.0, 10.0, 200.0};
  x[4] = ::sqrt(x[5]*x[5] + x[6]*x[6] + x[7]*x[7] + mass*mass);
  double deltaE = -20;
  GlobalTools::changeEnergy(x, deltaE, mass);
  double epsilon = 0.001;
  EXPECT_NEAR(x[4], 204.722, epsilon);
  EXPECT_NEAR(x[5], 17.753, epsilon);
  EXPECT_NEAR(x[6], 8.877, epsilon);
  EXPECT_NEAR(x[7], 177.531, epsilon);
}

TEST_F(GlobalToolsTest, TrackPointSort) {
  MAUS::DataStructure::Global::SpacePoint* sp =
      new MAUS::DataStructure::Global::SpacePoint();
  TLorentzVector position(0, 0, 10000.0, 0);
  sp->set_position(position);
  const MAUS::DataStructure::Global::TrackPoint* tp1 =
      new const MAUS::DataStructure::Global::TrackPoint(sp);
  position.SetZ(10001.1);
  sp->set_position(position);
  const MAUS::DataStructure::Global::TrackPoint* tp2 =
      new const MAUS::DataStructure::Global::TrackPoint(sp);

  EXPECT_TRUE(GlobalTools::TrackPointSort(tp1, tp2));
  EXPECT_FALSE(GlobalTools::TrackPointSort(tp2, tp1));
  EXPECT_FALSE(GlobalTools::TrackPointSort(tp1, tp1));

  delete sp;
  delete tp1;
  delete tp2;
}

} // ~namespace MAUS
