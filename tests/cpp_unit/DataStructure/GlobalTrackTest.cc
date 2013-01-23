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

#include "src/common_cpp/DataStructure/GlobalSpacePoint.hh"
#include "src/common_cpp/DataStructure/GlobalTrackPoint.hh"
#include "src/common_cpp/DataStructure/GlobalTrack.hh"

#include "gtest/gtest.h"

namespace MAUS {

class GlobalTrackTestDS : public ::testing::Test {
  protected:
    GlobalTrackTestDS()  {}
    virtual ~GlobalTrackTestDS() {}
    virtual void SetUp()    {}
    virtual void TearDown() {}
};

TEST_F(GlobalTrackTestDS, test_getters_setters) {
  GlobalTrack track;

  std::string mapper_name = "0";

  MAUS::recon::global::PID pid = MAUS::recon::global::kElectron; // 11
  
  int charge = 1;

  // Fill detectorpoints will current largest enum value, plus a
  // random selection.
  ASSERT_EQ(MAUS::recon::global::kDetectorPointSize - 1,
            (int) MAUS::recon::global::kBPM);
  unsigned int detectorpoints = (1u << MAUS::recon::global::kTracker2S5);
  detectorpoints += (1u << MAUS::recon::global::kTOF0);
  detectorpoints += (1u << MAUS::recon::global::kEMR);
  detectorpoints += (1u << MAUS::recon::global::kVirtual);
  detectorpoints += (1u << MAUS::recon::global::kTracker1S3);

  std::vector<std::string> geometry_paths;
  geometry_paths.push_back("Nowhere");
  geometry_paths.push_back("Somewhere");

  TRefArray* trackpoints = new TRefArray();
  int trackpoints_size = 3;
  for(int i = 0; i < trackpoints_size; ++i)
    trackpoints->Add(new GlobalTrackPoint());
  
  TRefArray* constituent_tracks = new TRefArray();
  int constituent_tracks_size = 3;
  for(int i = 0; i < constituent_tracks_size; ++i)
    constituent_tracks->Add(new GlobalTrack());
  
  double goodness_of_fit = 7.0;

  track.set_mapper_name(mapper_name);
  track.set_pid(pid);
  track.set_charge(charge);
  track.set_detectorpoints(detectorpoints);
  track.set_geometry_paths(geometry_paths);
  track.set_trackpoints(trackpoints);
  track.set_constituent_tracks(constituent_tracks);
  track.set_goodness_of_fit(goodness_of_fit);

  EXPECT_EQ(mapper_name, track.get_mapper_name());
  EXPECT_EQ(pid, track.get_pid());
  EXPECT_EQ(charge, track.get_charge());
  EXPECT_EQ(detectorpoints, track.get_detectorpoints());
  EXPECT_EQ(goodness_of_fit, track.get_goodness_of_fit());

  // Check size and contents of geometry_paths
  ASSERT_EQ(geometry_paths.size(), track.get_geometry_paths().size());
  for(size_t j = 0; j < geometry_paths.size(); ++j){
    EXPECT_EQ(geometry_paths.at(j), track.get_geometry_paths().at(j));
  }

  // Check size and contents of trackpoints
  ASSERT_EQ(trackpoints_size, track.get_trackpoints()->GetEntries());
  for(int j = 0; j < trackpoints_size; ++j){
    EXPECT_EQ(trackpoints->At(j), track.get_trackpoints()->At(j));
  }

  // Check size and contents of constituent_tracks
  ASSERT_EQ(constituent_tracks_size,
            track.get_constituent_tracks()->GetEntries());
  for(int j = 0; j < constituent_tracks_size; ++j){
    EXPECT_EQ(constituent_tracks->At(j), track.get_constituent_tracks()->At(j));
  }

}

TEST_F(GlobalTrackTestDS, test_TrackPoint_Access) {
  GlobalTrack track;

  // Prepare some detector points and virtual paths
  static const size_t kArraySize = 6;
  double zArray[kArraySize] = { 0., 1., 2., 3., 4., 5. };
  
  recon::global::DetectorPoint dpArray[kArraySize] =
      { MAUS::recon::global::kVirtual,
        MAUS::recon::global::kTracker1S1,
        MAUS::recon::global::kTracker1S2,
        MAUS::recon::global::kTracker1S3,
        MAUS::recon::global::kTracker1S4,
        MAUS::recon::global::kVirtual };

  std::string pathArray[kArraySize] =
      { "/Top/Of/The/Hill",
        "",
        "",
        "",
        "",
        "/Down/Again" };

  int unsortedArrayIndices[kArraySize] = {1, 0, 3, 4, 2, 5 };
    
  // Make some track points
  GlobalTrackPoint* skipTP1 = NULL;
  GlobalTrackPoint* skipTP2 = NULL;
  for(size_t i = 0; i < kArraySize; ++i){
    GlobalTrackPoint* tp = new GlobalTrackPoint();

    // The other data members are fully tested by the TrackPoint
    // tests, just want to confirm the track sorting and detector
    // bitmask and geometry path handling.
    TLorentzVector pos(0., 0., zArray[unsortedArrayIndices[i]], 0.);
    tp->set_position(pos);
    tp->set_detector(dpArray[unsortedArrayIndices[i]]);
    tp->set_geometry_path(pathArray[unsortedArrayIndices[i]]);

    track.AddTrackPoint(tp);

    // Save two TrackPoints, to remove later
    if(unsortedArrayIndices[i] == 3) skipTP1 = tp;
    if(unsortedArrayIndices[i] == 5) skipTP2 = tp;
  }

  track.SortTrackPointsByZ();

  track.RemoveTrackPoint(skipTP1);
  track.RemoveTrackPoint(skipTP2);
  
  // Check the track points are correctly sorted
  int kNewArraySize = kArraySize - 2;
  ASSERT_EQ(kNewArraySize, track.get_trackpoints()->GetEntries());
  TIterator *tp_iter =
      track.get_trackpoints()->MakeIterator();
  MAUS::GlobalTrackPoint* tp1 = (MAUS::GlobalTrackPoint*) tp_iter->Next();
  MAUS::GlobalTrackPoint* tp2 = NULL;
  while((tp2 = tp1) && (tp1 = (MAUS::GlobalTrackPoint*) tp_iter->Next())) {
    ASSERT_TRUE(tp1);
    ASSERT_TRUE(tp2);
    EXPECT_GT(tp1->get_position().Z(), tp2->get_position().Z());
  }
  
  // Check the detectorpoints are correctly set
  EXPECT_TRUE(track.HasDetector(dpArray[0]));
  EXPECT_TRUE(track.HasDetector(dpArray[1]));
  EXPECT_TRUE(track.HasDetector(dpArray[2]));
  EXPECT_FALSE(track.HasDetector(dpArray[3])); // Removed
  EXPECT_TRUE(track.HasDetector(dpArray[4]));
  EXPECT_TRUE(track.HasDetector(dpArray[5])); // Point removed, but other
                                              // kVirtual remains

  // Check the geometry path has one entry
  EXPECT_TRUE(track.HasGeometryPath(pathArray[0]));
  EXPECT_FALSE(track.HasGeometryPath(pathArray[1])); // Blank, not added
  EXPECT_FALSE(track.HasGeometryPath(pathArray[2])); // Blank, not added
  EXPECT_FALSE(track.HasGeometryPath(pathArray[3])); // Blank, not added
  EXPECT_FALSE(track.HasGeometryPath(pathArray[4])); // Blank, not added
  EXPECT_FALSE(track.HasGeometryPath(pathArray[5])); // Removed
  
}
  
TEST_F(GlobalTrackTestDS, test_ConstituentTrack_Access) {
  GlobalTrack track;

  GlobalTrack* constituentTrack1 = new GlobalTrack();
  GlobalTrack* constituentTrack2 = new GlobalTrack();
  GlobalTrack* constituentTrack3 = new GlobalTrack();
  GlobalTrack* constituentTrack4 = new GlobalTrack();

  track.AddTrack(constituentTrack1);
  track.AddTrack(constituentTrack2);
  track.AddTrack(constituentTrack3);
  track.AddTrack(constituentTrack4);

  EXPECT_TRUE(track.HasTrack(constituentTrack1));
  EXPECT_TRUE(track.HasTrack(constituentTrack2));
  EXPECT_TRUE(track.HasTrack(constituentTrack3));
  EXPECT_TRUE(track.HasTrack(constituentTrack4));

  track.RemoveTrack(constituentTrack2);
  track.RemoveTrack(constituentTrack4);
              
  EXPECT_TRUE(track.HasTrack(constituentTrack1));
  EXPECT_FALSE(track.HasTrack(constituentTrack2));
  EXPECT_TRUE(track.HasTrack(constituentTrack3));
  EXPECT_FALSE(track.HasTrack(constituentTrack4));

  track.ClearTracks();
              
  EXPECT_FALSE(track.HasTrack(constituentTrack1));
  EXPECT_FALSE(track.HasTrack(constituentTrack2));
  EXPECT_FALSE(track.HasTrack(constituentTrack3));
  EXPECT_FALSE(track.HasTrack(constituentTrack4));
}

TEST_F(GlobalTrackTestDS, test_default_constructor) {
  GlobalTrack track;

  size_t detectorpoints = 0;
  
  EXPECT_EQ("", track.get_mapper_name());
  EXPECT_EQ(MAUS::recon::global::kNoPID, track.get_pid());
  EXPECT_EQ(0, track.get_charge());
  EXPECT_TRUE(track.get_trackpoints()->GetEntries() == 0);
  EXPECT_EQ(detectorpoints, track.get_detectorpoints());
  EXPECT_TRUE(track.get_geometry_paths().empty());
  EXPECT_TRUE(track.get_constituent_tracks()->GetEntries() == 0);
  EXPECT_EQ(0., track.get_goodness_of_fit());
}

TEST_F(GlobalTrackTestDS, test_copy_constructor) {
  GlobalTrack* track1 = new GlobalTrack();

  std::string mapper_name = "0";

  MAUS::recon::global::PID pid = MAUS::recon::global::kElectron; // 11
  
  int charge = -1;

  MAUS::GlobalTrackPoint* tp0 = new MAUS::GlobalTrackPoint();
  tp0->set_mapper_name("tp0");
  MAUS::GlobalTrackPoint* tp1 = new MAUS::GlobalTrackPoint();
  tp1->set_mapper_name("tp1");

  size_t detectorpoints = 2;

  std::vector<std::string> geometry_paths;
  std::string path0 = "Nowhere";
  std::string path1 = "Somewhere";
  geometry_paths.push_back(path0);
  geometry_paths.push_back(path1);

  GlobalTrack *conTrack0 = new GlobalTrack();
  GlobalTrack *conTrack1 = new GlobalTrack();
  GlobalTrack *conTrack2 = new GlobalTrack();

  double goodness_of_fit = 7.0;

  track1->set_mapper_name(mapper_name);
  track1->set_pid(pid);
  track1->set_charge(charge);
  track1->AddTrackPoint(tp0);
  track1->AddTrackPoint(tp1);
  track1->set_detectorpoints(detectorpoints);
  track1->set_geometry_paths(geometry_paths);
  track1->AddTrack(conTrack0);
  track1->AddTrack(conTrack1);
  track1->AddTrack(conTrack2);
  track1->set_goodness_of_fit(goodness_of_fit);

  GlobalTrack *track2 =
      new GlobalTrack(*track1);
  
  EXPECT_EQ(mapper_name, track2->get_mapper_name());
  EXPECT_EQ(pid, track2->get_pid());
  EXPECT_EQ(charge, track2->get_charge());
  EXPECT_EQ(tp0, track2->get_trackpoints()->At(0));
  EXPECT_EQ(tp1, track2->get_trackpoints()->At(1));
  EXPECT_EQ(detectorpoints, track2->get_detectorpoints());
  EXPECT_EQ(path0, track2->get_geometry_paths()[0]);
  EXPECT_EQ(path1, track2->get_geometry_paths()[1]);
  EXPECT_EQ(conTrack0, track2->get_constituent_tracks()->At(0));
  EXPECT_EQ(conTrack1, track2->get_constituent_tracks()->At(1));
  EXPECT_EQ(conTrack2, track2->get_constituent_tracks()->At(2));
  EXPECT_EQ(goodness_of_fit, track2->get_goodness_of_fit());
}

TEST_F(GlobalTrackTestDS, test_assignment_operator) {
  GlobalTrack* track1 = new GlobalTrack();

  std::string mapper_name = "0";

  MAUS::recon::global::PID pid = MAUS::recon::global::kElectron; // 11
  
  int charge = -1;

  MAUS::GlobalTrackPoint* tp0 = new MAUS::GlobalTrackPoint();
  tp0->set_mapper_name("tp0");
  MAUS::GlobalTrackPoint* tp1 = new MAUS::GlobalTrackPoint();
  tp1->set_mapper_name("tp1");

  size_t detectorpoints = 2;

  std::vector<std::string> geometry_paths;
  std::string path0 = "Nowhere";
  std::string path1 = "Somewhere";
  geometry_paths.push_back(path0);
  geometry_paths.push_back(path1);

  GlobalTrack *conTrack0 = new GlobalTrack();
  GlobalTrack *conTrack1 = new GlobalTrack();
  GlobalTrack *conTrack2 = new GlobalTrack();

  double goodness_of_fit = 7.0;

  track1->set_mapper_name(mapper_name);
  track1->set_pid(pid);
  track1->set_charge(charge);
  track1->AddTrackPoint(tp0);
  track1->AddTrackPoint(tp1);
  track1->set_detectorpoints(detectorpoints);
  track1->set_geometry_paths(geometry_paths);
  track1->AddTrack(conTrack0);
  track1->AddTrack(conTrack1);
  track1->AddTrack(conTrack2);
  track1->set_goodness_of_fit(goodness_of_fit);

  GlobalTrack track2 = (*track1);
  
  EXPECT_EQ(mapper_name, track2.get_mapper_name());
  EXPECT_EQ(pid, track2.get_pid());
  EXPECT_EQ(charge, track2.get_charge());
  EXPECT_EQ(tp0, track2.get_trackpoints()->At(0));
  EXPECT_EQ(tp1, track2.get_trackpoints()->At(1));
  EXPECT_EQ(detectorpoints, track2.get_detectorpoints());
  EXPECT_EQ(path0, track2.get_geometry_paths()[0]);
  EXPECT_EQ(path1, track2.get_geometry_paths()[1]);
  EXPECT_EQ(conTrack0, track2.get_constituent_tracks()->At(0));
  EXPECT_EQ(conTrack1, track2.get_constituent_tracks()->At(1));
  EXPECT_EQ(conTrack2, track2.get_constituent_tracks()->At(2));
  EXPECT_EQ(goodness_of_fit, track2.get_goodness_of_fit());
}

TEST_F(GlobalTrackTestDS, test_clone_method) {
  GlobalTrack* track1 = new GlobalTrack();

  std::string mapper_name = "0";

  MAUS::recon::global::PID pid = MAUS::recon::global::kElectron; // 11
  
  int charge = -1;

  MAUS::GlobalTrackPoint* tp0 = new MAUS::GlobalTrackPoint();
  tp0->set_mapper_name("tp0");
  MAUS::GlobalTrackPoint* tp1 = new MAUS::GlobalTrackPoint();
  tp1->set_mapper_name("tp1");

  size_t detectorpoints = 2;

  std::vector<std::string> geometry_paths;
  std::string path0 = "Nowhere";
  std::string path1 = "Somewhere";
  geometry_paths.push_back(path0);
  geometry_paths.push_back(path1);

  GlobalTrack *conTrack0 = new GlobalTrack();
  GlobalTrack *conTrack1 = new GlobalTrack();
  GlobalTrack *conTrack2 = new GlobalTrack();

  double goodness_of_fit = 7.0;

  track1->set_mapper_name(mapper_name);
  track1->set_pid(pid);
  track1->set_charge(charge);
  track1->AddTrackPoint(tp0);
  track1->AddTrackPoint(tp1);
  track1->set_detectorpoints(detectorpoints);
  track1->set_geometry_paths(geometry_paths);
  track1->AddTrack(conTrack0);
  track1->AddTrack(conTrack1);
  track1->AddTrack(conTrack2);
  track1->set_goodness_of_fit(goodness_of_fit);

  GlobalTrack *track2 = track1->Clone();
  
  ASSERT_TRUE(track2);
  EXPECT_NE(track1, track2);
  
  EXPECT_EQ(mapper_name, track2->get_mapper_name());
  EXPECT_EQ(pid, track2->get_pid());
  EXPECT_EQ(charge, track2->get_charge());
  ASSERT_EQ(2, track2->get_trackpoints()->GetEntries());

  // New cloned objects, but contents should be the same.
  EXPECT_NE(tp0, track2->get_trackpoints()->At(0));
  MAUS::GlobalTrackPoint* not_tp0 = (MAUS::GlobalTrackPoint*) track2->get_trackpoints()->At(0);
  EXPECT_EQ("tp0", not_tp0->get_mapper_name());
  EXPECT_NE(tp1, track2->get_trackpoints()->At(1));
  MAUS::GlobalTrackPoint* not_tp1 = (MAUS::GlobalTrackPoint*) track2->get_trackpoints()->At(1);
  EXPECT_EQ("tp1", not_tp1->get_mapper_name());
  
  EXPECT_EQ(detectorpoints, track2->get_detectorpoints());

  ASSERT_EQ(2U, track2->get_geometry_paths().size());
  EXPECT_EQ(path0, track2->get_geometry_paths()[0]);
  EXPECT_EQ(path1, track2->get_geometry_paths()[1]);

  ASSERT_EQ(3, track2->get_constituent_tracks()->GetEntries());
  EXPECT_EQ(conTrack0, track2->get_constituent_tracks()->At(0));
  EXPECT_EQ(conTrack1, track2->get_constituent_tracks()->At(1));
  EXPECT_EQ(conTrack2, track2->get_constituent_tracks()->At(2));

  EXPECT_EQ(goodness_of_fit, track2->get_goodness_of_fit());
}

} // ~namespace MAUS
