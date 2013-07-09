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
#include "src/common_cpp/Recon/SciFi/TrackerDataManager.hh"

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
  EXPECT_TRUE(tdm._print_tracks);
  EXPECT_TRUE(tdm._print_seeds);
  EXPECT_EQ(0, tdm._t1_sp);
  EXPECT_EQ(0, tdm._t2_sp);
  EXPECT_EQ(0, tdm._t1_seeds);
  EXPECT_EQ(0, tdm._t2_seeds);
  EXPECT_EQ(0, tdm._t1_5pt_strks);
  EXPECT_EQ(0, tdm._t1_4pt_strks);
  EXPECT_EQ(0, tdm._t1_3pt_strks);
  EXPECT_EQ(0, tdm._t2_5pt_strks);
  EXPECT_EQ(0, tdm._t2_4pt_strks);
  EXPECT_EQ(0, tdm._t2_3pt_strks);
  EXPECT_EQ(0, tdm._t1_5pt_htrks);
  EXPECT_EQ(0, tdm._t1_4pt_htrks);
  EXPECT_EQ(0, tdm._t1_3pt_htrks);
  EXPECT_EQ(0, tdm._t2_5pt_htrks);
  EXPECT_EQ(0, tdm._t2_4pt_htrks);
  EXPECT_EQ(0, tdm._t2_3pt_htrks);
}

TEST_F(TrackerDataManagerTest, TestClear) {
  TrackerDataManager tdm;

  tdm._t1_sp = 1;
  tdm._t2_sp = 1;
  tdm._t1_seeds = 1;
  tdm._t2_seeds = 1;
  tdm._t1_5pt_strks = 1;
  tdm._t1_4pt_strks = 1;
  tdm._t1_3pt_strks = 1;
  tdm._t2_5pt_strks = 1;
  tdm._t2_4pt_strks = 1;
  tdm._t2_3pt_strks = 1;
  tdm._t1_5pt_htrks = 1;
  tdm._t1_4pt_htrks = 1;
  tdm._t1_3pt_htrks = 1;
  tdm._t2_5pt_htrks = 1;
  tdm._t2_4pt_htrks = 1;
  tdm._t2_3pt_htrks = 1;

  tdm.clear_run();

  EXPECT_EQ(0, tdm._t1_sp);
  EXPECT_EQ(0, tdm._t2_sp);
  EXPECT_EQ(0, tdm._t1_seeds);
  EXPECT_EQ(0, tdm._t2_seeds);
  EXPECT_EQ(0, tdm._t1_5pt_strks);
  EXPECT_EQ(0, tdm._t1_4pt_strks);
  EXPECT_EQ(0, tdm._t1_3pt_strks);
  EXPECT_EQ(0, tdm._t2_5pt_strks);
  EXPECT_EQ(0, tdm._t2_4pt_strks);
  EXPECT_EQ(0, tdm._t2_3pt_strks);
  EXPECT_EQ(0, tdm._t1_5pt_htrks);
  EXPECT_EQ(0, tdm._t1_4pt_htrks);
  EXPECT_EQ(0, tdm._t1_3pt_htrks);
  EXPECT_EQ(0, tdm._t2_5pt_htrks);
  EXPECT_EQ(0, tdm._t2_4pt_htrks);
  EXPECT_EQ(0, tdm._t2_3pt_htrks);
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
  ReconEventArray* revts = new ReconEventArray();
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

  // Set up some tracks
  std::vector<SciFiHelicalPRTrack*> trks;
  for (int i = 0; i < 6; ++i) {
    SciFiHelicalPRTrack* trk = new SciFiHelicalPRTrack();
    trks.push_back(trk);
    if (i < 3) {
      trk->set_tracker(0);
      if (i == 0) trk->set_num_points(3);
      if (i == 1) trk->set_num_points(4);
      if (i == 2) trk->set_num_points(5);
    } else {
      trk->set_tracker(1);
      if (i == 3) trk->set_num_points(3);
      if (i == 4) trk->set_num_points(4);
      if (i == 5) trk->set_num_points(5);
    }
  }

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
  EXPECT_EQ(-dsdz, xz1.GetParameter(2)); // Note the minus sign
  EXPECT_EQ(sz_c, xz1.GetParameter(3));
  EXPECT_EQ(handness, xz1.GetParameter(4));
  TF1 xz2 = tdm._t1._trks_xz[0];
  EXPECT_EQ(x0, xz2.GetParameter(0));
  EXPECT_EQ(rad, xz2.GetParameter(1));
  EXPECT_EQ(-dsdz, xz2.GetParameter(2)); // Note the minus sign
  EXPECT_EQ(sz_c, xz2.GetParameter(3));
  EXPECT_EQ(handness, xz2.GetParameter(4));

  // Check the first created z-y projections
  TF1 yz1 = tdm._t1._trks_yz[0];
  EXPECT_EQ(y0, yz1.GetParameter(0));
  EXPECT_EQ(rad, yz1.GetParameter(1));
  EXPECT_EQ(-dsdz, yz1.GetParameter(2)); // Note the minus sign
  EXPECT_EQ(sz_c, yz1.GetParameter(3));
  TF1 yz2 = tdm._t1._trks_yz[0];
  EXPECT_EQ(y0, yz2.GetParameter(0));
  EXPECT_EQ(rad, yz2.GetParameter(1));
  EXPECT_EQ(-dsdz, yz2.GetParameter(2)); // Note the minus sign
  EXPECT_EQ(sz_c, yz2.GetParameter(3));

  // Tidy up
  for (int i = 0; i < 6; ++i) {
    delete trks[i];
  }
}

} // ~namespace MAUS
