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



// #include "src/map/MapCppTrackerRecon/MapCppTrackerRecon.hh"
#include <string>
// #include "src/map/MapCppTrackerRecon/RealDataDigitization.hh"
#include "src/map/MapCppTrackerMCDigitization/MapCppTrackerMCDigitization.hh"
#include "gtest/gtest.h"

namespace {
class MapCppTrackerReconTest : public ::testing::Test {
 protected:
  MapCppTrackerReconTest()  {}
  virtual ~MapCppTrackerReconTest() {}
  virtual void SetUp()    {}
  virtual void TearDown() {}
};

TEST_F(MapCppTrackerReconTest, test_birth) {
  MapCppTrackerMCDigitization mapper;
  std::string argJsonConfigDocument("{}");
  int result = mapper.test();
  // bool sa = mapper.death();

  // EXPECT_TRUE(result==2);
  //  RealDataDigitization real;
  //  EXPECT_TRUE(real.load_bad_channels());
}

} // namespace
