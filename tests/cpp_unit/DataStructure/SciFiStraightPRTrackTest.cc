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

#include "TMatrixD.h"

#include "src/common_cpp/DataStructure/SciFiSpacePoint.hh"
#include "src/common_cpp/DataStructure/SciFiStraightPRTrack.hh"

#include "gtest/gtest.h"

namespace MAUS {

class SciFiStraightPRTrackTestDS : public ::testing::Test {
  protected:
    SciFiStraightPRTrackTestDS()  {}
    virtual ~SciFiStraightPRTrackTestDS() {}
    virtual void SetUp()    {}
    virtual void TearDown() {}
};

TEST_F(SciFiStraightPRTrackTestDS, test_default_constructor) {
  SciFiStraightPRTrack prtrack;
  EXPECT_EQ(prtrack.get_x0(), -1.0);
  EXPECT_EQ(prtrack.get_mx(), -1.0);
  EXPECT_EQ(prtrack.get_x_chisq(), -1.0);
  EXPECT_EQ(prtrack.get_y0(), -1.0);
  EXPECT_EQ(prtrack.get_my(), -1.0);
  EXPECT_EQ(prtrack.get_y_chisq(), -1.0);
  EXPECT_EQ(prtrack.get_global_x0(), -1.0);
  EXPECT_EQ(prtrack.get_global_mx(), -1.0);
  EXPECT_EQ(prtrack.get_global_y0(), -1.0);
  EXPECT_EQ(prtrack.get_global_my(), -1.0);
  EXPECT_EQ(prtrack.get_tracker(), -1);
  EXPECT_EQ(prtrack.get_num_points(), 0);
  EXPECT_EQ(prtrack.get_covariance().size(), 0);
}

TEST_F(SciFiStraightPRTrackTestDS, test_parameter_constructor) {
  std::vector<double> cov(0);
  cov.push_back(10.0);
  SciFiStraightPRTrack prtrack(0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, cov);
  EXPECT_EQ(prtrack.get_x0(), 1.0);
  EXPECT_EQ(prtrack.get_mx(), 2.0);
  EXPECT_EQ(prtrack.get_x_chisq(), 3.0);
  EXPECT_EQ(prtrack.get_y0(), 4.0);
  EXPECT_EQ(prtrack.get_my(), 5.0);
  EXPECT_EQ(prtrack.get_y_chisq(), 6.0);
  EXPECT_EQ(prtrack.get_global_x0(), -1.0);
  EXPECT_EQ(prtrack.get_global_mx(), -1.0);
  EXPECT_EQ(prtrack.get_global_y0(), -1.0);
  EXPECT_EQ(prtrack.get_global_my(), -1.0);
  EXPECT_EQ(prtrack.get_tracker(), 0);
  EXPECT_EQ(prtrack.get_num_points(), 0);
  ASSERT_EQ(prtrack.get_covariance().size(), 1);
  EXPECT_EQ(prtrack.get_covariance()[0], 10.0);
}

TEST_F(SciFiStraightPRTrackTestDS, test_simpleline_constructor) {
  SimpleLine lx, ly;
  std::vector<double> cov(0);
  cov.push_back(10.0);
  lx.set_c(1.0);
  lx.set_m(2.0);
  lx.set_chisq(3.0);
  ly.set_c(4.0);
  ly.set_m(5.0);
  ly.set_chisq(6.0);
  SciFiStraightPRTrack prtrack(0, lx, ly, cov);
  EXPECT_EQ(prtrack.get_x0(), 1.0);
  EXPECT_EQ(prtrack.get_mx(), 2.0);
  EXPECT_EQ(prtrack.get_x_chisq(), 3.0);
  EXPECT_EQ(prtrack.get_y0(), 4.0);
  EXPECT_EQ(prtrack.get_my(), 5.0);
  EXPECT_EQ(prtrack.get_y_chisq(), 6.0);
  EXPECT_EQ(prtrack.get_global_x0(), -1.0);
  EXPECT_EQ(prtrack.get_global_mx(), -1.0);
  EXPECT_EQ(prtrack.get_global_y0(), -1.0);
  EXPECT_EQ(prtrack.get_global_my(), -1.0);
  EXPECT_EQ(prtrack.get_tracker(), 0);
  EXPECT_EQ(prtrack.get_num_points(), 0);
  ASSERT_EQ(prtrack.get_covariance().size(), 1);
  EXPECT_EQ(prtrack.get_covariance()[0], 10.0);
}

TEST_F(SciFiStraightPRTrackTestDS, test_copy_constructor) {
  std::vector<double> cov(0);
  cov.push_back(10.0);
  SciFiStraightPRTrack trk1(0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, cov);
  trk1.set_global_x0(7.0);
  trk1.set_global_mx(8.0);
  trk1.set_global_y0(9.0);
  trk1.set_global_my(10.0);

  SciFiSpacePoint *spoint = new SciFiSpacePoint();
  spoint->set_tracker(1);
  SciFiSpacePointPArray spoints;
  spoints.push_back(spoint);
  trk1.set_spacepoints_pointers(spoints);

  SciFiStraightPRTrack trk2(trk1);

  EXPECT_EQ(trk2.get_x0(), 1.0);
  EXPECT_EQ(trk2.get_mx(), 2.0);
  EXPECT_EQ(trk2.get_x_chisq(), 3.0);
  EXPECT_EQ(trk2.get_y0(), 4.0);
  EXPECT_EQ(trk2.get_my(), 5.0);
  EXPECT_EQ(trk2.get_y_chisq(), 6.0);
  EXPECT_EQ(trk2.get_global_x0(), 7.0);
  EXPECT_EQ(trk2.get_global_mx(), 8.0);
  EXPECT_EQ(trk2.get_global_y0(), 9.0);
  EXPECT_EQ(trk2.get_global_my(), 10.0);
  EXPECT_EQ(trk2.get_tracker(), 0);
  EXPECT_EQ(trk2.get_num_points(), 1);
  EXPECT_EQ(trk2.get_spacepoints_pointers()[0]->get_tracker(), 1);
  ASSERT_EQ(trk2.get_covariance().size(), 1);
  EXPECT_EQ(trk2.get_covariance()[0], 10.0);
}

TEST_F(SciFiStraightPRTrackTestDS, test_equality_operator) {
  std::vector<double> cov(0);
  cov.push_back(10.0);
  SciFiStraightPRTrack trk1(0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, cov);
  trk1.set_global_x0(7.0);
  trk1.set_global_mx(8.0);
  trk1.set_global_y0(9.0);
  trk1.set_global_my(10.0);

  SciFiSpacePoint *spoint = new SciFiSpacePoint();
  spoint->set_tracker(1);
  SciFiSpacePointPArray spoints;
  spoints.push_back(spoint);
  trk1.set_spacepoints_pointers(spoints);

  SciFiStraightPRTrack trk2;
  trk2 = trk1;

  EXPECT_EQ(trk2.get_x0(), 1.0);
  EXPECT_EQ(trk2.get_mx(), 2.0);
  EXPECT_EQ(trk2.get_x_chisq(), 3.0);
  EXPECT_EQ(trk2.get_y0(), 4.0);
  EXPECT_EQ(trk2.get_my(), 5.0);
  EXPECT_EQ(trk2.get_y_chisq(), 6.0);
  EXPECT_EQ(trk2.get_global_x0(), 7.0);
  EXPECT_EQ(trk2.get_global_mx(), 8.0);
  EXPECT_EQ(trk2.get_global_y0(), 9.0);
  EXPECT_EQ(trk2.get_global_my(), 10.0);
  EXPECT_EQ(trk2.get_tracker(), 0);
  EXPECT_EQ(trk2.get_num_points(), 1);
  EXPECT_EQ(trk2.get_spacepoints_pointers()[0]->get_tracker(), 1);
  ASSERT_EQ(trk2.get_covariance().size(), 1);
  EXPECT_EQ(trk2.get_covariance()[0], 10.0);
}

TEST_F(SciFiStraightPRTrackTestDS, test_setters_getters) {
  double x0 = 1.0;
  double mx = 2.0;
  double x_chisq = 3.0;
  double y0 = 4.0;
  double my = 5.0;
  double y_chisq = 6.0;
  double gx0 = 7.0;
  double gmx = 8.0;
  double gy0 = 9.0;
  double gmy = 10.0;

  int tracker = 1;
  int num_points = 5;

  SciFiSpacePointPArray spoints;
  for (int it_p = 0; it_p < num_points; ++it_p) {
    SciFiSpacePoint *spoint = new SciFiSpacePoint();
    spoint->set_tracker(tracker);
    spoints.push_back(spoint);
  }

  std::vector<double> cov(0);
  cov.push_back(10.0);

  SciFiStraightPRTrack prtrack;

  prtrack.set_x0(x0);
  prtrack.set_mx(mx);
  prtrack.set_x_chisq(x_chisq);
  prtrack.set_y0(y0);
  prtrack.set_my(my);
  prtrack.set_y_chisq(y_chisq);
  prtrack.set_global_x0(gx0);
  prtrack.set_global_mx(gmx);
  prtrack.set_global_y0(gy0);
  prtrack.set_global_my(gmy);
  prtrack.set_tracker(tracker);
  prtrack.set_spacepoints_pointers(spoints);
  prtrack.set_covariance(cov);

  EXPECT_EQ(prtrack.get_x0(), x0);
  EXPECT_EQ(prtrack.get_mx(), mx);
  EXPECT_EQ(prtrack.get_x_chisq(), x_chisq);
  EXPECT_EQ(prtrack.get_y0(), y0);
  EXPECT_EQ(prtrack.get_my(), my);
  EXPECT_EQ(prtrack.get_y_chisq(), y_chisq);

  EXPECT_EQ(prtrack.get_global_x0(), gx0);
  EXPECT_EQ(prtrack.get_global_mx(), gmx);
  EXPECT_EQ(prtrack.get_global_y0(), gy0);
  EXPECT_EQ(prtrack.get_global_my(), gmy);

  EXPECT_EQ(prtrack.get_tracker(), tracker);
  EXPECT_EQ(prtrack.get_num_points(), num_points);
  EXPECT_EQ(prtrack.get_spacepoints_pointers()[0]->get_tracker(), tracker);
  ASSERT_EQ(prtrack.get_covariance().size(), 1);
  EXPECT_EQ(prtrack.get_covariance()[0], 10.0);
}

} // ~namespace MAUS
