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

#include "src/common_cpp/Recon/SciFi/PatternRecognition.hh"
#include "src/common_cpp/Recon/SciFi/SciFiSpacePoint.hh"
#include "src/common_cpp/Recon/SciFi/SciFiStraightPRTrack.hh"

namespace {
class PatternRecognitionTest : public ::testing::Test {
 protected:
  PatternRecognitionTest()  {}
  virtual ~PatternRecognitionTest() {}
  virtual void SetUp()    {}
  virtual void TearDown() {}
};

TEST_F(PatternRecognitionTest, test_linear_fit) {
  PatternRecognition pr;

  // Test with a simple line, c = 2, m = 1, with three points, small errors
  std::vector<double> x, y, y_err;
  x.push_back(1);
  x.push_back(2);
  x.push_back(3);
  y.push_back(3);
  y.push_back(4);
  y.push_back(5);
  y_err.push_back(0.05);
  y_err.push_back(0.05);
  y_err.push_back(0.05);

  SimpleLine line;
  pr.linear_fit(x, y, y_err, line);

  EXPECT_EQ(2.0, line.get_c());
  EXPECT_EQ(1.0, line.get_m());
}

}// namespace
