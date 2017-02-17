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
    _matching_tolerances["TOF0"] = std::make_pair(2.0, 0.0);
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
      "TrackMatchingTest", "kMuPlus", 1, _matching_tolerances, 10.0, _no_check_settings);

  Simulation::DetectorConstruction* dc =
      Globals::GetInstance()->GetGeant4Manager()->GetGeometry();
  std::string mod_path = std::string(getenv("MAUS_ROOT_DIR"))+
      "/tests/cpp_unit/Recon/Global/TestGeometries/";
  MiceModule geometry(mod_path+"SimpleBeamline.dat");
  dc->SetMiceModules(geometry);

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

  std::vector<DataStructure::Global::PrimaryChain*> us_chains = _global_event->GetUSPrimaryChains();
  std::vector<DataStructure::Global::PrimaryChain*> ds_chains = _global_event->GetDSPrimaryChains();
  std::vector<DataStructure::Global::PrimaryChain*> through_chains =
      _global_event->GetThroughPrimaryChains();

  EXPECT_EQ(us_chains.size(), 1);
  EXPECT_EQ(ds_chains.size(), 1);
  EXPECT_EQ(through_chains.size(), 1);

  if (us_chains.size() == 1 and ds_chains.size() == 1 and through_chains.size() == 1) {
    DataStructure::Global::PrimaryChain* us_chain = us_chains.at(0);
    DataStructure::Global::PrimaryChain* ds_chain = ds_chains.at(0);
    DataStructure::Global::PrimaryChain* through_chain = through_chains.at(0);
    EXPECT_EQ(through_chain->GetUSDaughter(), us_chain);
    EXPECT_EQ(through_chain->GetDSDaughter(), ds_chain);
    EXPECT_EQ(us_chain->GetMatchedTrack(
        DataStructure::Global::kMuPlus)->GetTrackPoints().size(), 3);
    EXPECT_EQ(ds_chain->GetMatchedTrack(
        DataStructure::Global::kMuPlus)->GetTrackPoints().size(), 4);
    EXPECT_EQ(through_chain->GetMatchedTrack(
        DataStructure::Global::kMuPlus)->GetTrackPoints().size(), 7);
    EXPECT_FLOAT_EQ(ds_chain->GetMatchedTrack(
        DataStructure::Global::kMuPlus)->get_emr_range_primary(), 20.0);
    EXPECT_FLOAT_EQ(through_chain->GetMatchedTrack(
        DataStructure::Global::kMuPlus)->get_emr_range_primary(), 20.0);
  }
}

TEST_F(TrackMatchingTest, GetDetectorTrackArray) {
  _track_matching = new recon::global::TrackMatching(_global_event,
      "TrackMatchingTest", "kMuPlus", 1, _matching_tolerances, 10.0, _no_check_settings);

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
      "TrackMatchingTest", "kMuPlus", 1, _matching_tolerances, 10.0, _no_check_settings);

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
      "TrackMatchingTest", "kMuPlus", 1, _matching_tolerances, 10.0, _no_check_settings);

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
      "TrackMatchingTest", "kMuPlus", 1, _matching_tolerances, 10.0, _no_check_settings);

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
  _track_matching = new recon::global::TrackMatching(_global_event,
      "TrackMatchingTest", "kMuPlus", 1, _matching_tolerances, 10.0, _no_check_settings);

  Simulation::DetectorConstruction* dc =
      Globals::GetInstance()->GetGeant4Manager()->GetGeometry();
  std::string mod_path = std::string(getenv("MAUS_ROOT_DIR"))+
      "/tests/cpp_unit/Recon/Global/TestGeometries/";
  MiceModule geometry(mod_path+"SimpleBeamline.dat");
  dc->SetMiceModules(geometry);

  DataStructure::Global::SpacePoint tof0_sp1;
  TLorentzVector tof0_position(0.0, 0.0, 100.0, 0.0);
  tof0_sp1.set_position(tof0_position);
  tof0_sp1.set_detector(DataStructure::Global::kTOF0);
  _global_event->add_space_point(&tof0_sp1);
  DataStructure::Global::SpacePoint tof0_sp2;
  tof0_position.SetT(5.0);
  tof0_sp2.set_position(tof0_position);
  tof0_sp2.set_detector(DataStructure::Global::kTOF0);
  _global_event->add_space_point(&tof0_sp2);


  std::vector<DataStructure::Global::SpacePoint*> TOF0_sp =
      _track_matching->GetDetectorSpacePoints(DataStructure::Global::kTOF0);

  TLorentzVector position(0.0, 0.0, 8537.0, 41.0);
  TLorentzVector momentum(10.0, 10.0, 150.0, 0.0);
  DataStructure::Global::PID pid = DataStructure::Global::kMuPlus;
  BTFieldConstructor* field = Globals::GetMCFieldConstructor();
  DataStructure::Global::Track hypothesis_track;
  _track_matching->MatchTOF0(position, momentum, 7902.0, 35.0, TOF0_sp, pid, field,
                             &hypothesis_track);

  std::vector<const DataStructure::Global::TrackPoint*> track_points =
      hypothesis_track.GetTrackPoints();

  EXPECT_EQ(track_points.size(), 1);
  if (track_points.size() > 0) {
    EXPECT_FLOAT_EQ(track_points.at(0)->get_position().T(), 5.0);
  }
}

TEST_F(TrackMatchingTest, MatchEMRTrack) {
  _track_matching = new recon::global::TrackMatching(_global_event,
      "TrackMatchingTest", "kMuPlus", 1, _matching_tolerances, 10.0, _no_check_settings);

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
      "TrackMatchingTest", "kMuPlus", 1, _matching_tolerances, 10.0, _no_check_settings);

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

TEST_F(TrackMatchingTest, TOFTimeFromTrackPoints) {
  _track_matching = new recon::global::TrackMatching(_global_event,
      "TrackMatchingTest", "kMuPlus", 1, _matching_tolerances, 10.0, _no_check_settings);

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
  _track_matching = new recon::global::TrackMatching(_global_event,
      "TrackMatchingTest", "kMuPlus", 1, _matching_tolerances, 10.0, _no_check_settings);
  TLorentzVector position(0.0, 0.0, 0.0, 0.0);
  DataStructure::Global::SpacePoint tof0_sp0;
  tof0_sp0.set_position(position);
  DataStructure::Global::SpacePoint tof1_sp0;
  tof1_sp0.set_position(position);
  DataStructure::Global::SpacePoint tof1_sp1;
  position.SetX(60.0);
  position.SetT(0.4);
  tof1_sp1.set_position(position);
  DataStructure::Global::SpacePoint tof1_sp2;
  position.SetT(0.6);
  tof1_sp2.set_position(position);
  DataStructure::Global::SpacePoint kl_sp0;
  kl_sp0.set_position(position);
  kl_sp0.set_detector(DataStructure::Global::kCalorimeter);
  DataStructure::Global::SpacePoint kl_sp1;
  position.SetY(45.0);
  kl_sp1.set_position(position);
  kl_sp1.set_detector(DataStructure::Global::kCalorimeter);
  std::vector<DataStructure::Global::SpacePoint*> tof0_sps;
  std::vector<DataStructure::Global::SpacePoint*> tof1_sps;
  std::vector<DataStructure::Global::SpacePoint*> kl_sps;
  tof0_sps.push_back(&tof0_sp0);
  tof1_sps.push_back(&tof1_sp0);
  tof1_sps.push_back(&tof1_sp1);
  tof1_sps.push_back(&tof1_sp2);
  kl_sps.push_back(&kl_sp0);
  kl_sps.push_back(&kl_sp1);
  DataStructure::Global::Track tof0_track;
  DataStructure::Global::Track tof1_track;
  DataStructure::Global::Track kl_track;
  _track_matching->AddIfConsistent(tof0_sps, &tof0_track);
  _track_matching->AddIfConsistent(tof1_sps, &tof1_track);
  _track_matching->AddIfConsistent(kl_sps, &kl_track);
  EXPECT_EQ(tof0_track.GetTrackPoints().size(), 1);
  EXPECT_EQ(tof1_track.GetTrackPoints().size(), 0);
  EXPECT_EQ(kl_track.GetTrackPoints().size(), 2);
}

TEST_F(TrackMatchingTest, CheckChainMultiplicity) {
  _track_matching = new recon::global::TrackMatching(_global_event,
      "TrackMatchingTest", "kMuPlus", 1, _matching_tolerances, 10.0, _no_check_settings);
  DataStructure::Global::PrimaryChain chain_us0("TrackMatchingTest", DataStructure::Global::kUS);
  DataStructure::Global::PrimaryChain chain_us1("TrackMatchingTest", DataStructure::Global::kUS);
  DataStructure::Global::PrimaryChain chain_us2("TrackMatchingTest", DataStructure::Global::kUS);
  DataStructure::Global::PrimaryChain chain_ds0("TrackMatchingTest", DataStructure::Global::kDS);
  DataStructure::Global::PrimaryChain chain_ds1("TrackMatchingTest", DataStructure::Global::kDS);
  DataStructure::Global::PrimaryChain chain_ds2("TrackMatchingTest", DataStructure::Global::kDS);
  DataStructure::Global::PrimaryChain chain_through0(
      "TrackMatchingTest", DataStructure::Global::kThrough);
  DataStructure::Global::PrimaryChain chain_through1(
      "TrackMatchingTest", DataStructure::Global::kThrough);
  DataStructure::Global::PrimaryChain chain_through2(
      "TrackMatchingTest", DataStructure::Global::kThrough);
  DataStructure::Global::PrimaryChain chain_through3(
      "TrackMatchingTest", DataStructure::Global::kThrough);
  chain_through0.SetUSDaughter(&chain_us0);
  chain_through0.SetDSDaughter(&chain_ds0);
  chain_through1.SetUSDaughter(&chain_us2);
  chain_through1.SetDSDaughter(&chain_ds1);
  chain_through2.SetUSDaughter(&chain_us1);
  chain_through2.SetDSDaughter(&chain_ds2);
  chain_through3.SetUSDaughter(&chain_us2);
  chain_through3.SetDSDaughter(&chain_ds2);
  _global_event->add_primary_chain(&chain_us0);
  _global_event->add_primary_chain(&chain_us1);
  _global_event->add_primary_chain(&chain_us2);
  _global_event->add_primary_chain(&chain_ds0);
  _global_event->add_primary_chain(&chain_ds1);
  _global_event->add_primary_chain(&chain_ds2);
  _global_event->add_primary_chain(&chain_through0);
  _global_event->add_primary_chain(&chain_through1);
  _global_event->add_primary_chain(&chain_through2);
  _global_event->add_primary_chain(&chain_through3);
  _track_matching->CheckChainMultiplicity();
  EXPECT_EQ(chain_through0.get_multiplicity(), DataStructure::Global::kUnique);
  EXPECT_EQ(chain_through1.get_multiplicity(), DataStructure::Global::kMultipleUS);
  EXPECT_EQ(chain_through2.get_multiplicity(), DataStructure::Global::kMultipleDS);
  EXPECT_EQ(chain_through3.get_multiplicity(), DataStructure::Global::kMultipleBoth);
}
}
}
} // ~namespace MAUS
