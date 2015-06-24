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

#include "TMatrixD.h"

#include "src/common_cpp/Recon/SciFi/LeastSquaresFitter.hh"
#include "src/common_cpp/Recon/SciFi/SimpleLine.hh"
#include "src/common_cpp/DataStructure/SciFiSpacePoint.hh"

namespace MAUS {

class LeastSquaresFitterTest : public ::testing::Test {
 protected:
  LeastSquaresFitterTest()  {}
  virtual ~LeastSquaresFitterTest() {}
  virtual void SetUp()    {}
  virtual void TearDown() {}
};

TEST_F(LeastSquaresFitterTest, test_circle_fit) {

  // Set up spacepoints from an MC helical track
  SciFiSpacePoint *sp1 = new SciFiSpacePoint();
  SciFiSpacePoint *sp2 = new SciFiSpacePoint();
  SciFiSpacePoint *sp3 = new SciFiSpacePoint();
  SciFiSpacePoint *sp4 = new SciFiSpacePoint();
  SciFiSpacePoint *sp5 = new SciFiSpacePoint();

  sp1->set_station(1);
  sp2->set_station(2);
  sp3->set_station(3);
  sp4->set_station(4);
  sp5->set_station(5);

  ThreeVector pos(14.1978, 9.05992, 0.6523);
  sp1->set_position(pos);
  pos.set(-7.97067, 10.3542, 200.652);
  sp2->set_position(pos);
  pos.set(-11.4578, -16.3941, 450.652);
  sp3->set_position(pos);
  pos.set(19.9267, -12.0799, 750.652);
  sp4->set_position(pos);
  pos.set(-5.47983, 12.9427, 1100.65);
  sp5->set_position(pos);

  std::vector<SciFiSpacePoint*> spnts;
  spnts.push_back(sp5);
  spnts.push_back(sp2);
  spnts.push_back(sp3);
  spnts.push_back(sp1);
  spnts.push_back(sp4);

  SimpleCircle circle;
  TMatrixD cov;
  bool good_radius = LeastSquaresFitter::circle_fit(0.3844, 0.4298, 150.0, spnts, circle, cov);

  double epsilon = 0.01;

  ASSERT_TRUE(good_radius);
  EXPECT_NEAR(circle.get_x0(), 2.56, epsilon);
  EXPECT_NEAR(circle.get_y0(), -4.62, epsilon);
  EXPECT_NEAR(circle.get_R(), 18.56, epsilon);
  EXPECT_NEAR(circle.get_chisq(), 0.0994, epsilon);
}

TEST_F(LeastSquaresFitterTest, test_linear_fit) {

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
  TMatrixD cov;
  LeastSquaresFitter::linear_fit(x, y, y_err, line, cov);

  double epsilon = 0.00001;

  EXPECT_NEAR(2.0, line.get_c(), epsilon);
  EXPECT_NEAR(1.0, line.get_m(), epsilon);
}

} // ~namespace MAUS
