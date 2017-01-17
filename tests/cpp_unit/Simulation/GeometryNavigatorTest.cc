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
#include "src/legacy/Config/MiceModule.hh"
#include "src/common_cpp/Globals/GlobalsManager.hh"
#include "src/common_cpp/Simulation/GeometryNavigator.hh"

namespace MAUS {

class GeometryNavigatorTest : public ::testing::Test {
  protected:
    GeometryNavigatorTest()  {
        std::string mod_path = getenv("MAUS_ROOT_DIR");
        mod_path += "/tests/cpp_unit/Simulation/TestGeometries/";
        MiceModule* materials = new MiceModule(mod_path+"GeometryNavigatorTest.dat");
        GlobalsManager::SetMonteCarloMiceModules(materials);
        auto nist_t = {60., 70., 80., 90., 100., 120., 140., 170., 200.};
        _nist_energy = std::vector<double>(nist_t);
    }

    virtual ~GeometryNavigatorTest() {}
    virtual void SetUp()    {}
    virtual void TearDown() {}

    double _mass = 105.658;
    std::vector<double> _nist_energy;
};

TEST_F(GeometryNavigatorTest, Fails) {
    EXPECT_TRUE(false) << "Many missing tests";
}

TEST_F(GeometryNavigatorTest, TestComputeStepCylinder) {
    GeometryNavigator* navigator = Globals::GetMCGeometryNavigator();
    double step = 0.;
    ThreeVector pos(0., 0., 0.);
    pos = ThreeVector(0., 0., 100.);
    step = navigator->ComputeStep(pos, ThreeVector(100., 0., 0.), 1e9);
    EXPECT_NEAR(step, 100., 1e-3);
    step = navigator->ComputeStep(pos, ThreeVector(100., 0., 0.), 1e9);
    EXPECT_NEAR(step, 100., 1e-3);
    step = navigator->ComputeStep(pos, ThreeVector(0., 100., 0.), 1e9);
    EXPECT_NEAR(step, 100., 1e-3);
    step = navigator->ComputeStep(pos, ThreeVector(0., 0., 100.), 1e9);
    EXPECT_NEAR(step, 5., 1e-3);
}

TEST_F(GeometryNavigatorTest, TestComputeStepBox) {
    GeometryNavigator* navigator = Globals::GetMCGeometryNavigator();
    double step = 0.;
    ThreeVector pos(0., 0., 0.);
    step = navigator->ComputeStep(pos, ThreeVector(0., 0., 100.), 1.);
    EXPECT_NEAR(step, 2.5, 1e-3);
    step = navigator->ComputeStep(pos, ThreeVector(100., 0., 100.), 1.);
    EXPECT_NEAR(step, 2.5, 1e-3);
    step = navigator->ComputeStep(pos, ThreeVector(100., 100., 100.), 1.);
    EXPECT_NEAR(step, 2.5, 1e-3);
}

TEST_F(GeometryNavigatorTest, TestComputeStepNestedCylinder) {
    GeometryNavigator* navigator = Globals::GetMCGeometryNavigator();
    double step = 0.;
    ThreeVector pos(0., 0., 300.);
    step = navigator->ComputeStep(pos, ThreeVector(0., 0., 100.), 1.);
    EXPECT_NEAR(step, 15., 1e-3);
    step = navigator->ComputeStep(pos, ThreeVector(0., 0., -100.), 1.);
    EXPECT_NEAR(step, 15., 1e-3);
}

}  // namespace MAUS

