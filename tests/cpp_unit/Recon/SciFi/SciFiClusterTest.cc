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
#include "src/common_cpp/Recon/SciFi/SciFiCluster.hh"
#include "src/common_cpp/Recon/SciFi/SciFiDigit.hh"

#include "gtest/gtest.h"

namespace {
class SciFiClusterTest : public ::testing::Test {
 protected:
  SciFiClusterTest()  {}
  virtual ~SciFiClusterTest() {}
  virtual void SetUp()    {}
  virtual void TearDown() {}
};

TEST_F(SciFiClusterTest, test_setters_getters) {
  int tracker = 1;
  int station = 2;
  int plane = 0;
  int channel = 106;
  double npe = 3.2;
  double time = 12.2;
  Hep3Vector direction(0.0, 0.0, 1.0);
  Hep3Vector position(1.0, 2.0, 3.0);

  SciFiCluster *Cluster = new SciFiCluster();

  Cluster->set_tracker(tracker);
  Cluster->set_station(station);
  Cluster->set_plane(plane);
  Cluster->set_channel(channel);
  Cluster->set_npe(npe);
  Cluster->set_time(time);
  Cluster->set_direction(direction);
  Cluster->set_position(position);

  EXPECT_EQ(Cluster->get_tracker(), tracker);
  EXPECT_EQ(Cluster->get_station(), station);
  EXPECT_EQ(Cluster->get_plane(), plane);
  EXPECT_EQ(Cluster->get_channel(), channel);
  EXPECT_EQ(Cluster->get_npe(), npe);
  EXPECT_EQ(Cluster->get_time(), time);
  EXPECT_EQ(Cluster->get_direction(), direction);
  EXPECT_EQ(Cluster->get_position(), position);
}

TEST_F(SciFiClusterTest, test_used_flag) {
  SciFiCluster *Cluster = new SciFiCluster();
  EXPECT_FALSE(Cluster->is_used());

  Cluster->set_used();
  EXPECT_TRUE(Cluster->is_used());
}

TEST_F(SciFiClusterTest, test_constructor) {
  int tracker = 1;
  int station = 2;
  int plane = 0;
  int channel = 106;
  double npe = 3.2;
  double time = 12.2;

  SciFiDigit *digit = new SciFiDigit(tracker, station, plane, channel, npe, time);
  EXPECT_FALSE(digit->is_used());

  SciFiCluster *cluster = new SciFiCluster(digit);

  EXPECT_EQ(cluster->get_tracker(), tracker);
  EXPECT_EQ(cluster->get_station(), station);
  EXPECT_EQ(cluster->get_plane(), plane);
  EXPECT_EQ(cluster->get_channel(), channel);
  EXPECT_EQ(cluster->get_npe(), npe);
  EXPECT_EQ(cluster->get_time(), time);
  EXPECT_FALSE(cluster->is_used());
  EXPECT_TRUE(digit->is_used());
}

TEST_F(SciFiClusterTest, test_add_digit) {
  int tracker = 1;
  int station = 2;
  int plane = 0;
  int channel1 = 106;
  double npe1 = 3.2;
  double time = 12.2;
  // Construct a digit.
  SciFiDigit *digit = new SciFiDigit(tracker, station, plane, channel1, npe1, time);
  // Start a cluster from "digit"
  SciFiCluster *cluster = new SciFiCluster(digit);
  // Construct a neighbour.
  int npe2 = 2.0;
  int channel2 = 107;
  SciFiDigit *neighbour = new SciFiDigit(tracker, station, plane, channel2, npe2, time);
  EXPECT_FALSE(neighbour->is_used());
  // Add neighbour.
  cluster->add_digit(neighbour);

  EXPECT_EQ(cluster->get_npe(), npe1+npe2);
  EXPECT_EQ(cluster->get_channel(), (channel1+channel2)/2.0);
  EXPECT_TRUE(neighbour->is_used());
}
/*
TEST_F(SciFiClusterTest, test_construct) {
  // Get the tracker modules.
  std::string filename;
  filename = "Stage6.dat";
  std::vector<const MiceModule*> modules;
  MiceModule*      _module;
  _module = new MiceModule(filename);
  modules = _module->findModulesByPropertyString("SensitiveDetector", "SciFi");

  int tracker = 1;
  int station = 2;
  int plane = 0;
  int channel1 = 106;
  double npe1 = 3.2;
  double time = 12.2;
  // Construct a digit.
  SciFiDigit *digit = new SciFiDigit(tracker, station, plane, channel1, npe1, time);
  // Start a cluster from "digit"
  SciFiCluster *cluster = new SciFiCluster(digit);

  cluster->construct(modules);
  // think of tests for direction and position...
}
*/
} // namespace
