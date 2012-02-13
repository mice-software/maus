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

#include "src/common_cpp/Recon/SciFi/SciFiDigit.hh"

namespace {
class SciFiDigitTest : public ::testing::Test {
 protected:
  SciFiDigitTest()  {}
  virtual ~SciFiDigitTest() {}
  virtual void SetUp()    {}
  virtual void TearDown() {}
};

TEST_F(SciFiDigitTest, test_setters_getters) {
  int tracker = 1;
  int station = 2;
  int plane = 0;
  int channel = 106;
  double npe = 3.2;
  double time = 12.2;
  SciFiDigit *digit = new SciFiDigit();

  digit->set_tracker(tracker);
  digit->set_station(station);
  digit->set_plane(plane);
  digit->set_channel(channel);
  digit->set_npe(npe);
  digit->set_time(time);

  EXPECT_EQ(digit->get_tracker(), tracker);
  EXPECT_EQ(digit->get_station(), station);
  EXPECT_EQ(digit->get_plane(), plane);
  EXPECT_EQ(digit->get_channel(), channel);
  EXPECT_EQ(digit->get_npe(), npe);
  EXPECT_EQ(digit->get_time(), time);
}

TEST_F(SciFiDigitTest, test_used_flag) {
  SciFiDigit *digit = new SciFiDigit();
  EXPECT_FALSE(digit->is_used());

  digit->set_used();
  EXPECT_TRUE(digit->is_used());
}

TEST_F(SciFiDigitTest, test_constructor) {
  int tracker = 1;
  int station = 2;
  int plane = 0;
  int channel = 106;
  double npe = 3.2;
  double time = 12.2;

  SciFiDigit *digit = new SciFiDigit(tracker, station, plane, channel, npe, time);

  EXPECT_EQ(digit->get_tracker(), tracker);
  EXPECT_EQ(digit->get_station(), station);
  EXPECT_EQ(digit->get_plane(), plane);
  EXPECT_EQ(digit->get_channel(), channel);
  EXPECT_EQ(digit->get_npe(), npe);
  EXPECT_EQ(digit->get_time(), time);
  EXPECT_FALSE(digit->is_used());
}

} // namespace
