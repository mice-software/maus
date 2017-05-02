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
// #include "src/common_cpp/DataStructure/SciFiCluster.hh"

#include "gtest/gtest.h"

namespace MAUS {

class SciFiSpacePointTestDS : public ::testing::Test {
  protected:
    SciFiSpacePointTestDS()  {}
    virtual ~SciFiSpacePointTestDS() {}
    virtual void SetUp()    {}
    virtual void TearDown() {}
};

TEST_F(SciFiSpacePointTestDS, test_getters_setters) {
  bool used = true;
  bool add_on = true;
  int spill = 1;
  int event = 2;
  int tracker = 1;
  int station = 3;
  double npe = 4.0;
  double time = 5.0;
  double chi2 = 6.0;
  double prxy_pull = 7.0;

  double x = 7.0;
  double y = 8.0;
  double z = 9.0;
  ThreeVector pos(x, y, z);

  int plane1 = 1;
  int plane2 = 2;
  SciFiCluster * c1 = new SciFiCluster();
  c1->set_plane(plane1);
  SciFiCluster * c2 = new SciFiCluster();
  c2->set_plane(plane2);
  std::vector<SciFiCluster*> cvec;
  cvec.push_back(c1);
  cvec.push_back(c2);
  unsigned int size = 2;

  std::string type = "triplet";

  SciFiSpacePoint sp;
  sp.set_used(used);
  sp.set_add_on(add_on);
  sp.set_spill(spill);
  sp.set_event(event);
  sp.set_tracker(tracker);
  sp.set_station(station);
  sp.set_npe(npe);
  sp.set_time(time);
  sp.set_chi2(chi2);
  sp.set_prxy_pull(prxy_pull);
  sp.set_position(pos);
  sp.set_channels_pointers(cvec);
  sp.set_type(type);

  EXPECT_EQ(used, sp.get_used());
  EXPECT_EQ(add_on, sp.get_add_on());
  EXPECT_EQ(spill, sp.get_spill());
  EXPECT_EQ(event, sp.get_event());
  EXPECT_EQ(tracker, sp.get_tracker());
  EXPECT_EQ(station, sp.get_station());
  EXPECT_EQ(npe, sp.get_npe());
  EXPECT_EQ(time, sp.get_time());
  EXPECT_EQ(chi2, sp.get_chi2());
  EXPECT_EQ(prxy_pull, sp.get_prxy_pull());
  EXPECT_EQ(x, sp.get_position().x());
  EXPECT_EQ(y, sp.get_position().y());
  EXPECT_EQ(z, sp.get_position().z());
  ASSERT_EQ(size, sp.get_channels()->GetLast() + 1);
  EXPECT_EQ(plane1, sp.get_channels_pointers()[0]->get_plane());
  EXPECT_EQ(plane2, sp.get_channels_pointers()[1]->get_plane());
  EXPECT_EQ(type, sp.get_type());
}

TEST_F(SciFiSpacePointTestDS, test_default_constructor) {
  unsigned int size = 0;
  SciFiSpacePoint sp;
  EXPECT_FALSE(sp.get_used());
  EXPECT_FALSE(sp.get_add_on());
  EXPECT_EQ(0, sp.get_spill());
  EXPECT_EQ(0, sp.get_event());
  EXPECT_EQ(0, sp.get_tracker());
  EXPECT_EQ(0, sp.get_station());
  EXPECT_EQ(0.0, sp.get_npe());
  EXPECT_EQ(0.0, sp.get_time());
  EXPECT_EQ(0.0, sp.get_chi2());
  EXPECT_EQ(0.0, sp.get_prxy_pull());
  EXPECT_EQ(0.0, sp.get_position().x());
  EXPECT_EQ(0.0, sp.get_position().y());
  EXPECT_EQ(0.0, sp.get_position().z());
  EXPECT_EQ(size, sp.get_channels()->GetLast() + 1);
  EXPECT_EQ("", sp.get_type());
}

TEST_F(SciFiSpacePointTestDS, test_copy_constructor) {
  bool used = true;
  bool add_on = true;
  int spill = 1;
  int event = 2;
  int tracker = 1;
  int station = 3;
  double npe = 4.0;
  double time = 5.0;
  double chi2 = 6.0;
  double prxy_pull = 7.0;

  double x = 7.0;
  double y = 8.0;
  double z = 9.0;
  ThreeVector pos(x, y, z);

  int plane1 = 1;
  int plane2 = 2;
  SciFiCluster * c1 = new SciFiCluster();
  c1->set_plane(plane1);
  SciFiCluster * c2 = new SciFiCluster();
  c2->set_plane(plane2);
  std::vector<SciFiCluster*> cvec;
  cvec.push_back(c1);
  cvec.push_back(c2);
  unsigned int size = 2;

  std::string type = "triplet";

  SciFiSpacePoint * sp1 = new SciFiSpacePoint();
  sp1->set_used(used);
  sp1->set_add_on(add_on);
  sp1->set_spill(spill);
  sp1->set_event(event);
  sp1->set_tracker(tracker);
  sp1->set_station(station);
  sp1->set_npe(npe);
  sp1->set_time(time);
  sp1->set_chi2(chi2);
  sp1->set_prxy_pull(prxy_pull);
  sp1->set_position(pos);
  sp1->set_channels_pointers(cvec);
  sp1->set_type(type);

  SciFiSpacePoint * sp2 = new SciFiSpacePoint(*sp1);
  delete sp1;

  EXPECT_EQ(used, sp2->get_used());
  EXPECT_EQ(used, sp2->get_add_on());
  EXPECT_EQ(spill, sp2->get_spill());
  EXPECT_EQ(event, sp2->get_event());
  EXPECT_EQ(tracker, sp2->get_tracker());
  EXPECT_EQ(station, sp2->get_station());
  EXPECT_EQ(npe, sp2->get_npe());
  EXPECT_EQ(time, sp2->get_time());
  EXPECT_EQ(chi2, sp2->get_chi2());
  EXPECT_EQ(prxy_pull, sp2->get_prxy_pull());
  EXPECT_EQ(x, sp2->get_position().x());
  EXPECT_EQ(y, sp2->get_position().y());
  EXPECT_EQ(z, sp2->get_position().z());
  ASSERT_EQ(size, sp2->get_channels()->GetLast() + 1);
  EXPECT_EQ(plane1, sp2->get_channels_pointers()[0]->get_plane());
  EXPECT_EQ(plane2, sp2->get_channels_pointers()[1]->get_plane());
  EXPECT_EQ(type, sp2->get_type());
}

TEST_F(SciFiSpacePointTestDS, test_two_cluster_constructor) {
  int spill = 1;
  int event = 2;
  int tracker = 1;
  int station = 3;
  double npe1 = 4.0;
  double npe2 = 8.0;

  SciFiCluster * c1 = new SciFiCluster();
  c1->set_spill(spill);
  c1->set_event(event);
  c1->set_tracker(tracker);
  c1->set_station(station);
  c1->set_npe(npe1);

  SciFiCluster * c2 = new SciFiCluster();
  c2->set_spill(spill);
  c2->set_event(event);
  c2->set_tracker(tracker);
  c2->set_station(station);
  c2->set_npe(npe2);

  unsigned int size = 2;

  SciFiSpacePoint sp(c1, c2);
  EXPECT_FALSE(sp.get_used());
  EXPECT_FALSE(sp.get_add_on());
  EXPECT_EQ(spill, sp.get_spill());
  EXPECT_EQ(event, sp.get_event());
  EXPECT_EQ(tracker, sp.get_tracker());
  EXPECT_EQ(station, sp.get_station());
  EXPECT_EQ(npe1+npe2, sp.get_npe());
  EXPECT_EQ(0.0, sp.get_time());
  EXPECT_EQ(0.0, sp.get_prxy_pull());
  EXPECT_EQ(0.0, sp.get_position().x());
  EXPECT_EQ(0.0, sp.get_position().y());
  EXPECT_EQ(0.0, sp.get_position().z());
  ASSERT_EQ(size, sp.get_channels()->GetLast() + 1);
  EXPECT_EQ(true, sp.get_channels_pointers()[0]->is_used());
  EXPECT_EQ(true, sp.get_channels_pointers()[1]->is_used());
  EXPECT_EQ("duplet", sp.get_type());

  delete c1;
  delete c2;
}

TEST_F(SciFiSpacePointTestDS, test_three_cluster_constructor) {
  int spill = 1;
  int event = 2;
  int tracker = 1;
  int station = 3;
  double npe1 = 4.0;
  double npe2 = 8.0;
  double npe3 = 2.0;

  SciFiCluster * c1 = new SciFiCluster();
  c1->set_spill(spill);
  c1->set_event(event);
  c1->set_tracker(tracker);
  c1->set_station(station);
  c1->set_npe(npe1);

  SciFiCluster * c2 = new SciFiCluster();
  c2->set_spill(spill);
  c2->set_event(event);
  c2->set_tracker(tracker);
  c2->set_station(station);
  c2->set_npe(npe2);

  SciFiCluster * c3 = new SciFiCluster();
  c3->set_spill(spill);
  c3->set_event(event);
  c3->set_tracker(tracker);
  c3->set_station(station);
  c3->set_npe(npe3);

  unsigned int size = 3;

  SciFiSpacePoint sp(c1, c2, c3);
  EXPECT_FALSE(sp.get_used());
  EXPECT_FALSE(sp.get_add_on());
  EXPECT_EQ(spill, sp.get_spill());
  EXPECT_EQ(event, sp.get_event());
  EXPECT_EQ(tracker, sp.get_tracker());
  EXPECT_EQ(station, sp.get_station());
  EXPECT_EQ(npe1+npe2+npe3, sp.get_npe());
  EXPECT_EQ(0.0, sp.get_time());
  EXPECT_EQ(0.0, sp.get_prxy_pull());
  EXPECT_EQ(0.0, sp.get_position().x());
  EXPECT_EQ(0.0, sp.get_position().y());
  EXPECT_EQ(0.0, sp.get_position().z());
  ASSERT_EQ(size, sp.get_channels()->GetLast() + 1);
  EXPECT_EQ(true, sp.get_channels_pointers()[0]->is_used());
  EXPECT_EQ(true, sp.get_channels_pointers()[1]->is_used());
  EXPECT_EQ(true, sp.get_channels_pointers()[2]->is_used());
  EXPECT_EQ("triplet", sp.get_type());

  delete c1;
  delete c2;
  delete c3;
}


TEST_F(SciFiSpacePointTestDS, test_assignment_operator) {
  bool used = true;
  bool add_on = true;
  int spill = 1;
  int event = 2;
  int tracker = 1;
  int station = 3;
  double npe = 4.0;
  double time = 5.0;
  double chi2 = 6.0;
  double prxy_pull = 7.0;

  double x = 7.0;
  double y = 8.0;
  double z = 9.0;
  ThreeVector pos(x, y, z);

  int plane1 = 1;
  int plane2 = 2;
  SciFiCluster * c1 = new SciFiCluster();
  c1->set_plane(plane1);
  SciFiCluster * c2 = new SciFiCluster();
  c2->set_plane(plane2);
  std::vector<SciFiCluster*> cvec;
  cvec.push_back(c1);
  cvec.push_back(c2);

  unsigned int size = 2;

  std::string type = "triplet";

  SciFiSpacePoint * sp1 = new SciFiSpacePoint();
  sp1->set_used(used);
  sp1->set_add_on(add_on);
  sp1->set_spill(spill);
  sp1->set_event(event);
  sp1->set_tracker(tracker);
  sp1->set_station(station);
  sp1->set_npe(npe);
  sp1->set_time(time);
  sp1->set_chi2(chi2);
  sp1->set_prxy_pull(prxy_pull);
  sp1->set_position(pos);
  sp1->set_channels_pointers(cvec);
  sp1->set_type(type);

  SciFiSpacePoint * sp2 = new SciFiSpacePoint();
  *sp2 = *sp1;
  delete sp1;

  EXPECT_EQ(used, sp2->get_used());
  EXPECT_EQ(add_on, sp2->get_add_on());
  EXPECT_EQ(spill, sp2->get_spill());
  EXPECT_EQ(event, sp2->get_event());
  EXPECT_EQ(tracker, sp2->get_tracker());
  EXPECT_EQ(station, sp2->get_station());
  EXPECT_EQ(npe, sp2->get_npe());
  EXPECT_EQ(time, sp2->get_time());
  EXPECT_EQ(chi2, sp2->get_chi2());
  EXPECT_EQ(prxy_pull, sp2->get_prxy_pull());
  EXPECT_EQ(x, sp2->get_position().x());
  EXPECT_EQ(y, sp2->get_position().y());
  EXPECT_EQ(z, sp2->get_position().z());
  ASSERT_EQ(size, sp2->get_channels()->GetLast() + 1);
  EXPECT_EQ(plane1, sp2->get_channels_pointers()[0]->get_plane());
  EXPECT_EQ(plane2, sp2->get_channels_pointers()[1]->get_plane());
  EXPECT_EQ(type, sp2->get_type());
}

} // ~namespace MAUS
