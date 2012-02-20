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
#include "src/common_cpp/Recon/SciFi/SciFiStraightPRTrack.hh"
#include "src/common_cpp/Recon/SciFi/PatternRecognition.hh"

#include "gtest/gtest.h"

// namespace MAUS {
class SciFiStraightPRTrackTest : public ::testing::Test {
  protected:
    SciFiStraightPRTrackTest()  {}
    virtual ~SciFiStraightPRTrackTest() {}
    virtual void SetUp()    {}
    virtual void TearDown() {}
};

TEST_F(SciFiStraightPRTrackTest, test_default_constructor) {
  SciFiStraightPRTrack prtrack;
  prtrack.get_vsl();

  EXPECT_EQ(prtrack.get_x0(), -1);
  EXPECT_EQ(prtrack.get_mx, -1);
  EXPECT_EQ(prtrack.get_y0(), -1);
  EXPECT_EQ(prtrack.get_my, -1);
  EXPECT_EQ(prtrack.get_tracker, -1);

  EXPECT_EQ(vsl[0], -1);
  EXPECT_EQ(vsl[1], -1);
  EXPECT_EQ(vsl[2], -1);
  EXPECT_EQ(vsl[3], -1);
}

TEST_F(SciFiStraightPRTrackTest, test_other_contructor) {
  double x0 = 1.0;
  double mx = 2.0;
  double y0 = 3.0;
  double my = 4.0;
  int tracker = 1;

  SciFiStraightPRTrack prtrack(tracker, x0, mx, yo, my);

  std::vector<double> vsl = prtrack.get_vsl()

  EXPECT_EQ(prtrack.get_x0(), x0);
  EXPECT_EQ(prtrack.get_mx, mx);
  EXPECT_EQ(prtrack.get_y0(), y0);
  EXPECT_EQ(prtrack.get_my, my);
  EXPECT_EQ(prtrack.get_tracker, tracker);

  EXPECT_EQ(vsl[0], x0);
  EXPECT_EQ(vsl[1], mx);
  EXPECT_EQ(vsl[2], y0);
  EXPECT_EQ(vsl[3], my);
}

TEST_F(SciFiStraightPRTrackTest, test_setters_getters) {
  double x0 = 1.0;
  double mx = 2.0;
  double y0 = 3.0;
  double my = 4.0;
  int tracker = 1;

  SciFiSpacePoint spoint;
  spoint.set_tracker(tracker);
  std::vector<SciFiSpacePoint> spoints;
  spoints.push_back(spoint);

  SciFiStraightPRTrack prtrack;

  prtrack.set_x0(x0);
  prtrack.set_mx(mx);
  prtrack.set_y0(y0);
  prtrack.set_my(my);
  prtrack.set_tracker(tracker);

  std::vector<double> vsl = prtrack.get_vsl();

  prtrack.set_spacepoints(spoints);

  EXPECT_EQ(prtrack.get_x0(), x0);
  EXPECT_EQ(prtrack.get_mx, mx);
  EXPECT_EQ(prtrack.get_y0(), y0);
  EXPECT_EQ(prtrack.get_my, my);
  EXPECT_EQ(prtrack.get_tracker, tracker);

  EXPECT_EQ(vsl[0], x0);
  EXPECT_EQ(vsl[1], mx);
  EXPECT_EQ(vsl[2], y0);
  EXPECT_EQ(vsl[3], my);

  EXPECT_EQ(prtrack.get_spacepoints()[0].get_tracker(), tracker);
}

// } // namespace
