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

// C headers
#include <assert.h>

// Other headers
#include "Config/MiceModule.hh"
#include "src/common_cpp/Recon/SciFi/SciFiClusterRec.hh"
#include "src/common_cpp/DataStructure/SciFiCluster.hh"
#include "src/common_cpp/DataStructure/SciFiDigit.hh"
#include "src/common_cpp/DataStructure/SciFiEvent.hh"

#include "gtest/gtest.h"

namespace MAUS {

class SciFiClusterRecTest : public ::testing::Test {
 protected:
  SciFiClusterRecTest()  {
    cluster_exception = 80;
    min_npe = 2.0;
    // Get the tracker modules.
    std::string filename;
    filename = "Stage6.dat";
    MiceModule*      _module;
    _module = new MiceModule(filename);
    modules = _module->findModulesByPropertyString("SensitiveDetector", "SciFi");

    // SciFiCluster *cluster = new SciFiCluster();

    // event.add_cluster(cluster);
  }
  virtual ~SciFiClusterRecTest() {}
  virtual void SetUp()    {}
  virtual void TearDown() {}
  void create_scifievent_digits();
  std::vector<const MiceModule*> modules;
  int cluster_exception;
  double min_npe;
  SciFiEvent event;
};

// process(SciFiEvent &evt, std::vector<const MiceModule*> modules)
void SciFiClusterRecTest::create_scifievent_digits() {
  // evt.digits();
  // Create 15 good digits + 2 digits to be merged + 1 digit to be left over (noise)
/*
get_npe
is_used()
get_spill()
get_event(
get_tracker()
get_station() 
get_plane()
get_channel()
*/
}

TEST_F(SciFiClusterRecTest, test_process) {
  SciFiClusterRec clustering(cluster_exception, min_npe, modules);
  clustering.process(event);

  // EXPECT_EQ(tracker,1);
  // EXPECT_EQ(station,2.0);
  /*
  int tracker = 1;
  int station = 2;
  int plane = 0;
  int channel1 = 106;
  double npe1 = 3.2;
  double time = 12.2;
  // Construct a digit.
  SciFiDigit *digit = new SciFiDigit(tracker, station, plane, channel1, npe1, time);
  // Start a cluster from "digit"
  SciFiCluster *cluster = new SciFiCluster(digit);

  cluster->construct(modules);*/
  // think of tests for direction and position...
}

TEST_F(SciFiClusterRecTest, test_get_seeds) {
  // SciFiEvent evt;
/*  SciFiEvent* evt = new SciFiEvent();
  SciFiDigit *digit_1 = new SciFiDigit();
  digit_1->set_npe(3.0);
  evt->add_digit(digit_1);

  SciFiDigit *digit_2;
  digit_2->set_npe(3.0);
  // evt->add_digit(digit_2);
  SciFiDigit *digit_3;
  digit_3->set_npe(1.0);
  // evt->add_digit(digit_3);
  std::vector<SciFiDigit*> seeds;

  SciFiClusterRec *worker = new SciFiClusterRec(cluster_exception, min_npe, modules);
  // seeds = worker->get_seeds(*evt);
  EXPECT_EQ(2, seeds.size());
*/
}

TEST_F(SciFiClusterRecTest, test_construct) {
/*
  SciFiCluster *clust;
  int tracker = 1;
  int station = 1;
  int plane   = 1;
  clust->set_plane(plane);
  clust->set_station(station);
  clust->set_tracker(tracker);

  SciFiClusterRec worker(cluster_exception, min_npe, modules);
  const MiceModule* this_plane = NULL;
  this_plane = worker.find_plane(tracker, station, plane);
  assert(this_plane != NULL);
  // compute it's direction & position in TRF...
  ThreeVector trf_dir(0., 1., 0.);
  ThreeVector trf_pos(0., 0., 0.);
  // double alpha;

  // worker.construct(clust, this_plane, trf_dir, trf_pos, alpha);
*/
}

TEST_F(SciFiClusterRecTest, test_find_plane) {
  SciFiClusterRec worker(cluster_exception, min_npe, modules);
  // Check that every tracker plane exists and that "find_plane()" finds it.
  for ( int tracker = 0; tracker < 2; ++tracker ) {
    for ( int station = 1; station < 6; ++station ) {
      for ( int plane = 0; plane < 3; ++plane ) {
        const MiceModule* a_plane = NULL;
        a_plane = worker.find_plane(tracker, station, plane);
        EXPECT_TRUE(a_plane);
      }
    }
  }
  // Check that "find_plane()" returns null is plane doesn't exist.
  const MiceModule* a_plane = NULL;
  int tracker = 0;
  int station = 6;
  int plane   = 0;
  a_plane = worker.find_plane(tracker, station, plane);
  EXPECT_FALSE(a_plane);
}

TEST_F(SciFiClusterRecTest, test_neighbours) {
  int spill   = 1;
  int event   = 1;
  int tracker = 1;
  int station = 1;
  int plane   = 1;
  int channel_1 = 1;
  int channel_2 = 5;
  int channel_3 = 2;
  int npe     = 1;
  int time    = 1;

  SciFiDigit *digit1 = new SciFiDigit(spill, event, tracker, station, plane, channel_1, npe, time);
  SciFiDigit *digit2 = new SciFiDigit(spill, event, tracker, station, plane, channel_2, npe, time);
  SciFiDigit *digit3 = new SciFiDigit(spill, event, tracker, station, plane, channel_3, npe, time);

  SciFiClusterRec worker(cluster_exception, min_npe, modules);
  bool expect_false;
  expect_false = worker.are_neighbours(digit1, digit2);
  EXPECT_FALSE(expect_false);
  bool expect_true;
  expect_true = worker.are_neighbours(digit1, digit3);
  EXPECT_TRUE(expect_true);
}

/*
  std::vector<SciFiDigit*> get_seeds(SciFiEvent &evt);

  ThreeVector get_reference_frame_pos(int tracker);

  bool are_neighbours(SciFiDigit *seed_i, SciFiDigit *seed_j);

  void construct(SciFiCluster *clust, const MiceModule* this_plane,
                 ThreeVector &dir, ThreeVector &tracker_ref_frame_pos, double &alpha);

  void process_cluster(SciFiCluster *clust);

  void process(SciFiEvent &evt);

  void make_clusters(SciFiEvent &evt, std::vector<SciFiDigit*> &seeds);
*/
} // ~namespace MAUS
