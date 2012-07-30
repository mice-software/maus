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

#include "Config/MiceModule.hh"
#include "src/common_cpp/DataStructure/SciFiCluster.hh"
#include "src/common_cpp/DataStructure/SciFiDigit.hh"
#include "src/common_cpp/DataStructure/ThreeVector.hh"
#include "gtest/gtest.h"

namespace MAUS {

class SciFiClusterTestDS : public ::testing::Test {
 protected:
  SciFiClusterTestDS()  {}
  virtual ~SciFiClusterTestDS() {}
  virtual void SetUp()    {}
  virtual void TearDown() {}
};

TEST_F(SciFiClusterTestDS, test_from_default_constructor) {
  SciFiCluster *cluster = new SciFiCluster();
  EXPECT_FALSE(cluster->is_used());
  EXPECT_EQ(cluster->get_spill(), 0);
  EXPECT_EQ(cluster->get_event(), 0);
  EXPECT_EQ(cluster->get_tracker(), 0);
  EXPECT_EQ(cluster->get_station(), 0);
  EXPECT_EQ(cluster->get_plane(), 0);
  EXPECT_EQ(cluster->get_channel(), 0);
  EXPECT_EQ(cluster->get_npe(), 0.0);
  EXPECT_EQ(cluster->get_time(), 0.0);
}

TEST_F(SciFiClusterTestDS, test_from_copy_constructor) {
  int spill = 1;
  int event = 2;
  int tracker = 1;
  int station = 2;
  int plane = 0;
  int channel = 106;
  double npe = 3.2;
  double time = 12.2;
  ThreeVector direction(0.0, 0.0, 1.0);
  ThreeVector position(1.0, 2.0, 3.0);

  SciFiCluster *cluster1 = new SciFiCluster();

  cluster1->set_spill(spill);
  cluster1->set_event(event);
  cluster1->set_tracker(tracker);
  cluster1->set_station(station);
  cluster1->set_plane(plane);
  cluster1->set_channel(channel);
  cluster1->set_npe(npe);
  cluster1->set_time(time);
  cluster1->set_direction(direction);
  cluster1->set_position(position);

  SciFiCluster *cluster2 = new SciFiCluster(*cluster1);
  delete cluster1;

  EXPECT_EQ(cluster2->get_spill(), spill);
  EXPECT_EQ(cluster2->get_event(), event);
  EXPECT_EQ(cluster2->get_tracker(), tracker);
  EXPECT_EQ(cluster2->get_station(), station);
  EXPECT_EQ(cluster2->get_plane(), plane);
  EXPECT_EQ(cluster2->get_channel(), channel);
  EXPECT_EQ(cluster2->get_npe(), npe);
  EXPECT_EQ(cluster2->get_time(), time);
  EXPECT_EQ(cluster2->get_direction(), direction);
  EXPECT_EQ(cluster2->get_position(), position);
}

TEST_F(SciFiClusterTestDS, test_from_digit_constructor) {
  int spill = 1;
  int event = 2;
  int tracker = 1;
  int station = 2;
  int plane = 0;
  int channel = 106;
  double npe = 3.2;
  double time = 12.2;

  SciFiDigit *digit = new SciFiDigit(spill, event, tracker, station, plane, channel, npe, time);
  EXPECT_FALSE(digit->is_used());

  SciFiCluster *cluster = new SciFiCluster(digit);

  EXPECT_EQ(cluster->get_spill(), spill);
  EXPECT_EQ(cluster->get_event(), event);
  EXPECT_EQ(tracker, cluster->get_tracker());
  EXPECT_EQ(station, cluster->get_station());
  EXPECT_EQ(plane, cluster->get_plane());
  EXPECT_EQ(channel, cluster->get_channel());
  EXPECT_EQ(npe, cluster->get_npe());
  EXPECT_EQ(time, cluster->get_time());
  EXPECT_FALSE(cluster->is_used());
  EXPECT_TRUE(digit->is_used());
}

TEST_F(SciFiClusterTestDS, test_assignment_operator) {
  int spill = 1;
  int event = 2;
  int tracker = 1;
  int station = 2;
  int plane = 0;
  int channel = 106;
  double npe = 3.2;
  double time = 12.2;
  ThreeVector direction(0.0, 0.0, 1.0);
  ThreeVector position(1.0, 2.0, 3.0);

  SciFiCluster *cluster1 = new SciFiCluster();

  cluster1->set_spill(spill);
  cluster1->set_event(event);
  cluster1->set_tracker(tracker);
  cluster1->set_station(station);
  cluster1->set_plane(plane);
  cluster1->set_channel(channel);
  cluster1->set_npe(npe);
  cluster1->set_time(time);
  cluster1->set_direction(direction);
  cluster1->set_position(position);

  SciFiCluster *cluster2 = new SciFiCluster();
  *cluster2 = *cluster1;
  delete cluster1;

  EXPECT_EQ(cluster2->get_spill(), spill);
  EXPECT_EQ(cluster2->get_event(), event);
  EXPECT_EQ(cluster2->get_tracker(), tracker);
  EXPECT_EQ(cluster2->get_station(), station);
  EXPECT_EQ(cluster2->get_plane(), plane);
  EXPECT_EQ(cluster2->get_channel(), channel);
  EXPECT_EQ(cluster2->get_npe(), npe);
  EXPECT_EQ(cluster2->get_time(), time);
  EXPECT_EQ(cluster2->get_direction(), direction);
  EXPECT_EQ(cluster2->get_position(), position);
}

TEST_F(SciFiClusterTestDS, test_setters_getters) {
  int spill = 1;
  int event = 2;
  int tracker = 1;
  int station = 2;
  int plane = 0;
  int channel = 106;
  double npe = 3.2;
  double time = 12.2;
  ThreeVector direction(0.0, 0.0, 1.0);
  ThreeVector position(1.0, 2.0, 3.0);

  SciFiCluster *cluster = new SciFiCluster();

  cluster->set_spill(spill);
  cluster->set_event(event);
  cluster->set_tracker(tracker);
  cluster->set_station(station);
  cluster->set_plane(plane);
  cluster->set_channel(channel);
  cluster->set_npe(npe);
  cluster->set_time(time);
  cluster->set_direction(direction);
  cluster->set_position(position);

  EXPECT_EQ(cluster->get_spill(), spill);
  EXPECT_EQ(cluster->get_event(), event);
  EXPECT_EQ(cluster->get_tracker(), tracker);
  EXPECT_EQ(cluster->get_station(), station);
  EXPECT_EQ(cluster->get_plane(), plane);
  EXPECT_EQ(cluster->get_channel(), channel);
  EXPECT_EQ(cluster->get_npe(), npe);
  EXPECT_EQ(cluster->get_time(), time);
  EXPECT_EQ(cluster->get_direction(), direction);
  EXPECT_EQ(cluster->get_position(), position);
}

TEST_F(SciFiClusterTestDS, test_used_flag) {
  SciFiCluster *cluster = new SciFiCluster();
  EXPECT_FALSE(cluster->is_used());
  cluster->set_used();
  EXPECT_TRUE(cluster->is_used());
}



TEST_F(SciFiClusterTestDS, test_add_digit) {
  int spill = 1;
  int event = 2;
  int tracker = 1;
  int station = 2;
  int plane = 0;
  int channel1 = 106;
  double npe1 = 3.2;
  double time = 12.2;
  // Construct a digit.
  SciFiDigit *digit = new SciFiDigit(spill, event,
                                     tracker, station, plane, channel1, npe1, time);
  // Start a cluster from "digit"
  SciFiCluster *cluster = new SciFiCluster(digit);
  // Construct a neighbour.
  double npe2 = 2.0;
  int channel2 = 107;
  SciFiDigit *neighbour = new SciFiDigit(spill, event,
                                         tracker, station, plane, channel2, npe2, time);
  EXPECT_FALSE(neighbour->is_used());
  // Add neighbour.
  cluster->add_digit(neighbour);

  EXPECT_EQ(cluster->get_npe(), npe1+npe2);
  EXPECT_EQ(cluster->get_channel(), (channel1+channel2)/2.0);
  EXPECT_TRUE(neighbour->is_used());
}
/*
TEST_F(SciFiClusterTestDS, test_construct) {
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
} // ~namespace MAUS
