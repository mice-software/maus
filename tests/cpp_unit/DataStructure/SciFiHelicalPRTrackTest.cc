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

#include "src/common_cpp/DataStructure/SciFiSpacePoint.hh"
#include "src/common_cpp/DataStructure/SciFiHelicalPRTrack.hh"

#include "gtest/gtest.h"

namespace MAUS {

class SciFiHelicalPRTrackTestDS : public ::testing::Test {
  protected:
    SciFiHelicalPRTrackTestDS()  {}
    virtual ~SciFiHelicalPRTrackTestDS() {}
    virtual void SetUp()    {}
    virtual void TearDown() {}
};

TEST_F(SciFiHelicalPRTrackTestDS, test_default_constructor) {
  SciFiHelicalPRTrack prtrack;
  EXPECT_EQ(prtrack.get_x0(), -1.0);
  EXPECT_EQ(prtrack.get_y0(), -1.0);
  EXPECT_EQ(prtrack.get_z0(), -1.0);
  EXPECT_EQ(prtrack.get_phi0(), -1.0);
  EXPECT_EQ(prtrack.get_psi0(), -1.0);
  EXPECT_EQ(prtrack.get_dsdz(), -1.0);
  EXPECT_EQ(prtrack.get_R(), -1.0);
  EXPECT_EQ(prtrack.get_line_sz_chisq(), -1.0);
  EXPECT_EQ(prtrack.get_circle_chisq(), -1.0);
  EXPECT_EQ(prtrack.get_chisq(), -1.0);
  EXPECT_EQ(prtrack.get_chisq_dof(), -1.0);
  EXPECT_EQ(prtrack.get_tracker(), -1);
  EXPECT_EQ(prtrack.get_num_points(), -1);
}

TEST_F(SciFiHelicalPRTrackTestDS, test_parameter_constructor) {
  SciFiHelicalPRTrack prtrack(0, 3, 1.0, 2.0, 1100.0, 4.0, 2.4, 0.02, 15.0, 30.0);
  EXPECT_EQ(prtrack.get_x0(), 1.0);
  EXPECT_EQ(prtrack.get_y0(), 2.0);
  EXPECT_EQ(prtrack.get_z0(), 1100.0);
  EXPECT_EQ(prtrack.get_phi0(), 4.0);
  EXPECT_EQ(prtrack.get_psi0(), 2.4);
  EXPECT_EQ(prtrack.get_dsdz(), 0.02);
  EXPECT_EQ(prtrack.get_tracker(), 0);
  EXPECT_EQ(prtrack.get_num_points(), 3);
  EXPECT_EQ(prtrack.get_chisq(), 30.0);
}

TEST_F(SciFiHelicalPRTrackTestDS, test_simplefit_constructor) {
  CLHEP::Hep3Vector pos0(1.0, 2.0, 1100.0);
  SimpleLine line_sz;
  SimpleCircle circle;
  line_sz.set_c(1.0);
  line_sz.set_m(0.03);
  line_sz.set_chisq(7.0);
  std::vector<double> azm_angles;
  azm_angles.push_back(3.0);
  azm_angles.push_back(4.0);
  circle.set_turning_angle(azm_angles);
  circle.set_R(13.0);
  circle.set_chisq(6.0);

  SciFiHelicalPRTrack prtrack(0, 5, pos0, circle, line_sz);
  EXPECT_EQ(prtrack.get_x0(), 1.0);
  EXPECT_EQ(prtrack.get_y0(), 2.0);
  EXPECT_EQ(prtrack.get_z0(), 1100.0);
  EXPECT_EQ(prtrack.get_phi0(), 3.0);
  EXPECT_EQ(prtrack.get_psi0(), -1.0);
  EXPECT_EQ(prtrack.get_dsdz(), 0.03);
  EXPECT_EQ(prtrack.get_R(), 13.0);
  EXPECT_EQ(prtrack.get_line_sz_chisq(), 7.0);
  EXPECT_EQ(prtrack.get_circle_chisq(), 6.0);
  EXPECT_EQ(prtrack.get_tracker(), 0);
  EXPECT_EQ(prtrack.get_num_points(), 5);
}

TEST_F(SciFiHelicalPRTrackTestDS, test_copy_constructor) {
  SciFiHelicalPRTrack trk1(0, 3, 1.0, 2.0, 1100.0, 4.0, 2.4, 0.02, 15.0, 30.0);
  trk1.set_line_sz_chisq(5.0);
  trk1.set_circle_chisq(6.0);
  trk1.set_chisq_dof(7.0);

  SciFiSpacePoint spoint;
  spoint.set_tracker(1);
  SciFiSpacePointArray spoints;
  spoints.push_back(spoint);
  trk1.set_spacepoints(spoints);

  SciFiHelicalPRTrack trk2(trk1);

  EXPECT_EQ(trk2.get_x0(), 1.0);
  EXPECT_EQ(trk2.get_y0(), 2.0);
  EXPECT_EQ(trk2.get_z0(), 1100.0);
  EXPECT_EQ(trk2.get_phi0(), 4.0);
  EXPECT_EQ(trk2.get_psi0(), 2.4);
  EXPECT_EQ(trk2.get_dsdz(), 0.02);
  EXPECT_EQ(trk2.get_line_sz_chisq(), 5.0);
  EXPECT_EQ(trk2.get_circle_chisq(), 6.0);
  EXPECT_EQ(trk2.get_chisq(), 30.0);
  EXPECT_EQ(trk2.get_chisq_dof(), 7.0);
  EXPECT_EQ(trk2.get_tracker(), 0);
  EXPECT_EQ(trk2.get_num_points(), 3);
  EXPECT_EQ(trk2.get_spacepoints()[0].get_tracker(), 1);
}

TEST_F(SciFiHelicalPRTrackTestDS, test_equality_operator) {
  SciFiHelicalPRTrack trk1(0, 3, 1.0, 2.0, 1100.0, 4.0, 2.4, 0.02, 15.0, 20.0);
  trk1.set_line_sz_chisq(5.0);
  trk1.set_circle_chisq(6.0);
  trk1.set_chisq_dof(7.0);

  SciFiSpacePoint spoint;
  spoint.set_tracker(1);
  SciFiSpacePointArray spoints;
  spoints.push_back(spoint);
  trk1.set_spacepoints(spoints);

  SciFiHelicalPRTrack trk2;

  trk2 = trk1;

  EXPECT_EQ(trk2.get_x0(), 1.0);
  EXPECT_EQ(trk2.get_y0(), 2.0);
  EXPECT_EQ(trk2.get_z0(), 1100.0);
  EXPECT_EQ(trk2.get_phi0(), 4.0);
  EXPECT_EQ(trk2.get_psi0(), 2.4);
  EXPECT_EQ(trk2.get_dsdz(), 0.02);
  EXPECT_EQ(trk2.get_R(), 15.0);
  EXPECT_EQ(trk2.get_line_sz_chisq(), 5.0);
  EXPECT_EQ(trk2.get_circle_chisq(), 6.0);
  EXPECT_EQ(trk2.get_chisq(), 20.0);
  EXPECT_EQ(trk2.get_chisq_dof(), 7.0);
  EXPECT_EQ(trk2.get_tracker(), 0);
  EXPECT_EQ(trk2.get_num_points(), 3);
  EXPECT_EQ(trk2.get_spacepoints()[0].get_tracker(), 1);
}

TEST_F(SciFiHelicalPRTrackTestDS, test_setters_getters) {
  double x0      = 1.0;
  double y0      = 2.0;
  double z0      = 1100.0;
  double phi0    = 4.0;
  double psi0    = 2.4;
  double dsdz    = 0.02;
  double R       = 15.0;
  double line_sz_chisq = 2.0;
  double circle_chisq = 3.0;
  double chisq   = 6.0;
  int tracker    = 1;
  int num_points = 5;

  SciFiSpacePoint spoint;
  spoint.set_tracker(tracker);
  SciFiSpacePointArray spoints;
  spoints.push_back(spoint);

  SciFiHelicalPRTrack prtrack;

  prtrack.set_x0(x0);
  prtrack.set_y0(y0);
  prtrack.set_z0(z0);
  prtrack.set_phi0(phi0);
  prtrack.set_psi0(psi0);
  prtrack.set_dsdz(dsdz);
  prtrack.set_R(R);
  prtrack.set_line_sz_chisq(line_sz_chisq);
  prtrack.set_circle_chisq(circle_chisq);
  prtrack.set_chisq(chisq);
  prtrack.set_tracker(tracker);
  prtrack.set_num_points(num_points);

  prtrack.set_spacepoints(spoints);

  EXPECT_EQ(prtrack.get_x0(), x0);
  EXPECT_EQ(prtrack.get_y0(), y0);
  EXPECT_EQ(prtrack.get_z0(), z0);
  EXPECT_EQ(prtrack.get_phi0(), phi0);
  EXPECT_EQ(prtrack.get_psi0(), psi0);
  EXPECT_EQ(prtrack.get_dsdz(), dsdz);
  EXPECT_EQ(prtrack.get_R(), R);
  EXPECT_EQ(prtrack.get_line_sz_chisq(), line_sz_chisq);
  EXPECT_EQ(prtrack.get_circle_chisq(), circle_chisq);
  EXPECT_EQ(prtrack.get_chisq(), chisq);
  EXPECT_EQ(prtrack.get_tracker(), tracker);
  EXPECT_EQ(prtrack.get_num_points(), num_points);

  EXPECT_EQ(prtrack.get_spacepoints()[0].get_tracker(), tracker);
}

} // namespace
