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

#include "src/common_cpp/DataStructure/Global/SpacePoint.hh"
#include "src/common_cpp/DataStructure/Global/TrackPoint.hh"
#include "src/common_cpp/DataStructure/Global/Track.hh"
#include "Utils/Exception.hh"

#include "gtest/gtest.h"

namespace MAUS {

class TrackTestDS : public ::testing::Test {
  protected:
    TrackTestDS()  {}
    virtual ~TrackTestDS() {}
    virtual void SetUp()    {}
    virtual void TearDown() {}
};

TEST_F(TrackTestDS, test_getters_setters) {
  MAUS::DataStructure::Global::Track track;

  std::string mapper_name = "0";

  MAUS::DataStructure::Global::PID pid =
      MAUS::DataStructure::Global::kEMinus; // 11

  int charge = 1;

  // Fill detectorpoints will current largest enum value, plus a
  // random selection.
  ASSERT_EQ(MAUS::DataStructure::Global::kDetectorPointSize - 1,
            static_cast<int>(MAUS::DataStructure::Global::kEMR));
  unsigned int detectorpoints =
      (1u << MAUS::DataStructure::Global::kTracker1_5);
  detectorpoints += (1u << MAUS::DataStructure::Global::kTOF0_1);
  detectorpoints += (1u << MAUS::DataStructure::Global::kCalorimeter);
  detectorpoints += (1u << MAUS::DataStructure::Global::kVirtual);
  detectorpoints += (1u << MAUS::DataStructure::Global::kTracker0_3);

  std::vector<std::string> geometry_paths;
  geometry_paths.push_back("Nowhere");
  geometry_paths.push_back("Somewhere");

  TRefArray* track_points = new TRefArray();
  int track_points_size = 3;
  for (int i = 0; i < track_points_size; ++i)
    track_points->Add(new MAUS::DataStructure::Global::TrackPoint());

  TRefArray* constituent_tracks = new TRefArray();
  int constituent_tracks_size = 3;
  for (int i = 0; i < constituent_tracks_size; ++i)
    constituent_tracks->Add(new MAUS::DataStructure::Global::Track());

  double goodness_of_fit = 7.0;
  double p_value = 0.05;

  track.set_mapper_name(mapper_name);
  track.set_pid(pid);
  track.set_charge(charge);
  track.set_detectorpoints(detectorpoints);
  track.set_geometry_paths(geometry_paths);
  track.set_track_points(track_points);
  track.set_constituent_tracks(constituent_tracks);
  track.set_goodness_of_fit(goodness_of_fit);
  track.set_p_value(p_value);

  EXPECT_EQ(mapper_name, track.get_mapper_name());
  EXPECT_EQ(pid, track.get_pid());
  EXPECT_EQ(charge, track.get_charge());
  EXPECT_EQ(detectorpoints, track.get_detectorpoints());
  EXPECT_EQ(goodness_of_fit, track.get_goodness_of_fit());
  EXPECT_EQ(p_value, track.get_p_value());

  // Check size and contents of geometry_paths
  ASSERT_EQ(geometry_paths.size(), track.get_geometry_paths().size());
  for (size_t j = 0; j < geometry_paths.size(); ++j) {
    EXPECT_EQ(geometry_paths.at(j), track.get_geometry_paths().at(j));
  }

  // Check size and contents of track_points
  ASSERT_EQ(track_points_size, track.get_track_points()->GetEntries());
  for (int j = 0; j < track_points_size; ++j) {
    EXPECT_EQ(track_points->At(j), track.get_track_points()->At(j));
  }

  // Check size and contents of constituent_tracks
  ASSERT_EQ(constituent_tracks_size,
            track.get_constituent_tracks()->GetEntries());
  for (int j = 0; j < constituent_tracks_size; ++j) {
    EXPECT_EQ(constituent_tracks->At(j), track.get_constituent_tracks()->At(j));
  }
}

TEST_F(TrackTestDS, test_TrackPoint_Access) {
  MAUS::DataStructure::Global::Track track;

  // Prepare some detector points and virtual paths
  static const size_t kArraySize = 6;
  double zArray[kArraySize] = { 0., 1., 2., 3., 4., 5. };

  DataStructure::Global::DetectorPoint dpArray[kArraySize] =
      { MAUS::DataStructure::Global::kVirtual,
        MAUS::DataStructure::Global::kTracker0_1,
        MAUS::DataStructure::Global::kTracker0_2,
        MAUS::DataStructure::Global::kTracker0_3,
        MAUS::DataStructure::Global::kTracker0_4,
        MAUS::DataStructure::Global::kVirtual };

  std::string pathArray[kArraySize] =
      { "/Top/Of/The/Hill",
        "",
        "",
        "",
        "",
        "/Down/Again" };

  int unsortedArrayIndices[kArraySize] = {1, 0, 3, 4, 2, 5 };

  // Make some track points
  MAUS::DataStructure::Global::TrackPoint* skipTP1 = NULL;
  MAUS::DataStructure::Global::TrackPoint* skipTP2 = NULL;
  for (size_t i = 0; i < kArraySize; ++i) {
    MAUS::DataStructure::Global::TrackPoint* tp =
        new MAUS::DataStructure::Global::TrackPoint();

    // The other data members are fully tested by the TrackPoint
    // tests, just want to confirm the track sorting and detector
    // bitmask and geometry path handling.
    TLorentzVector pos(0., 0., zArray[unsortedArrayIndices[i]], 0.);
    tp->set_position(pos);
    tp->set_detector(dpArray[unsortedArrayIndices[i]]);
    tp->set_geometry_path(pathArray[unsortedArrayIndices[i]]);

    track.AddTrackPoint(tp);

    // Save two TrackPoints, to remove later
    if (unsortedArrayIndices[i] == 3) skipTP1 = tp;
    if (unsortedArrayIndices[i] == 5) skipTP2 = tp;
  }

  track.SortTrackPointsByZ();

  track.RemoveTrackPoint(skipTP1);
  track.RemoveTrackPoint(skipTP2);

  // Check the track points are correctly sorted
  int kNewArraySize = kArraySize - 2;
  ASSERT_EQ(kNewArraySize, track.get_track_points()->GetEntries());
  TIterator *tp_iter =
      track.get_track_points()->MakeIterator();
  MAUS::DataStructure::Global::TrackPoint* tp1 =
      (MAUS::DataStructure::Global::TrackPoint*) tp_iter->Next();
  MAUS::DataStructure::Global::TrackPoint* tp2 = NULL;
  while ((tp2 = tp1) &&
         (tp1 = (MAUS::DataStructure::Global::TrackPoint*) tp_iter->Next())) {
    ASSERT_TRUE(tp1);
    ASSERT_TRUE(tp2);
    EXPECT_GT(tp1->get_position().Z(), tp2->get_position().Z());
  }

  // Check the GetTrackPoints method
  std::vector<const MAUS::DataStructure::Global::TrackPoint*> tps =
      track.GetTrackPoints();

  ASSERT_EQ((size_t)kNewArraySize, tps.size());

  // Check the detectorpoints are correctly set
  EXPECT_TRUE(track.HasDetector(dpArray[0]));
  EXPECT_TRUE(track.HasDetector(dpArray[1]));
  EXPECT_TRUE(track.HasDetector(dpArray[2]));
  EXPECT_FALSE(track.HasDetector(dpArray[3])); // Removed
  EXPECT_TRUE(track.HasDetector(dpArray[4]));
  EXPECT_TRUE(track.HasDetector(dpArray[5])); // Point removed, but other
                                              // kVirtual remains

  // Check the GetDetectorPoints method
  std::vector<MAUS::DataStructure::Global::DetectorPoint> dps =
      track.GetDetectorPoints();
  // MAUS::DataStructure::Global::kVirtual    // 1
  // MAUS::DataStructure::Global::kTracker0_1 // 2
  // MAUS::DataStructure::Global::kTracker0_2 // 3
  // MAUS::DataStructure::Global::kTracker0_3 // Removed
  // MAUS::DataStructure::Global::kTracker0_4 // 4
  // MAUS::DataStructure::Global::kVirtual    // Doesn't count twice
  ASSERT_EQ(4U, dps.size());

  // Check the ClearDetectors method
  track.ClearDetectors();
  EXPECT_FALSE(track.HasDetector(dpArray[0]));
  EXPECT_FALSE(track.HasDetector(dpArray[1]));
  EXPECT_FALSE(track.HasDetector(dpArray[2]));
  EXPECT_FALSE(track.HasDetector(dpArray[3]));
  EXPECT_FALSE(track.HasDetector(dpArray[4]));
  EXPECT_FALSE(track.HasDetector(dpArray[5]));

  // Check the geometry path has one entry
  EXPECT_TRUE(track.HasGeometryPath(pathArray[0]));
  EXPECT_FALSE(track.HasGeometryPath(pathArray[1])); // Blank, not added
  EXPECT_FALSE(track.HasGeometryPath(pathArray[2])); // Blank, not added
  EXPECT_FALSE(track.HasGeometryPath(pathArray[3])); // Blank, not added
  EXPECT_FALSE(track.HasGeometryPath(pathArray[4])); // Blank, not added
  EXPECT_FALSE(track.HasGeometryPath(pathArray[5])); // Removed

  // Check the ClearGeometryPaths method
  track.ClearGeometryPaths();
  EXPECT_FALSE(track.HasGeometryPath(pathArray[0]));
}

TEST_F(TrackTestDS, test_ConstituentTrack_Access) {
  MAUS::DataStructure::Global::Track track;

  MAUS::DataStructure::Global::Track* constituentTrack1 =
      new MAUS::DataStructure::Global::Track();
  MAUS::DataStructure::Global::Track* constituentTrack2 =
      new MAUS::DataStructure::Global::Track();
  MAUS::DataStructure::Global::Track* constituentTrack3 =
      new MAUS::DataStructure::Global::Track();
  MAUS::DataStructure::Global::Track* constituentTrack4 =
      new MAUS::DataStructure::Global::Track();

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

  std::vector<const MAUS::DataStructure::Global::Track*> ts =
      track.GetConstituentTracks();
  ASSERT_EQ(2U, ts.size());
  EXPECT_TRUE(constituentTrack1 == ts.at(0));
  EXPECT_TRUE(constituentTrack3 == ts.at(1));

  track.ClearTracks();

  EXPECT_FALSE(track.HasTrack(constituentTrack1));
  EXPECT_FALSE(track.HasTrack(constituentTrack2));
  EXPECT_FALSE(track.HasTrack(constituentTrack3));
  EXPECT_FALSE(track.HasTrack(constituentTrack4));
}

TEST_F(TrackTestDS, test_default_constructor) {
  MAUS::DataStructure::Global::Track track;

  size_t detectorpoints = 0;

  EXPECT_EQ("", track.get_mapper_name());
  EXPECT_EQ(MAUS::DataStructure::Global::kNoPID, track.get_pid());
  EXPECT_EQ(0, track.get_charge());
  EXPECT_TRUE(track.get_track_points()->GetEntries() == 0);
  EXPECT_EQ(detectorpoints, track.get_detectorpoints());
  EXPECT_TRUE(track.get_geometry_paths().empty());
  EXPECT_TRUE(track.get_constituent_tracks()->GetEntries() == 0);
  EXPECT_EQ(0., track.get_goodness_of_fit());
  EXPECT_EQ(0., track.get_p_value());
}

TEST_F(TrackTestDS, test_copy_constructor) {
  MAUS::DataStructure::Global::Track* track1 =
      new MAUS::DataStructure::Global::Track();

  std::string mapper_name = "0";

  MAUS::DataStructure::Global::PID pid =
      MAUS::DataStructure::Global::kEMinus; // 11

  int charge = -1;

  MAUS::DataStructure::Global::TrackPoint* tp0 =
      new MAUS::DataStructure::Global::TrackPoint();
  tp0->set_mapper_name("tp0");
  MAUS::DataStructure::Global::TrackPoint* tp1 =
      new MAUS::DataStructure::Global::TrackPoint();
  tp1->set_mapper_name("tp1");

  size_t detectorpoints = 2;

  std::vector<std::string> geometry_paths;
  std::string path0 = "Nowhere";
  std::string path1 = "Somewhere";
  geometry_paths.push_back(path0);
  geometry_paths.push_back(path1);

  MAUS::DataStructure::Global::Track *conTrack0 =
      new MAUS::DataStructure::Global::Track();
  MAUS::DataStructure::Global::Track *conTrack1 =
      new MAUS::DataStructure::Global::Track();
  MAUS::DataStructure::Global::Track *conTrack2 =
      new MAUS::DataStructure::Global::Track();

  double goodness_of_fit = 7.0;
  double p_value = 0.05;

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
  track1->set_p_value(p_value);

  MAUS::DataStructure::Global::Track *track2 =
      new MAUS::DataStructure::Global::Track(*track1);

  EXPECT_EQ(mapper_name, track2->get_mapper_name());
  EXPECT_EQ(pid, track2->get_pid());
  EXPECT_EQ(charge, track2->get_charge());
  EXPECT_EQ(tp0, track2->get_track_points()->At(0));
  EXPECT_EQ(tp1, track2->get_track_points()->At(1));
  EXPECT_EQ(detectorpoints, track2->get_detectorpoints());
  EXPECT_EQ(path0, track2->get_geometry_paths()[0]);
  EXPECT_EQ(path1, track2->get_geometry_paths()[1]);
  EXPECT_EQ(conTrack0, track2->get_constituent_tracks()->At(0));
  EXPECT_EQ(conTrack1, track2->get_constituent_tracks()->At(1));
  EXPECT_EQ(conTrack2, track2->get_constituent_tracks()->At(2));
  EXPECT_EQ(goodness_of_fit, track2->get_goodness_of_fit());
  EXPECT_EQ(p_value, track2->get_p_value());
}

TEST_F(TrackTestDS, test_assignment_operator) {
  MAUS::DataStructure::Global::Track* track1 =
      new MAUS::DataStructure::Global::Track();

  std::string mapper_name = "0";

  MAUS::DataStructure::Global::PID pid =
      MAUS::DataStructure::Global::kEMinus; // 11

  int charge = -1;

  MAUS::DataStructure::Global::TrackPoint* tp0 =
      new MAUS::DataStructure::Global::TrackPoint();
  tp0->set_mapper_name("tp0");
  MAUS::DataStructure::Global::TrackPoint* tp1 =
      new MAUS::DataStructure::Global::TrackPoint();
  tp1->set_mapper_name("tp1");

  size_t detectorpoints = 2;

  std::vector<std::string> geometry_paths;
  std::string path0 = "Nowhere";
  std::string path1 = "Somewhere";
  geometry_paths.push_back(path0);
  geometry_paths.push_back(path1);

  MAUS::DataStructure::Global::Track *conTrack0 =
      new MAUS::DataStructure::Global::Track();
  MAUS::DataStructure::Global::Track *conTrack1 =
      new MAUS::DataStructure::Global::Track();
  MAUS::DataStructure::Global::Track *conTrack2 =
      new MAUS::DataStructure::Global::Track();

  double goodness_of_fit = 7.0;
  double p_value = 0.05;

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
  track1->set_p_value(p_value);

  MAUS::DataStructure::Global::Track track2;
  track2 = (*track1);

  EXPECT_EQ(mapper_name, track2.get_mapper_name());
  EXPECT_EQ(pid, track2.get_pid());
  EXPECT_EQ(charge, track2.get_charge());
  EXPECT_EQ(tp0, track2.get_track_points()->At(0));
  EXPECT_EQ(tp1, track2.get_track_points()->At(1));
  EXPECT_EQ(detectorpoints, track2.get_detectorpoints());
  EXPECT_EQ(path0, track2.get_geometry_paths()[0]);
  EXPECT_EQ(path1, track2.get_geometry_paths()[1]);
  EXPECT_EQ(conTrack0, track2.get_constituent_tracks()->At(0));
  EXPECT_EQ(conTrack1, track2.get_constituent_tracks()->At(1));
  EXPECT_EQ(conTrack2, track2.get_constituent_tracks()->At(2));
  EXPECT_EQ(goodness_of_fit, track2.get_goodness_of_fit());
  EXPECT_EQ(p_value, track2.get_p_value());
}

TEST_F(TrackTestDS, test_clone_method) {
  MAUS::DataStructure::Global::Track* track1 =
      new MAUS::DataStructure::Global::Track();

  std::string mapper_name = "0";

  MAUS::DataStructure::Global::PID pid =
      MAUS::DataStructure::Global::kEMinus; // 11

  int charge = -1;

  MAUS::DataStructure::Global::TrackPoint* tp0 =
      new MAUS::DataStructure::Global::TrackPoint();
  tp0->set_mapper_name("tp0");
  MAUS::DataStructure::Global::TrackPoint* tp1 =
      new MAUS::DataStructure::Global::TrackPoint();
  tp1->set_mapper_name("tp1");

  size_t detectorpoints = 2;

  std::vector<std::string> geometry_paths;
  std::string path0 = "Nowhere";
  std::string path1 = "Somewhere";
  geometry_paths.push_back(path0);
  geometry_paths.push_back(path1);

  MAUS::DataStructure::Global::Track *conTrack0 =
      new MAUS::DataStructure::Global::Track();
  MAUS::DataStructure::Global::Track *conTrack1 =
      new MAUS::DataStructure::Global::Track();
  MAUS::DataStructure::Global::Track *conTrack2 =
      new MAUS::DataStructure::Global::Track();

  double goodness_of_fit = 7.0;
  double p_value = 0.05;

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
  track1->set_p_value(p_value);

  MAUS::DataStructure::Global::Track *track2 = track1->Clone();

  ASSERT_TRUE(track2);
  EXPECT_NE(track1, track2);

  EXPECT_EQ(mapper_name, track2->get_mapper_name());
  EXPECT_EQ(pid, track2->get_pid());
  EXPECT_EQ(charge, track2->get_charge());
  ASSERT_EQ(2, track2->get_track_points()->GetEntries());

  // New cloned objects, but contents should be the same.
  EXPECT_NE(tp0, track2->get_track_points()->At(0));
  MAUS::DataStructure::Global::TrackPoint* not_tp0 =
      (MAUS::DataStructure::Global::TrackPoint*)
      track2->get_track_points()->At(0);
  EXPECT_EQ("tp0", not_tp0->get_mapper_name());
  EXPECT_NE(tp1, track2->get_track_points()->At(1));
  MAUS::DataStructure::Global::TrackPoint* not_tp1 =
      (MAUS::DataStructure::Global::TrackPoint*)
      track2->get_track_points()->At(1);
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
  EXPECT_EQ(p_value, track2->get_p_value());
}

TEST_F(TrackTestDS, test_Throws) {
  MAUS::DataStructure::Global::Track track0;
  MAUS::DataStructure::Global::Track *track1 =
      new MAUS::DataStructure::Global::Track();
  MAUS::DataStructure::Global::Track *track2 = NULL;

  MAUS::DataStructure::Global::TrackPoint *tp0 = NULL;
  MAUS::DataStructure::Global::TrackPoint *tp1 =
      new MAUS::DataStructure::Global::TrackPoint();
  MAUS::DataStructure::Global::TrackPoint *tp2 =
      new MAUS::DataStructure::Global::TrackPoint();

  ASSERT_THROW(track0.AddTrackPoint(tp0), MAUS::Exceptions::Exception);

  track0.AddTrackPoint(tp1);

  ASSERT_THROW(track0.RemoveTrackPoint(tp0), MAUS::Exceptions::Exception);
  ASSERT_NO_THROW(track0.RemoveTrackPoint(tp1));
  ASSERT_THROW(track0.RemoveTrackPoint(tp2), MAUS::Exceptions::Exception);

  ASSERT_THROW(track0.RemoveTrack(track1), MAUS::Exceptions::Exception);
  ASSERT_THROW(track0.RemoveTrack(track2), MAUS::Exceptions::Exception);

  ASSERT_THROW(track0.RemoveGeometryPath("Narnia"), MAUS::Exceptions::Exception);
}
} // ~namespace MAUS
