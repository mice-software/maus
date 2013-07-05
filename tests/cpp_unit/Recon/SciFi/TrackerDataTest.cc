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

// C++ vector
#include <vector>

// ROOT headers
#include "TArc.h"
#include "TF1.h"

// Google test headers
#include "gtest/gtest.h"
#include "gtest/gtest_prod.h"

// MAUS headers
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

TEST_F(TrackerDataTest, TestClear) {
  TrackerData td;
  TArc arc(1.0, 1.0, 1.0);
  TF1 f1("f1", "2*x", 0.0, 1.0);
  std::vector<double> x;
  x.push_back(1.0);
  
  // Set the member variables to some non-zero values
  td._spill_num = 1;
  td._trker_num = 1;
  td._num_events = 1;
  td._num_digits = 1;
  td._num_clusters = 1;
  td._num_spoints = 1;
  td._num_seeds = 1;
  td._num_stracks_5pt = 1;
  td._num_stracks_4pt = 1;
  td._num_stracks_3pt = 1;
  td._num_htracks_5pt = 1;
  td._num_htracks_4pt = 1;
  td._num_htracks_3pt = 1;
  td._spoints_x.push_back(1.0);
  td._spoints_y.push_back(1.0);
  td._spoints_z.push_back(1.0);
  td._seeds_z.push_back(x);
  td._seeds_phi.push_back(x);
  td._seeds_s.push_back(x);
  td._trks_str_xz.push_back(f1);
  td._trks_str_yz.push_back(f1);
  td._trks_xy.push_back(arc);
  td._trks_xz.push_back(f1);
  td._trks_yz.push_back(f1);
  td._trks_sz.push_back(f1);
  
  // Clear the container
  td.clear();
  
  // Check everything is set to zero
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
  EXPECT_EQ(0, td._spoints_x.size());
  EXPECT_EQ(0, td._spoints_y.size());
  EXPECT_EQ(0, td._spoints_z.size());
  EXPECT_EQ(0, td._seeds_z.size());
  EXPECT_EQ(0, td._seeds_phi.size());
  EXPECT_EQ(0, td._seeds_s.size());
  EXPECT_EQ(0, td._trks_str_xz.size());
  EXPECT_EQ(0, td._trks_str_yz.size());
  EXPECT_EQ(0, td._trks_xy.size());
  EXPECT_EQ(0, td._trks_xz.size());
  EXPECT_EQ(0, td._trks_yz.size());
}

} // ~namespace MAUS
