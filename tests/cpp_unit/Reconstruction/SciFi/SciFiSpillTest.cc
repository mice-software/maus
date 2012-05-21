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

#include "gtest/gtest.h"

#include "src/common_cpp/Reconstruction/SciFi/SciFiSpill.hh"
#include "src/common_cpp/Reconstruction/SciFi/SciFiEvent.hh"

namespace {
class SciFiSpillTest : public ::testing::Test {
 protected:
  SciFiSpillTest()  {}
  virtual ~SciFiSpillTest() {}
  virtual void SetUp()    {}
  virtual void TearDown() {}
};

// SciFiSpill does very little work.
// This simple test is here for further development,
// once more work is added to SciFiSpill.
TEST_F(SciFiSpillTest, init_test) {
  SciFiSpill spill;
  EXPECT_EQ(0, spill.events().size());
}

// Test if behaviour is the expected.
TEST_F(SciFiSpillTest, memory_test) {
  if (1) {
    SciFiSpill spill;
    if (1) {
      SciFiEvent* event = new SciFiEvent();
      SciFiDigit* digit = new SciFiDigit();
      event->scifidigits.push_back(digit);
      spill.add_event(event);
      // digit is a non-NULL pointer,
      // scifigits has size 1
      // and event_in_spill has size 1
      EXPECT_TRUE(digit);
      EXPECT_EQ(1, spill.events().size());
    }
  }
}

} // namespace
