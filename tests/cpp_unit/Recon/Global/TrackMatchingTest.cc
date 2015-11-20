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
//~ #include <Geant4/G4NistManager.hh> 
#include "gtest/gtest.h"
//~ #include "src/common_cpp/Utils/JsonWrapper.hh"
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
    _global_event = new MAUS::GlobalEvent;
    _matching_tolerances["TOF0"] = std::make_pair(30.0, 30.0);
    _matching_tolerances["TOF1"] = std::make_pair(40.0, 40.0);
    _matching_tolerances["TOF2"] = std::make_pair(40.0, 40.0);
    _matching_tolerances["KL"] = std::make_pair(10000, 32.0);
    _matching_tolerances["TOF12dT"] = std::make_pair(27.0, 54.0);
  }
  virtual void TearDown() {}

 public:
  MAUS::GlobalEvent* _global_event;
  MAUS::recon::global::TrackMatching* _track_matching;
  std::map<std::string, std::pair<double, double> > _matching_tolerances;
};

TEST_F(TrackMatchingTest, USTrack_DSTrack_throughTrack) {
  _track_matching = new MAUS::recon::global::TrackMatching(_global_event,
      "TrackMatchingTest", "kMuPlus", _matching_tolerances, 10.0);

  MAUS::Simulation::DetectorConstruction* dc =
      Globals::GetInstance()->GetGeant4Manager()->GetGeometry();
  std::string mod_path = std::string(getenv("MAUS_ROOT_DIR"))+
      "/tests/cpp_unit/Recon/Global/TestGeometries/";
  MiceModule geometry(mod_path+"SimpleBeamline.dat");
  dc->SetMiceModules(geometry);

  MAUS::DataStructure::Global::PID pid = MAUS::DataStructure::Global::kMuPlus;
  TLorentzVector tof0_pos(0.0, 0.0, 100.0, 0.0);
  TLorentzVector tof1_pos(30.0, 0.0, 7902.0, 31.0);
  TLorentzVector tracker0_pos(-1.11, -20.74, 8537.0, 33.0);
  TLorentzVector tracker0_mom(-5.3091, 16.5831, 164.164, 196.002);
  TLorentzVector tracker1_pos(13.88, 3.38, 14452.0, 57.0);
  TLorentzVector tracker1_mom(16.3777, 3.10231, 157.157, 190.105);
  TLorentzVector tof2_pos(0.0, -30.0, 15260.0, 60.0);
  TLorentzVector kl_pos(0.0, 0.0, 15383.0, 61.0);
  TLorentzVector emr_pos(0.0, -13.2, 16100.0, 64.0);
  TLorentzVector emr_pos_error(2.0, 2.0, 5.0, 10.0);

  // Add Spacepoints for TOFs and KL to global event
  MAUS::DataStructure::Global::SpacePoint tof0_sp;
  tof0_sp.set_position(tof0_pos);
  tof0_sp.set_detector(MAUS::DataStructure::Global::kTOF0);
  _global_event->add_space_point(&tof0_sp);
  MAUS::DataStructure::Global::SpacePoint tof1_sp;
  tof1_sp.set_position(tof1_pos);
  tof1_sp.set_detector(MAUS::DataStructure::Global::kTOF1);
  _global_event->add_space_point(&tof1_sp);
  MAUS::DataStructure::Global::SpacePoint tof2_sp;
  tof2_sp.set_position(tof2_pos);
  tof2_sp.set_detector(MAUS::DataStructure::Global::kTOF2);
  _global_event->add_space_point(&tof2_sp);
  MAUS::DataStructure::Global::SpacePoint kl_sp;
  kl_sp.set_position(kl_pos);
  kl_sp.set_detector(MAUS::DataStructure::Global::kCalorimeter);
  _global_event->add_space_point(&kl_sp);

  // For Tracker and EMR make trackpoints and tracks
  MAUS::DataStructure::Global::SpacePoint sp;
  sp.set_position(tracker0_pos);
  MAUS::DataStructure::Global::TrackPoint tracker0_tp(&sp);
  tracker0_tp.set_momentum(tracker0_mom);
  tracker0_tp.set_detector(MAUS::DataStructure::Global::kTracker0);
  MAUS::DataStructure::Global::Track tracker0_track;
  tracker0_track.AddTrackPoint(&tracker0_tp);
  _global_event->add_track(&tracker0_track);

  sp.set_position(tracker1_pos);
  MAUS::DataStructure::Global::TrackPoint tracker1_tp(&sp);
  tracker1_tp.set_momentum(tracker1_mom);
  tracker1_tp.set_detector(MAUS::DataStructure::Global::kTracker1);
  MAUS::DataStructure::Global::Track tracker1_track;
  tracker1_track.AddTrackPoint(&tracker1_tp);
  _global_event->add_track(&tracker1_track);

  sp.set_position(emr_pos);
  sp.set_position_error(emr_pos_error);
  MAUS::DataStructure::Global::TrackPoint emr_tp(&sp);
  emr_tp.set_detector(MAUS::DataStructure::Global::kEMR);
  MAUS::DataStructure::Global::Track emr_track;
  emr_track.AddTrackPoint(&emr_tp);
  emr_track.set_emr_range_primary(20.0);
  _global_event->add_track(&emr_track);

  _track_matching->USTrack();
  _track_matching->DSTrack();
  _track_matching->throughTrack();

  MAUS::DataStructure::Global::TrackPArray* us_tracks =
      new MAUS::DataStructure::Global::TrackPArray();
  MAUS::DataStructure::Global::TrackPArray* ds_tracks =
      new MAUS::DataStructure::Global::TrackPArray();
  MAUS::DataStructure::Global::TrackPArray* through_tracks =
      new MAUS::DataStructure::Global::TrackPArray();
      
  MAUS::DataStructure::Global::TrackPArray* global_tracks =
    _global_event->get_tracks();
  MAUS::DataStructure::Global::TrackPArray::iterator global_track_iter;
  for (global_track_iter = global_tracks->begin();
       global_track_iter != global_tracks->end();
       ++global_track_iter) {
    if (((*global_track_iter)->get_mapper_name() ==
            "MapCppGlobalTrackMatching-US")
        and ((*global_track_iter)->get_pid() == pid)) {
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
  _track_matching = new MAUS::recon::global::TrackMatching(_global_event,
      "TrackMatchingTest", "kMuPlus", _matching_tolerances, 10.0);

  MAUS::DataStructure::Global::Track tracker0_track1;
  tracker0_track1.SetDetector(MAUS::DataStructure::Global::kTracker0);
  tracker0_track1.set_mapper_name("Tracker0");
  _global_event->add_track(&tracker0_track1);
  
  MAUS::DataStructure::Global::Track tracker0_track2;
  tracker0_track2.SetDetector(MAUS::DataStructure::Global::kTracker0);
  tracker0_track2.set_mapper_name("Tracker0");
  _global_event->add_track(&tracker0_track2);
  
  MAUS::DataStructure::Global::Track tracker1_track;
  tracker1_track.SetDetector(MAUS::DataStructure::Global::kTracker1);
  tracker1_track.set_mapper_name("Tracker1");
  _global_event->add_track(&tracker1_track);
  
  MAUS::DataStructure::Global::Track emr_primary_track;
  emr_primary_track.SetDetector(MAUS::DataStructure::Global::kEMR);
  emr_primary_track.set_mapper_name("EMR Primary");
  emr_primary_track.set_emr_range_primary(10.0);
  _global_event->add_track(&emr_primary_track);

  MAUS::DataStructure::Global::Track emr_secondary_track;
  emr_secondary_track.SetDetector(MAUS::DataStructure::Global::kEMR);
  emr_secondary_track.set_mapper_name("EMR Secondary");
  emr_secondary_track.set_emr_range_secondary(10.0);
  _global_event->add_track(&emr_secondary_track);
  
  MAUS::DataStructure::Global::TrackPArray* tracker0_array =
      _track_matching->GetDetectorTrackArray(
          MAUS::DataStructure::Global::kTracker0);
  EXPECT_EQ(tracker0_array->size(), 2);
  if (tracker0_array->size() > 1) {
    EXPECT_EQ(tracker0_array->at(0)->get_mapper_name(), "Tracker0");
    EXPECT_EQ(tracker0_array->at(0)->get_mapper_name(), "Tracker0");
  }

  MAUS::DataStructure::Global::TrackPArray* tracker1_array =
      _track_matching->GetDetectorTrackArray(
          MAUS::DataStructure::Global::kTracker1);
  EXPECT_EQ(tracker1_array->size(), 1);
  if (tracker1_array->size() > 0) {
    EXPECT_EQ(tracker1_array->at(0)->get_mapper_name(), "Tracker1");
  }

  MAUS::DataStructure::Global::TrackPArray* emr_array =
      _track_matching->GetDetectorTrackArray(
          MAUS::DataStructure::Global::kEMR);
  EXPECT_EQ(emr_array->size(), 1);
  if (emr_array->size() > 0) {
    EXPECT_EQ(emr_array->at(0)->get_mapper_name(), "EMR Primary");
  }
}

TEST_F(TrackMatchingTest, GetDetectorTrackPoints) {
  _track_matching = new MAUS::recon::global::TrackMatching(_global_event,
      "TrackMatchingTest", "kMuPlus", _matching_tolerances, 10.0);

  MAUS::DataStructure::Global::DetectorPoint tof0 =
      MAUS::DataStructure::Global::kTOF0;
  MAUS::DataStructure::Global::DetectorPoint tof1 =
      MAUS::DataStructure::Global::kTOF1;
  MAUS::DataStructure::Global::DetectorPoint tof2 =
      MAUS::DataStructure::Global::kTOF2;
  MAUS::DataStructure::Global::DetectorPoint kl =
      MAUS::DataStructure::Global::kCalorimeter;

  MAUS::DataStructure::Global::SpacePoint tof0_sp1;
  TLorentzVector position(0.0, 0.0, 100.0, 0.0);
  tof0_sp1.set_position(position);
  tof0_sp1.set_detector(tof0);
  MAUS::DataStructure::Global::SpacePoint tof0_sp2;
  tof0_sp2.set_position(position);
  tof0_sp2.set_detector(tof0);
  MAUS::DataStructure::Global::SpacePoint tof1_sp;
  position.SetZ(300.0);
  tof1_sp.set_position(position);
  tof1_sp.set_detector(tof1);
  MAUS::DataStructure::Global::SpacePoint tof2_sp;
  position.SetZ(800.0);
  tof2_sp.set_position(position);
  tof2_sp.set_detector(tof2);
  MAUS::DataStructure::Global::SpacePoint kl_sp;
  position.SetZ(900.0);
  kl_sp.set_position(position);
  kl_sp.set_detector(kl);

  _global_event->add_space_point(&tof0_sp1);
  _global_event->add_space_point(&tof0_sp2);
  _global_event->add_space_point(&tof1_sp);
  _global_event->add_space_point(&tof2_sp);
  _global_event->add_space_point(&kl_sp);

  std::vector<MAUS::DataStructure::Global::TrackPoint*> tof0_tps;
  std::vector<MAUS::DataStructure::Global::TrackPoint*> tof1_tps;
  std::vector<MAUS::DataStructure::Global::TrackPoint*> tof2_tps;
  std::vector<MAUS::DataStructure::Global::TrackPoint*> kl_tps;

  tof0_tps = _track_matching->GetDetectorTrackPoints(tof0,
      "GetDetectorTrackPointsTest");
  tof1_tps = _track_matching->GetDetectorTrackPoints(tof1,
      "GetDetectorTrackPointsTest");
  tof2_tps = _track_matching->GetDetectorTrackPoints(tof2,
      "GetDetectorTrackPointsTest");
  kl_tps = _track_matching->GetDetectorTrackPoints(kl,
      "GetDetectorTrackPointsTest");

  EXPECT_EQ(tof0_tps.size(), 2);
  EXPECT_EQ(tof1_tps.size(), 1);
  EXPECT_EQ(tof2_tps.size(), 1);
  EXPECT_EQ(kl_tps.size(), 1);

  if (tof0_tps.size() > 1) {
    EXPECT_EQ(tof0_tps[0]->get_detector(), tof0);
    EXPECT_EQ(tof0_tps[0]->get_mapper_name(), "GetDetectorTrackPointsTest");
    EXPECT_EQ(tof0_tps[1]->get_detector(), tof0);
    EXPECT_EQ(tof0_tps[1]->get_mapper_name(), "GetDetectorTrackPointsTest");
  }

  if (tof1_tps.size() > 0) {
    EXPECT_EQ(tof1_tps[0]->get_detector(), tof1);
    EXPECT_EQ(tof1_tps[0]->get_mapper_name(), "GetDetectorTrackPointsTest");
  }

  if (tof2_tps.size() > 0) {
    EXPECT_EQ(tof2_tps[0]->get_detector(), tof2);
    EXPECT_EQ(tof2_tps[0]->get_mapper_name(), "GetDetectorTrackPointsTest");
  }

  if (kl_tps.size() > 0) {
    EXPECT_EQ(kl_tps[0]->get_detector(), kl);
    EXPECT_EQ(kl_tps[0]->get_mapper_name(), "GetDetectorTrackPointsTest");
  }
}

TEST_F(TrackMatchingTest, PIDHypotheses) {
  _track_matching = new MAUS::recon::global::TrackMatching(_global_event,
      "TrackMatchingTest", "kMuPlus", _matching_tolerances, 10.0);

  std::vector<MAUS::DataStructure::Global::PID> pids =
      _track_matching->PIDHypotheses(0, "kMuPlus");
  EXPECT_EQ(pids.size(), 1);
  if (pids.size() > 0) {
    EXPECT_EQ(pids[0], MAUS::DataStructure::Global::kMuPlus);
  }
  
  pids = _track_matching->PIDHypotheses(1, "kPiMinus");
  EXPECT_EQ(pids.size(), 1);
  if (pids.size() > 0) {
    EXPECT_EQ(pids[0], MAUS::DataStructure::Global::kPiMinus);
  }
  
  pids = _track_matching->PIDHypotheses(-1, "all");
  EXPECT_EQ(pids.size(), 3);
  if (pids.size() > 2) {
    EXPECT_EQ(pids[0], MAUS::DataStructure::Global::kEMinus);
    EXPECT_EQ(pids[1], MAUS::DataStructure::Global::kMuMinus);
    EXPECT_EQ(pids[2], MAUS::DataStructure::Global::kPiMinus);
  }

  pids = _track_matching->PIDHypotheses(0, "all");
  EXPECT_EQ(pids.size(), 6);
  if (pids.size() > 5) {
    EXPECT_EQ(pids[0], MAUS::DataStructure::Global::kEPlus);
    EXPECT_EQ(pids[1], MAUS::DataStructure::Global::kMuPlus);
    EXPECT_EQ(pids[2], MAUS::DataStructure::Global::kPiPlus);
    EXPECT_EQ(pids[3], MAUS::DataStructure::Global::kEMinus);
    EXPECT_EQ(pids[4], MAUS::DataStructure::Global::kMuMinus);
    EXPECT_EQ(pids[5], MAUS::DataStructure::Global::kPiMinus);
  }
}

TEST_F(TrackMatchingTest, MatchTrackPoint) {
  _track_matching = new MAUS::recon::global::TrackMatching(_global_event,
      "TrackMatchingTest", "kMuPlus", _matching_tolerances, 10.0);

  MAUS::Simulation::DetectorConstruction* dc =
      Globals::GetInstance()->GetGeant4Manager()->GetGeometry();
  std::string mod_path = std::string(getenv("MAUS_ROOT_DIR"))+
      "/tests/cpp_unit/Recon/Global/TestGeometries/";
  MiceModule geometry(mod_path+"PropagationTest.dat");
  dc->SetMiceModules(geometry);

  MAUS::DataStructure::Global::SpacePoint tof1_sp1;
  TLorentzVector tof1_position(30.0, -30.0, 100.0, 0.0);
  tof1_sp1.set_position(tof1_position);
  tof1_sp1.set_detector(MAUS::DataStructure::Global::kTOF1);
  _global_event->add_space_point(&tof1_sp1);
  MAUS::DataStructure::Global::SpacePoint tof1_sp2;
  tof1_position.SetY(30.0);
  tof1_sp2.set_position(tof1_position);
  tof1_sp2.set_detector(MAUS::DataStructure::Global::kTOF1);
  _global_event->add_space_point(&tof1_sp2);
  MAUS::DataStructure::Global::SpacePoint tof2_sp;
  TLorentzVector tof2_position(0.0, -30.0, 9000.0, 0.0);
  tof2_sp.set_position(tof2_position);
  tof2_sp.set_detector(MAUS::DataStructure::Global::kTOF2);
  _global_event->add_space_point(&tof2_sp);

  std::vector<MAUS::DataStructure::Global::TrackPoint*> TOF1_tp =
      _track_matching->GetDetectorTrackPoints(
          MAUS::DataStructure::Global::kTOF1, "MatchTrackPointTest");
  std::vector<MAUS::DataStructure::Global::TrackPoint*> TOF2_tp =
      _track_matching->GetDetectorTrackPoints(
          MAUS::DataStructure::Global::kTOF2, "MatchTrackPointTest");
  TLorentzVector start_position(0.0, 0.0, 4000.0, 0.0);
  TLorentzVector start_momentum(10.0, 10.0, 150.0, 0.0);
  MAUS::DataStructure::Global::PID pid = MAUS::DataStructure::Global::kMuPlus;
  BTFieldConstructor* field = MAUS::Globals::GetMCFieldConstructor();
  MAUS::DataStructure::Global::Track hypothesis_track;

  _track_matching->MatchTrackPoint(start_position, start_momentum, TOF1_tp, pid,
      field, "TOF1", &hypothesis_track);

  _track_matching->MatchTrackPoint(start_position, start_momentum, TOF2_tp, pid,
      field, "TOF2", &hypothesis_track);

  std::vector<const MAUS::DataStructure::Global::TrackPoint*> track_points =
      hypothesis_track.GetTrackPoints();
  std::vector<const MAUS::DataStructure::Global::TrackPoint*> tof1_tps =
      hypothesis_track.GetTrackPoints(MAUS::DataStructure::Global::kTOF1);
  std::vector<const MAUS::DataStructure::Global::TrackPoint*> tof2_tps =
      hypothesis_track.GetTrackPoints(MAUS::DataStructure::Global::kTOF2);

  EXPECT_EQ(track_points.size(), 2);
  EXPECT_EQ(tof1_tps.size(), 1);
  EXPECT_EQ(tof2_tps.size(), 1);
  EXPECT_FLOAT_EQ(tof1_tps[0]->get_position().Y(), -30.0);
}

TEST_F(TrackMatchingTest, MatchEMRTrack) {
  _track_matching = new MAUS::recon::global::TrackMatching(_global_event,
      "TrackMatchingTest", "kMuPlus", _matching_tolerances, 10.0);

  MAUS::Simulation::DetectorConstruction* dc =
      Globals::GetInstance()->GetGeant4Manager()->GetGeometry();
  std::string mod_path = std::string(getenv("MAUS_ROOT_DIR"))+
      "/tests/cpp_unit/Recon/Global/TestGeometries/";
  MiceModule geometry(mod_path+"PropagationTest.dat");
  dc->SetMiceModules(geometry);

  MAUS::DataStructure::Global::SpacePoint sp;
  TLorentzVector position(30.0, -45.0, 4000.0, 0.0);
  TLorentzVector position_error(10.0, 10.0, 10.0, 10.0);
  sp.set_position(position);
  sp.set_position_error(position_error);
  MAUS::DataStructure::Global::TrackPoint emr_tp1(&sp);
  emr_tp1.set_detector(MAUS::DataStructure::Global::kEMR);
  position.SetX(-30.0);
  sp.set_position(position);
  MAUS::DataStructure::Global::TrackPoint emr_tp2(&sp);
  emr_tp2.set_detector(MAUS::DataStructure::Global::kEMR);

  MAUS::DataStructure::Global::Track emr_track1;
  emr_track1.AddTrackPoint(&emr_tp1);
  MAUS::DataStructure::Global::Track emr_track2;
  emr_track2.AddTrackPoint(&emr_tp2);

  _global_event->add_track_recursive(&emr_track1);
  _global_event->add_track_recursive(&emr_track2);

  MAUS::DataStructure::Global::TrackPArray* emr_track_array =
      _track_matching->GetDetectorTrackArray(MAUS::DataStructure::Global::kEMR);

  TLorentzVector start_position(0.0, 0.0, 500.0, 0.0);
  TLorentzVector start_momentum(20.0, 20.0, 150.0, 0.0);
  MAUS::DataStructure::Global::PID pid = MAUS::DataStructure::Global::kMuPlus;
  BTFieldConstructor* field = MAUS::Globals::GetMCFieldConstructor();
  MAUS::DataStructure::Global::Track hypothesis_track;

  _track_matching->MatchEMRTrack(start_position, start_momentum,
      emr_track_array, pid, field, &hypothesis_track);

  std::vector<const MAUS::DataStructure::Global::TrackPoint*> track_points =
      hypothesis_track.GetTrackPoints();

  EXPECT_EQ(track_points.size(), 1);
  EXPECT_FLOAT_EQ(track_points[0]->get_position().X(), 30.0);
}

TEST_F(TrackMatchingTest, AddTrackerTrackPoints) {
  _track_matching = new MAUS::recon::global::TrackMatching(_global_event,
      "TrackMatchingTest", "kMuPlus", _matching_tolerances, 10.0);

  double mass = 105.65837;
  std::string mapper_name = "AddTrackPointsTest";
  MAUS::DataStructure::Global::Track tracker_track;
  MAUS::DataStructure::Global::Track target_track;
  MAUS::DataStructure::Global::SpacePoint sp;
  double z_positions[] = {150.0, 200.0, 300.0};
  TLorentzVector position(0.0, 0.0, z_positions[0], 0.0);
  TLorentzVector momentum(0.0, 0.0, 150.0, 0.0);
  sp.set_position(position);
  MAUS::DataStructure::Global::TrackPoint tp1(&sp);
  tp1.set_momentum(momentum);
  position.SetZ(z_positions[2]);
  sp.set_position(position);
  MAUS::DataStructure::Global::TrackPoint tp2(&sp);
  tp2.set_momentum(momentum);
  position.SetZ(z_positions[1]);
  sp.set_position(position);
  MAUS::DataStructure::Global::TrackPoint tp3(&sp);
  tp3.set_momentum(momentum);
  tracker_track.AddTrackPoint(&tp1);
  tracker_track.AddTrackPoint(&tp2);
  tracker_track.AddTrackPoint(&tp3);
  _track_matching->AddTrackerTrackPoints(&tracker_track, mapper_name, mass,
      &target_track);
  std::vector<const MAUS::DataStructure::Global::TrackPoint*> track_points =
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
  _track_matching = new MAUS::recon::global::TrackMatching(_global_event,
      "TrackMatchingTest", "kMuPlus", _matching_tolerances, 10.0);

  std::vector<MAUS::DataStructure::Global::Track*> global_tracks;
  std::vector<MAUS::DataStructure::Global::Track*> us_tracks1;
  std::vector<MAUS::DataStructure::Global::Track*> us_tracks2;
  std::vector<MAUS::DataStructure::Global::Track*> ds_tracks1;
  std::vector<MAUS::DataStructure::Global::Track*> ds_tracks2;
  MAUS::DataStructure::Global::Track us_muplus_track1;
  MAUS::DataStructure::Global::Track us_muplus_track2;
  MAUS::DataStructure::Global::Track us_muplus_track3;
  MAUS::DataStructure::Global::Track ds_muplus_track;
  MAUS::DataStructure::Global::Track ds_piminus_track;
  us_muplus_track1.set_mapper_name("MapCppGlobalTrackMatching-US");
  us_muplus_track2.set_mapper_name("MapCppGlobalTrackMatching-US");
  us_muplus_track3.set_mapper_name("MapCppGlobalTrackMatching-US");
  ds_muplus_track.set_mapper_name("MapCppGlobalTrackMatching-DS");
  ds_piminus_track.set_mapper_name("MapCppGlobalTrackMatching-DS");
  us_muplus_track1.set_pid(MAUS::DataStructure::Global::kMuPlus);
  us_muplus_track2.set_pid(MAUS::DataStructure::Global::kMuPlus);
  us_muplus_track3.set_pid(MAUS::DataStructure::Global::kMuPlus);
  ds_muplus_track.set_pid(MAUS::DataStructure::Global::kMuPlus);
  ds_piminus_track.set_pid(MAUS::DataStructure::Global::kPiMinus);
  us_muplus_track1.SetDetector(MAUS::DataStructure::Global::kTOF1);
  us_muplus_track2.SetDetector(MAUS::DataStructure::Global::kTOF1);
  ds_muplus_track.SetDetector(MAUS::DataStructure::Global::kTOF2);;
  ds_piminus_track.SetDetector(MAUS::DataStructure::Global::kTOF2);;
  global_tracks.push_back(&us_muplus_track1);
  global_tracks.push_back(&us_muplus_track2);
  global_tracks.push_back(&us_muplus_track3);
  global_tracks.push_back(&ds_muplus_track);
  global_tracks.push_back(&ds_piminus_track);
  
  _track_matching->USDSTracks(&global_tracks,
      MAUS::DataStructure::Global::kMuPlus, &us_tracks1, &ds_tracks1);
  _track_matching->USDSTracks(&global_tracks,
      MAUS::DataStructure::Global::kPiMinus, &us_tracks2, &ds_tracks2);

  EXPECT_EQ(us_tracks1.size(), 2);
  EXPECT_EQ(ds_tracks1.size(), 1);
  EXPECT_EQ(us_tracks2.size(), 0);
  EXPECT_EQ(ds_tracks2.size(), 1);

  if (us_tracks1.size() > 1) {
    EXPECT_EQ(us_tracks1[0]->get_mapper_name(), "MapCppGlobalTrackMatching-US");
    EXPECT_EQ(us_tracks1[0]->get_pid(), MAUS::DataStructure::Global::kMuPlus);
    EXPECT_TRUE(us_tracks1[0]->HasDetector(MAUS::DataStructure::Global::kTOF1));
    EXPECT_EQ(us_tracks1[1]->get_mapper_name(), "MapCppGlobalTrackMatching-US");
    EXPECT_EQ(us_tracks1[1]->get_pid(), MAUS::DataStructure::Global::kMuPlus);
    EXPECT_TRUE(us_tracks1[1]->HasDetector(MAUS::DataStructure::Global::kTOF1));
  }

  if (ds_tracks1.size() > 0) {
    EXPECT_EQ(ds_tracks1[0]->get_mapper_name(), "MapCppGlobalTrackMatching-DS");
    EXPECT_EQ(ds_tracks1[0]->get_pid(), MAUS::DataStructure::Global::kMuPlus);
    EXPECT_TRUE(ds_tracks1[0]->HasDetector(MAUS::DataStructure::Global::kTOF2));
  }

  if (ds_tracks2.size() > 0) {
    EXPECT_EQ(ds_tracks2[0]->get_mapper_name(), "MapCppGlobalTrackMatching-DS");
    EXPECT_EQ(ds_tracks2[0]->get_pid(), MAUS::DataStructure::Global::kPiMinus);
    EXPECT_TRUE(ds_tracks2[0]->HasDetector(MAUS::DataStructure::Global::kTOF2));
  }
}

TEST_F(TrackMatchingTest, MatchUSDS) {
  _track_matching = new MAUS::recon::global::TrackMatching(_global_event,
      "TrackMatchingTest", "kMuPlus", _matching_tolerances, 10.0);

  MAUS::DataStructure::Global::SpacePoint tof1_sp;
  TLorentzVector tof1_position(0.0, 0.0, 100.0, 20.0);
  tof1_sp.set_position(tof1_position);
  tof1_sp.set_detector(MAUS::DataStructure::Global::kTOF1);
  const MAUS::DataStructure::Global::TrackPoint tof1_tp(&tof1_sp);
  MAUS::DataStructure::Global::SpacePoint tof2_sp;
  TLorentzVector tof2_position(0.0, 0.0, 8300.0, 65.0);
  tof2_sp.set_position(tof2_position);
  tof2_sp.set_detector(MAUS::DataStructure::Global::kTOF2);
  const MAUS::DataStructure::Global::TrackPoint tof2_tp(&tof2_sp);

  MAUS::DataStructure::Global::TrackPointCPArray tof1_cparray;
  tof1_cparray.push_back(&tof1_tp);
  MAUS::DataStructure::Global::TrackPointCPArray tof2_cparray;
  tof2_cparray.push_back(&tof2_tp);

  MAUS::DataStructure::Global::PID pid = MAUS::DataStructure::Global::kMuPlus;

  _track_matching->MatchUSDS(tof1_cparray, tof2_cparray, pid, 15.0);

  std::vector<MAUS::DataStructure::Global::Track*>* through_tracks =
      GlobalTools::GetTracksByMapperName(_global_event,
      "MapCppGlobalTrackMatching-Through");

  EXPECT_EQ(through_tracks->size(), 1);
  if (through_tracks->size() > 0) {
    EXPECT_FLOAT_EQ(through_tracks->at(0)->get_emr_range_primary(), 15.0);
    EXPECT_EQ(through_tracks->at(0)->get_pid(),
        MAUS::DataStructure::Global::kMuPlus);
    EXPECT_EQ(through_tracks->at(0)->GetTrackPoints().size(), 2);
    if (through_tracks->at(0)->GetTrackPoints().size() > 1) {
      const MAUS::DataStructure::Global::TrackPoint* tof1_tp =
          through_tracks->at(0)->GetTrackPoints().at(0);
      const MAUS::DataStructure::Global::TrackPoint* tof2_tp =
          through_tracks->at(0)->GetTrackPoints().at(1);
      EXPECT_FLOAT_EQ(tof1_tp->get_position().Z(), 100.0);
      EXPECT_FLOAT_EQ(tof2_tp->get_position().Z(), 8300.0);
    }
  }
}

TEST_F(TrackMatchingTest, TOFTimeFromTrackPoints) {
  _track_matching = new MAUS::recon::global::TrackMatching(_global_event,
      "TrackMatchingTest", "kMuPlus", _matching_tolerances, 10.0);

  std::vector<const MAUS::DataStructure::Global::TrackPoint*> track_points;
  MAUS::DataStructure::Global::SpacePoint sp;
  TLorentzVector position(0.0, 0.0, 0.0, 10.0);
  sp.set_position(position);
  MAUS::DataStructure::Global::TrackPoint tp1(&sp);
  tp1.set_detector(MAUS::DataStructure::Global::kTOF1);
  position.SetT(15.7);
  sp.set_position(position);
  MAUS::DataStructure::Global::TrackPoint tp2(&sp);
  tp2.set_detector(MAUS::DataStructure::Global::kTOF2);
  // rather than fiddling around with casting we just use the internal track
  // mechanics
  MAUS::DataStructure::Global::Track track;
  track.AddTrackPoint(&tp1);
  track.AddTrackPoint(&tp2);
  track_points = track.GetTrackPoints();
  double TOF_time;
  
  TOF_time = _track_matching->TOFTimeFromTrackPoints(track_points,
      MAUS::DataStructure::Global::kTOF1);
  EXPECT_FLOAT_EQ(TOF_time, 10.0);
  TOF_time = _track_matching->TOFTimeFromTrackPoints(track_points,
      MAUS::DataStructure::Global::kTOF2);
  EXPECT_FLOAT_EQ(TOF_time, 15.7);
}

}
}
} // ~namespace MAUS
