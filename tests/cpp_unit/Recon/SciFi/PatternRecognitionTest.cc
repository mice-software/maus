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

#include "src/common_cpp/Recon/SciFi/SciFiTools.hh"
#include "src/common_cpp/Recon/SciFi/PatternRecognition.hh"
#include "src/common_cpp/Recon/SciFi/LeastSquaresFitter.hh"
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

  std::vector<SciFiSpacePoint*> set_up_spacepoints() {
    SciFiSpacePoint *sp1 = new SciFiSpacePoint();
    SciFiSpacePoint *sp2 = new SciFiSpacePoint();
    SciFiSpacePoint *sp3 = new SciFiSpacePoint();
    SciFiSpacePoint *sp4 = new SciFiSpacePoint();
    SciFiSpacePoint *sp5 = new SciFiSpacePoint();

    ThreeVector pos(-68.24883333333334, -57.810948479361, -0.652299999999741);
    sp1->set_position(pos);
    sp1->set_tracker(1);
    sp1->set_station(1);
    sp1->set_type("triplet");
    sp1->set_used(false);

    pos.set(-62.84173333333334, -67.17694825239995, -200.6168999999991);
    sp2->set_position(pos);
    sp2->set_tracker(1);
    sp2->set_station(2);
    sp2->set_type("triplet");
    sp2->set_used(false);

    pos.set(-56.99676666666667, -76.0964980027428, -450.4798999999994);
    sp3->set_position(pos);
    sp3->set_tracker(1);
    sp3->set_station(3);
    sp3->set_type("triplet");
    sp3->set_used(false);

    pos.set(-47.89523333333333, -87.75184770769343, -750.4801999999991);
    sp4->set_position(pos);
    sp4->set_tracker(1);
    sp4->set_station(4);
    sp4->set_type("triplet");
    sp4->set_used(false);

    pos.set(-35.86799999999999, -99.22774738994798, -1100.410099999999);
    sp5->set_position(pos);
    sp5->set_tracker(1);
    sp5->set_station(5);
    sp5->set_type("triplet");
    sp5->set_used(false);

    std::vector<SciFiSpacePoint*> spnts;
    spnts.push_back(sp1);
    spnts.push_back(sp2);
    spnts.push_back(sp3);
    spnts.push_back(sp4);
    spnts.push_back(sp5);

    return spnts;
  }
};

TEST_F(PatternRecognitionTest, test_constructor) {
  PatternRecognition pr;
  EXPECT_FALSE(pr._debug);
  EXPECT_TRUE(pr._up_straight_pr_on);
  EXPECT_TRUE(pr._down_straight_pr_on);
  EXPECT_TRUE(pr._up_helical_pr_on);
  EXPECT_TRUE(pr._down_helical_pr_on);
  EXPECT_EQ(0, pr._verb);
  EXPECT_EQ(2, pr._n_trackers);
  EXPECT_EQ(5, pr._n_stations);
  EXPECT_EQ(0.3844, pr._sd_1to4);
  EXPECT_EQ(0.4298, pr._sd_5);
  EXPECT_EQ(1.0, pr._sd_phi_1to4);
  EXPECT_EQ(1.0, pr._sd_phi_5);
  EXPECT_EQ(7.0, pr._res_cut);
  EXPECT_EQ(150.0, pr._R_res_cut);
  EXPECT_EQ(50.0, pr._straight_chisq_cut);
  EXPECT_EQ(2.0, pr._circle_chisq_cut);
  EXPECT_EQ(3.0, pr._sz_chisq_cut);
  EXPECT_EQ(0.75, pr._n_turns_cut);
  EXPECT_EQ(180.0, pr._Pt_max);
  EXPECT_EQ(50.0, pr._Pz_min);
}

TEST_F(PatternRecognitionTest, test_set_parameters_to_default) {
  PatternRecognition pr;
  pr.set_parameters_to_default();
  EXPECT_FALSE(pr._debug);
  EXPECT_TRUE(pr._up_straight_pr_on);
  EXPECT_TRUE(pr._down_straight_pr_on);
  EXPECT_TRUE(pr._up_helical_pr_on);
  EXPECT_TRUE(pr._down_helical_pr_on);
  EXPECT_EQ(0, pr._verb);
  EXPECT_EQ(2, pr._n_trackers);
  EXPECT_EQ(5, pr._n_stations);
  EXPECT_EQ(0.3844, pr._sd_1to4);
  EXPECT_EQ(0.4298, pr._sd_5);
  EXPECT_EQ(1.0, pr._sd_phi_1to4);
  EXPECT_EQ(1.0, pr._sd_phi_5);
  EXPECT_EQ(7.0, pr._res_cut);
  EXPECT_EQ(150.0, pr._R_res_cut);
  EXPECT_EQ(50.0, pr._straight_chisq_cut);
  EXPECT_EQ(2.0, pr._circle_chisq_cut);
  EXPECT_EQ(3.0, pr._sz_chisq_cut);
  EXPECT_EQ(0.75, pr._n_turns_cut);
  EXPECT_EQ(180.0, pr._Pt_max);
  EXPECT_EQ(50.0, pr._Pz_min);
}

/*
TEST_F(PatternRecognitionTest, test_process_good) {

  PatternRecognition pr;
  pr.set_parameters_to_default();

  // Set up the spacepoints vector
  std::vector<SciFiSpacePoint*> spnts = set_up_spacepoints();

  // For a straight fit
  // ------------------
  SciFiEvent evt1;
  evt1.set_spacepoints(spnts);

  pr.process(false, true, evt1); // Helical off, Straight on

  std::vector<SciFiStraightPRTrack*> strks = evt1.straightprtracks();
  std::vector<SciFiHelicalPRTrack*> htrks = evt1.helicalprtracks();

  // The track parameters that should be reconstructed from the spacepoints
  int num_points = 5;

  double line_y0 = -58.85201389;
  double line_x0 = 68.94108927;
  double line_my = 0.03755825;
  double line_mx = -0.02902014;
  double line_x_chisq = 22.87148204;
  double line_y_chisq = 20.99052559;

  // Check it matches to within a tolerance epsilon
  double epsilon = 0.1;
  ASSERT_EQ(1u, strks.size());
  EXPECT_EQ(0u, htrks.size());
  EXPECT_NEAR(line_x0, strks[0]->get_x0(), epsilon);
  EXPECT_NEAR(line_mx, strks[0]->get_mx(), epsilon);
  EXPECT_NEAR(line_x_chisq, strks[0]->get_x_chisq(), epsilon);
  EXPECT_NEAR(line_y0, strks[0]->get_y0(), epsilon);
  EXPECT_NEAR(line_my, strks[0]->get_my(), epsilon);
  EXPECT_NEAR(line_y_chisq, strks[0]->get_y_chisq(), epsilon);
  EXPECT_EQ(num_points, strks[0]->get_num_points());

  // For a helical fit
  //------------------
  std::vector<SciFiSpacePoint*>::iterator it;
  for (it = spnts.begin(); it != spnts.end(); ++it) {
    (*it)->set_used(false);
  }

  pr.process(true, false, evt1); // Helical on, Straight off

  strks = evt1.straightprtracks();
  htrks = evt1.helicalprtracks();

  // double helix_x0 = 189.91;
  // double helix_y0 = 3.55;
  // double helix_x0 = -68.25;
  // double helix_y0 = -57.81;
  double helix_x0 = -35.92;
  double helix_y0 = -99.29;
  double helix_R = 136.335;
  double helix_dsdz = -0.0470962; // Need to check this value is physical

  ASSERT_EQ(1u, htrks.size());
  EXPECT_EQ(1u, strks.size());
  EXPECT_NEAR(helix_x0, htrks[0]->get_x0(), epsilon);
  EXPECT_NEAR(helix_y0, htrks[0]->get_y0(), epsilon);
  EXPECT_NEAR(helix_R, htrks[0]->get_R(), epsilon);
  EXPECT_NEAR(helix_dsdz, htrks[0]->get_dsdz(), epsilon);
  EXPECT_EQ(num_points, htrks[0]->get_num_points());

  // evt descoping will delete the spacepoints
}
*/

TEST_F(PatternRecognitionTest, test_multiple_evts_per_trigger) {

  PatternRecognition pr;
  pr.set_parameters_to_default();

  // Set up the spacepoints vector
  std::vector<SciFiSpacePoint*> spnts_t1_trk1;
  for ( size_t i = 0; i < 5; ++i ) {
    spnts_t1_trk1.push_back(new SciFiSpacePoint());
    spnts_t1_trk1[i]->set_tracker(0);
    spnts_t1_trk1[i]->set_station(5-i);
    spnts_t1_trk1[i]->set_used(false);
  }
  std::vector<SciFiSpacePoint*> spnts_t1_trk2;
  for ( size_t i = 0; i < 5; ++i ) {
    spnts_t1_trk2.push_back(new SciFiSpacePoint());
    spnts_t1_trk2[i]->set_tracker(0);
    spnts_t1_trk2[i]->set_station(5-i);
    spnts_t1_trk2[i]->set_used(false);
  }
  std::vector<SciFiSpacePoint*> spnts_t1_trk3;
  for ( size_t i = 0; i < 5; ++i ) {
    spnts_t1_trk3.push_back(new SciFiSpacePoint());
    spnts_t1_trk3[i]->set_tracker(0);
    spnts_t1_trk3[i]->set_station(5-i);
    spnts_t1_trk3[i]->set_used(false);
  }
  std::vector<SciFiSpacePoint*> spnts_t1_trk4;
  for ( size_t i = 0; i < 5; ++i ) {
    spnts_t1_trk4.push_back(new SciFiSpacePoint());
    spnts_t1_trk4[i]->set_tracker(0);
    spnts_t1_trk4[i]->set_station(5-i);
    spnts_t1_trk4[i]->set_used(false);
  }

  std::vector<SciFiSpacePoint*> spnts_t2_trk1;
  for ( size_t i = 0; i < 5; ++i ) {
    spnts_t2_trk1.push_back(new SciFiSpacePoint());
    spnts_t2_trk1[i]->set_tracker(1);
    spnts_t2_trk1[i]->set_station(i+1);
    spnts_t2_trk1[i]->set_used(false);
  }
  std::vector<SciFiSpacePoint*> spnts_t2_trk2;
  for ( size_t i = 0; i < 5; ++i ) {
    spnts_t2_trk2.push_back(new SciFiSpacePoint());
    spnts_t2_trk2[i]->set_tracker(1);
    spnts_t2_trk2[i]->set_station(i+1);
    spnts_t2_trk2[i]->set_used(false);
  }
  std::vector<SciFiSpacePoint*> spnts_t2_trk3;
  for ( size_t i = 0; i < 5; ++i ) {
    spnts_t2_trk3.push_back(new SciFiSpacePoint());
    spnts_t2_trk3[i]->set_tracker(1);
    spnts_t2_trk3[i]->set_station(i+1);
    spnts_t2_trk3[i]->set_used(false);
  }
  std::vector<SciFiSpacePoint*> spnts_t2_trk4;
  for ( size_t i = 0; i < 5; ++i ) {
    spnts_t2_trk4.push_back(new SciFiSpacePoint());
    spnts_t2_trk4[i]->set_tracker(1);
    spnts_t2_trk4[i]->set_station(i+1);
    spnts_t2_trk4[i]->set_used(false);
  }


  // Spill 4, mu plus
  spnts_t1_trk1[0]->set_position(ThreeVector(0.0, 66.44, 1100.0));
  spnts_t1_trk1[1]->set_position(ThreeVector(-26.4, 47.46, 750.5));
  spnts_t1_trk1[2]->set_position(ThreeVector(-2.491, 28.47, 450.5));
  spnts_t1_trk1[3]->set_position(ThreeVector(14.45, 49.18, 200.6));
  spnts_t1_trk1[4]->set_position(ThreeVector(1.993, 69.03, 0.6523));

  spnts_t2_trk1[0]->set_position(ThreeVector(-26.4, 56.09, 0.6523));
  spnts_t2_trk1[1]->set_position(ThreeVector(-25.9, -1.726, 200.7));
  spnts_t2_trk1[2]->set_position(ThreeVector(43.84, -17.26, 450.7));
  spnts_t2_trk1[3]->set_position(ThreeVector(57.79, 63.85, 750.7));
  spnts_t2_trk1[4]->set_position(ThreeVector(-32.38, 47.46, 1101));

  // Spill 5, mu plus
  spnts_t1_trk2[0]->set_position(ThreeVector(-16.44, 15.53, 1100.0));
  spnts_t1_trk2[1]->set_position(ThreeVector(-19.93, 10.35, 750.5 ));
  spnts_t1_trk2[2]->set_position(ThreeVector(-15.44, 9.491, 450.5));
  spnts_t1_trk2[3]->set_position(ThreeVector(-15.44, 12.94, 200.6));
  spnts_t1_trk2[4]->set_position(ThreeVector(-18.93, 13.81, 0.6523));

  spnts_t2_trk2[0]->set_position(ThreeVector(-4.982, 15.53, 0.6523));
  spnts_t2_trk2[1]->set_position(ThreeVector(-12.7, -5.609, 200.7));
  spnts_t2_trk2[2]->set_position(ThreeVector(10.71, -20.28, 450.7));
  spnts_t2_trk2[3]->set_position(ThreeVector(23.41, 9.491, 750.7));
  spnts_t2_trk2[4]->set_position(ThreeVector(-12.95, 5.177, 1101 ));

  // Spill 6, mu plus
  spnts_t1_trk3[0]->set_position(ThreeVector(-50.81, -23.3,  1100));
  spnts_t1_trk3[1]->set_position(ThreeVector(33.88,  8.628,  750.5));
  spnts_t1_trk3[2]->set_position(ThreeVector(-41.35, 44.01,  450.5));
  spnts_t1_trk3[3]->set_position(ThreeVector(-43.84, -31.06, 200.6));
  spnts_t1_trk3[4]->set_position(ThreeVector(18.93,  -27.61, 0.6523 ));

  spnts_t2_trk3[0]->set_position(ThreeVector(-3.487, 47.46,  0.6523 ));
  spnts_t2_trk3[1]->set_position(ThreeVector(13.95,  24.16,  200.7));
  spnts_t2_trk3[2]->set_position(ThreeVector(40.85,  44.87,  450.7));
  spnts_t2_trk3[3]->set_position(ThreeVector(8.469,  61.26,  750.7 ));
  spnts_t2_trk3[4]->set_position(ThreeVector(18.43,  26.75,  1101 ));

  // Spill 2, mu minus
  spnts_t1_trk4[0]->set_position(ThreeVector(-0.4982, 31.06, 1100));
  spnts_t1_trk4[1]->set_position(ThreeVector(-9.465, -0.8628, 750.5));
  spnts_t1_trk4[2]->set_position(ThreeVector(20.42, 4.314, 450.5));
  spnts_t1_trk4[3]->set_position(ThreeVector(11.46, 30.2, 200.6));
  spnts_t1_trk4[4]->set_position(ThreeVector(-9.465, 25.02, 0.6523));

  spnts_t2_trk4[0]->set_position(ThreeVector(-2.491, -19.85, 0.6523));
  spnts_t2_trk4[1]->set_position(ThreeVector(12.95, -24.16, 200.7));
  spnts_t2_trk4[2]->set_position(ThreeVector(18.93, -6.903, 450.7));
  spnts_t2_trk4[3]->set_position(ThreeVector(-2.491, -4.314, 750.7));
  spnts_t2_trk4[4]->set_position(ThreeVector(7.971, -25.89, 1101.0));

  std::vector<SciFiSpacePoint*> spnts(spnts_t1_trk1);
  spnts.insert(spnts.end(), spnts_t1_trk2.begin(), spnts_t1_trk2.end());
  spnts.insert(spnts.end(), spnts_t1_trk3.begin(), spnts_t1_trk3.end());
  spnts.insert(spnts.end(), spnts_t1_trk4.begin(), spnts_t1_trk4.end());
  spnts.insert(spnts.end(), spnts_t2_trk1.begin(), spnts_t2_trk1.end());
  spnts.insert(spnts.end(), spnts_t2_trk2.begin(), spnts_t2_trk2.end());
  spnts.insert(spnts.end(), spnts_t2_trk3.begin(), spnts_t2_trk3.end());
  spnts.insert(spnts.end(), spnts_t2_trk4.begin(), spnts_t2_trk4.end());
  SciFiEvent evt1;
  evt1.set_spacepoints(spnts);

  // Randomise things a bit to make it harder
  SciFiSpacePoint *sp1, *sp2;
  sp1 = spnts[3];
  sp2 = spnts[14];
  spnts[3] = sp2;
  spnts[14] = sp1;
  sp1 = spnts[4];
  sp2 = spnts[17];
  spnts[4] = sp2;
  spnts[17] = sp1;

  // Perform the recon
  pr.set_up_helical_pr_on(true);
  pr.set_down_helical_pr_on(true);
  pr.set_up_straight_pr_on(false);
  pr.set_down_straight_pr_on(false);
  pr.process(evt1);

  std::vector<SciFiStraightPRTrack*> strks;
  std::vector<SciFiHelicalPRTrack*> htrks;
  strks = evt1.straightprtracks();
  htrks = evt1.helicalprtracks();

  ASSERT_EQ(8u, htrks.size());
  EXPECT_EQ(0u, strks.size());
  EXPECT_EQ(5, htrks[0]->get_num_points());
  EXPECT_EQ(5, htrks[1]->get_num_points());
  EXPECT_EQ(5, htrks[2]->get_num_points());
  EXPECT_EQ(5, htrks[3]->get_num_points());
  EXPECT_EQ(5, htrks[4]->get_num_points());
  EXPECT_EQ(5, htrks[5]->get_num_points());
  EXPECT_EQ(5, htrks[6]->get_num_points());
  EXPECT_EQ(5, htrks[7]->get_num_points());
//   EXPECT_NEAR(htrks[0]->get_dsdz(), -0.1156, 0.001);
//   EXPECT_NEAR(htrks[1]->get_dsdz(), -0.342, 0.01);
//   EXPECT_NEAR(htrks[2]->get_dsdz(), -0.01834, 0.01);
//   EXPECT_NEAR(htrks[3]->get_dsdz(), -0.1178, 0.01);
//   EXPECT_NEAR(htrks[4]->get_dsdz(), 0.1504, 0.001);
//   EXPECT_NEAR(htrks[5]->get_dsdz(), 0.3126, 0.001);
//   EXPECT_NEAR(htrks[6]->get_dsdz(), 0.08396, 0.001);
//   EXPECT_NEAR(htrks[7]->get_dsdz(), 0.1257, 0.001);

  // evt descoping will delete the spacepoints
}

TEST_F(PatternRecognitionTest, test_make_tracks) {

  // Set up the spacepoints vector
  std::vector<SciFiSpacePoint*> spnts_all = set_up_spacepoints();
  std::vector<SciFiSpacePoint*> spnts;
  spnts.push_back(spnts_all[4]);
  spnts.push_back(spnts_all[1]);
  spnts.push_back(spnts_all[0]);

  PatternRecognition pr;
  pr.set_parameters_to_default();
  int n_stations = 5;

  // Set up the spacepoints by station 2D vector
  std::vector< std::vector<SciFiSpacePoint*> > spnts_by_station(n_stations);
  SciFiTools::sort_by_station(spnts, spnts_by_station);

  SciFiEvent evt;
  bool track_type = 0; // Straight tracks
  int tracker_num = 0;

  // The track parameters that should be reconstructed from the spacepoints
  double x_chisq = 22.87148204;
  double y_chisq = 20.99052559;
  double y0 = -58.85201389;
  double x0 = -68.94108927;
  double my = 0.03755825;
  double mx = -0.02902014;

  // Make a 3 point track
  // ---------------------
  pr.make_all_tracks(track_type, tracker_num, spnts_by_station, evt);
  std::vector<SciFiStraightPRTrack*> strks = evt.straightprtracks();
  std::vector<SciFiHelicalPRTrack*> htrks = evt.helicalprtracks();

  // Check it matches to within a tolerance
  EXPECT_EQ(1u, strks.size());
  EXPECT_EQ(0u, htrks.size());
  EXPECT_EQ(3, strks[0]->get_num_points());
  EXPECT_NEAR(x0, strks[0]->get_x0(), 1);
  EXPECT_NEAR(mx, strks[0]->get_mx(), 0.001);
  EXPECT_NEAR(0.9, strks[0]->get_x_chisq(), 0.1);
  EXPECT_NEAR(y0, strks[0]->get_y0(), 1);
  EXPECT_NEAR(my, strks[0]->get_my(), 0.001);
  EXPECT_NEAR(13.3, strks[0]->get_y_chisq(), 0.1);

  std::vector<SciFiStraightPRTrack*>::iterator strack;
  for (strack = strks.begin(); strack != strks.end(); ++strack) {
    delete (*strack);
  }
  std::vector<SciFiHelicalPRTrack*>::iterator htrack;
  for (htrack = htrks.begin(); htrack != htrks.end(); ++htrack) {
    delete (*htrack);
  }

  // Make a 4 point track
  // ---------------------
  spnts.push_back(spnts_all[2]);
  std::vector<SciFiSpacePoint*>::iterator it;
  for (it = spnts.begin(); it != spnts.end(); ++it) {
    (*it)->set_used(false);
  }

  spnts_by_station.clear();
  spnts_by_station.resize(0);
  spnts_by_station.resize(n_stations);
  SciFiTools::sort_by_station(spnts, spnts_by_station);
  strks.resize(0);
  evt.set_straightprtrack(strks);
  pr.make_all_tracks(track_type, tracker_num, spnts_by_station, evt);
  strks = evt.straightprtracks();
  htrks = evt.helicalprtracks();

  // Check it matches to within a tolerance
  EXPECT_EQ(1u, strks.size());
  EXPECT_EQ(0u, htrks.size());
  EXPECT_EQ(4, strks[0]->get_num_points());
  EXPECT_NEAR(x0, strks[0]->get_x0(), 1);
  EXPECT_NEAR(mx, strks[0]->get_mx(), 0.001);
  EXPECT_NEAR(17.5, strks[0]->get_x_chisq(), 0.1);
  EXPECT_NEAR(y0, strks[0]->get_y0(), 1);
  EXPECT_NEAR(my, strks[0]->get_my(), 0.001);
  EXPECT_NEAR(16.0, strks[0]->get_y_chisq(), 0.1);

  for (strack = strks.begin(); strack != strks.end(); ++strack) {
    delete (*strack);
  }
  for (htrack = htrks.begin(); htrack != htrks.end(); ++htrack) {
    delete (*htrack);
  }

  // Make a 5 point track
  // ---------------------
  spnts.push_back(spnts_all[3]);
  for (it = spnts.begin(); it != spnts.end(); ++it) {
    (*it)->set_used(false);
  }

  spnts_by_station.clear();
  spnts_by_station.resize(0);
  spnts_by_station.resize(n_stations);
  SciFiTools::sort_by_station(spnts, spnts_by_station);
  strks.resize(0);
  evt.set_straightprtrack(strks);
  pr.make_all_tracks(track_type, tracker_num, spnts_by_station, evt);
  strks = evt.straightprtracks();
  htrks = evt.helicalprtracks();

  // Check it matches to within a tolerance
  EXPECT_EQ(1u, strks.size());
  EXPECT_EQ(0u, htrks.size());
  EXPECT_EQ(5, strks[0]->get_num_points());
  EXPECT_NEAR(x0, strks[0]->get_x0(), 1);
  EXPECT_NEAR(mx, strks[0]->get_mx(), 0.001);
  EXPECT_NEAR(x_chisq, strks[0]->get_x_chisq(), 0.1);
  EXPECT_NEAR(y0, strks[0]->get_y0(), 1);
  EXPECT_NEAR(my, strks[0]->get_my(), 0.001);
  EXPECT_NEAR(y_chisq, strks[0]->get_y_chisq(), 0.1);

  for (it = spnts.begin(); it != spnts.end(); ++it) {
    delete (*it);
  }
  // evt descoping will delete tracks created
}

TEST_F(PatternRecognitionTest, test_make_4pt_tracks) {

  // Set up the spacepoints vector
  std::vector<SciFiSpacePoint*> spnts_all = set_up_spacepoints();
  std::vector<SciFiSpacePoint*> spnts;
  spnts.push_back(spnts_all[4]);
  spnts.push_back(spnts_all[2]);
  spnts.push_back(spnts_all[1]);
  spnts.push_back(spnts_all[0]);

  PatternRecognition pr;
  pr.set_parameters_to_default();
  int n_stations = 5;

  // Set up the spacepoints by station 2D vector
  std::vector< std::vector<SciFiSpacePoint*> > spnts_by_station(n_stations);
  SciFiTools::sort_by_station(spnts, spnts_by_station);

  bool track_type = 0; // Straight tracks

  // The track parameters that should be reconstructed from the spacepoints in 5 pt track case
  int num_points = 4;
  int tracker_num = 0;
  double y0 = -58.85201389;
  double x0 = -68.94108927;
  double my = 0.03755825;
  double mx = -0.02902014;
  std::vector<SciFiHelicalPRTrack*> htrks;
  std::vector<SciFiStraightPRTrack*> strks;

  // Make a 4 point track with 4 spacepoints
  // ---------------------------------------
  std::vector<SciFiSpacePoint*>::iterator it;
  for (it = spnts.begin(); it != spnts.end(); ++it) {
    (*it)->set_used(false);
  }

  spnts_by_station.clear();
  spnts_by_station.resize(0);
  spnts_by_station.resize(n_stations);
  SciFiTools::sort_by_station(spnts, spnts_by_station);
  strks.resize(0);

  pr.make_4tracks(track_type, tracker_num, spnts_by_station, strks, htrks);

  // Check it matches to within a tolerance
  EXPECT_EQ(1u, strks.size());
  EXPECT_EQ(0u, htrks.size());
  EXPECT_EQ(num_points, strks[0]->get_num_points());
  EXPECT_NEAR(x0, strks[0]->get_x0(), 3);
  EXPECT_NEAR(mx, strks[0]->get_mx(), 0.002);
  EXPECT_NEAR(17.5, strks[0]->get_x_chisq(), 0.1);
  EXPECT_NEAR(y0, strks[0]->get_y0(), 3);
  EXPECT_NEAR(my, strks[0]->get_my(), 0.005);
  EXPECT_NEAR(15.9, strks[0]->get_y_chisq(), 0.1);

  // Make a 4 point track with 5 spacepoints
  // ---------------------------------------
  spnts.push_back(spnts_all[3]);
  for (it = spnts.begin(); it != spnts.end(); ++it) {
    (*it)->set_used(false);
  }

  spnts_by_station.clear();
  spnts_by_station.resize(0);
  spnts_by_station.resize(n_stations);
  SciFiTools::sort_by_station(spnts, spnts_by_station);

  std::vector<SciFiStraightPRTrack*>::iterator strack;
  for (strack = strks.begin(); strack != strks.end(); ++strack) {
    delete (*strack);
  }
  strks.resize(0);

  pr.make_4tracks(track_type, tracker_num, spnts_by_station, strks, htrks);

  // Check it matches to within a tolerance
  EXPECT_EQ(5u, strks.size());
  EXPECT_EQ(0u, htrks.size());
  EXPECT_EQ(num_points, strks[0]->get_num_points());
  EXPECT_NEAR(x0, strks[0]->get_x0(), 3);
  EXPECT_NEAR(mx, strks[0]->get_mx(), 0.01); // Needed a wider tolerance than the others
  EXPECT_NEAR(16.3, strks[0]->get_x_chisq(), 0.1);
  EXPECT_NEAR(y0, strks[0]->get_y0(), 3);
  EXPECT_NEAR(my, strks[0]->get_my(), 0.005);
  EXPECT_NEAR(4.5, strks[0]->get_y_chisq(), 0.1);

  // Tidy up
  for (it = spnts.begin(); it != spnts.end(); ++it) {
    delete (*it);
  }
  for (strack = strks.begin(); strack != strks.end(); ++strack) {
    delete (*strack);
  }
  std::vector<SciFiHelicalPRTrack*>::iterator htrack;
  for (htrack = htrks.begin(); htrack != htrks.end(); ++htrack) {
    delete (*htrack);
  }
}

TEST_F(PatternRecognitionTest, test_make_3pt_tracks) {

  // Set up the spacepoints vector
  std::vector<SciFiSpacePoint*> spnts_all = set_up_spacepoints();
  std::vector<SciFiSpacePoint*> spnts;
  spnts.push_back(spnts_all[4]);
  spnts.push_back(spnts_all[1]);
  spnts.push_back(spnts_all[0]);

  PatternRecognition pr;
  pr.set_parameters_to_default();
  int n_stations = 5;

  // Set up the spacepoints by station 2D vector
  std::vector< std::vector<SciFiSpacePoint*> > spnts_by_station(n_stations);
  SciFiTools::sort_by_station(spnts, spnts_by_station);

  // The track parameters that should be reconstructed from the spacepoints in 5 pt track case
  int num_points = 3;
  int tracker_num = 0;
  double y0 = -58.85201389;
  double x0 = -68.94108927;
  double my = 0.03755825;
  double mx = -0.02902014;
  std::vector<SciFiHelicalPRTrack*> htrks;
  std::vector<SciFiStraightPRTrack*> strks;

  // Make a 3 point track with 3 spacepoints
  // ---------------------------------------
  pr.make_3tracks(tracker_num, spnts_by_station, strks);

  // Check it matches to within a tolerance
  EXPECT_EQ(1u, strks.size());
  EXPECT_EQ(0u, htrks.size());
  EXPECT_EQ(num_points, strks[0]->get_num_points());
  EXPECT_NEAR(x0, strks[0]->get_x0(), 3);
  EXPECT_NEAR(mx, strks[0]->get_mx(), 0.002);
  EXPECT_NEAR(0.9, strks[0]->get_x_chisq(), 0.1);
  EXPECT_NEAR(y0, strks[0]->get_y0(), 3);
  EXPECT_NEAR(my, strks[0]->get_my(), 0.005);
  EXPECT_NEAR(13.3, strks[0]->get_y_chisq(), 0.1);

  std::vector<SciFiStraightPRTrack*>::iterator strack;
  for (strack = strks.begin(); strack != strks.end(); ++strack) {
    delete (*strack);
  }
  std::vector<SciFiHelicalPRTrack*>::iterator htrack;
  for (htrack = htrks.begin(); htrack != htrks.end(); ++htrack) {
    delete (*htrack);
  }

  // Make a 3 point track with 4 spacepoints
  // ---------------------------------------
  spnts.push_back(spnts_all[2]);
  std::vector<SciFiSpacePoint*>::iterator it;
  for (it = spnts.begin(); it != spnts.end(); ++it) {
    (*it)->set_used(false);
  }

  spnts_by_station.clear();
  spnts_by_station.resize(0);
  spnts_by_station.resize(n_stations);
  SciFiTools::sort_by_station(spnts, spnts_by_station);
  strks.resize(0);

  pr.make_3tracks(tracker_num, spnts_by_station, strks);

  // Check it matches to within a tolerance
  EXPECT_EQ(4u, strks.size());
  EXPECT_EQ(0u, htrks.size());
  EXPECT_EQ(num_points, strks[0]->get_num_points());
  EXPECT_NEAR(x0, strks[0]->get_x0(), 3);
  EXPECT_NEAR(mx, strks[0]->get_mx(), 0.002);
  EXPECT_NEAR(11.3, strks[0]->get_x_chisq(), 0.1);
  EXPECT_NEAR(y0, strks[0]->get_y0(), 3);
  EXPECT_NEAR(my, strks[0]->get_my(), 0.005);
  EXPECT_NEAR(0.0015, strks[0]->get_y_chisq(), 0.001);

  for (strack = strks.begin(); strack != strks.end(); ++strack) {
    delete (*strack);
  }
  for (htrack = htrks.begin(); htrack != htrks.end(); ++htrack) {
    delete (*htrack);
  }

  // Make a 3 point track with 5 spacepoints
  // ---------------------------------------
  spnts.push_back(spnts_all[3]);
  for (it = spnts.begin(); it != spnts.end(); ++it) {
    (*it)->set_used(false);
  }

  spnts_by_station.clear();
  spnts_by_station.resize(0);
  spnts_by_station.resize(n_stations);
  SciFiTools::sort_by_station(spnts, spnts_by_station);
  strks.resize(0);

  pr.make_3tracks(tracker_num, spnts_by_station, strks);

  // Check it matches to within a tolerance
  EXPECT_EQ(10u, strks.size());
  EXPECT_EQ(0u, htrks.size());
  EXPECT_EQ(num_points, strks[0]->get_num_points());
  EXPECT_NEAR(x0, strks[0]->get_x0(), 3);
  EXPECT_NEAR(mx, strks[0]->get_mx(), 0.01); // Needed a wider tolerance than the others
  EXPECT_NEAR(1.8, strks[0]->get_x_chisq(), 0.1);
  EXPECT_NEAR(y0, strks[0]->get_y0(), 3);
  EXPECT_NEAR(my, strks[0]->get_my(), 0.005);
  EXPECT_NEAR(4.2, strks[0]->get_y_chisq(), 0.1);

  // Tidy up
  for (it = spnts.begin(); it != spnts.end(); ++it) {
    delete (*it);
  }
  for (strack = strks.begin(); strack != strks.end(); ++strack) {
    delete (*strack);
  }
  for (htrack = htrks.begin(); htrack != htrks.end(); ++htrack) {
    delete (*htrack);
  }
}

TEST_F(PatternRecognitionTest, test_make_straight_tracks) {

  int n_stations = 5;
  int tracker_num = 0;
  PatternRecognition pr;
  pr.set_parameters_to_default();

  // Set up the spacepoints vector
  std::vector<SciFiSpacePoint*> spnts = set_up_spacepoints();

  // Set up the spacepoints by station 2D vector
  std::vector< std::vector<SciFiSpacePoint*> > spnts_by_station(n_stations);
  SciFiTools::sort_by_station(spnts, spnts_by_station);

  // Check the spacepoints have setup correctly
  EXPECT_EQ(spnts[0], spnts_by_station[0][0]);
  EXPECT_EQ(spnts[1], spnts_by_station[1][0]);
  EXPECT_EQ(spnts[2], spnts_by_station[2][0]);
  EXPECT_EQ(spnts[3], spnts_by_station[3][0]);
  EXPECT_EQ(spnts[4], spnts_by_station[4][0]);
  EXPECT_EQ(-68.24883333333334, spnts_by_station[0][0]->get_position().x());

  std::vector<int> ignore_stations;
  std::vector<SciFiStraightPRTrack*> strks;

  // The track parameters that should be reconstructed from the spacepoints
  int num_points = 5;
  double x_chisq = 22.87148204;
  double y_chisq = 20.99052559;
  double y0 = -58.85201389;
  double x0 = -68.94108927;
  double my = 0.03755825;
  double mx = -0.02902014;

  // Make the track from the spacepoints
  pr.make_straight_tracks(num_points, tracker_num, ignore_stations, spnts_by_station, strks);

  // Check it matches to within a tolerance epsilon
  double epsilon = 0.000001;
  EXPECT_EQ(1u, strks.size());
  EXPECT_NEAR(x0, strks[0]->get_x0(), epsilon);
  EXPECT_NEAR(mx, strks[0]->get_mx(), epsilon);
  EXPECT_NEAR(x_chisq, strks[0]->get_x_chisq(), epsilon);
  EXPECT_NEAR(y0, strks[0]->get_y0(), epsilon);
  EXPECT_NEAR(my, strks[0]->get_my(), epsilon);
  EXPECT_NEAR(y_chisq, strks[0]->get_y_chisq(), epsilon);

  // Tidy up
  std::vector<SciFiSpacePoint*>::iterator it;
  for (it = spnts.begin(); it != spnts.end(); ++it) {
    delete (*it);
  }
  std::vector<SciFiStraightPRTrack*>::iterator strack;
  for (strack = strks.begin(); strack != strks.end(); ++strack) {
    delete (*strack);
  }
}

TEST_F(PatternRecognitionTest, test_set_ignore_stations) {

  PatternRecognition pr;
  pr.set_parameters_to_default();
  std::vector<int> ignore_stations(0);
  int is1, is2;

  EXPECT_TRUE(pr.set_ignore_stations(ignore_stations, is1, is2));
  EXPECT_EQ(-1, is1);
  EXPECT_EQ(-1, is2);

  ignore_stations.push_back(4);
  EXPECT_TRUE(pr.set_ignore_stations(ignore_stations, is1, is2));
  EXPECT_EQ(4, is1);
  EXPECT_EQ(-1, is2);

  ignore_stations.push_back(1);
  EXPECT_TRUE(pr.set_ignore_stations(ignore_stations, is1, is2));
  EXPECT_EQ(4, is1);
  EXPECT_EQ(1, is2);

  ignore_stations.push_back(2);
  EXPECT_FALSE(pr.set_ignore_stations(ignore_stations, is1, is2));
  EXPECT_EQ(-1, is1);
  EXPECT_EQ(-1, is2);
}

TEST_F(PatternRecognitionTest, test_set_seed_stations) {

  PatternRecognition pr;
  pr.set_parameters_to_default();
  std::vector<int> ignore_stations(0);
  int outer_st_num, inner_st_num, mid_st_num;

  // 5 pt track case
  EXPECT_TRUE(pr.set_seed_stations(ignore_stations, outer_st_num, inner_st_num, mid_st_num));
  EXPECT_EQ(4, outer_st_num);
  EXPECT_EQ(0, inner_st_num);
  EXPECT_EQ(2, mid_st_num);

  // 4 pt track case
  ignore_stations.push_back(4);
  EXPECT_TRUE(pr.set_seed_stations(ignore_stations, outer_st_num, inner_st_num, mid_st_num));
  EXPECT_EQ(3, outer_st_num);
  EXPECT_EQ(0, inner_st_num);
  EXPECT_EQ(2, mid_st_num);

  ignore_stations[0] = 3;
  EXPECT_TRUE(pr.set_seed_stations(ignore_stations, outer_st_num, inner_st_num, mid_st_num));
  EXPECT_EQ(4, outer_st_num);
  EXPECT_EQ(0, inner_st_num);
  EXPECT_EQ(2, mid_st_num);

  ignore_stations[0] = 2;
  EXPECT_TRUE(pr.set_seed_stations(ignore_stations, outer_st_num, inner_st_num, mid_st_num));
  EXPECT_EQ(4, outer_st_num);
  EXPECT_EQ(0, inner_st_num);
  EXPECT_EQ(1, mid_st_num);

  ignore_stations[0] = 1;
  EXPECT_TRUE(pr.set_seed_stations(ignore_stations, outer_st_num, inner_st_num, mid_st_num));
  EXPECT_EQ(4, outer_st_num);
  EXPECT_EQ(0, inner_st_num);
  EXPECT_EQ(2, mid_st_num);

  ignore_stations[0] = 0;
  EXPECT_TRUE(pr.set_seed_stations(ignore_stations, outer_st_num, inner_st_num, mid_st_num));
  EXPECT_EQ(4, outer_st_num);
  EXPECT_EQ(1, inner_st_num);
  EXPECT_EQ(2, mid_st_num);

  // 3 pt track case (not testing all possible combinations)
  ignore_stations[0] = 4;
  ignore_stations.push_back(3);
  EXPECT_TRUE(pr.set_seed_stations(ignore_stations, outer_st_num, inner_st_num, mid_st_num));
  EXPECT_EQ(2, outer_st_num);
  EXPECT_EQ(0, inner_st_num);
  EXPECT_EQ(1, mid_st_num);

  ignore_stations[0] = 4;
  ignore_stations[1] = 2;
  EXPECT_TRUE(pr.set_seed_stations(ignore_stations, outer_st_num, inner_st_num, mid_st_num));
  EXPECT_EQ(3, outer_st_num);
  EXPECT_EQ(0, inner_st_num);
  EXPECT_EQ(1, mid_st_num);

  ignore_stations[0] = 4;
  ignore_stations[1] = 1;
  EXPECT_TRUE(pr.set_seed_stations(ignore_stations, outer_st_num, inner_st_num, mid_st_num));
  EXPECT_EQ(3, outer_st_num);
  EXPECT_EQ(0, inner_st_num);
  EXPECT_EQ(2, mid_st_num);

  ignore_stations[0] = 4;
  ignore_stations[1] = 0;
  EXPECT_TRUE(pr.set_seed_stations(ignore_stations, outer_st_num, inner_st_num, mid_st_num));
  EXPECT_EQ(3, outer_st_num);
  EXPECT_EQ(1, inner_st_num);
  EXPECT_EQ(2, mid_st_num);

  ignore_stations[0] = 3;
  ignore_stations[1] = 4;
  EXPECT_TRUE(pr.set_seed_stations(ignore_stations, outer_st_num, inner_st_num, mid_st_num));
  EXPECT_EQ(2, outer_st_num);
  EXPECT_EQ(0, inner_st_num);
  EXPECT_EQ(1, mid_st_num);

  ignore_stations[0] = 3;
  ignore_stations[1] = 2;
  EXPECT_TRUE(pr.set_seed_stations(ignore_stations, outer_st_num, inner_st_num, mid_st_num));
  EXPECT_EQ(4, outer_st_num);
  EXPECT_EQ(0, inner_st_num);
  EXPECT_EQ(1, mid_st_num);

  ignore_stations[0] = 3;
  ignore_stations[1] = 1;
  EXPECT_TRUE(pr.set_seed_stations(ignore_stations, outer_st_num, inner_st_num, mid_st_num));
  EXPECT_EQ(4, outer_st_num);
  EXPECT_EQ(0, inner_st_num);
  EXPECT_EQ(2, mid_st_num);

  ignore_stations[0] = 3;
  ignore_stations[1] = 0;
  EXPECT_TRUE(pr.set_seed_stations(ignore_stations, outer_st_num, inner_st_num, mid_st_num));
  EXPECT_EQ(4, outer_st_num);
  EXPECT_EQ(1, inner_st_num);
  EXPECT_EQ(2, mid_st_num);

  ignore_stations[0] = 2;
  ignore_stations[1] = 1;
  EXPECT_TRUE(pr.set_seed_stations(ignore_stations, outer_st_num, inner_st_num, mid_st_num));
  EXPECT_EQ(4, outer_st_num);
  EXPECT_EQ(0, inner_st_num);
  EXPECT_EQ(3, mid_st_num);

  ignore_stations[0] = 2;
  ignore_stations[1] = 0;
  EXPECT_TRUE(pr.set_seed_stations(ignore_stations, outer_st_num, inner_st_num, mid_st_num));
  EXPECT_EQ(4, outer_st_num);
  EXPECT_EQ(1, inner_st_num);
  EXPECT_EQ(3, mid_st_num);

  ignore_stations[0] = 1;
  ignore_stations[1] = 0;
  EXPECT_TRUE(pr.set_seed_stations(ignore_stations, outer_st_num, inner_st_num, mid_st_num));
  EXPECT_EQ(4, outer_st_num);
  EXPECT_EQ(2, inner_st_num);
  EXPECT_EQ(3, mid_st_num);

  ignore_stations[0] = 0;
  ignore_stations[1] = 1;
  EXPECT_TRUE(pr.set_seed_stations(ignore_stations, outer_st_num, inner_st_num, mid_st_num));
  EXPECT_EQ(4, outer_st_num);
  EXPECT_EQ(2, inner_st_num);
  EXPECT_EQ(3, mid_st_num);

  // Test input error cases

  // Same two stations numbers entered
  ignore_stations[0] = 4;
  ignore_stations.push_back(4);
  EXPECT_FALSE(pr.set_seed_stations(ignore_stations, outer_st_num, inner_st_num, mid_st_num));

  // Out of bounds station numbers
  ignore_stations.resize(1);
  ignore_stations[0] = 12;
  EXPECT_FALSE(pr.set_seed_stations(ignore_stations, outer_st_num, inner_st_num, mid_st_num));

  ignore_stations[0] = -1;
  EXPECT_FALSE(pr.set_seed_stations(ignore_stations, outer_st_num, inner_st_num, mid_st_num));

  // Too large ignore_stations vector
  ignore_stations.resize(3);
  EXPECT_FALSE(pr.set_seed_stations(ignore_stations, outer_st_num, inner_st_num, mid_st_num));
}


TEST_F(PatternRecognitionTest, test_set_end_stations) {

  PatternRecognition pr;
  pr.set_parameters_to_default();
  std::vector<int> ignore_stations(0);
  int outer_st_num, inner_st_num;

  // 5 pt track case
  EXPECT_TRUE(pr.set_end_stations(ignore_stations, outer_st_num, inner_st_num));
  EXPECT_EQ(4, outer_st_num);
  EXPECT_EQ(0, inner_st_num);

  // 4 pt track case
  ignore_stations.push_back(4);
  EXPECT_TRUE(pr.set_end_stations(ignore_stations, outer_st_num, inner_st_num));
  EXPECT_EQ(3, outer_st_num);
  EXPECT_EQ(0, inner_st_num);

  ignore_stations[0] = 3;
  EXPECT_TRUE(pr.set_end_stations(ignore_stations, outer_st_num, inner_st_num));
  EXPECT_EQ(4, outer_st_num);
  EXPECT_EQ(0, inner_st_num);

  ignore_stations[0] = 2;
  EXPECT_TRUE(pr.set_end_stations(ignore_stations, outer_st_num, inner_st_num));
  EXPECT_EQ(4, outer_st_num);
  EXPECT_EQ(0, inner_st_num);

  ignore_stations[0] = 1;
  EXPECT_TRUE(pr.set_end_stations(ignore_stations, outer_st_num, inner_st_num));
  EXPECT_EQ(4, outer_st_num);
  EXPECT_EQ(0, inner_st_num);

  ignore_stations[0] = 0;
  EXPECT_TRUE(pr.set_end_stations(ignore_stations, outer_st_num, inner_st_num));
  EXPECT_EQ(4, outer_st_num);
  EXPECT_EQ(1, inner_st_num);

  // 3 pt track case (not testing all possible combinations)
  ignore_stations[0] = 4;
  ignore_stations.push_back(3);
  EXPECT_TRUE(pr.set_end_stations(ignore_stations, outer_st_num, inner_st_num));
  EXPECT_EQ(2, outer_st_num);
  EXPECT_EQ(0, inner_st_num);

  ignore_stations[0] = 4;
  ignore_stations[1] = 2;
  EXPECT_TRUE(pr.set_end_stations(ignore_stations, outer_st_num, inner_st_num));
  EXPECT_EQ(3, outer_st_num);
  EXPECT_EQ(0, inner_st_num);

  ignore_stations[0] = 4;
  ignore_stations[1] = 1;
  EXPECT_TRUE(pr.set_end_stations(ignore_stations, outer_st_num, inner_st_num));
  EXPECT_EQ(3, outer_st_num);
  EXPECT_EQ(0, inner_st_num);

  ignore_stations[0] = 4;
  ignore_stations[1] = 0;
  EXPECT_TRUE(pr.set_end_stations(ignore_stations, outer_st_num, inner_st_num));
  EXPECT_EQ(3, outer_st_num);
  EXPECT_EQ(1, inner_st_num);

  ignore_stations[0] = 3;
  ignore_stations[1] = 4;
  EXPECT_TRUE(pr.set_end_stations(ignore_stations, outer_st_num, inner_st_num));
  EXPECT_EQ(2, outer_st_num);
  EXPECT_EQ(0, inner_st_num);

  ignore_stations[0] = 3;
  ignore_stations[1] = 2;
  EXPECT_TRUE(pr.set_end_stations(ignore_stations, outer_st_num, inner_st_num));
  EXPECT_EQ(4, outer_st_num);
  EXPECT_EQ(0, inner_st_num);

  ignore_stations[0] = 3;
  ignore_stations[1] = 1;
  EXPECT_TRUE(pr.set_end_stations(ignore_stations, outer_st_num, inner_st_num));
  EXPECT_EQ(4, outer_st_num);
  EXPECT_EQ(0, inner_st_num);

  ignore_stations[0] = 3;
  ignore_stations[1] = 0;
  EXPECT_TRUE(pr.set_end_stations(ignore_stations, outer_st_num, inner_st_num));
  EXPECT_EQ(4, outer_st_num);
  EXPECT_EQ(1, inner_st_num);

  ignore_stations[0] = 2;
  ignore_stations[1] = 1;
  EXPECT_TRUE(pr.set_end_stations(ignore_stations, outer_st_num, inner_st_num));
  EXPECT_EQ(4, outer_st_num);
  EXPECT_EQ(0, inner_st_num);

  ignore_stations[0] = 2;
  ignore_stations[1] = 0;
  EXPECT_TRUE(pr.set_end_stations(ignore_stations, outer_st_num, inner_st_num));
  EXPECT_EQ(4, outer_st_num);
  EXPECT_EQ(1, inner_st_num);

  ignore_stations[0] = 1;
  ignore_stations[1] = 0;
  EXPECT_TRUE(pr.set_end_stations(ignore_stations, outer_st_num, inner_st_num));
  EXPECT_EQ(4, outer_st_num);
  EXPECT_EQ(2, inner_st_num);

  ignore_stations[0] = 0;
  ignore_stations[1] = 1;
  EXPECT_TRUE(pr.set_end_stations(ignore_stations, outer_st_num, inner_st_num));
  EXPECT_EQ(4, outer_st_num);
  EXPECT_EQ(2, inner_st_num);

  // Test input error cases

  // Same two stations numbers entered
  ignore_stations[0] = 4;
  ignore_stations.push_back(4);
  EXPECT_FALSE(pr.set_end_stations(ignore_stations, outer_st_num, inner_st_num));

  // Out of bounds station numbers
  ignore_stations.resize(1);
  ignore_stations[0] = 12;
  EXPECT_FALSE(pr.set_end_stations(ignore_stations, outer_st_num, inner_st_num));

  ignore_stations[0] = -1;
  EXPECT_FALSE(pr.set_end_stations(ignore_stations, outer_st_num, inner_st_num));

  // Too large ignore_stations vector
  ignore_stations.resize(3);
  EXPECT_FALSE(pr.set_end_stations(ignore_stations, outer_st_num, inner_st_num));
}

TEST_F(PatternRecognitionTest, test_find_dsdz) {

  PatternRecognition pr;
  pr.set_parameters_to_default();

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

  sp1->set_tracker(1);
  sp2->set_tracker(1);
  sp3->set_tracker(1);
  sp4->set_tracker(1);
  sp5->set_tracker(1);

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
  TMatrixD cov(3, 3);
  bool good_radius = LeastSquaresFitter::circle_fit(0.3844, 0.4298, 150.0, spnts, circle, cov);

  std::vector<double> sigma_s;
  for (auto sp : spnts) {
    sigma_s.push_back(pr.sigma_on_s(circle, cov, sp));
  }

  double epsilon = 0.01;

  ASSERT_TRUE(good_radius);
  EXPECT_NEAR(circle.get_x0(), 2.56, epsilon);
  EXPECT_NEAR(circle.get_y0(), -4.62, epsilon);
  EXPECT_NEAR(circle.get_R(), 18.56, epsilon);
  EXPECT_NEAR(circle.get_chisq(), 0.0994, epsilon);

  SimpleLine line_sz;
  std::vector<double> dphi;
  int n_points = 5;
  int charge = 0;

  TMatrixD cov_sz;
  std::cerr << "Testing find_dsdz now" <<std::endl;
  pr.find_dsdz(n_points, spnts, circle, sigma_s, dphi, line_sz, cov_sz, charge);

  ASSERT_EQ(charge, 1);
  // EXPECT_NEAR(line_sz.get_c(), 15.47, epsilon);
  EXPECT_NEAR(line_sz.get_c(), 15.79, epsilon);
  EXPECT_NEAR(line_sz.get_m(), 0.126, epsilon);
  // EXPECT_NEAR(line_sz.get_chisq(), 1.005 , epsilon);
  EXPECT_NEAR(line_sz.get_chisq(), 0.0646, epsilon);

  delete sp1;
  delete sp2;
  delete sp3;
  delete sp4;
  delete sp5;
}

TEST_F(PatternRecognitionTest, test_find_n_turns) {

  PatternRecognition pr;
  pr.set_parameters_to_default();

  // T1 positives
  double arr_z[] = {1100.41, 750.48, 450.48, 200.617, 0.6523};
  std::vector<double> z(arr_z, arr_z + sizeof(arr_z) / sizeof(double));
  double arr_phi[] = {1.80696, 3.93027, 5.81611, 1.16642, 2.39529};
  std::vector<double> phi(arr_phi, arr_phi + sizeof(arr_phi) / sizeof(double));
  std::vector<double> true_phi;
  int charge = 0;
  pr.find_n_turns(z, phi, true_phi, charge);
  double epsilon = 0.01;
  ASSERT_EQ(charge, 1);
  ASSERT_EQ(true_phi.size(), 5);
  EXPECT_NEAR(true_phi[0], 1.80696, epsilon);
  EXPECT_NEAR(true_phi[1], 3.93027, epsilon);
  EXPECT_NEAR(true_phi[2], 5.81611, epsilon);
  EXPECT_NEAR(true_phi[3], 7.44961, epsilon);
  EXPECT_NEAR(true_phi[4], 8.67847, epsilon);
}

TEST_F(PatternRecognitionTest, test_setup_debug) {

  PatternRecognition* pr = new PatternRecognition();
  pr->set_parameters_to_default();
  pr->setup_debug();

  EXPECT_TRUE(pr->_debug);
  EXPECT_TRUE(pr->_rfile);
  EXPECT_TRUE(pr->_hx);
  EXPECT_TRUE(pr->_hy);
  EXPECT_TRUE(pr->_hxchisq);
  EXPECT_TRUE(pr->_hychisq);

  delete pr;
}

} // ~namespace MAUS
