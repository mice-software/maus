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
#include "src/common_cpp/Recon/SciFi/SciFiEvent.hh"
#include "src/common_cpp/Recon/SciFi/SciFiSpacePointRec.hh"
#include "src/common_cpp/Recon/SciFi/SciFiSpacePoint.hh"
#include "src/common_cpp/Recon/SciFi/SciFiCluster.hh"

#include "gtest/gtest.h"

// MAUS namespace {
class SciFiSpacePointRecTest : public ::testing::Test {
 protected:
  SciFiSpacePointRecTest()  {}
  virtual ~SciFiSpacePointRecTest() {}
  virtual void SetUp()    {}
  virtual void TearDown() {}
};

// TEST_F(SciFiSpacePointRecTest, test_process) {
// }


TEST_F(SciFiSpacePointRecTest, test_process) {
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

  SciFiEvent event;
  event.scificlusters.push_back(Cluster);

  SciFiSpacePointRec test;
  test.process(event);
  EXPECT_EQ(event.scifispacepoints.size(), 0);
}
// } // namespace
