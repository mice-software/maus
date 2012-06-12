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

// C headers
#include <assert.h>

// Other headers
#include "Config/MiceModule.hh"
#include "src/common_cpp/Recon/SciFi/SciFiSpacePoint.hh"
#include "src/common_cpp/Recon/SciFi/SciFiCluster.hh"
#include "src/common_cpp/Recon/SciFi/SciFiSpacePointRec.hh"

#include "gtest/gtest.h"

// namespace {
class SciFiSpacePointTest : public ::testing::Test {
 protected:
  SciFiSpacePointTest()  {}
  virtual ~SciFiSpacePointTest() {}
  virtual void SetUp()    {}
  virtual void TearDown() {}
};

TEST_F(SciFiSpacePointTest, test_setters_getters) {
  int tracker = 1;
  int station = 2;
  double npe = 3.2;
  double time = 12.2;
  // Hep3Vector direction(0.0, 0.0, 1.0);
  Hep3Vector position(1.0, 2.0, 3.0);

  SciFiSpacePoint *spacepoint = new SciFiSpacePoint();

  spacepoint->set_tracker(tracker);
  spacepoint->set_station(station);
  spacepoint->set_npe(npe);
  // spacepoint->set_time(time);
  spacepoint->set_position(position);

  EXPECT_EQ(spacepoint->get_tracker(), tracker);
  EXPECT_EQ(spacepoint->get_station(), station);

  EXPECT_EQ(spacepoint->get_npe(), npe);
  // EXPECT_EQ(Cluster->get_time(), time);
  EXPECT_EQ(spacepoint->get_position(), position);
}

TEST_F(SciFiSpacePointTest, test_used_flag) {
  SciFiSpacePoint *spacepoint = new SciFiSpacePoint();
  EXPECT_FALSE(spacepoint->is_used());
  spacepoint->set_used();
  EXPECT_TRUE(spacepoint->is_used());
}

TEST_F(SciFiSpacePointTest, test_triplet_constructor) {
  SciFiCluster *clust1 = new SciFiCluster();
  SciFiCluster *clust2 = new SciFiCluster();
  SciFiCluster *clust3 = new SciFiCluster();
  EXPECT_FALSE(clust1->is_used());
  EXPECT_FALSE(clust2->is_used());
  EXPECT_FALSE(clust3->is_used());

  SciFiSpacePoint *spacepoint = new SciFiSpacePoint(clust1, clust2, clust3);
  EXPECT_TRUE(clust1->is_used());
  EXPECT_TRUE(clust2->is_used());
  EXPECT_TRUE(clust3->is_used());

  std::vector<SciFiCluster*> channels = spacepoint->get_channels();
  EXPECT_EQ(channels.size(), 3);
}

TEST_F(SciFiSpacePointTest, test_duplet_constructor) {
  SciFiCluster *clust1 = new SciFiCluster();
  SciFiCluster *clust2 = new SciFiCluster();
  EXPECT_FALSE(clust1->is_used());
  EXPECT_FALSE(clust2->is_used());

  SciFiSpacePoint *spacepoint = new SciFiSpacePoint(clust1, clust2);
  EXPECT_TRUE(clust1->is_used());
  EXPECT_TRUE(clust2->is_used());

  std::vector<SciFiCluster*> channels = spacepoint->get_channels();
  EXPECT_EQ(channels.size(), 2);
}
// } // namespace
