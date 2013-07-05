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
#include "gtest/gtest_prod.h"

#include "src/common_cpp/Recon/SciFi/TrackerData.hh"

namespace MAUS {

class TrackerDataTest : public ::testing::Test {
 protected:
  TrackerDataTest()  {}
  virtual ~TrackerDataTest() {}
  virtual void SetUp()    {}
  virtual void TearDown() {}
};

TEST_F(TrackerDataTest, TestConstructor) {
  TrackerData td;
  EXPECT_EQ(0, td._spill_num);
  EXPECT_EQ(0, td._trker_num);
  EXPECT_EQ(0, td._num_events);
  EXPECT_EQ(0, td._num_digits);
  EXPECT_EQ(0, td._num_clusters);
  EXPECT_EQ(0, td._num_spoints);
  EXPECT_EQ(0, td._num_seeds);
  EXPECT_EQ(0, td._num_stracks_5pt);
  EXPECT_EQ(0, td._num_stracks_4pt);
  EXPECT_EQ(0, td._num_stracks_3pt);
  EXPECT_EQ(0, td._num_htracks_5pt);
  EXPECT_EQ(0, td._num_htracks_4pt);
  EXPECT_EQ(0, td._num_htracks_3pt);
}

} // ~namespace MAUS
