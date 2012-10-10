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

  void set_up_spacepoints() {
    _sp1 = new SciFiSpacePoint();
    _sp2 = new SciFiSpacePoint();
    _sp3 = new SciFiSpacePoint();
    _sp4 = new SciFiSpacePoint();
    _sp5 = new SciFiSpacePoint();

    ThreeVector pos(-68.24883333333334, -57.810948479361, -0.652299999999741);
    _sp1->set_position(pos);
    _sp1->set_tracker(0);
    _sp1->set_station(1);
    _sp1->set_type("triplet");
    _sp1->set_used(false);

    pos.set(-62.84173333333334, -67.17694825239995, -200.6168999999991);
    _sp2->set_position(pos);
    _sp2->set_tracker(0);
    _sp2->set_station(2);
    _sp2->set_type("triplet");
    _sp2->set_used(false);

    pos.set(-56.99676666666667, -76.0964980027428, -450.4798999999994);
    _sp3->set_position(pos);
    _sp3->set_tracker(0);
    _sp3->set_station(3);
    _sp3->set_type("triplet");
    _sp3->set_used(false);

    pos.set(-47.89523333333333, -87.75184770769343, -750.4801999999991);
    _sp4->set_position(pos);
    _sp4->set_tracker(0);
    _sp4->set_station(4);
    _sp4->set_type("triplet");
    _sp4->set_used(false);

    pos.set(-35.86799999999999, -99.22774738994798, -1100.410099999999);
    _sp5->set_position(pos);
    _sp5->set_tracker(0);
    _sp5->set_station(5);
    _sp5->set_type("triplet");
    _sp5->set_used(false);
  }

  SciFiSpacePoint *_sp1;
  SciFiSpacePoint *_sp2;
  SciFiSpacePoint *_sp3;
  SciFiSpacePoint *_sp4;
  SciFiSpacePoint *_sp5;
};

TEST_F(PatternRecognitionTest, test_process_good) {

  PatternRecognition pr;

  // Set up the spacepoints vector
  set_up_spacepoints();
  std::vector<SciFiSpacePoint*> spnts;
  spnts.push_back(_sp5);
  spnts.push_back(_sp2);
  spnts.push_back(_sp3);
  spnts.push_back(_sp1);
  spnts.push_back(_sp4);

  // For a straight fit
  // ------------------
  SciFiEvent evt1;
  evt1.set_spacepoints(spnts);

  pr.process(false, true, evt1); // Helical off, Straight on

  std::vector<SciFiStraightPRTrack> strks = evt1.straightprtracks();
  std::vector<SciFiHelicalPRTrack> htrks = evt1.helicalprtracks();

  // The track parameters that should be reconstructed from the spacepoints
  int num_points = 5;

  double line_y0 = -58.85201389;
  double line_x0 = -68.94108927;
  double line_my = 0.03755825;
  double line_mx = -0.02902014;
  double line_x_chisq = 22.87148204;
  double line_y_chisq = 20.99052559;

  // Check it matches to within a tolerance epsilon
  double epsilon = 0.001;
  ASSERT_EQ(1, strks.size());
  EXPECT_EQ(0, htrks.size());
  EXPECT_NEAR(line_x0, strks[0].get_x0(), epsilon);
  EXPECT_NEAR(line_mx, strks[0].get_mx(), epsilon);
  EXPECT_NEAR(line_x_chisq, strks[0].get_x_chisq(), epsilon);
  EXPECT_NEAR(line_y0, strks[0].get_y0(), epsilon);
  EXPECT_NEAR(line_my, strks[0].get_my(), epsilon);
  EXPECT_NEAR(line_y_chisq, strks[0].get_y_chisq(), epsilon);
  EXPECT_EQ(num_points, strks[0].get_num_points());

  // For a helical fit
  //------------------
  _sp1->set_used(false);
  _sp2->set_used(false);
  _sp3->set_used(false);
  _sp4->set_used(false);
  _sp5->set_used(false);

  pr.process(true, false, evt1); // Helical on, Straight off

  strks = evt1.straightprtracks();
  htrks = evt1.helicalprtracks();

  double helix_x0 = -68.2488;
  double helix_y0 = -57.8109;
  double helix_R = 136.335;
  double helix_dsdz = -0.0470962; // Need to check this value is physical

  ASSERT_EQ(1, htrks.size());
  EXPECT_EQ(1, strks.size());
  EXPECT_NEAR(helix_x0, htrks[0].get_x0(), epsilon);
  EXPECT_NEAR(helix_y0, htrks[0].get_y0(), epsilon);
  EXPECT_NEAR(helix_R, htrks[0].get_R(), epsilon);
  EXPECT_NEAR(helix_dsdz, htrks[0].get_dsdz(), epsilon);
  EXPECT_EQ(num_points, htrks[0].get_num_points());
}

TEST_F(PatternRecognitionTest, test_make_tracks) {

  // Set up the spacepoints vector
  set_up_spacepoints();
  std::vector<SciFiSpacePoint*> spnts;
  spnts.push_back(_sp5);
  spnts.push_back(_sp2);
  spnts.push_back(_sp1);

  PatternRecognition pr;
  int n_stations = 5;

  // Set up the spacepoints by station 2D vector
  std::vector< std::vector<SciFiSpacePoint*> > spnts_by_station(n_stations);
  pr.sort_by_station(spnts, spnts_by_station);

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
  std::vector<SciFiStraightPRTrack> strks = evt.straightprtracks();
  std::vector<SciFiHelicalPRTrack> htrks = evt.helicalprtracks();

  // Check it matches to within a tolerance
  EXPECT_EQ(1, strks.size());
  EXPECT_EQ(0, htrks.size());
  EXPECT_EQ(3, strks[0].get_num_points());
  EXPECT_NEAR(x0, strks[0].get_x0(), 1);
  EXPECT_NEAR(mx, strks[0].get_mx(), 0.001);
  EXPECT_NEAR(0.9, strks[0].get_x_chisq(), 0.1);
  EXPECT_NEAR(y0, strks[0].get_y0(), 1);
  EXPECT_NEAR(my, strks[0].get_my(), 0.001);
  EXPECT_NEAR(13.3, strks[0].get_y_chisq(), 0.1);

  // Make a 4 point track
  // ---------------------
  spnts.push_back(_sp3);
  _sp1->set_used(false);
  _sp2->set_used(false);
  _sp3->set_used(false);
  _sp5->set_used(false);

  spnts_by_station.clear();
  spnts_by_station.resize(0);
  spnts_by_station.resize(n_stations);
  pr.sort_by_station(spnts, spnts_by_station);
  strks.resize(0);
  evt.set_straightprtrack(strks);
  pr.make_all_tracks(track_type, tracker_num, spnts_by_station, evt);
  strks = evt.straightprtracks();
  htrks = evt.helicalprtracks();

  // Check it matches to within a tolerance
  EXPECT_EQ(1, strks.size());
  EXPECT_EQ(0, htrks.size());
  EXPECT_EQ(4, strks[0].get_num_points());
  EXPECT_NEAR(x0, strks[0].get_x0(), 1);
  EXPECT_NEAR(mx, strks[0].get_mx(), 0.001);
  EXPECT_NEAR(17.5, strks[0].get_x_chisq(), 0.1);
  EXPECT_NEAR(y0, strks[0].get_y0(), 1);
  EXPECT_NEAR(my, strks[0].get_my(), 0.001);
  EXPECT_NEAR(16.0, strks[0].get_y_chisq(), 0.1);

  // Make a 5 point track
  // ---------------------
  spnts.push_back(_sp4);
  _sp1->set_used(false);
  _sp2->set_used(false);
  _sp3->set_used(false);
  _sp4->set_used(false);
  _sp5->set_used(false);

  spnts_by_station.clear();
  spnts_by_station.resize(0);
  spnts_by_station.resize(n_stations);
  pr.sort_by_station(spnts, spnts_by_station);
  strks.resize(0);
  evt.set_straightprtrack(strks);
  pr.make_all_tracks(track_type, tracker_num, spnts_by_station, evt);
  strks = evt.straightprtracks();
  htrks = evt.helicalprtracks();

  // Check it matches to within a tolerance
  EXPECT_EQ(1, strks.size());
  EXPECT_EQ(0, htrks.size());
  EXPECT_EQ(5, strks[0].get_num_points());
  EXPECT_NEAR(x0, strks[0].get_x0(), 1);
  EXPECT_NEAR(mx, strks[0].get_mx(), 0.001);
  EXPECT_NEAR(x_chisq, strks[0].get_x_chisq(), 0.1);
  EXPECT_NEAR(y0, strks[0].get_y0(), 1);
  EXPECT_NEAR(my, strks[0].get_my(), 0.001);
  EXPECT_NEAR(y_chisq, strks[0].get_y_chisq(), 0.1);
}

TEST_F(PatternRecognitionTest, test_make_4pt_tracks) {

  // Set up the spacepoints vector
  set_up_spacepoints();
  std::vector<SciFiSpacePoint*> spnts;
  spnts.push_back(_sp5);
  spnts.push_back(_sp3);
  spnts.push_back(_sp2);
  spnts.push_back(_sp1);

  PatternRecognition pr;
  int n_stations = 5;

  // Set up the spacepoints by station 2D vector
  std::vector< std::vector<SciFiSpacePoint*> > spnts_by_station(n_stations);
  pr.sort_by_station(spnts, spnts_by_station);

  SciFiEvent evt;
  bool track_type = 0; // Straight tracks

  // The track parameters that should be reconstructed from the spacepoints in 5 pt track case
  int num_points = 4;
  double y0 = -58.85201389;
  double x0 = -68.94108927;
  double my = 0.03755825;
  double mx = -0.02902014;
  std::vector<SciFiHelicalPRTrack> htrks;
  std::vector<SciFiStraightPRTrack> strks;

  // Make a 4 point track with 4 spacepoints
  // ---------------------------------------
  _sp1->set_used(false);
  _sp2->set_used(false);
  _sp3->set_used(false);
  _sp5->set_used(false);

  spnts_by_station.clear();
  spnts_by_station.resize(0);
  spnts_by_station.resize(n_stations);
  pr.sort_by_station(spnts, spnts_by_station);
  strks.resize(0);

  pr.make_4tracks(track_type, spnts_by_station, strks, htrks);

  // Check it matches to within a tolerance
  EXPECT_EQ(1, strks.size());
  EXPECT_EQ(0, htrks.size());
  EXPECT_EQ(num_points, strks[0].get_num_points());
  EXPECT_NEAR(x0, strks[0].get_x0(), 3);
  EXPECT_NEAR(mx, strks[0].get_mx(), 0.002);
  EXPECT_NEAR(17.5, strks[0].get_x_chisq(), 0.1);
  EXPECT_NEAR(y0, strks[0].get_y0(), 3);
  EXPECT_NEAR(my, strks[0].get_my(), 0.005);
  EXPECT_NEAR(15.9, strks[0].get_y_chisq(), 0.1);

    // Make a 4 point track with 5 spacepoints
  // ---------------------------------------
  spnts.push_back(_sp4);
  _sp1->set_used(false);
  _sp2->set_used(false);
  _sp3->set_used(false);
  _sp4->set_used(false);
  _sp5->set_used(false);

  spnts_by_station.clear();
  spnts_by_station.resize(0);
  spnts_by_station.resize(n_stations);
  pr.sort_by_station(spnts, spnts_by_station);
  strks.resize(0);

  pr.make_4tracks(track_type, spnts_by_station, strks, htrks);

  // Check it matches to within a tolerance
  EXPECT_EQ(1, strks.size());
  EXPECT_EQ(0, htrks.size());
  EXPECT_EQ(num_points, strks[0].get_num_points());
  EXPECT_NEAR(x0, strks[0].get_x0(), 3);
  EXPECT_NEAR(mx, strks[0].get_mx(), 0.01); // Needed a wider tolerance than the others
  EXPECT_NEAR(16.3, strks[0].get_x_chisq(), 0.1);
  EXPECT_NEAR(y0, strks[0].get_y0(), 3);
  EXPECT_NEAR(my, strks[0].get_my(), 0.005);
  EXPECT_NEAR(4.5, strks[0].get_y_chisq(), 0.1);
}

TEST_F(PatternRecognitionTest, test_make_3pt_tracks) {

  // Set up the spacepoints vector
  set_up_spacepoints();
  std::vector<SciFiSpacePoint*> spnts;
  spnts.push_back(_sp5);
  spnts.push_back(_sp2);
  spnts.push_back(_sp1);

  PatternRecognition pr;
  int n_stations = 5;

  // Set up the spacepoints by station 2D vector
  std::vector< std::vector<SciFiSpacePoint*> > spnts_by_station(n_stations);
  pr.sort_by_station(spnts, spnts_by_station);

  SciFiEvent evt;
  bool track_type = 0; // Straight tracks

  // The track parameters that should be reconstructed from the spacepoints in 5 pt track case
  int num_points = 3;
  double y0 = -58.85201389;
  double x0 = -68.94108927;
  double my = 0.03755825;
  double mx = -0.02902014;
  std::vector<SciFiHelicalPRTrack> htrks;
  std::vector<SciFiStraightPRTrack> strks;

  // Make a 3 point track with 3 spacepoints
  // ---------------------------------------
  pr.make_3tracks(track_type, spnts_by_station, strks, htrks);

  // Check it matches to within a tolerance
  EXPECT_EQ(1, strks.size());
  EXPECT_EQ(0, htrks.size());
  EXPECT_EQ(num_points, strks[0].get_num_points());
  EXPECT_NEAR(x0, strks[0].get_x0(), 3);
  EXPECT_NEAR(mx, strks[0].get_mx(), 0.002);
  EXPECT_NEAR(0.9, strks[0].get_x_chisq(), 0.1);
  EXPECT_NEAR(y0, strks[0].get_y0(), 3);
  EXPECT_NEAR(my, strks[0].get_my(), 0.005);
  EXPECT_NEAR(13.3, strks[0].get_y_chisq(), 0.1);

  // Make a 3 point track with 4 spacepoints
  // ---------------------------------------
  spnts.push_back(_sp3);
  _sp1->set_used(false);
  _sp2->set_used(false);
  _sp3->set_used(false);
  _sp5->set_used(false);

  spnts_by_station.clear();
  spnts_by_station.resize(0);
  spnts_by_station.resize(n_stations);
  pr.sort_by_station(spnts, spnts_by_station);
  strks.resize(0);

  pr.make_3tracks(track_type, spnts_by_station, strks, htrks);

  // Check it matches to within a tolerance
  EXPECT_EQ(1, strks.size());
  EXPECT_EQ(0, htrks.size());
  EXPECT_EQ(num_points, strks[0].get_num_points());
  EXPECT_NEAR(x0, strks[0].get_x0(), 3);
  EXPECT_NEAR(mx, strks[0].get_mx(), 0.002);
  EXPECT_NEAR(11.3, strks[0].get_x_chisq(), 0.1);
  EXPECT_NEAR(y0, strks[0].get_y0(), 3);
  EXPECT_NEAR(my, strks[0].get_my(), 0.005);
  EXPECT_NEAR(0.0015, strks[0].get_y_chisq(), 0.001);

    // Make a 3 point track with 5 spacepoints
  // ---------------------------------------
  spnts.push_back(_sp4);
  _sp1->set_used(false);
  _sp2->set_used(false);
  _sp3->set_used(false);
  _sp4->set_used(false);
  _sp5->set_used(false);

  spnts_by_station.clear();
  spnts_by_station.resize(0);
  spnts_by_station.resize(n_stations);
  pr.sort_by_station(spnts, spnts_by_station);
  strks.resize(0);

  pr.make_3tracks(track_type, spnts_by_station, strks, htrks);

  // Check it matches to within a tolerance
  EXPECT_EQ(1, strks.size());
  EXPECT_EQ(0, htrks.size());
  EXPECT_EQ(num_points, strks[0].get_num_points());
  EXPECT_NEAR(x0, strks[0].get_x0(), 3);
  EXPECT_NEAR(mx, strks[0].get_mx(), 0.01); // Needed a wider tolerance than the others
  EXPECT_NEAR(1.8, strks[0].get_x_chisq(), 0.1);
  EXPECT_NEAR(y0, strks[0].get_y0(), 3);
  EXPECT_NEAR(my, strks[0].get_my(), 0.005);
  EXPECT_NEAR(4.2, strks[0].get_y_chisq(), 0.1);
}

TEST_F(PatternRecognitionTest, test_make_straight_tracks) {

  int n_stations = 5;
  PatternRecognition pr;

  // Set up the spacepoints vector
  set_up_spacepoints();
  std::vector<SciFiSpacePoint*> spnts;
  spnts.push_back(_sp5);
  spnts.push_back(_sp2);
  spnts.push_back(_sp3);
  spnts.push_back(_sp1);
  spnts.push_back(_sp4);

  // Set up the spacepoints by station 2D vector
  std::vector< std::vector<SciFiSpacePoint*> > spnts_by_station(n_stations);
  pr.sort_by_station(spnts, spnts_by_station);

  // Check the spacepoints have setup correctly
  EXPECT_EQ(_sp1, spnts_by_station[0][0]);
  EXPECT_EQ(_sp2, spnts_by_station[1][0]);
  EXPECT_EQ(_sp3, spnts_by_station[2][0]);
  EXPECT_EQ(_sp4, spnts_by_station[3][0]);
  EXPECT_EQ(_sp5, spnts_by_station[4][0]);
  EXPECT_EQ(-68.24883333333334, spnts_by_station[0][0]->get_position().x());

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
  EXPECT_NEAR(x0, strks[0].get_x0(), epsilon);
  EXPECT_NEAR(mx, strks[0].get_mx(), epsilon);
  EXPECT_NEAR(x_chisq, strks[0].get_x_chisq(), epsilon);
  EXPECT_NEAR(y0, strks[0].get_y0(), epsilon);
  EXPECT_NEAR(my, strks[0].get_my(), epsilon);
  EXPECT_NEAR(y_chisq, strks[0].get_y_chisq(), epsilon);
}

TEST_F(PatternRecognitionTest, test_set_ignore_stations) {

  PatternRecognition pr;
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

TEST_F(PatternRecognitionTest, test_stations_with_unused_sp) {

  // Set up spacepoints, leaving station 3 empty to check function copes with an empty station
  SciFiSpacePoint *sp1 = new SciFiSpacePoint();
  SciFiSpacePoint *sp2 = new SciFiSpacePoint();
  // SciFiSpacePoint *sp3 = new SciFiSpacePoint();
  SciFiSpacePoint *sp4 = new SciFiSpacePoint();
  SciFiSpacePoint *sp4_1 = new SciFiSpacePoint();
  SciFiSpacePoint *sp5 = new SciFiSpacePoint();

  sp1->set_station(1);
  sp2->set_station(2);
  // sp3->set_station(3);
  sp4->set_station(4);
  sp4_1->set_station(4);
  sp5->set_station(5);

  sp1->set_used(true);
  sp2->set_used(true);
  // sp3->set_used(true);
  sp4->set_used(false);
  sp4_1->set_used(true);
  sp5->set_used(false);

  std::vector<SciFiSpacePoint*> spnts;
  spnts.push_back(sp5);
  spnts.push_back(sp2);
  // spnts.push_back(sp3);
  spnts.push_back(sp1);
  spnts.push_back(sp4);
  spnts.push_back(sp4_1);

  SpacePoint2dPArray spnts_by_station(5);

  PatternRecognition pr;
  pr.sort_by_station(spnts, spnts_by_station);
  ASSERT_EQ(5, spnts_by_station.size());
  ASSERT_EQ(1, spnts_by_station[0].size());
  ASSERT_EQ(1, spnts_by_station[1].size());
  ASSERT_EQ(2, spnts_by_station[3].size());
  ASSERT_EQ(1, spnts_by_station[4].size());

  std::vector<int> stations_hit, stations_not_hit;
  pr.stations_with_unused_spnts(spnts_by_station, stations_hit, stations_not_hit);

  ASSERT_EQ(2, stations_hit.size());
  ASSERT_EQ(3, stations_not_hit.size());
  EXPECT_EQ(3, stations_hit[0]);
  EXPECT_EQ(4, stations_hit[1]);
  EXPECT_EQ(0, stations_not_hit[0]);
  EXPECT_EQ(1, stations_not_hit[1]);
  EXPECT_EQ(2, stations_not_hit[2]);

  int stats_with_unused = pr.num_stations_with_unused_spnts(spnts_by_station);
  EXPECT_EQ(2, stats_with_unused);
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

TEST_F(PatternRecognitionTest, test_AB_ratio) {

  PatternRecognition pr;
  double phi_i = 1.0;
  double phi_j = 0.5;
  double z_i = 200.0;
  double z_j = 450.0;

  double epsilon = 0.01;

  bool result = pr.AB_ratio(phi_i, phi_j, z_i, z_j);
  ASSERT_TRUE(result);
  EXPECT_NEAR(phi_i, 7.28319, epsilon);
  EXPECT_NEAR(phi_j, 6.783, epsilon);
  EXPECT_EQ(z_i, 200.0);
  EXPECT_EQ(z_j, 450.0);
}

/*
TEST_F(PatternRecognitionTest, test_process_bad) {

  PatternRecognition pr;

  // Now give it random spacepoints and check it produces no tracks
  SciFiSpacePoint *sp1 = new SciFiSpacePoint();
  SciFiSpacePoint *sp2 = new SciFiSpacePoint();
  SciFiSpacePoint *sp3 = new SciFiSpacePoint();
  SciFiSpacePoint *sp4 = new SciFiSpacePoint();
  SciFiSpacePoint *sp5 = new SciFiSpacePoint();

  ThreeVector pos(-50.0, -50.0, -0.652299999999741);
  sp1->set_position(pos);
  sp1->set_tracker(0);
  sp1->set_station(1);
  sp1->set_type("triplet");

  pos.set(-20, -70, -200.6168999999991);
  sp2->set_position(pos);
  sp2->set_tracker(0);
  sp2->set_station(2);
  sp2->set_type("triplet");

  pos.set(40.0, -80, -450.4798999999994);
  sp3->set_position(pos);
  sp3->set_tracker(0);
  sp3->set_station(3);
  sp3->set_type("triplet");

  pos.set(-10.0, 90, -750.4801999999991);
  sp4->set_position(pos);
  sp4->set_tracker(0);
  sp4->set_station(4);
  sp4->set_type("triplet");

  pos.set(1.0, 20.0, -1100.410099999999);
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

  SciFiHelicalPRTrack htrk = htrks[0];
  std::cerr << " x0 is " << htrk.get_x0() << std::endl;
  std::cerr << " y0 is " << htrk.get_y0() << std::endl;
  std::cerr << " z0 is " << htrk.get_z0() << std::endl;
  std::cerr << " phi0 is " << htrk.get_phi0() << std::endl;
  std::cerr << " psi0 is " << htrk.get_psi0() << std::endl;
  std::cerr << " ds/dz is " << htrk.get_dsdz() << std::endl;
  std::cerr << " R is " << htrk.get_R() << std::endl;
  std::cerr << " line_sz_chi2 is " << htrk.get_line_sz_chisq() << std::endl;
  std::cerr << " circle_x0 is " << htrk.get_circle_x0() << std::endl;
  std::cerr << " circle_y0 is " << htrk.get_circle_y0() << std::endl;
  std::cerr << " circle_chi2 is " << htrk.get_circle_chisq() << std::endl;
  std::cerr << " chi2 is " << htrk.get_chisq() << std::endl;
  std::cerr << " chi2_dof is " << htrk.get_chisq_dof() << std::endl;
  std::cerr << " num_points is " << htrk.get_num_points() << std::endl;

  EXPECT_EQ(0, strks.size());
  EXPECT_EQ(0, htrks.size());
}
*/


} // ~namespace MAUS
