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

#include "src/common_cpp/Reconstruction/SciFi/SciFiHit.hh"

// MAUS namespace {
class SciFiHitTest : public ::testing::Test {
 protected:
  SciFiHitTest()  {}
  virtual ~SciFiHitTest() {}
  virtual void SetUp()    {}
  virtual void TearDown() {}
};

TEST_F(SciFiHitTest, test_setters_getters) {
  int tracker = 1;
  int station = 2;
  int plane = 0;
  int fibre = 106;
  double edep = 3.2;
  double time = 12.2;
  SciFiHit *Hit = new SciFiHit();

  Hit->set_tracker(tracker);
  Hit->set_station(station);
  Hit->set_plane(plane);
  Hit->set_fibre(fibre);
  Hit->set_edep(edep);
  Hit->set_time(time);

  EXPECT_EQ(Hit->get_tracker(), tracker);
  EXPECT_EQ(Hit->get_station(), station);
  EXPECT_EQ(Hit->get_plane(), plane);
  EXPECT_EQ(Hit->get_fibre(), fibre);
  EXPECT_EQ(Hit->get_edep(), edep);
  EXPECT_EQ(Hit->get_time(), time);
}

TEST_F(SciFiHitTest, test_used_flag) {
  SciFiHit *Hit = new SciFiHit();
  EXPECT_FALSE(Hit->is_used());

  Hit->set_used();
  EXPECT_TRUE(Hit->is_used());
}

TEST_F(SciFiHitTest, test_constructor) {
  int tracker = 1;
  int station = 2;
  int plane = 0;
  int fibre = 106;
  double edep = 3.2;
  double time = 12.2;

  SciFiHit *Hit = new SciFiHit(tracker, station, plane, fibre, edep, time);

  EXPECT_EQ(Hit->get_tracker(), tracker);
  EXPECT_EQ(Hit->get_station(), station);
  EXPECT_EQ(Hit->get_plane(), plane);
  EXPECT_EQ(Hit->get_fibre(), fibre);
  EXPECT_EQ(Hit->get_edep(), edep);
  EXPECT_EQ(Hit->get_time(), time);
  EXPECT_FALSE(Hit->is_used());
}
// } // namespace
