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
#include "src/common_cpp/Recon/SciFi/SciFiHit.hh"
#include "src/common_cpp/Recon/SciFi/SciFiDigit.hh"
#include "src/common_cpp/Recon/SciFi/SciFiCluster.hh"
#include "src/common_cpp/Recon/SciFi/SciFiSpacePoint.hh"

namespace {
class SciFiEventTest : public ::testing::Test {
 protected:
  SciFiEventTest()  {}
  virtual ~SciFiEventTest() {}
  virtual void SetUp()    {}
  virtual void TearDown() {}
};


TEST_F(SciFiEventTest, test_default_contructor) {
  SciFiEvent* event = new SciFiEvent();
  EXPECT_EQ(0, event->hits().size());
  EXPECT_EQ(0, event->digits().size());
  EXPECT_EQ(0, event->clusters().size());
  EXPECT_EQ(0, event->spacepoints().size());
  EXPECT_EQ(0, event->seeds().size());
  EXPECT_EQ(0, event->straightprtracks().size());
}

TEST_F(SciFiEventTest, test_hit_getters_setters) {
  SciFiEvent* event = new SciFiEvent();
  SciFiHit* hit = new SciFiHit();
  event->add_hit(hit);
  EXPECT_EQ(1, event->hits().size());
  EXPECT_EQ(hit, event->hits()[0]);
}

TEST_F(SciFiEventTest, test_digit_getters_setters) {
  SciFiEvent* event = new SciFiEvent();
  SciFiDigit* digit = new SciFiDigit();
  event->add_digit(digit);
  EXPECT_EQ(1, event->digits().size());
  EXPECT_EQ(digit, event->digits()[0]);
}

TEST_F(SciFiEventTest, test_cluster_getters_setters) {
  SciFiEvent* event = new SciFiEvent();
  SciFiCluster* cluster = new SciFiCluster();
  event->add_cluster(cluster);
  EXPECT_EQ(1, event->clusters().size());
  EXPECT_EQ(cluster, event->clusters()[0]);
}

TEST_F(SciFiEventTest, test_spoint_getters_setters) {
  SciFiEvent* event = new SciFiEvent();
  SciFiSpacePoint* spacepoint = new SciFiSpacePoint();
  event->add_spacepoint(spacepoint);
  EXPECT_EQ(1, event->spacepoints().size());
  EXPECT_EQ(spacepoint, event->spacepoints()[0]);
}

TEST_F(SciFiEventTest, test_seed_getters_setters) {
  SciFiEvent* event = new SciFiEvent();
  SciFiSpacePoint* seed = new SciFiSpacePoint();
  event->add_seeds(seed);
  EXPECT_EQ(1, event->seeds().size());
  EXPECT_EQ(seed, event->seeds()[0]);
}

TEST_F(SciFiEventTest, test_straightprtrack_getters_setters) {
  SciFiEvent* event = new SciFiEvent();
  SciFiStraightPRTrack trk;
  trk.set_x0(5.0);
  event->add_straightprtrack(trk);
  EXPECT_EQ(1, event->straightprtracks().size());
  EXPECT_EQ(5.0, event->straightprtracks()[0].get_x0());

  SciFiStraightPRTrack trk2;
  trk2.set_x0(10.0);
  std::vector<SciFiStraightPRTrack> trks;
  trks.push_back(trk);
  trks.push_back(trk2);
  event->set_straightprtrack(trks);
  EXPECT_EQ(2, event->straightprtracks().size());
  EXPECT_EQ(5.0, event->straightprtracks()[0].get_x0());
  EXPECT_EQ(10.0, event->straightprtracks()[1].get_x0());
}

} // namespace
