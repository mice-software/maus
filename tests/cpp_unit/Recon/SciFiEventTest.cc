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

#include "src/common_cpp/Recon/SciFi/SciFiEvent.hh"
#include "src/common_cpp/Recon/SciFi/SciFiDigit.hh"
#include "src/common_cpp/Recon/SciFi/SciFiSpacePoint.hh"

namespace {
class SciFiEventTest : public ::testing::Test {
 protected:
  SciFiEventTest()  {}
  virtual ~SciFiEventTest() {}
  virtual void SetUp()    {}
  virtual void TearDown() {}
};

// SciFiEvent does very little work.
// This simple test is here for further development,
// once more work is added to SciFiEvent.
TEST_F(SciFiEventTest, memory_test) {
  SciFiEvent* event = new SciFiEvent();
  EXPECT_EQ(0, event->hits().size());
  EXPECT_EQ(0, event->digits().size());
  EXPECT_EQ(0, event->clusters().size());
  EXPECT_EQ(0, event->spacepoints().size());
  EXPECT_EQ(0, event->seeds().size());
  // EXPECT_EQ(0, event->scifistraightprtracks.size());

  SciFiSpacePoint* spacepoint = new SciFiSpacePoint();
  event->add_spacepoint(spacepoint);
  EXPECT_EQ(1, event->spacepoints().size());
}

} // namespace
