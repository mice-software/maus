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
#include "src/common_cpp/Recon/Global/TrackMatching.hh"
#include "src/legacy/BeamTools/BTFieldConstructor.hh"
#include "src/common_cpp/Globals/GlobalsManager.hh"
#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/Simulation/MAUSGeant4Manager.hh"
#include "src/common_cpp/Simulation/DetectorConstruction.hh"
#include "src/common_cpp/Recon/Global/GlobalTools.hh"

namespace MAUS {
namespace recon {
namespace global {


class TrackMatchingTest : public ::testing::Test {
 protected:
  TrackMatchingTest()  {}
  virtual ~TrackMatchingTest() {}
  virtual void SetUp() {
    _global_event = new GlobalEvent;
    _matching_tolerances["TOF0"] = std::make_pair(30.0, 30.0);
    _matching_tolerances["TOF1"] = std::make_pair(40.0, 40.0);
    _matching_tolerances["TOF2"] = std::make_pair(40.0, 40.0);
    _matching_tolerances["KL"] = std::make_pair(10000, 32.0);
    _matching_tolerances["EMR"] = std::make_pair(1.0, 1.0);
    _matching_tolerances["TOF12dT"] = std::make_pair(27.0, 54.0);
    std::map<std::string, double> no_check_thresholds;
    _no_check_settings = std::make_pair(false, no_check_thresholds);
  }
  virtual void TearDown() {}

 public:
  GlobalEvent* _global_event;
  recon::global::TrackMatching* _track_matching;
  std::map<std::string, std::pair<double, double> > _matching_tolerances;
  std::pair<bool, std::map<std::string, double> > _no_check_settings;
};

TEST_F(TrackMatchingTest, USTrack_DSTrack_throughTrack) {
  _track_matching = new recon::global::TrackMatching(_global_event,
      "TrackMatchingTest", "kMuPlus", _matching_tolerances, 10.0, _no_check_settings);

  Simulation::DetectorConstruction* dc =
      Globals::GetInstance()->GetGeant4Manager()->GetGeometry();
  std::string mod_path = std::string(getenv("MAUS_ROOT_DIR"))+
      "/tests/cpp_unit/Recon/Global/TestGeometries/";
  MiceModule geometry(mod_path+"SimpleBeamline.dat");
  dc->SetMiceModules(geometry);

  DataStructure::Global::PID pid = DataStructure::Global::kMuPlus;
  TLorentzVector tof0_pos(0.0, 0.0, 100.0, 0.0);
  TLorentzVector tof1_pos(30.0, 0.0, 7902.0, 31.0);
  TLorentzVector tracker0_pos(-1.11, -20.74, 8537.0, 33.0);
  TLorentzVector tracker0_mom(-5.3091, 16.5831, 164.164, 196.002);
  TLorentzVector tracker1_pos(13.88, 3.38, 14452.0, 57.0);
  TLorentzVector tracker1_mom(16.3777, 3.10231, 157.157, 190.105);
  TLorentzVector tof2_pos(0.0, -30.0, 15260.0, 60.0);
  TLorentzVector kl_pos(0.0, 0.0, 15383.0, 61.0);
  TLorentzVector emr_pos(0.0, -13.0, 16100.0, 64.0);
  TLorentzVector emr_pos_error(2.0, 2.0, 5.0, 10.0);

  // Add Spacepoints for TOFs and KL to global event
  DataStructure::Global::SpacePoint tof0_sp;
  tof0_sp.set_position(tof0_pos);
  tof0_sp.set_detector(DataStructure::Global::kTOF0);
  _global_event->add_space_point(&tof0_sp);
  DataStructure::Global::SpacePoint tof1_sp;
  tof1_sp.set_position(tof1_pos);
  tof1_sp.set_detector(DataStructure::Global::kTOF1);
  _global_event->add_space_point(&tof1_sp);
  DataStructure::Global::SpacePoint tof2_sp;
  tof2_sp.set_position(tof2_pos);
  tof2_sp.set_detector(DataStructure::Global::kTOF2);
  _global_event->add_space_point(&tof2_sp);
  DataStructure::Global::SpacePoint kl_sp;
  kl_sp.set_position(kl_pos);
  kl_sp.set_detector(DataStructure::Global::kCalorimeter);
  _global_event->add_space_point(&kl_sp);

  // For Tracker and EMR make trackpoints and tracks
  DataStructure::Global::SpacePoint sp;
  sp.set_position(tracker0_pos);
  DataStructure::Global::TrackPoint tracker0_tp(&sp);
  tracker0_tp.set_momentum(tracker0_mom);
  tracker0_tp.set_detector(DataStructure::Global::kTracker0);
  DataStructure::Global::Track tracker0_track;
  tracker0_track.AddTrackPoint(&tracker0_tp);
  _global_event->add_track_recursive(&tracker0_track);

  sp.set_position(tracker1_pos);
  DataStructure::Global::TrackPoint tracker1_tp(&sp);
  tracker1_tp.set_momentum(tracker1_mom);
  tracker1_tp.set_detector(DataStructure::Global::kTracker1);
  DataStructure::Global::Track tracker1_track;
  tracker1_track.AddTrackPoint(&tracker1_tp);
  _global_event->add_track_recursive(&tracker1_track);

  sp.set_position(emr_pos);
  sp.set_position_error(emr_pos_error);
  DataStructure::Global::TrackPoint emr_tp(&sp);
  emr_tp.set_detector(DataStructure::Global::kEMR);
  DataStructure::Global::Track emr_track;
  emr_track.AddTrackPoint(&emr_tp);
  emr_track.set_emr_range_primary(20.0);
  _global_event->add_track_recursive(&emr_track);

  _track_matching->USTrack();
  _track_matching->DSTrack();
  _track_matching->throughTrack();

  DataStructure::Global::TrackPArray* us_tracks =
      new DataStructure::Global::TrackPArray();
  DataStructure::Global::TrackPArray* ds_tracks =
      new DataStructure::Global::TrackPArray();
  DataStructure::Global::TrackPArray* through_tracks =
      new DataStructure::Global::TrackPArray();

  DataStructure::Global::TrackPArray* global_tracks =
    _global_event->get_tracks();
  DataStructure::Global::TrackPArray::iterator global_track_iter;
  for (global_track_iter = global_tracks->begin();
       global_track_iter != global_tracks->end();
       ++global_track_iter) {
    if (((*global_track_iter)->get_mapper_name() ==
            "MapCppGlobalTrackMatching-US") and
        ((*global_track_iter)->get_pid() == pid)) {
      us_tracks->push_back(*global_track_iter);
    } else if (((*global_track_iter)->get_mapper_name() ==
                   "MapCppGlobalTrackMatching-DS") and
               ((*global_track_iter)->get_pid() == pid)) {
      ds_tracks->push_back(*global_track_iter);
    } else if (((*global_track_iter)->get_mapper_name() ==
                   "MapCppGlobalTrackMatching-Through") and
               ((*global_track_iter)->get_pid() == pid)) {
      through_tracks->push_back(*global_track_iter);
    }
  }
  EXPECT_EQ(us_tracks->size(), 1);
  EXPECT_EQ(ds_tracks->size(), 1);
  EXPECT_EQ(through_tracks->size(), 1);
  if (us_tracks->size() > 0) {
    EXPECT_EQ(us_tracks->at(0)->GetTrackPoints().size(), 3);
  }
  if (ds_tracks->size() > 0) {
    EXPECT_EQ(ds_tracks->at(0)->GetTrackPoints().size(), 4);
    EXPECT_FLOAT_EQ(ds_tracks->at(0)->get_emr_range_primary(), 20.0);
  }
  if (through_tracks->size() > 0) {
    EXPECT_EQ(through_tracks->at(0)->GetTrackPoints().size(), 7);
    EXPECT_FLOAT_EQ(ds_tracks->at(0)->get_emr_range_primary(), 20.0);
  }

  delete us_tracks;
  delete ds_tracks;
  delete through_tracks;
}

TEST_F(TrackMatchingTest, GetDetectorTrackArray) {
  _track_matching = new recon::global::TrackMatching(_global_event,
      "TrackMatchingTest", "kMuPlus", _matching_tolerances, 10.0, _no_check_settings);

  DataStructure::Global::Track tracker0_track1;
  tracker0_track1.SetDetector(DataStructure::Global::kTracker0);
  tracker0_track1.set_mapper_name("Tracker0");
  _global_event->add_track(&tracker0_track1);

  DataStructure::Global::Track tracker0_track2;
  tracker0_track2.SetDetector(DataStructure::Global::kTracker0);
  tracker0_track2.set_mapper_name("Tracker0");
  _global_event->add_track(&tracker0_track2);

  DataStructure::Global::Track tracker1_track;
  tracker1_track.SetDetector(DataStructure::Global::kTracker1);
  tracker1_track.set_mapper_name("Tracker1");
  _global_event->add_track(&tracker1_track);

  DataStructure::Global::Track emr_primary_track;
  emr_primary_track.SetDetector(DataStructure::Global::kEMR);
  emr_primary_track.set_mapper_name("EMR Primary");
  emr_primary_track.set_emr_range_primary(10.0);
  _global_event->add_track(&emr_primary_track);

  DataStructure::Global::Track emr_secondary_track;
  emr_secondary_track.SetDetector(DataStructure::Global::kEMR);
  emr_secondary_track.set_mapper_name("EMR Secondary");
  emr_secondary_track.set_emr_range_secondary(10.0);
  _global_event->add_track(&emr_secondary_track);

  DataStructure::Global::TrackPArray* tracker0_array =
      _track_matching->GetDetectorTrackArray(
          DataStructure::Global::kTracker0);
  EXPECT_EQ(tracker0_array->size(), 2);
  if (tracker0_array->size() > 1) {
    EXPECT_EQ(tracker0_array->at(0)->get_mapper_name(), "Tracker0");
    EXPECT_EQ(tracker0_array->at(0)->get_mapper_name(), "Tracker0");
  }

  DataStructure::Global::TrackPArray* tracker1_array =
      _track_matching->GetDetectorTrackArray(
          DataStructure::Global::kTracker1);
  EXPECT_EQ(tracker1_array->size(), 1);
  if (tracker1_array->size() > 0) {
    EXPECT_EQ(tracker1_array->at(0)->get_mapper_name(), "Tracker1");
  }

  DataStructure::Global::TrackPArray* emr_array =
      _track_matching->GetDetectorTrackArray(
          DataStructure::Global::kEMR);
  EXPECT_EQ(emr_array->size(), 1);
  if (emr_array->size() > 0) {
    EXPECT_EQ(emr_array->at(0)->get_mapper_name(), "EMR Primary");
  }
}

TEST_F(TrackMatchingTest, GetDetectorSpacePoints) {
  _track_matching = new recon::global::TrackMatching(_global_event,
      "TrackMatchingTest", "kMuPlus", _matching_tolerances, 10.0, _no_check_settings);

  DataStructure::Global::DetectorPoint tof0 = DataStructure::Global::kTOF0;
  DataStructure::Global::DetectorPoint tof1 = DataStructure::Global::kTOF1;
  DataStructure::Global::DetectorPoint tof2 = DataStructure::Global::kTOF2;
  DataStructure::Global::DetectorPoint kl = DataStructure::Global::kCalorimeter;

  DataStructure::Global::SpacePoint tof0_sp1;
  TLorentzVector position(0.0, 0.0, 100.0, 0.0);
  tof0_sp1.set_position(position);
  tof0_sp1.set_detector(tof0);
  DataStructure::Global::SpacePoint tof0_sp2;
  tof0_sp2.set_position(position);
  tof0_sp2.set_detector(tof0);
  DataStructure::Global::SpacePoint tof1_sp;
  position.SetZ(300.0);
  tof1_sp.set_position(position);
  tof1_sp.set_detector(tof1);
  DataStructure::Global::SpacePoint tof2_sp;
  position.SetZ(800.0);
  tof2_sp.set_position(position);
  tof2_sp.set_detector(tof2);
  DataStructure::Global::SpacePoint kl_sp;
  position.SetZ(900.0);
  kl_sp.set_position(position);
  kl_sp.set_detector(kl);

  _global_event->add_space_point(&tof0_sp1);
  _global_event->add_space_point(&tof0_sp2);
  _global_event->add_space_point(&tof1_sp);
  _global_event->add_space_point(&tof2_sp);
  _global_event->add_space_point(&kl_sp);

  std::vector<DataStructure::Global::SpacePoint*> tof0_sps;
  std::vector<DataStructure::Global::SpacePoint*> tof1_sps;
  std::vector<DataStructure::Global::SpacePoint*> tof2_sps;
  std::vector<DataStructure::Global::SpacePoint*> kl_sps;

  tof0_sps = _track_matching->GetDetectorSpacePoints(tof0);
  tof1_sps = _track_matching->GetDetectorSpacePoints(tof1);
  tof2_sps = _track_matching->GetDetectorSpacePoints(tof2);
  kl_sps = _track_matching->GetDetectorSpacePoints(kl);

  EXPECT_EQ(tof0_sps.size(), 2);
  EXPECT_EQ(tof1_sps.size(), 1);
  EXPECT_EQ(tof2_sps.size(), 1);
  EXPECT_EQ(kl_sps.size(), 1);

  if (tof0_sps.size() > 1) {
    EXPECT_EQ(tof0_sps[0]->get_detector(), tof0);
    EXPECT_EQ(tof0_sps[1]->get_detector(), tof0);
  }

  if (tof1_sps.size() > 0) {
    EXPECT_EQ(tof1_sps[0]->get_detector(), tof1);
  }

  if (tof2_sps.size() > 0) {
    EXPECT_EQ(tof2_sps[0]->get_detector(), tof2);
  }

  if (kl_sps.size() > 0) {
    EXPECT_EQ(kl_sps[0]->get_detector(), kl);
  }
}

TEST_F(TrackMatchingTest, PIDHypotheses) {
  _track_matching = new recon::global::TrackMatching(_global_event,
      "TrackMatchingTest", "kMuPlus", _matching_tolerances, 10.0, _no_check_settings);

  std::vector<DataStructure::Global::PID> pids =
      _track_matching->PIDHypotheses(0, "kMuPlus");
  EXPECT_EQ(pids.size(), 1);
  if (pids.size() > 0) {
    EXPECT_EQ(pids[0], DataStructure::Global::kMuPlus);
  }

  pids = _track_matching->PIDHypotheses(1, "kPiMinus");
  EXPECT_EQ(pids.size(), 1);
  if (pids.size() > 0) {
    EXPECT_EQ(pids[0], DataStructure::Global::kPiMinus);
  }

  pids = _track_matching->PIDHypotheses(-1, "all");
  EXPECT_EQ(pids.size(), 3);
  if (pids.size() > 2) {
    EXPECT_EQ(pids[0], DataStructure::Global::kEMinus);
    EXPECT_EQ(pids[1], DataStructure::Global::kMuMinus);
    EXPECT_EQ(pids[2], DataStructure::Global::kPiMinus);
  }

  pids = _track_matching->PIDHypotheses(0, "all");
  EXPECT_EQ(pids.size(), 6);
  if (pids.size() > 5) {
    EXPECT_EQ(pids[0], DataStructure::Global::kEPlus);
    EXPECT_EQ(pids[1], DataStructure::Global::kMuPlus);
    EXPECT_EQ(pids[2], DataStructure::Global::kPiPlus);
    EXPECT_EQ(pids[3], DataStructure::Global::kEMinus);
    EXPECT_EQ(pids[4], DataStructure::Global::kMuMinus);
    EXPECT_EQ(pids[5], DataStructure::Global::kPiMinus);
  }
}

TEST_F(TrackMatchingTest, MatchTrackPoint) {
  _track_matching = new recon::global::TrackMatching(_global_event,
      "TrackMatchingTest", "kMuPlus", _matching_tolerances, 10.0, _no_check_settings);

  Simulation::DetectorConstruction* dc =
      Globals::GetInstance()->GetGeant4Manager()->GetGeometry();
  std::string mod_path = std::string(getenv("MAUS_ROOT_DIR"))+
      "/tests/cpp_unit/Recon/Global/TestGeometries/";
  MiceModule geometry(mod_path+"PropagationTest.dat");
  dc->SetMiceModules(geometry);

  DataStructure::Global::SpacePoint tof1_sp1;
  TLorentzVector tof1_position(30.0, 0.0, 100.0, 0.0);
  tof1_sp1.set_position(tof1_position);
  tof1_sp1.set_detector(DataStructure::Global::kTOF1);
  _global_event->add_space_point(&tof1_sp1);
  DataStructure::Global::SpacePoint tof1_sp2;
  tof1_position.SetY(60.0);
  tof1_sp2.set_position(tof1_position);
  tof1_sp2.set_detector(DataStructure::Global::kTOF1);
  _global_event->add_space_point(&tof1_sp2);
  DataStructure::Global::SpacePoint tof2_sp;
  TLorentzVector tof2_position(0.0, -30.0, 9000.0, 0.0);
  tof2_sp.set_position(tof2_position);
  tof2_sp.set_detector(DataStructure::Global::kTOF2);
  _global_event->add_space_point(&tof2_sp);

  std::vector<DataStructure::Global::SpacePoint*> TOF1_sp =
      _track_matching->GetDetectorSpacePoints(
      DataStructure::Global::kTOF1);
  std::vector<DataStructure::Global::SpacePoint*> TOF2_sp =
      _track_matching->GetDetectorSpacePoints(
      DataStructure::Global::kTOF2);
  TLorentzVector start_position(0.0, 0.0, 4000.0, 0.0);
  TLorentzVector start_momentum(10.0, 10.0, 150.0, 0.0);
  DataStructure::Global::PID pid = DataStructure::Global::kMuPlus;
  BTFieldConstructor* field = Globals::GetMCFieldConstructor();
  DataStructure::Global::Track hypothesis_track;

  _track_matching->MatchTrackPoint(start_position, start_momentum, TOF1_sp, pid,
      field, "TOF1", &hypothesis_track);

  _track_matching->MatchTrackPoint(start_position, start_momentum, TOF2_sp, pid,
      field, "TOF2", &hypothesis_track);

  std::vector<const DataStructure::Global::TrackPoint*> track_points =
      hypothesis_track.GetTrackPoints();
  std::vector<const DataStructure::Global::TrackPoint*> tof1_tps =
      hypothesis_track.GetTrackPoints(DataStructure::Global::kTOF1);
  std::vector<const DataStructure::Global::TrackPoint*> tof2_tps =
      hypothesis_track.GetTrackPoints(DataStructure::Global::kTOF2);

  EXPECT_EQ(track_points.size(), 2);
  EXPECT_EQ(tof1_tps.size(), 1);
  EXPECT_EQ(tof2_tps.size(), 1);
  if (tof1_tps.size() > 0) {
    EXPECT_FLOAT_EQ(tof1_tps.at(0)->get_position().Y(), 0.0);
  }
}


TEST_F(TrackMatchingTest, MatchTOF0) {
  std::cerr << "Not currently implemented\n";
/*
  _track_matching = new recon::global::TrackMatching(_global_event,
      "TrackMatchingTest", "kMuPlus", _matching_tolerances, 10.0, _no_check_settings);

  Simulation::DetectorConstruction* dc =
      Globals::GetInstance()->GetGeant4Manager()->GetGeometry();
  std::string mod_path = std::string(getenv("MAUS_ROOT_DIR"))+
      "/tests/cpp_unit/Recon/Global/TestGeometries/";
  MiceModule geometry(mod_path+"SimpleBeamline.dat");
  dc->SetMiceModules(geometry);

  DataStructure::Global::SpacePoint tof0_sp1;
  TLorentzVector tof0_position(0.0, 0.0, 100.0, 10.0);
  tof0_sp1.set_position(tof0_position);
  tof0_sp1.set_detector(DataStructure::Global::kTOF0);
  _global_event->add_space_point(&tof0_sp1);
  DataStructure::Global::SpacePoint tof0_sp2;
  tof0_position.SetT(13.0);
  tof0_sp2.set_position(tof0_position);
  tof0_sp2.set_detector(DataStructure::Global::kTOF0);
  _global_event->add_space_point(&tof0_sp2);


  std::vector<DataStructure::Global::SpacePoint*> TOF0_sp =
      _track_matching->GetDetectorSpacePoints(DataStructure::Global::kTOF0);

  TLorentzVector position(0.0, 0.0, 7902.0, 41.0);
  TLorentzVector momentum(10.0, 10.0, 150.0, 0.0);
  DataStructure::Global::PID pid = DataStructure::Global::kMuPlus;
  DataStructure::Global::Track hypothesis_track;
  _track_matching->MatchTOF0(position, momentum, TOF0_sp, pid,
                             &hypothesis_track);

  std::vector<const DataStructure::Global::TrackPoint*> track_points =
      hypothesis_track.GetTrackPoints();

  EXPECT_EQ(track_points.size(), 1);
  if (track_points.size() > 0) {
    EXPECT_FLOAT_EQ(track_points.at(0)->get_position().T(), 10.0);
  }
*/
}

TEST_F(TrackMatchingTest, MatchEMRTrack) {
  _track_matching = new recon::global::TrackMatching(_global_event,
      "TrackMatchingTest", "kMuPlus", _matching_tolerances, 10.0, _no_check_settings);

  Simulation::DetectorConstruction* dc =
      Globals::GetInstance()->GetGeant4Manager()->GetGeometry();
  std::string mod_path = std::string(getenv("MAUS_ROOT_DIR"))+
      "/tests/cpp_unit/Recon/Global/TestGeometries/";
  MiceModule geometry(mod_path+"PropagationTest.dat");
  dc->SetMiceModules(geometry);

  DataStructure::Global::SpacePoint sp;
  TLorentzVector position(30.0, -45.0, 4000.0, 0.0);
  TLorentzVector position_error(10.0, 10.0, 10.0, 10.0);
  sp.set_position(position);
  sp.set_position_error(position_error);
  DataStructure::Global::TrackPoint emr_tp1(&sp);
  emr_tp1.set_detector(DataStructure::Global::kEMR);
  position.SetX(-30.0);
  sp.set_position(position);
  DataStructure::Global::TrackPoint emr_tp2(&sp);
  emr_tp2.set_detector(DataStructure::Global::kEMR);

  DataStructure::Global::Track emr_track1;
  emr_track1.AddTrackPoint(&emr_tp1);
  DataStructure::Global::Track emr_track2;
  emr_track2.AddTrackPoint(&emr_tp2);

  _global_event->add_track_recursive(&emr_track1);
  _global_event->add_track_recursive(&emr_track2);

  DataStructure::Global::TrackPArray* emr_track_array =
      _track_matching->GetDetectorTrackArray(DataStructure::Global::kEMR);

  TLorentzVector start_position(0.0, 0.0, 500.0, 0.0);
  TLorentzVector start_momentum(20.0, 20.0, 150.0, 0.0);
  DataStructure::Global::PID pid = DataStructure::Global::kMuPlus;
  BTFieldConstructor* field = Globals::GetMCFieldConstructor();
  DataStructure::Global::Track hypothesis_track;

  _track_matching->MatchEMRTrack(start_position, start_momentum,
      emr_track_array, pid, field, &hypothesis_track);

  std::vector<const DataStructure::Global::TrackPoint*> track_points =
      hypothesis_track.GetTrackPoints();

  EXPECT_EQ(track_points.size(), 1);
  if (track_points.size() > 0) {
    EXPECT_FLOAT_EQ(track_points.at(0)->get_position().X(), 30.0);
  }
}

TEST_F(TrackMatchingTest, AddTrackerTrackPoints) {
  _track_matching = new recon::global::TrackMatching(_global_event,
      "TrackMatchingTest", "kMuPlus", _matching_tolerances, 10.0, _no_check_settings);

  double mass = 105.65837;
  std::string mapper_name = "AddTrackPointsTest";
  DataStructure::Global::Track tracker_track;
  DataStructure::Global::Track target_track;
  DataStructure::Global::SpacePoint sp;
  double z_positions[] = {150.0, 200.0, 300.0};
  TLorentzVector position(0.0, 0.0, z_positions[0], 0.0);
  TLorentzVector momentum(0.0, 0.0, 150.0, 0.0);
  sp.set_position(position);
  DataStructure::Global::TrackPoint tp1(&sp);
  tp1.set_momentum(momentum);
  position.SetZ(z_positions[2]);
  sp.set_position(position);
  DataStructure::Global::TrackPoint tp2(&sp);
  tp2.set_momentum(momentum);
  position.SetZ(z_positions[1]);
  sp.set_position(position);
  DataStructure::Global::TrackPoint tp3(&sp);
  tp3.set_momentum(momentum);
  tracker_track.AddTrackPoint(&tp1);
  tracker_track.AddTrackPoint(&tp2);
  tracker_track.AddTrackPoint(&tp3);
  _track_matching->AddTrackerTrackPoints(&tracker_track, mapper_name, mass,
      &target_track);
  std::vector<const DataStructure::Global::TrackPoint*> track_points =
      target_track.GetTrackPoints();

  EXPECT_EQ(track_points.size(), 3);
  for (size_t i = 0; i < track_points.size(); i++) {
    EXPECT_NEAR(track_points[i]->get_position().Z(), z_positions[i], 0.001);
    EXPECT_NEAR(track_points[i]->get_momentum().E(),
        ::sqrt(mass*mass + 150.0*150.0), 0.001);
    EXPECT_EQ(track_points[i]->get_mapper_name(), "AddTrackPointsTest");
  }
}

TEST_F(TrackMatchingTest, USDSTracks) {
  _track_matching = new recon::global::TrackMatching(_global_event,
      "TrackMatchingTest", "kMuPlus", _matching_tolerances, 10.0, _no_check_settings);

  std::vector<DataStructure::Global::Track*> global_tracks;
  std::vector<DataStructure::Global::Track*> us_tracks1;
  std::vector<DataStructure::Global::Track*> us_tracks2;
  std::vector<DataStructure::Global::Track*> ds_tracks1;
  std::vector<DataStructure::Global::Track*> ds_tracks2;
  DataStructure::Global::Track us_muplus_track1;
  DataStructure::Global::Track us_muplus_track2;
  DataStructure::Global::Track us_muplus_track3;
  DataStructure::Global::Track ds_muplus_track;
  DataStructure::Global::Track ds_piminus_track;
  us_muplus_track1.set_mapper_name("MapCppGlobalTrackMatching-US");
  us_muplus_track2.set_mapper_name("MapCppGlobalTrackMatching-US");
  us_muplus_track3.set_mapper_name("MapCppGlobalTrackMatching-US");
  ds_muplus_track.set_mapper_name("MapCppGlobalTrackMatching-DS");
  ds_piminus_track.set_mapper_name("MapCppGlobalTrackMatching-DS");
  us_muplus_track1.set_pid(DataStructure::Global::kMuPlus);
  us_muplus_track2.set_pid(DataStructure::Global::kMuPlus);
  us_muplus_track3.set_pid(DataStructure::Global::kMuPlus);
  ds_muplus_track.set_pid(DataStructure::Global::kMuPlus);
  ds_piminus_track.set_pid(DataStructure::Global::kPiMinus);
  us_muplus_track1.SetDetector(DataStructure::Global::kTOF1);
  us_muplus_track2.SetDetector(DataStructure::Global::kTOF1);
  ds_muplus_track.SetDetector(DataStructure::Global::kTOF2);
  ds_piminus_track.SetDetector(DataStructure::Global::kTOF2);
  global_tracks.push_back(&us_muplus_track1);
  global_tracks.push_back(&us_muplus_track2);
  global_tracks.push_back(&us_muplus_track3);
  global_tracks.push_back(&ds_muplus_track);
  global_tracks.push_back(&ds_piminus_track);

  _track_matching->USDSTracks(&global_tracks,
      DataStructure::Global::kMuPlus, &us_tracks1, &ds_tracks1);
  _track_matching->USDSTracks(&global_tracks,
      DataStructure::Global::kPiMinus, &us_tracks2, &ds_tracks2);

  EXPECT_EQ(us_tracks1.size(), 2);
  EXPECT_EQ(ds_tracks1.size(), 1);
  EXPECT_EQ(us_tracks2.size(), 0);
  EXPECT_EQ(ds_tracks2.size(), 1);

  if (us_tracks1.size() > 1) {
    EXPECT_EQ(us_tracks1[0]->get_mapper_name(), "MapCppGlobalTrackMatching-US");
    EXPECT_EQ(us_tracks1[0]->get_pid(), DataStructure::Global::kMuPlus);
    EXPECT_TRUE(us_tracks1[0]->HasDetector(DataStructure::Global::kTOF1));
    EXPECT_EQ(us_tracks1[1]->get_mapper_name(), "MapCppGlobalTrackMatching-US");
    EXPECT_EQ(us_tracks1[1]->get_pid(), DataStructure::Global::kMuPlus);
    EXPECT_TRUE(us_tracks1[1]->HasDetector(DataStructure::Global::kTOF1));
  }

  if (ds_tracks1.size() > 0) {
    EXPECT_EQ(ds_tracks1[0]->get_mapper_name(), "MapCppGlobalTrackMatching-DS");
    EXPECT_EQ(ds_tracks1[0]->get_pid(), DataStructure::Global::kMuPlus);
    EXPECT_TRUE(ds_tracks1[0]->HasDetector(DataStructure::Global::kTOF2));
  }

  if (ds_tracks2.size() > 0) {
    EXPECT_EQ(ds_tracks2[0]->get_mapper_name(), "MapCppGlobalTrackMatching-DS");
    EXPECT_EQ(ds_tracks2[0]->get_pid(), DataStructure::Global::kPiMinus);
    EXPECT_TRUE(ds_tracks2[0]->HasDetector(DataStructure::Global::kTOF2));
  }
}

TEST_F(TrackMatchingTest, MatchUSDS) {
  _track_matching = new recon::global::TrackMatching(_global_event,
      "TrackMatchingTest", "kMuPlus", _matching_tolerances, 10.0, _no_check_settings);

  DataStructure::Global::SpacePoint tof1_sp;
  TLorentzVector tof1_position(0.0, 0.0, 100.0, 20.0);
  tof1_sp.set_position(tof1_position);
  tof1_sp.set_detector(DataStructure::Global::kTOF1);
  DataStructure::Global::TrackPoint tof1_tp(&tof1_sp);
  DataStructure::Global::SpacePoint tof2_sp;
  TLorentzVector tof2_position(0.0, 0.0, 8300.0, 65.0);
  tof2_sp.set_position(tof2_position);
  tof2_sp.set_detector(DataStructure::Global::kTOF2);
  DataStructure::Global::TrackPoint tof2_tp(&tof2_sp);

  DataStructure::Global::Track* tof1_track = new DataStructure::Global::Track;
  tof1_track->AddTrackPoint(&tof1_tp);
  DataStructure::Global::Track* tof2_track = new DataStructure::Global::Track;
  tof2_track->AddTrackPoint(&tof2_tp);

  DataStructure::Global::PID pid = DataStructure::Global::kMuPlus;

  _track_matching->MatchUSDS(tof1_track, tof2_track, pid, 15.0);

  std::vector<DataStructure::Global::Track*>* through_tracks =
      GlobalTools::GetTracksByMapperName(_global_event,
      "MapCppGlobalTrackMatching-Through");

  EXPECT_EQ(through_tracks->size(), 1);
  if (through_tracks->size() > 0) {
    EXPECT_FLOAT_EQ(through_tracks->at(0)->get_emr_range_primary(), 15.0);
    EXPECT_EQ(through_tracks->at(0)->get_pid(),
        DataStructure::Global::kMuPlus);
    EXPECT_EQ(through_tracks->at(0)->GetTrackPoints().size(), 2);
    if (through_tracks->at(0)->GetTrackPoints().size() > 1) {
      const DataStructure::Global::TrackPoint* tof1_tp =
          through_tracks->at(0)->GetTrackPoints().at(0);
      const DataStructure::Global::TrackPoint* tof2_tp =
          through_tracks->at(0)->GetTrackPoints().at(1);
      EXPECT_FLOAT_EQ(tof1_tp->get_position().Z(), 100.0);
      EXPECT_FLOAT_EQ(tof2_tp->get_position().Z(), 8300.0);
    }
  }
}

TEST_F(TrackMatchingTest, TOFTimeFromTrackPoints) {
  _track_matching = new recon::global::TrackMatching(_global_event,
      "TrackMatchingTest", "kMuPlus", _matching_tolerances, 10.0, _no_check_settings);

  std::vector<const DataStructure::Global::TrackPoint*> track_points;
  DataStructure::Global::SpacePoint sp;
  TLorentzVector position(0.0, 0.0, 0.0, 10.0);
  sp.set_position(position);
  DataStructure::Global::TrackPoint tp1(&sp);
  tp1.set_detector(DataStructure::Global::kTOF1);
  position.SetT(15.7);
  sp.set_position(position);
  DataStructure::Global::TrackPoint tp2(&sp);
  tp2.set_detector(DataStructure::Global::kTOF2);
  // rather than fiddling around with casting we just use the internal track
  // mechanics
  DataStructure::Global::Track track;
  track.AddTrackPoint(&tp1);
  track.AddTrackPoint(&tp2);
  track_points = track.GetTrackPoints();
  double TOF_time;

  TOF_time = _track_matching->TOFTimeFromTrackPoints(track_points,
      DataStructure::Global::kTOF1);
  EXPECT_FLOAT_EQ(TOF_time, 10.0);
  TOF_time = _track_matching->TOFTimeFromTrackPoints(track_points,
      DataStructure::Global::kTOF2);
  EXPECT_FLOAT_EQ(TOF_time, 15.7);
}

TEST_F(TrackMatchingTest, AddIfConsistent) {
  std::cerr << "Not currently implemented\n";
}
}
}
} // ~namespace MAUS
