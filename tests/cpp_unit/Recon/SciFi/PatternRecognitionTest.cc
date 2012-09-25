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

#include <cmath>

#include "gtest/gtest.h"

#include "src/common_cpp/Recon/SciFi/PatternRecognition.hh"
#include "src/common_cpp/DataStructure/SciFiSpacePoint.hh"
#include "src/common_cpp/DataStructure/SciFiStraightPRTrack.hh"
#include "src/common_cpp/DataStructure/SciFiEvent.hh"
#include "src/common_cpp/DataStructure/ThreeVector.hh"

namespace MAUS {

class PatternRecognitionTest : public ::testing::Test {
 protected:
  PatternRecognitionTest()  {}
  virtual ~PatternRecognitionTest() {}
  virtual void SetUp()    {}
  virtual void TearDown() {}
  bool compare_doubles(double a, double b, double epsilon) {return fabs(a - b) < epsilon;}
};

TEST_F(PatternRecognitionTest, test_AB_ratio) {

  PatternRecognition pr;
  double phi_i = 1.0;
  double phi_j = 0.5;
  double z_i = 200.0;
  double z_j = 450.0;

  double epsilon = 0.01;

  bool result = pr.AB_ratio(phi_i, phi_j, z_i, z_j);
  ASSERT_TRUE(result);
  // EXPECT_NEAR(phi_i, 7.28319, epsilon);
  // EXPECT_NEAR(phi_j, 6.783, epsilon);
  EXPECT_EQ(z_i, 200.0);
  EXPECT_EQ(z_j, 450.0);
}

TEST_F(PatternRecognitionTest, test_circle_fit) {

  PatternRecognition pr;

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
  bool good_radius = pr.circle_fit(spnts, circle);

  double epsilon = 0.01;

  ASSERT_TRUE(good_radius);
  EXPECT_NEAR(circle.get_x0(), 2.56, epsilon);
  EXPECT_NEAR(circle.get_y0(), -4.62, epsilon);
  EXPECT_NEAR(circle.get_R(), 18.56, epsilon);
  EXPECT_NEAR(circle.get_chisq(), 0.0994, epsilon);
}


TEST_F(PatternRecognitionTest, test_calculate_dipangle) {

  PatternRecognition pr;

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
  spnts.push_back(sp1);
  spnts.push_back(sp2);
  spnts.push_back(sp3);
  spnts.push_back(sp4);
  spnts.push_back(sp5);

  SimpleCircle circle;
  bool good_radius = pr.circle_fit(spnts, circle);

  double epsilon = 0.01;

  ASSERT_TRUE(good_radius);
  EXPECT_NEAR(circle.get_x0(), 2.56, epsilon);
  EXPECT_NEAR(circle.get_y0(), -4.62, epsilon);
  EXPECT_NEAR(circle.get_R(), 18.56, epsilon);
  EXPECT_NEAR(circle.get_chisq(), 0.0994, epsilon);

  SimpleLine line_sz;
  std::vector<double> dphi;
  double phi_0;

  pr.calculate_dipangle(spnts, circle, dphi, line_sz, phi_0);

  EXPECT_NEAR(line_sz.get_c(), -1.09, epsilon);
  EXPECT_NEAR(line_sz.get_m(), 0.126, epsilon);
  EXPECT_NEAR(line_sz.get_chisq(), 0.440, epsilon);
}

TEST_F(PatternRecognitionTest, test_sort_by_station) {

  PatternRecognition pr;

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

  std::vector<SciFiSpacePoint*> spnts;
  spnts.push_back(sp5);
  spnts.push_back(sp2);
  spnts.push_back(sp3);
  spnts.push_back(sp1);
  spnts.push_back(sp4);

  std::vector< std::vector<SciFiSpacePoint*> > spnts_by_station(5);
  pr.sort_by_station(spnts, spnts_by_station);
  EXPECT_EQ(sp1, spnts_by_station[0][0]);
  EXPECT_EQ(sp2, spnts_by_station[1][0]);
  EXPECT_EQ(sp3, spnts_by_station[2][0]);
  EXPECT_EQ(sp4, spnts_by_station[3][0]);
  EXPECT_EQ(sp5, spnts_by_station[4][0]);
}

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

TEST_F(PatternRecognitionTest, test_make_straight_tracks) {

  int n_stations = 5;
  PatternRecognition pr;

  // Set up spacepoints corresponding to straight line
  SciFiSpacePoint *sp1 = new SciFiSpacePoint();
  SciFiSpacePoint *sp2 = new SciFiSpacePoint();
  SciFiSpacePoint *sp3 = new SciFiSpacePoint();
  SciFiSpacePoint *sp4 = new SciFiSpacePoint();
  SciFiSpacePoint *sp5 = new SciFiSpacePoint();

  ThreeVector pos(-68.24883333333334, -57.810948479361, -0.652299999999741);
  sp1->set_position(pos);
  sp1->set_tracker(0);
  sp1->set_station(1);
  sp1->set_type("triplet");

  pos.set(-62.84173333333334, -67.17694825239995, -200.6168999999991);
  sp2->set_position(pos);
  sp2->set_tracker(0);
  sp2->set_station(2);
  sp2->set_type("triplet");

  pos.set(-56.99676666666667, -76.0964980027428, -450.4798999999994);
  sp3->set_position(pos);
  sp3->set_tracker(0);
  sp3->set_station(3);
  sp3->set_type("triplet");

  pos.set(-47.89523333333333, -87.75184770769343, -750.4801999999991);
  sp4->set_position(pos);
  sp4->set_tracker(0);
  sp4->set_station(4);
  sp4->set_type("triplet");

  pos.set(-35.86799999999999, -99.22774738994798, -1100.410099999999);
  sp5->set_position(pos);
  sp5->set_tracker(0);
  sp5->set_station(5);
  sp5->set_type("triplet");

  // Set up the spacepoints vector
  std::vector<SciFiSpacePoint*> spnts;
  spnts.push_back(sp5);
  spnts.push_back(sp2);
  spnts.push_back(sp3);
  spnts.push_back(sp1);
  spnts.push_back(sp4);

  // Set up the spacepoints by station 2D vector
  std::vector< std::vector<SciFiSpacePoint*> > spnts_by_station(n_stations);
  pr.sort_by_station(spnts, spnts_by_station);

  // Check the spacepoints have setup correctly
  EXPECT_EQ(sp1, spnts_by_station[0][0]);
  EXPECT_EQ(sp2, spnts_by_station[1][0]);
  EXPECT_EQ(sp3, spnts_by_station[2][0]);
  EXPECT_EQ(sp4, spnts_by_station[3][0]);
  EXPECT_EQ(sp5, spnts_by_station[4][0]);
  EXPECT_EQ(-68.24883333333334, spnts_by_station[0][0]->get_position().x());

  // Set the tracks and residuals containers
  std::vector<int> ignore_stations;
  std::vector<SciFiStraightPRTrack> strks;

  // The track parameters that should be reconstructed from the spacepoints
  int num_points = 5;
  double x_chisq = 22.87148204;
  double y_chisq = 20.99052559;
  double y0 = -58.85201389;
  double x0 = -68.94108927;
  double my = 0.03755825;
  double mx = -0.02902014;

  // Make the track from the spacepoints
  pr.make_straight_tracks(num_points, ignore_stations, spnts_by_station, strks);

  // Check it matches to within a tolerance epsilon
  double epsilon = 0.000001;
  EXPECT_EQ(1, strks.size());
  EXPECT_TRUE(compare_doubles(x0, strks[0].get_x0(), epsilon));
  EXPECT_TRUE(compare_doubles(mx, strks[0].get_mx(), epsilon));
  EXPECT_TRUE(compare_doubles(x_chisq, strks[0].get_x_chisq(), epsilon));
  EXPECT_TRUE(compare_doubles(y0, strks[0].get_y0(), epsilon));
  EXPECT_TRUE(compare_doubles(my, strks[0].get_my(), epsilon));
  EXPECT_TRUE(compare_doubles(y_chisq, strks[0].get_y_chisq(), epsilon));
}

TEST_F(PatternRecognitionTest, test_process_good) {

  int n_stations = 5;
  PatternRecognition pr;

  // Set up spacepoints corresponding to a nearly straight line
  SciFiSpacePoint *sp1 = new SciFiSpacePoint();
  SciFiSpacePoint *sp2 = new SciFiSpacePoint();
  SciFiSpacePoint *sp3 = new SciFiSpacePoint();
  SciFiSpacePoint *sp4 = new SciFiSpacePoint();
  SciFiSpacePoint *sp5 = new SciFiSpacePoint();

  ThreeVector pos(-68.24883333333334, -57.810948479361, -0.652299999999741);
  sp1->set_position(pos);
  sp1->set_tracker(0);
  sp1->set_station(1);
  sp1->set_type("triplet");

  pos.set(-62.84173333333334, -67.17694825239995, -200.6168999999991);
  sp2->set_position(pos);
  sp2->set_tracker(0);
  sp2->set_station(2);
  sp2->set_type("triplet");

  pos.set(-56.99676666666667, -76.0964980027428, -450.4798999999994);
  sp3->set_position(pos);
  sp3->set_tracker(0);
  sp3->set_station(3);
  sp3->set_type("triplet");

  pos.set(-47.89523333333333, -87.75184770769343, -750.4801999999991);
  sp4->set_position(pos);
  sp4->set_tracker(0);
  sp4->set_station(4);
  sp4->set_type("triplet");

  pos.set(-35.86799999999999, -99.22774738994798, -1100.410099999999);
  sp5->set_position(pos);
  sp5->set_tracker(0);
  sp5->set_station(5);
  sp5->set_type("triplet");

  // Set up the spacepoints vector
  std::vector<SciFiSpacePoint*> spnts;
  spnts.push_back(sp5);
  spnts.push_back(sp2);
  spnts.push_back(sp3);
  spnts.push_back(sp1);
  spnts.push_back(sp4);

  SciFiEvent evt;
  evt.set_spacepoints(spnts);

  pr.process(evt);

  std::vector<SciFiStraightPRTrack> strks = evt.straightprtracks();
  std::vector<SciFiHelicalPRTrack> htrks = evt.helicalprtracks();

  // The track parameters that should be reconstructed from the spacepoints
  int num_points = 5;

  // For a straight fit
  double line_y0 = -58.85201389;
  double line_x0 = -68.94108927;
  double line_my = 0.03755825;
  double line_mx = -0.02902014;
  double line_x_chisq = 22.87148204;
  double line_y_chisq = 20.99052559;

  // For a circle fit
  double helix_x0 = -68.2488;
  double helix_y0 = -57.8109;
  double helix_R = 136.335;

  unsigned int trks_size = 0;
  double true_par1 = 0.0;
  double true_par2 = 0.0;
  double true_par3 = 0.0;
  double true_par4 = 0.0;
  double true_par5 = 0.0;
  double true_par6 = 0.0;
  int true_par7 = 0;
  double recon_par1 = 0.0;
  double recon_par2 = 0.0;
  double recon_par3 = 0.0;
  double recon_par4 = 0.0;
  double recon_par5 = 0.0;
  double recon_par6 = 0.0;
  int recon_par7 = 0;

  if ( pr.get_helical_pr_on() ) {
    trks_size = htrks.size();
    true_par1 = helix_x0;
    true_par2 = helix_y0;
    true_par3 = helix_R;
    true_par7 = num_points;
    recon_par1 = htrks[0].get_x0();
    recon_par2 = htrks[0].get_y0();
    recon_par3 = htrks[0].get_R();
    recon_par7 = htrks[0].get_num_points();
  } else if ( pr.get_straight_pr_on() ) {
    trks_size = strks.size();
    true_par1 = line_x0;
    true_par2 = line_mx;
    true_par3 = line_x_chisq;
    true_par4 = line_y0;
    true_par5 = line_my;
    true_par6 = line_y_chisq;
    true_par7 = num_points;
    recon_par1 = strks[0].get_x0();
    recon_par2 = strks[0].get_mx();
    recon_par3 = strks[0].get_x_chisq();
    recon_par4 = strks[0].get_y0();
    recon_par5 = strks[0].get_my();
    recon_par6 = strks[0].get_y_chisq();
    recon_par7 = strks[0].get_num_points();
  }

  // Check it matches to within a tolerance epsilon
  double epsilon = 0.001;

  EXPECT_EQ(1, trks_size);
  EXPECT_NEAR(true_par1, recon_par1, epsilon);
  EXPECT_NEAR(true_par2, recon_par2, epsilon);
  EXPECT_NEAR(true_par3, recon_par3, epsilon);
  EXPECT_NEAR(true_par4, recon_par4, epsilon);
  EXPECT_NEAR(true_par5, recon_par5, epsilon);
  EXPECT_NEAR(true_par6, recon_par6, epsilon);
  EXPECT_EQ(true_par7, recon_par7);
}




} // ~namespace MAUS
