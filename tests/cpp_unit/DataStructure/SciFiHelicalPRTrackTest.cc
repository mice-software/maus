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
  unsigned int size = 0;
  SciFiHelicalPRTrack prtrack;
  EXPECT_EQ(prtrack.get_pos0().x(), -1.0);
  EXPECT_EQ(prtrack.get_pos0().y(), -1.0);
  EXPECT_EQ(prtrack.get_pos0().z(), -1.0);
  EXPECT_EQ(prtrack.get_phi0(), -1.0);
  EXPECT_EQ(prtrack.get_dsdz(), -1.0);
  EXPECT_EQ(prtrack.get_R(), -1.0);
  EXPECT_EQ(prtrack.get_chi_squared(), -1.0);
  EXPECT_EQ(prtrack.get_ndf(), -1.0);
  EXPECT_EQ(prtrack.get_point_spread(), -1.0);
  EXPECT_EQ(prtrack.get_line_sz_chisq(), -1.0);
  EXPECT_EQ(prtrack.get_circle_x0(), -1.0);
  EXPECT_EQ(prtrack.get_circle_y0(), -1.0);
  EXPECT_EQ(prtrack.get_circle_chisq(), -1.0);
  EXPECT_EQ(prtrack.get_tracker(), -1);
  EXPECT_EQ(prtrack.get_num_points(), 0);
  EXPECT_EQ(prtrack.get_num_triplets(), 0);
  EXPECT_EQ(prtrack.get_charge(), 0);
  EXPECT_EQ(prtrack.get_spacepoints()->GetEntries(), size);
  EXPECT_EQ(prtrack.get_covariance().size(), 0);
}

// TEST_F(SciFiHelicalPRTrackTestDS, test_parameter_constructor) {
//   SciFiHelicalPRTrack prtrack(0, 3, -1, 1.0, 2.0, 1100.0, 4.0, 2.4, 0.02, 15.0, 30.0);
//   EXPECT_EQ(prtrack.get_x0(), 1.0);
//   EXPECT_EQ(prtrack.get_y0(), 2.0);
//   EXPECT_EQ(prtrack.get_z0(), 1100.0);
//   EXPECT_EQ(prtrack.get_phi0(), 4.0);
//   EXPECT_EQ(prtrack.get_dsdz(), 0.02);
//   EXPECT_EQ(prtrack.get_tracker(), 0);
//   EXPECT_EQ(prtrack.get_num_points(), 3);
//   EXPECT_EQ(prtrack.get_charge(), -1);
//   EXPECT_EQ(prtrack.get_chisq(), 30.0);
// }

TEST_F(SciFiHelicalPRTrackTestDS, test_simplefit_constructor) {
  int tracker = 0;
  int num_points = 5;
  int charge = -1;
  double phi0 = 3.0;
  double x = 1.0;
  double y = 2.0;
  double z = 1100.0;
  double c = 1.0;
  double m = 0.03;
  double sz_chisq = 7.0;
  double R = 13.0;
  double circ_chisq = 6.0;
  double chisq = sz_chisq + circ_chisq;
  int ndf = 5.0;
  double point_spread = 10.0;
  std::vector<double> cov(0);
  cov.push_back(10.0);

  ThreeVector pos0(x, y, z);
  SimpleLine line_sz;
  SimpleCircle circle;
  line_sz.set_c(c);
  line_sz.set_m(m);
  line_sz.set_chisq(sz_chisq);
  circle.set_R(R);
  circle.set_chisq(circ_chisq);
  std::vector<double> phi;
  std::vector<SciFiSpacePoint*> spnts;

  for (int it_p = 0; it_p < num_points; ++it_p) {
    SciFiSpacePoint *spoint = new SciFiSpacePoint();
    spoint->set_tracker(tracker);
    spnts.push_back(spoint);
  }

  SciFiHelicalPRTrack trk(tracker, charge, pos0, phi0, circle, line_sz,
                          point_spread, phi, spnts, cov);

  EXPECT_EQ(trk.get_pos0().x(), x);
  EXPECT_EQ(trk.get_pos0().y(), y);
  EXPECT_EQ(trk.get_pos0().z(), z);
  EXPECT_EQ(trk.get_phi0(), phi0);
  EXPECT_EQ(trk.get_dsdz(), m);
  EXPECT_EQ(trk.get_R(), R);
  EXPECT_EQ(trk.get_line_sz_chisq(), sz_chisq);
  EXPECT_EQ(trk.get_circle_chisq(), circ_chisq);
  EXPECT_EQ(trk.get_tracker(), tracker);
  EXPECT_EQ(trk.get_num_points(), num_points);
  EXPECT_EQ(trk.get_num_triplets(), 0);
  EXPECT_EQ(trk.get_charge(), charge);
  EXPECT_EQ(trk.get_chi_squared(), chisq);
  EXPECT_EQ(trk.get_ndf(), ndf);
  EXPECT_EQ(trk.get_point_spread(), point_spread);
  size_t size = 1;
  EXPECT_EQ(trk.get_spacepoints()->GetEntries(), size);
  EXPECT_EQ(static_cast<SciFiSpacePoint*>(trk.get_spacepoints()->At(0))->get_tracker(), tracker);
  size = 0;
  EXPECT_EQ(trk.get_phi().size(), size);
  ASSERT_EQ(trk.get_covariance().size(), 1);
  EXPECT_EQ(trk.get_covariance()[0], 10.0);
}

TEST_F(SciFiHelicalPRTrackTestDS, test_copy_constructor) {
  int tracker = 0;
  int num_points = 3;
  int charge = -1;
  double x0 = 1.0;
  double y0 = 2.0;
  double z0 = 1100.0;
  double phi0 = 4.0;
  double dsdz = 0.02;
  double R = 15.0;
  double chisq = 30.0;
  int ndf = 1.0;
  double point_spread = 50.0;
  double line_sz_chisq = 5.0;
  double circle_x0 = 6.0;
  double circle_y0 = 7.0;
  double circle_chisq = 8.0;
  std::vector<double> cov(0);
  cov.push_back(10.0);

  SciFiHelicalPRTrack* trk1 = new SciFiHelicalPRTrack;
  trk1->set_tracker(tracker);
  trk1->set_charge(charge);
  trk1->set_pos0(ThreeVector(x0, y0, z0));
  trk1->set_phi0(phi0);
  trk1->set_dsdz(dsdz);
  trk1->set_R(R);
  trk1->set_chi_squared(chisq);
  trk1->set_ndf(ndf);
  trk1->set_point_spread(point_spread);
  trk1->set_line_sz_chisq(line_sz_chisq);
  trk1->set_circle_x0(circle_x0);
  trk1->set_circle_y0(circle_y0);
  trk1->set_circle_chisq(circle_chisq);
  trk1->set_covariance(cov);

  SciFiSpacePointPArray spoints;
  for (int it_p = 0; it_p < num_points; ++it_p) {
    SciFiSpacePoint *spoint = new SciFiSpacePoint(new SciFiCluster(), new SciFiCluster(),
                                                                               new SciFiCluster());
    spoint->set_tracker(tracker);
    spoints.push_back(spoint);
  }

  trk1->set_spacepoints_pointers(spoints);

  std::vector<double> phi_i(0);
  phi_i.push_back(1.0);
  phi_i.push_back(-2.0);

  SciFiHelicalPRTrack trk2(*trk1);
  delete trk1;

  EXPECT_EQ(trk2.get_pos0().x(), x0);
  EXPECT_EQ(trk2.get_pos0().y(), y0);
  EXPECT_EQ(trk2.get_pos0().z(), z0);
  EXPECT_EQ(trk2.get_phi0(), phi0);
  EXPECT_EQ(trk2.get_dsdz(), dsdz);
  EXPECT_EQ(trk2.get_R(), R);
  EXPECT_EQ(trk2.get_chi_squared(), chisq);
  EXPECT_EQ(trk2.get_ndf(), ndf);
  EXPECT_EQ(trk2.get_point_spread(), point_spread);
  EXPECT_EQ(trk2.get_line_sz_chisq(), line_sz_chisq);
  EXPECT_EQ(trk2.get_circle_x0(), circle_x0);
  EXPECT_EQ(trk2.get_circle_y0(), circle_y0);
  EXPECT_EQ(trk2.get_circle_chisq(), circle_chisq);
  EXPECT_EQ(trk2.get_tracker(), tracker);
  EXPECT_EQ(trk2.get_num_points(), num_points);
  EXPECT_EQ(trk2.get_num_triplets(), 5);
  EXPECT_EQ(trk2.get_charge(), charge);
  EXPECT_EQ(static_cast<SciFiSpacePoint*>(trk2.get_spacepoints()->At(0))->get_tracker(), tracker);
  ASSERT_EQ(trk2.get_covariance().size(), 1);
  EXPECT_EQ(trk2.get_covariance()[0], 10.0);
}

// TEST_F(SciFiHelicalPRTrackTestDS, test_helix_constructor) {
//   int tracker = 0;
//   int num_points = 3;
//   int charge = -1;
//   double x0 = 1.0;
//   double y0 = 2.0;
//   double z0 = 1100.0;
//   double phi0 = 4.0;
//   double dsdz = 0.02;
//   double R = 15.0;
//   double chisq = 30.0;
//   double chisq_dof = 10.0;
//
//   SimpleHelix hlx;
//   hlx.set_R(R);
//   hlx.set_Phi_0(phi0);
//   hlx.set_dsdz(dsdz);
//   hlx.set_chisq(chisq);
//   hlx.set_chisq_dof(chisq_dof);
//
//   ThreeVector pos(x0, y0, z0);
//
//   SciFiHelicalPRTrack prtrack(tracker, num_points, charge, pos, hlx);
//
//   unsigned int size = 0;
//
//   EXPECT_EQ(prtrack.get_x0(), x0);
//   EXPECT_EQ(prtrack.get_y0(), y0);
//   EXPECT_EQ(prtrack.get_z0(), z0);
//   EXPECT_EQ(prtrack.get_phi0(), phi0);
//   EXPECT_EQ(prtrack.get_dsdz(), dsdz);
//   EXPECT_EQ(prtrack.get_R(), R);
//   EXPECT_EQ(prtrack.get_chisq(), chisq);
//   EXPECT_EQ(prtrack.get_chisq_dof(), chisq_dof);
//   EXPECT_EQ(prtrack.get_line_sz_chisq(), -1);
//   EXPECT_EQ(prtrack.get_circle_x0(), -1);
//   EXPECT_EQ(prtrack.get_circle_y0(), -1);
//   EXPECT_EQ(prtrack.get_circle_chisq(), -1);
//   EXPECT_EQ(prtrack.get_spacepoints().size(), size);
//   EXPECT_EQ(prtrack.get_tracker(), tracker);
//   EXPECT_EQ(prtrack.get_num_points(), num_points);
//   EXPECT_EQ(prtrack.get_charge(), charge);
// }

TEST_F(SciFiHelicalPRTrackTestDS, test_assignment_operator) {
  int tracker = 0;
  int num_points = 3;
  int charge = -1;
  double x0 = 1.0;
  double y0 = 2.0;
  double z0 = 1100.0;
  double phi0 = 4.0;
  double dsdz = 0.02;
  double R = 15.0;
  double chisq = 30.0;
  int ndf = 10.0;
  double point_spread = 50.0;
  double line_sz_chisq = 5.0;
  double circle_x0 = 6.0;
  double circle_y0 = 7.0;
  double circle_chisq = 8.0;
  std::vector<double> cov(0);
  cov.push_back(10.0);

  SciFiHelicalPRTrack* trk1 = new SciFiHelicalPRTrack();
  trk1->set_tracker(tracker);
  trk1->set_charge(charge);
  trk1->set_pos0(ThreeVector(x0, y0, z0));
  trk1->set_phi0(phi0);
  trk1->set_dsdz(dsdz);
  trk1->set_R(R);
  trk1->set_chi_squared(chisq);
  trk1->set_ndf(ndf);
  trk1->set_point_spread(point_spread);
  trk1->set_line_sz_chisq(line_sz_chisq);
  trk1->set_circle_x0(circle_x0);
  trk1->set_circle_y0(circle_y0);
  trk1->set_circle_chisq(circle_chisq);
  trk1->set_covariance(cov);

  SciFiSpacePointPArray spoints;
  for (int it_p = 0; it_p < num_points; ++it_p) {
    SciFiSpacePoint *spoint = new SciFiSpacePoint();
    spoint->set_tracker(tracker);
    spoints.push_back(spoint);
  }

  trk1->set_spacepoints_pointers(spoints);

  SciFiHelicalPRTrack trk2;
  trk2 = *trk1;

  EXPECT_EQ(trk2.get_pos0().x(), x0);
  EXPECT_EQ(trk2.get_pos0().y(), y0);
  EXPECT_EQ(trk2.get_pos0().z(), z0);
  EXPECT_EQ(trk2.get_phi0(), phi0);
  EXPECT_EQ(trk2.get_dsdz(), dsdz);
  EXPECT_EQ(trk2.get_R(), R);
  EXPECT_EQ(trk2.get_chi_squared(), chisq);
  EXPECT_EQ(trk2.get_ndf(), ndf);
  EXPECT_EQ(trk2.get_point_spread(), point_spread);
  EXPECT_EQ(trk2.get_line_sz_chisq(), line_sz_chisq);
  EXPECT_EQ(trk2.get_circle_x0(), circle_x0);
  EXPECT_EQ(trk2.get_circle_y0(), circle_y0);
  EXPECT_EQ(trk2.get_circle_chisq(), circle_chisq);
  EXPECT_EQ(trk2.get_tracker(), tracker);
  EXPECT_EQ(trk2.get_num_points(), num_points);
  EXPECT_EQ(trk2.get_num_triplets(), num_points);
  EXPECT_EQ(trk2.get_num_triplets(), 0);
  EXPECT_EQ(trk2.get_charge(), charge);
  EXPECT_EQ(static_cast<SciFiSpacePoint*>(trk2.get_spacepoints()->At(0))->get_tracker(), tracker);
  ASSERT_EQ(trk2.get_covariance().size(), 1);
  EXPECT_EQ(trk2.get_covariance()[0], 10.0);
}

TEST_F(SciFiHelicalPRTrackTestDS, test_setters_getters) {
  int tracker = 0;
  int num_points = 3;
  int charge = -1;
  double x0 = 1.0;
  double y0 = 2.0;
  double z0 = 1100.0;
  double phi0 = 4.0;
  double dsdz = 0.02;
  double R = 15.0;
  double chisq = 30.0;
  int ndf = 5;
  double point_spread = 50.0;
  double line_sz_chisq = 5.0;
  double circle_x0 = 6.0;
  double circle_y0 = 7.0;
  double circle_chisq = 8.0;
  std::vector<double> cov(0);
  cov.push_back(10.0);

  SciFiSpacePointPArray spoints;
  for (int it_p = 0; it_p < num_points-2; ++it_p) {
    SciFiSpacePoint *spoint = new SciFiSpacePoint();
    spoint->set_tracker(tracker);
    spoints.push_back(spoint);
  }
  for (int it_p = 0; it_p < 2; ++it_p) {
    SciFiSpacePoint* spoint = new SciFiSpacePoint(new SciFiCluster(), new SciFiCluster(),
                                                                               new SciFiCluster());
    spoint->set_tracker(tracker);
    spoints.push_back(spoint);
  }

  SciFiHelicalPRTrack trk;

  trk.set_tracker(tracker);
  trk.set_charge(-1);
  trk.set_phi0(phi0);
  trk.set_dsdz(dsdz);
  trk.set_R(R);
  trk.set_chi_squared(chisq);
  trk.set_ndf(ndf);
  trk.set_point_spread(point_spread);
  trk.set_line_sz_chisq(line_sz_chisq);
  trk.set_circle_x0(circle_x0);
  trk.set_circle_y0(circle_y0);
  trk.set_circle_chisq(circle_chisq);
  trk.set_pos0(ThreeVector(x0, y0, z0));
  trk.set_spacepoints_pointers(spoints);
  trk.set_covariance(cov);

  EXPECT_EQ(trk.get_tracker(), tracker);
  EXPECT_EQ(trk.get_num_points(), num_points);
  EXPECT_EQ(trk.get_num_triplets(), 2);
  EXPECT_EQ(trk.get_charge(), charge);
  EXPECT_EQ(trk.get_phi0(), phi0);
  EXPECT_EQ(trk.get_dsdz(), dsdz);
  EXPECT_EQ(trk.get_R(), R);
  EXPECT_EQ(trk.get_chi_squared(), chisq);
  EXPECT_EQ(trk.get_ndf(), ndf);
  EXPECT_EQ(trk.get_point_spread(), point_spread);
  EXPECT_EQ(trk.get_line_sz_chisq(), line_sz_chisq);
  EXPECT_EQ(trk.get_circle_x0(), circle_x0);
  EXPECT_EQ(trk.get_circle_y0(), circle_y0);
  EXPECT_EQ(trk.get_circle_chisq(), circle_chisq);
  EXPECT_EQ(trk.get_pos0().x(), x0);
  EXPECT_EQ(trk.get_pos0().y(), y0);
  EXPECT_EQ(trk.get_pos0().z(), z0);
  EXPECT_EQ(
    static_cast<SciFiSpacePoint*>(trk.get_spacepoints()->At(0))->get_tracker(), tracker);
  EXPECT_EQ(trk.get_spacepoints_pointers()[0]->get_tracker(), tracker);
  ASSERT_EQ(trk.get_covariance().size(), 1);
  EXPECT_EQ(trk.get_covariance()[0], 10.0);
}

} // ~namespace MAUS
