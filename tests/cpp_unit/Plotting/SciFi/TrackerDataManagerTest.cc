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

// C++ vector
#include <vector>

// ROOT headers
#include "TArc.h"
#include "TF1.h"

// Google test headers
#include "gtest/gtest.h"
#include "gtest/gtest_prod.h"

// MAUS headers
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/DataStructure/ReconEvent.hh"
#include "src/common_cpp/DataStructure/SciFiDigit.hh"
#include "src/common_cpp/DataStructure/SciFiCluster.hh"
#include "src/common_cpp/DataStructure/SciFiSpacePoint.hh"
#include "src/common_cpp/DataStructure/SciFiStraightPRTrack.hh"
#include "src/common_cpp/DataStructure/SciFiHelicalPRTrack.hh"
#include "src/common_cpp/Plotting/SciFi/TrackerDataManager.hh"
#include "src/common_cpp/Plotting/SciFi/TrackerDataPlotterInfoBox.hh"

namespace MAUS {

class TrackerDataManagerTest : public ::testing::Test {
 protected:
  TrackerDataManagerTest()  {}
  virtual ~TrackerDataManagerTest() {}
  virtual void SetUp()    {}
  virtual void TearDown() {}
};

TEST_F(TrackerDataManagerTest, TestConstructor) {
  TrackerDataManager tdm;
  EXPECT_FALSE(tdm._print_tracks);
  EXPECT_FALSE(tdm._print_seeds);
}

TEST_F(TrackerDataManagerTest, TestDraw) {
  // Set up and use an infoBox plotter to test the draw method
  TrackerDataManager tdm;
  TrackerDataPlotterInfoBox * infoBox = new TrackerDataPlotterInfoBox();

  // Check that the infoBox is not reading as setup first
  ASSERT_FALSE(infoBox->_setup_true);

  // Now draw with the data manager
  std::vector<TrackerDataPlotterBase*> plotters;
  plotters.push_back(infoBox);
  tdm.draw(plotters);

  // Check the infoBox has now been initialised
  EXPECT_TRUE(infoBox->_setup_true);
}

TEST_F(TrackerDataManagerTest, TestProcess) {

  // Some setup
  TrackerDataManager tdm;
  Spill* spill = new Spill();
  spill->SetDaqEventType("physics_event");
  spill->SetSpillNumber(1);
  std::vector<ReconEvent*>* revts = new std::vector<ReconEvent*>;
  ReconEvent* revt =  new ReconEvent();
  revts->push_back(revt);
  SciFiEvent* sfevt = new SciFiEvent();
  revt->SetSciFiEvent(sfevt);
  spill->SetReconEvents(revts);

  // Set up some spacepoints
  std::vector<SciFiSpacePoint*> spnts;
  std::vector<SciFiSpacePoint*> spnts_up;
  std::vector<SciFiSpacePoint*> spnts_down;
  for (int i = 0; i < 5; ++i) {
    SciFiSpacePoint *sp = new SciFiSpacePoint();
    sp->set_tracker(0);
    spnts.push_back(sp);
    spnts_up.push_back(sp);
  }
  for (int i = 0; i < 5; ++i) {
    SciFiSpacePoint *sp = new SciFiSpacePoint();
    sp->set_tracker(1);
    spnts.push_back(sp);
    spnts_down.push_back(sp);
  }
  sfevt->set_spacepoints(spnts);

  // Set up some straight and helical Pattern Recogntion tracks
  std::vector<SciFiHelicalPRTrack*> htrks;
  for (int i = 0; i < 6; ++i) {
    SciFiHelicalPRTrack* trk = new SciFiHelicalPRTrack();
    if (i < 3) {
      trk->set_tracker(0);
      trk->set_spacepoints_pointers(spnts_up);
    } else {
      trk->set_tracker(1);
      trk->set_spacepoints_pointers(spnts_down);
    }
    htrks.push_back(trk);
  }
  sfevt->set_helicalprtrack(htrks);

  // Process the spill
  tdm.process(spill);

  // Check some of the data manager internal state is correct
  EXPECT_EQ(1, tdm._t1._spill_num);
  EXPECT_EQ(1, tdm._t2._spill_num);
  EXPECT_EQ(5, tdm._t1._num_spoints);
  EXPECT_EQ(5, tdm._t2._num_spoints);
  EXPECT_EQ(3, tdm._t1._num_htracks_5pt);
  EXPECT_EQ(0, tdm._t1._num_htracks_4pt);
  EXPECT_EQ(0, tdm._t1._num_htracks_3pt);
  EXPECT_EQ(3, tdm._t2._num_htracks_5pt);
  EXPECT_EQ(0, tdm._t2._num_htracks_4pt);
  EXPECT_EQ(0, tdm._t2._num_htracks_3pt);

  // Tidy up. Deleting spill should deallocated all other memory.
  delete spill;
}

TEST_F(TrackerDataManagerTest, TestProcessClusters) {
  TrackerDataManager tdm;
  std::vector<SciFiCluster*> clusters;
  SciFiCluster* clus1 = new SciFiCluster();
  SciFiCluster* clus2 = new SciFiCluster();
  clus1->set_tracker(0);
  clus2->set_tracker(1);
  clusters.push_back(clus1);
  clusters.push_back(clus2);
  tdm.process_clusters(clusters);
  EXPECT_EQ(1, tdm._t1._num_clusters);
  EXPECT_EQ(1, tdm._t2._num_clusters);

  delete clus1;
  delete clus2;
}

TEST_F(TrackerDataManagerTest, TestProcessDigits) {
  TrackerDataManager tdm;

  Spill* spill = new Spill();
  ReconEvent* revt = NULL;
  ReconEventPArray* revts = new ReconEventPArray();
  revts->push_back(revt);
  spill->SetReconEvents(revts);

  std::vector<SciFiDigit*> digits;
  SciFiDigit* dig1 = new SciFiDigit();
  SciFiDigit* dig2 = new SciFiDigit();
  dig1->set_tracker(0);
  dig2->set_tracker(1);
  digits.push_back(dig1);
  digits.push_back(dig2);

  tdm.process_digits(spill, digits);
  EXPECT_EQ(1, tdm._t1._num_events);
  EXPECT_EQ(1, tdm._t2._num_events);
  EXPECT_EQ(1, tdm._t1._num_digits);
  EXPECT_EQ(1, tdm._t2._num_digits);

  delete dig1;
  delete dig2;
  delete spill;
}

TEST_F(TrackerDataManagerTest, TestProcessHtrks) {
  TrackerDataManager tdm;
  tdm.set_print_seeds(false);

  // Set up some tracks
  std::vector<SciFiHelicalPRTrack*> trks;
  for (int i = 0; i < 6; ++i) {
    SciFiHelicalPRTrack* trk = new SciFiHelicalPRTrack();
    trks.push_back(trk);
  }

  std::vector<SciFiSpacePoint*> spnts;
  std::vector<SciFiSpacePoint*> spnts_up_5;
  std::vector<SciFiSpacePoint*> spnts_up_4;
  std::vector<SciFiSpacePoint*> spnts_up_3;
  std::vector<SciFiSpacePoint*> spnts_down_5;
  std::vector<SciFiSpacePoint*> spnts_down_4;
  std::vector<SciFiSpacePoint*> spnts_down_3;

  for (int i = 0; i < 5; ++i) {
    SciFiSpacePoint *sp = new SciFiSpacePoint();
    sp->set_tracker(0);
    spnts.push_back(sp);
    spnts_up_5.push_back(sp);
  }
  for (int i = 0; i < 4; ++i) {
    SciFiSpacePoint *sp = new SciFiSpacePoint();
    sp->set_tracker(0);
    spnts.push_back(sp);
    spnts_up_4.push_back(sp);
  }
  for (int i = 0; i < 3; ++i) {
    SciFiSpacePoint *sp = new SciFiSpacePoint();
    sp->set_tracker(0);
    spnts.push_back(sp);
    spnts_up_3.push_back(sp);
  }

  for (int i = 0; i < 5; ++i) {
    SciFiSpacePoint *sp = new SciFiSpacePoint();
    sp->set_tracker(1);
    spnts.push_back(sp);
    spnts_down_5.push_back(sp);
  }
  for (int i = 0; i < 4; ++i) {
    SciFiSpacePoint *sp = new SciFiSpacePoint();
    sp->set_tracker(1);
    spnts.push_back(sp);
    spnts_down_4.push_back(sp);
  }
  for (int i = 0; i < 3; ++i) {
    SciFiSpacePoint *sp = new SciFiSpacePoint();
    sp->set_tracker(1);
    spnts.push_back(sp);
    spnts_down_3.push_back(sp);
  }

  trks[0]->set_spacepoints_pointers(spnts_up_5);
  trks[1]->set_spacepoints_pointers(spnts_up_4);
  trks[2]->set_spacepoints_pointers(spnts_up_3);
  trks[3]->set_spacepoints_pointers(spnts_down_5);
  trks[4]->set_spacepoints_pointers(spnts_down_4);
  trks[5]->set_spacepoints_pointers(spnts_down_3);

  trks[0]->set_tracker(0);
  trks[1]->set_tracker(0);
  trks[2]->set_tracker(0);
  trks[3]->set_tracker(1);
  trks[4]->set_tracker(1);
  trks[5]->set_tracker(1);

  std::vector<double> phi_i;
  phi_i.push_back(1.0);
  double x0 = 1.0;
  double y0 = 1.0;
  double rad = 2.0;
  double dsdz = 1.0;
  double sz_c = 1.0;
  int handness = -1;

  trks[0]->set_phi(phi_i);
  trks[0]->set_R(rad);
  trks[0]->set_circle_x0(x0);
  trks[0]->set_circle_y0(y0);
  trks[0]->set_dsdz(dsdz);
  trks[0]->set_line_sz_c(sz_c);
  trks[3]->set_phi(phi_i);
  trks[3]->set_R(rad);
  trks[3]->set_circle_x0(x0);
  trks[3]->set_circle_y0(y0);
  trks[3]->set_dsdz(dsdz);
  trks[3]->set_line_sz_c(sz_c);

  // Run them through the function and check the output
  tdm.process_htrks(trks);

  // Check the spill totals
  EXPECT_EQ(1, tdm._t1._num_htracks_5pt);
  EXPECT_EQ(1, tdm._t1._num_htracks_4pt);
  EXPECT_EQ(1, tdm._t1._num_htracks_3pt);
  EXPECT_EQ(1, tdm._t2._num_htracks_5pt);
  EXPECT_EQ(1, tdm._t2._num_htracks_4pt);
  EXPECT_EQ(1, tdm._t2._num_htracks_3pt);
  EXPECT_EQ(phi_i[0], tdm._t1._seeds_phi[0][0]);
  EXPECT_NEAR(phi_i[0]*rad, tdm._t2._seeds_s[0][0], 0.01);
  EXPECT_EQ(phi_i[0], tdm._t2._seeds_phi[0][0]);
  EXPECT_NEAR(phi_i[0]*rad, tdm._t1._seeds_s[0][0], 0.01);
  EXPECT_EQ(12, tdm._t1._num_seeds);
  EXPECT_EQ(12, tdm._t2._num_seeds);

  // Check the first created x-y projections
  TArc arc1 = tdm._t1._trks_xy[0];
  EXPECT_EQ(arc1.GetR1(), rad);
  EXPECT_EQ(arc1.GetX1(), x0);
  EXPECT_EQ(arc1.GetY1(), y0);
  TArc arc2 = tdm._t2._trks_xy[0];
  EXPECT_EQ(arc2.GetR1(), rad);
  EXPECT_EQ(arc2.GetX1(), x0);
  EXPECT_EQ(arc2.GetY1(), y0);

  // Check the first created z-x projections
  TF1 xz1 = tdm._t1._trks_xz[0];
  EXPECT_EQ(x0, xz1.GetParameter(0));
  EXPECT_EQ(rad, xz1.GetParameter(1));
  EXPECT_EQ(dsdz, xz1.GetParameter(2));
  EXPECT_EQ(sz_c, xz1.GetParameter(3));
  EXPECT_EQ(handness, xz1.GetParameter(4));
  TF1 xz2 = tdm._t1._trks_xz[0];
  EXPECT_EQ(x0, xz2.GetParameter(0));
  EXPECT_EQ(rad, xz2.GetParameter(1));
  EXPECT_EQ(dsdz, xz2.GetParameter(2));
  EXPECT_EQ(sz_c, xz2.GetParameter(3));
  EXPECT_EQ(handness, xz2.GetParameter(4));

  // Check the first created z-y projections
  TF1 yz1 = tdm._t1._trks_yz[0];
  EXPECT_EQ(y0, yz1.GetParameter(0));
  EXPECT_EQ(rad, yz1.GetParameter(1));
  EXPECT_EQ(dsdz, yz1.GetParameter(2));
  EXPECT_EQ(sz_c, yz1.GetParameter(3));
  TF1 yz2 = tdm._t1._trks_yz[0];
  EXPECT_EQ(y0, yz2.GetParameter(0));
  EXPECT_EQ(rad, yz2.GetParameter(1));
  EXPECT_EQ(dsdz, yz2.GetParameter(2));
  EXPECT_EQ(sz_c, yz2.GetParameter(3));

  // Tidy up
  for (int i = 0; i < spnts.size(); ++i) {
    delete spnts[i];
  }
  for (int i = 0; i < 6; ++i) {
    delete trks[i];
  }
}

TEST_F(TrackerDataManagerTest, TestProcessSpoints) {

  // Some setup
  TrackerDataManager tdm;
  SciFiSpacePoint *sp1 = new SciFiSpacePoint();
  SciFiSpacePoint *sp2 = new SciFiSpacePoint();
  ThreeVector pos1(1.0, 2.0, 3.0);
  ThreeVector pos2(4.0, 5.0, 6.0);
  sp1->set_tracker(0);
  sp2->set_tracker(1);
  sp1->set_position(pos1);
  sp2->set_position(pos2);
  std::vector<SciFiSpacePoint*> spnts;
  spnts.push_back(sp1);
  spnts.push_back(sp2);

  // Process the spoints
  tdm.process_spoints(spnts);

  // Check the data manager internal state correctly matches
  EXPECT_EQ(1, tdm._t1._num_spoints);
  EXPECT_EQ(pos1.x(), tdm._t1._spoints_x[0]);
  EXPECT_EQ(pos1.y(), tdm._t1._spoints_y[0]);
  EXPECT_EQ(pos1.z(), tdm._t1._spoints_z[0]);
  EXPECT_EQ(1, tdm._t2._num_spoints);
  EXPECT_EQ(pos2.x(), tdm._t2._spoints_x[0]);
  EXPECT_EQ(pos2.y(), tdm._t2._spoints_y[0]);
  EXPECT_EQ(pos2.z(), tdm._t2._spoints_z[0]);

  delete sp1;
  delete sp2;
}


TEST_F(TrackerDataManagerTest, TestProcessStrks) {
  TrackerDataManager tdm;

  // Set up some tracks
  std::vector<SciFiStraightPRTrack*> trks;
  for (int i = 0; i < 6; ++i) {
    SciFiStraightPRTrack* trk = new SciFiStraightPRTrack();
    trks.push_back(trk);
  }

  std::vector<SciFiSpacePoint*> spnts;
  std::vector<SciFiSpacePoint*> spnts_up_5;
  std::vector<SciFiSpacePoint*> spnts_up_4;
  std::vector<SciFiSpacePoint*> spnts_up_3;
  std::vector<SciFiSpacePoint*> spnts_down_5;
  std::vector<SciFiSpacePoint*> spnts_down_4;
  std::vector<SciFiSpacePoint*> spnts_down_3;

  for (int i = 0; i < 5; ++i) {
    SciFiSpacePoint *sp = new SciFiSpacePoint();
    sp->set_tracker(0);
    spnts.push_back(sp);
    spnts_up_5.push_back(sp);
  }
  for (int i = 0; i < 4; ++i) {
    SciFiSpacePoint *sp = new SciFiSpacePoint();
    sp->set_tracker(0);
    spnts.push_back(sp);
    spnts_up_4.push_back(sp);
  }
  for (int i = 0; i < 3; ++i) {
    SciFiSpacePoint *sp = new SciFiSpacePoint();
    sp->set_tracker(0);
    spnts.push_back(sp);
    spnts_up_3.push_back(sp);
  }

  for (int i = 0; i < 5; ++i) {
    SciFiSpacePoint *sp = new SciFiSpacePoint();
    sp->set_tracker(1);
    spnts.push_back(sp);
    spnts_down_5.push_back(sp);
  }
  for (int i = 0; i < 4; ++i) {
    SciFiSpacePoint *sp = new SciFiSpacePoint();
    sp->set_tracker(1);
    spnts.push_back(sp);
    spnts_down_4.push_back(sp);
  }
  for (int i = 0; i < 3; ++i) {
    SciFiSpacePoint *sp = new SciFiSpacePoint();
    sp->set_tracker(1);
    spnts.push_back(sp);
    spnts_down_3.push_back(sp);
  }

  trks[0]->set_spacepoints_pointers(spnts_up_5);
  trks[1]->set_spacepoints_pointers(spnts_up_4);
  trks[2]->set_spacepoints_pointers(spnts_up_3);
  trks[3]->set_spacepoints_pointers(spnts_down_5);
  trks[4]->set_spacepoints_pointers(spnts_down_4);
  trks[5]->set_spacepoints_pointers(spnts_down_3);

  trks[0]->set_tracker(0);
  trks[1]->set_tracker(0);
  trks[2]->set_tracker(0);
  trks[3]->set_tracker(1);
  trks[4]->set_tracker(1);
  trks[5]->set_tracker(1);

  // Set up some tracks
  double x0_1 = 1.0;
  double mx_1 = 2.0;
  double y0_1 = 3.0;
  double my_1 = 4.0;
  double x0_2 = 5.0;
  double mx_2 = 6.0;
  double y0_2 = 7.0;
  double my_2 = 8.0;

  // Only bother to more fully initialise the first track in each tracker
  trks[0]->set_x0(x0_1);
  trks[0]->set_mx(mx_1);
  trks[0]->set_y0(y0_1);
  trks[0]->set_my(my_1);
  trks[3]->set_x0(x0_2);
  trks[3]->set_mx(mx_2);
  trks[3]->set_y0(y0_2);
  trks[3]->set_my(my_2);

  // Run them through the function and check the output
  tdm.process_strks(trks);

  EXPECT_EQ(5, trks[0]->get_num_points());
  EXPECT_EQ(4, trks[1]->get_num_points());
  EXPECT_EQ(3, trks[2]->get_num_points());
  EXPECT_EQ(5, trks[3]->get_num_points());
  EXPECT_EQ(4, trks[4]->get_num_points());
  EXPECT_EQ(3, trks[5]->get_num_points());

  // Check the spill totals
  EXPECT_EQ(1, tdm._t1._num_stracks_5pt);
  EXPECT_EQ(1, tdm._t1._num_stracks_4pt);
  EXPECT_EQ(1, tdm._t1._num_stracks_3pt);
  EXPECT_EQ(1, tdm._t2._num_stracks_5pt);
  EXPECT_EQ(1, tdm._t2._num_stracks_4pt);
  EXPECT_EQ(1, tdm._t2._num_stracks_3pt);

  // Check the first created z-x projections
  TF1 xz1 = tdm._t1._trks_str_xz[0];
  EXPECT_EQ(x0_1, xz1.GetParameter(0));
  EXPECT_EQ(mx_1, xz1.GetParameter(1));

  TF1 xz2 = tdm._t2._trks_str_xz[0];
  EXPECT_EQ(x0_2, xz2.GetParameter(0));
  EXPECT_EQ(mx_2, xz2.GetParameter(1));

  // Check the first created z-y projections
  TF1 yz1 = tdm._t1._trks_str_yz[0];
  EXPECT_EQ(y0_1, yz1.GetParameter(0));
  EXPECT_EQ(my_1, yz1.GetParameter(1));

  TF1 yz2 = tdm._t2._trks_str_yz[0];
  EXPECT_EQ(y0_2, yz2.GetParameter(0));
  EXPECT_EQ(my_2, yz2.GetParameter(1));

  // Tidy up
  for (int i = 0; i < spnts.size(); ++i) {
    delete spnts[i];
  }
  for (int i = 0; i < 6; ++i) {
    delete trks[i];
  }
}

} // ~namespace MAUS
