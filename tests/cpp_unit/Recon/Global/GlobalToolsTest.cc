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
#include "Geant4/G4NistManager.hh"
#include "gtest/gtest.h"
#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/Recon/Global/GlobalTools.hh"
#include "src/legacy/BeamTools/BTFieldConstructor.hh"
#include "src/common_cpp/Globals/GlobalsManager.hh"
#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/Simulation/MAUSGeant4Manager.hh"
#include "src/common_cpp/Simulation/DetectorConstruction.hh"

namespace MAUS {

class GlobalToolsTest : public ::testing::Test {
 protected:
  GlobalToolsTest()  {}
  virtual ~GlobalToolsTest() {}
  virtual void SetUp()    {}
  virtual void TearDown() {}
};

TEST_F(GlobalToolsTest, GetReconDetectors) {
  GlobalEvent* global_event = new GlobalEvent;
  DataStructure::Global::SpacePoint sp;
  TLorentzVector position(0.0, 0.0, 0.0, 0.0);
  sp.set_position(position);
  DataStructure::Global::TrackPoint tp(&sp);

  DataStructure::Global::SpacePoint* tof1_sp = sp.Clone();
  tof1_sp->set_detector(DataStructure::Global::kTOF1);
  global_event->add_space_point(tof1_sp);
  DataStructure::Global::SpacePoint* tof2_sp = sp.Clone();
  tof2_sp->set_detector(DataStructure::Global::kTOF2);
  global_event->add_space_point(tof2_sp);
  DataStructure::Global::SpacePoint* kl_sp = sp.Clone();
  kl_sp->set_detector(DataStructure::Global::kCalorimeter);
  global_event->add_space_point(kl_sp);

  DataStructure::Global::TrackPoint* tr0_tp = tp.Clone();
  tr0_tp->set_detector(DataStructure::Global::kTracker0);
  DataStructure::Global::TrackPoint* tr1_2_tp = tp.Clone();
  tr1_2_tp->set_detector(DataStructure::Global::kTracker1_2);
  DataStructure::Global::TrackPoint* emr_tp = tp.Clone();
  emr_tp->set_detector(DataStructure::Global::kEMR);

  DataStructure::Global::Track* track = new DataStructure::Global::Track;
  track->AddTrackPoint(tr0_tp);
  track->AddTrackPoint(tr1_2_tp);
  track->AddTrackPoint(emr_tp);
  global_event->add_track(track);

  std::map<DataStructure::Global::DetectorPoint, bool> recon_detectors =
      GlobalTools::GetReconDetectors(global_event);
  int num_exist = 0;
  for (auto detector_iter = recon_detectors.begin();
       detector_iter != recon_detectors.end(); detector_iter++) {
    if (detector_iter->second) {
      num_exist++;
    }
  }

  EXPECT_EQ(num_exist, 6);
  EXPECT_TRUE(recon_detectors.at(DataStructure::Global::kTOF1));
  EXPECT_TRUE(recon_detectors.at(DataStructure::Global::kTracker0));
  EXPECT_TRUE(recon_detectors.at(DataStructure::Global::kTracker1_2));
  EXPECT_TRUE(recon_detectors.at(DataStructure::Global::kTOF2));
  EXPECT_TRUE(recon_detectors.at(DataStructure::Global::kCalorimeter));
  EXPECT_TRUE(recon_detectors.at(DataStructure::Global::kEMR));

  delete global_event;
}

TEST_F(GlobalToolsTest, GetSpillDetectorTracks) {
  Spill* spill = new Spill;
  ReconEvent* recon_event = new ReconEvent;
  GlobalEvent* global_event = new GlobalEvent;
  DataStructure::Global::Track* track = new DataStructure::Global::Track;
  track->set_mapper_name("GetSpillDetectorTracksTest");
  // Use PIDs to uniquely identify tracks later
  DataStructure::Global::Track* tr0_track1 = track->Clone();
  tr0_track1->set_pid(DataStructure::Global::kEMinus);
  tr0_track1->SetDetector(DataStructure::Global::kTracker0);
  global_event->add_track(tr0_track1);
  DataStructure::Global::Track* tr0_track2 = track->Clone();
  tr0_track2->set_pid(DataStructure::Global::kEPlus);
  tr0_track2->SetDetector(DataStructure::Global::kTracker0);
  global_event->add_track(tr0_track2);
  DataStructure::Global::Track* tr1_track = track->Clone();
  tr1_track->set_pid(DataStructure::Global::kMuMinus);
  tr1_track->SetDetector(DataStructure::Global::kTracker1);
  global_event->add_track(tr1_track);
  DataStructure::Global::Track* tr1_track_wrong_mn = track->Clone();
  tr1_track_wrong_mn->set_mapper_name("TEST");
  tr1_track_wrong_mn->SetDetector(DataStructure::Global::kTracker1);
  global_event->add_track(tr1_track_wrong_mn);
  DataStructure::Global::Track* emr_prim = track->Clone();
  emr_prim->set_pid(DataStructure::Global::kPiPlus);
  emr_prim->SetDetector(DataStructure::Global::kEMR);
  global_event->add_track(emr_prim);
  DataStructure::Global::Track* emr_sec = track->Clone();
  emr_sec->set_pid(DataStructure::Global::kPiMinus);
  emr_sec->set_emr_range_secondary(10.0);
  emr_sec->SetDetector(DataStructure::Global::kEMR);
  global_event->add_track(emr_sec);
  DataStructure::Global::Track* mix_track = track->Clone();
  mix_track->set_pid(DataStructure::Global::kPhoton);
  mix_track->SetDetector(DataStructure::Global::kTracker1);
  mix_track->SetDetector(DataStructure::Global::kEMR);
  global_event->add_track(mix_track);
  recon_event->SetGlobalEvent(global_event);
  std::vector<ReconEvent*>* recon_events = new std::vector<ReconEvent*>;
  recon_events->push_back(recon_event);
  spill->SetReconEvents(recon_events);

  auto tracker0_tracks = GlobalTools::GetSpillDetectorTracks(spill,
      DataStructure::Global::kTracker0, "GetSpillDetectorTracksTest");
  EXPECT_EQ(tracker0_tracks->size(), 2);
  if (tracker0_tracks->size() > 1) {
    EXPECT_EQ(tracker0_tracks->at(0)->get_pid(),
              DataStructure::Global::kEMinus);
    EXPECT_EQ(tracker0_tracks->at(1)->get_pid(),
              DataStructure::Global::kEPlus);
  }

  auto tracker1_tracks = GlobalTools::GetSpillDetectorTracks(spill,
      DataStructure::Global::kTracker1, "GetSpillDetectorTracksTest");
  EXPECT_EQ(tracker1_tracks->size(), 2);
  if (tracker1_tracks->size() > 1) {
    EXPECT_EQ(tracker1_tracks->at(0)->get_pid(),
              DataStructure::Global::kMuMinus);
    EXPECT_EQ(tracker1_tracks->at(1)->get_pid(),
              DataStructure::Global::kPhoton);
  }

  auto emr_tracks = GlobalTools::GetSpillDetectorTracks(spill,
      DataStructure::Global::kEMR, "GetSpillDetectorTracksTest");
  EXPECT_EQ(emr_tracks->size(), 2);
  if (emr_tracks->size() > 1) {
    EXPECT_EQ(emr_tracks->at(0)->get_pid(),
              DataStructure::Global::kPiPlus);
    EXPECT_EQ(emr_tracks->at(1)->get_pid(),
              DataStructure::Global::kPhoton);
  }

  delete spill;
}

TEST_F(GlobalToolsTest, GetSpillSpacePoints) {
  Spill* spill = new Spill;
  ReconEvent* recon_event = new ReconEvent;
  GlobalEvent* global_event = new GlobalEvent;
  DataStructure::Global::SpacePoint* tof1_sp1 =
      new DataStructure::Global::SpacePoint();
  tof1_sp1->set_detector(DataStructure::Global::kTOF1);
  DataStructure::Global::SpacePoint* tof1_sp2 =
      new DataStructure::Global::SpacePoint();
  tof1_sp2->set_detector(DataStructure::Global::kTOF1);
  DataStructure::Global::SpacePoint* tof2_sp =
      new DataStructure::Global::SpacePoint();
  tof2_sp->set_detector(DataStructure::Global::kTOF2);
  DataStructure::Global::SpacePoint* kl_sp =
      new DataStructure::Global::SpacePoint();
  kl_sp->set_detector(DataStructure::Global::kCalorimeter);

  global_event->add_space_point(tof1_sp1);
  global_event->add_space_point(tof1_sp2);
  global_event->add_space_point(tof2_sp);
  global_event->add_space_point(kl_sp);
  recon_event->SetGlobalEvent(global_event);
  std::vector<ReconEvent*>* recon_events = new std::vector<ReconEvent*>;
  recon_events->push_back(recon_event);
  spill->SetReconEvents(recon_events);

  auto tof0_sps = GlobalTools::GetSpillSpacePoints(spill,
      DataStructure::Global::kTOF0);
  EXPECT_FALSE(tof0_sps);

  auto tof1_sps = GlobalTools::GetSpillSpacePoints(spill,
      DataStructure::Global::kTOF1);
  EXPECT_EQ(tof1_sps->size(), 2);
  if (tof1_sps->size() > 1) {
    EXPECT_EQ(tof1_sps->at(0)->get_detector(), DataStructure::Global::kTOF1);
    EXPECT_EQ(tof1_sps->at(1)->get_detector(), DataStructure::Global::kTOF1);
  }

  auto tof2_sps = GlobalTools::GetSpillSpacePoints(spill,
      DataStructure::Global::kTOF2);
  EXPECT_EQ(tof2_sps->size(), 1);
  if (tof2_sps->size() > 0) {
    EXPECT_EQ(tof2_sps->at(0)->get_detector(), DataStructure::Global::kTOF2);
  }

  auto kl_sps = GlobalTools::GetSpillSpacePoints(spill,
      DataStructure::Global::kCalorimeter);
  EXPECT_EQ(kl_sps->size(), 1);
  if (kl_sps->size() > 0) {
    EXPECT_EQ(kl_sps->at(0)->get_detector(),
              DataStructure::Global::kCalorimeter);
  }

  delete spill;
}

TEST_F(GlobalToolsTest, GetTracksByMapperName) {
  GlobalEvent* global_event = new GlobalEvent;
  DataStructure::Global::Track* track1 = new DataStructure::Global::Track;
  track1->set_mapper_name("GetTracksByMapperNameTest");
  DataStructure::Global::Track* track2 = new DataStructure::Global::Track;
  track2->set_mapper_name("GetTracksByMapperNameTest");
  DataStructure::Global::Track* track3 = new DataStructure::Global::Track;
  track3->set_mapper_name("WrongMapperName");
  global_event->add_track(track1);
  global_event->add_track(track2);
  global_event->add_track(track3);

  auto tracks = GlobalTools::GetTracksByMapperName(global_event,
      "GetTracksByMapperNameTest");

  EXPECT_EQ(tracks->size(), 2);

  delete global_event;
}

TEST_F(GlobalToolsTest, GetTrackerPlane) {
  std::vector<double> z_positions =
      GlobalTools::GetTrackerPlaneZPositions("Stage4.dat");
  DataStructure::Global::SpacePoint* sp =
    new DataStructure::Global::SpacePoint();
  TLorentzVector position(0, 0, 12106.2, 0);
  sp->set_position(position);
  const DataStructure::Global::TrackPoint* tp1 =
      new const DataStructure::Global::TrackPoint(sp);
  position.SetZ(16773.0);
  sp->set_position(position);
  const DataStructure::Global::TrackPoint* tp2 =
      new const DataStructure::Global::TrackPoint(sp);
  position.SetZ(19000.0);
  sp->set_position(position);
  const DataStructure::Global::TrackPoint* tp3 =
      new const DataStructure::Global::TrackPoint(sp);
  std::vector<int> plane1 =
      GlobalTools::GetTrackerPlane(tp1, z_positions);
  std::vector<int> plane2 =
      GlobalTools::GetTrackerPlane(tp2, z_positions);
  std::vector<int> plane3 =
      GlobalTools::GetTrackerPlane(tp3, z_positions);

  delete sp;
  delete tp1;
  delete tp2;
  delete tp3;

  EXPECT_EQ(plane1[0], 0);
  EXPECT_EQ(plane1[1], 2);
  EXPECT_EQ(plane1[2], 1);
  EXPECT_EQ(plane2[0], 1);
  EXPECT_EQ(plane2[1], 4);
  EXPECT_EQ(plane2[2], 2);
  EXPECT_EQ(plane3[0], 99);
  EXPECT_EQ(plane3[1], 0);
  EXPECT_EQ(plane3[2], 0);
}

TEST_F(GlobalToolsTest, GetTrackerPlaneZPositions) {
  std::vector<double> z_positions =
      GlobalTools::GetTrackerPlaneZPositions("Stage4.dat");
  ASSERT_EQ(z_positions.size(), 30);
  double z_reference[] = {11205.7, 11206.3, 11207,
                          11555.6, 11556.3, 11556.9,
                          11855.6, 11856.3, 11856.9,
                          12105.5, 12106.1, 12106.8,
                          12305.5, 12306.1, 12306.8,
                          16021.6, 16022.3, 16022.9,
                          16221.6, 16222.3, 16222.9,
                          16471.6, 16472.3, 16472.9,
                          16771.6, 16772.3, 16772.9,
                          17121.6, 17122.3, 17122.9};
  double epsilon = 0.1;
  for (size_t i = 0; i < z_positions.size(); i++) {
    EXPECT_NEAR(z_positions.at(i), z_reference[i], epsilon);
  }
}

TEST_F(GlobalToolsTest, approx) {
  EXPECT_PRED3(GlobalTools::approx, 1.1, 1.0, 0.15);
  EXPECT_PRED3(GlobalTools::approx, 10.05, 10.06, 0.02);
  // Predicate assertions always expect true
  EXPECT_FALSE(GlobalTools::approx(1.1, 1.0, 0.05));
}

TEST_F(GlobalToolsTest, GetNearestZTrackPoint) {
  DataStructure::Global::SpacePoint sp;
  DataStructure::Global::TrackPoint tp(&sp);
  TLorentzVector position(0.0, 0.0, 0.0, 0.0);
  DataStructure::Global::TrackPoint* tp1 = tp.Clone();
  position.SetZ(100.0);
  tp1->set_position(position);
  DataStructure::Global::TrackPoint* tp2 = tp.Clone();
  position.SetZ(200.0);
  tp2->set_position(position);
  DataStructure::Global::TrackPoint* tp3 = tp.Clone();
  position.SetZ(300.0);
  tp3->set_position(position);
  DataStructure::Global::TrackPoint* tp4 = tp.Clone();
  position.SetZ(400.0);
  tp4->set_position(position);
  DataStructure::Global::Track track;
  track.AddTrackPoint(tp1);
  track.AddTrackPoint(tp2);
  track.AddTrackPoint(tp3);
  track.AddTrackPoint(tp4);

  const DataStructure::Global::Track* track_ptr = &track;

  auto neartp1 = GlobalTools::GetNearestZTrackPoint(track_ptr, 240.0);
  EXPECT_FLOAT_EQ(neartp1->get_position().Z(), 200.0);
  auto neartp2 = GlobalTools::GetNearestZTrackPoint(track_ptr, 0.0);
  EXPECT_FLOAT_EQ(neartp2->get_position().Z(), 100.0);
  auto neartp3 = GlobalTools::GetNearestZTrackPoint(track_ptr, 100000.0);
  EXPECT_FLOAT_EQ(neartp3->get_position().Z(), 400.0);
}

TEST_F(GlobalToolsTest, dEdx) {
  G4NistManager* manager = G4NistManager::Instance();
  G4Material* galactic = manager->FindOrBuildMaterial("G4_Galactic");
  G4Material* hydrogen = manager->FindOrBuildMaterial("G4_H");
  G4Material* polystyrene = manager->FindOrBuildMaterial("G4_POLYSTYRENE");
  G4Material* lead = manager->FindOrBuildMaterial("G4_Pb");
  double mass = 105.65837;
  EXPECT_NEAR(GlobalTools::dEdx(galactic, 130.0, mass), -8.746e-26, 1.0e-28);
  EXPECT_NEAR(GlobalTools::dEdx(galactic, 230.0, mass), -4.354e-26, 1.0e-28);
  EXPECT_NEAR(GlobalTools::dEdx(hydrogen, 130.0, mass), -7.420e-05, 1.0e-7);
  EXPECT_NEAR(GlobalTools::dEdx(hydrogen, 230.0, mass), -3.687e-05, 1.0e-7);
  EXPECT_NEAR(GlobalTools::dEdx(polystyrene, 130.0, mass), -0.4432, 0.001);
  EXPECT_NEAR(GlobalTools::dEdx(polystyrene, 230.0, mass), -0.2232, 0.001);
  EXPECT_NEAR(GlobalTools::dEdx(lead, 130.0, mass), -2.484, 0.01);
  EXPECT_NEAR(GlobalTools::dEdx(lead, 230.0, mass), -1.336, 0.01);
}

TEST_F(GlobalToolsTest, propagate) {
  Simulation::DetectorConstruction* dc =
      Globals::GetInstance()->GetGeant4Manager()->GetGeometry();
  std::string mod_path = std::string(getenv("MAUS_ROOT_DIR"))+
      "/tests/cpp_unit/Recon/Global/TestGeometries/";
  MiceModule geometry1(mod_path+"PropagationTest.dat");
  MiceModule geometry2(mod_path+"PropagationTest_NoField.dat");
  DataStructure::Global::PID pid = DataStructure::Global::kMuPlus;
  double x1[] = {0.0, 0.0, 0.0, 0.0, 184.699, 15.0, 15.0, 150.0};
  double x2[] = {0.0, 0.0, 0.0, 0.0, 184.699, 15.0, 15.0, 150.0};
  double x3[] = {0.0, 0.0, 0.0, 0.0, 184.699, 15.0, 15.0, 150.0};
  double x4[] = {0.0, 0.0, 0.0, 0.0, 184.699, 15.0, 15.0, 150.0};

  dc->SetMiceModules(geometry1);

  Json::Value config = (*Globals::GetInstance()->GetConfigurationCards());
  config["reconstruction_geometry_filename"] = mod_path + "PropagationTest.dat";
  config["simulation_geometry_filename"] = mod_path + "PropagationTest.dat";
  if (Globals::HasInstance()) {
    GlobalsManager::DeleteGlobals();
  }
  MAUS::GlobalsManager::InitialiseGlobals(JsonWrapper::JsonToString(config));

  BTFieldConstructor* field = Globals::GetMCFieldConstructor();
  double epsilon = 0.001;

  double field2[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  double xfield[4] = {x1[1], x1[2], 100, x1[0]};
  field->GetFieldValue(xfield, field2);
  std::cerr << field2[0] << " " << field2[1] << " " << field2[2] << " "
            << field2[3] << " " << field2[4] << " " << field2[5] << "\n";

  // Energy Loss, Magnetic Field
  try {
    GlobalTools::propagate(x1, 2000.0, field, 10.0, pid);
  } catch (Exception exc) {
    std::cerr << exc.what() << std::endl;
  }
  EXPECT_NEAR(x1[1], 8.345, epsilon);
  EXPECT_NEAR(x1[2], -21.653, epsilon);
  EXPECT_NEAR(x1[4], 133.855, epsilon);
  EXPECT_NEAR(x1[5], -11.217, epsilon);
  EXPECT_NEAR(x1[6], 2.571, epsilon);
  EXPECT_NEAR(x1[7], 81.370, epsilon);

  // No Energy Loss, Magnetic Field
  try {
    GlobalTools::propagate(x2, 2000.0, field, 10.0, pid, false);
  } catch (Exception exc) {
    std::cerr << exc.what() << std::endl;
  }
  EXPECT_NEAR(x2[1], -1.807, epsilon);
  EXPECT_NEAR(x2[2], -13.779, epsilon);
  EXPECT_NEAR(x2[4], 184.700, epsilon);
  EXPECT_NEAR(x2[5], -1.896, epsilon);
  EXPECT_NEAR(x2[6], 21.129, epsilon);
  EXPECT_NEAR(x2[7], 150.000, epsilon);

  dc->SetMiceModules(geometry2);
  field = Globals::GetMCFieldConstructor();

  // Energy Loss, No Magnetic Field
  try {
    GlobalTools::propagate(x3, 2000.0, field, 10.0, pid);
  } catch (Exception exc) {
    std::cerr << exc.what() << std::endl;
  }
  EXPECT_NEAR(x3[1], 200.000, epsilon);
  EXPECT_NEAR(x3[2], 200.000, epsilon);
  EXPECT_NEAR(x3[4], 133.855, epsilon);
  EXPECT_NEAR(x3[5], 8.137, epsilon);
  EXPECT_NEAR(x3[6], 8.137, epsilon);
  EXPECT_NEAR(x3[7], 81.370, epsilon);

  // No Energy Loss, No Magnetic Field
  try {
    GlobalTools::propagate(x4, 2000.0, field, 10.0, pid, false);
  } catch (Exception exc) {
    std::cerr << exc.what() << std::endl;
  }
  EXPECT_NEAR(x4[1], 200.000, epsilon);
  EXPECT_NEAR(x4[2], 200.000, epsilon);
  EXPECT_NEAR(x4[4], 184.699, epsilon);
  EXPECT_NEAR(x4[5], 15.000, epsilon);
  EXPECT_NEAR(x4[6], 15.000, epsilon);
  EXPECT_NEAR(x4[7], 150.000, epsilon);
}

TEST_F(GlobalToolsTest, changeEnergy) {
  double mass = 100.0;
  double x[] = {0.0, 0.0, 0.0, 0.0, 0.0, 20.0, 10.0, 200.0};
  x[4] = ::sqrt(x[5]*x[5] + x[6]*x[6] + x[7]*x[7] + mass*mass);
  double deltaE = -20;
  GlobalTools::changeEnergy(x, deltaE, mass);
  double epsilon = 0.001;
  EXPECT_NEAR(x[4], 204.722, epsilon);
  EXPECT_NEAR(x[5], 17.753, epsilon);
  EXPECT_NEAR(x[6], 8.877, epsilon);
  EXPECT_NEAR(x[7], 177.531, epsilon);
}

TEST_F(GlobalToolsTest, TrackPointSort) {
  DataStructure::Global::SpacePoint* sp =
      new DataStructure::Global::SpacePoint();
  TLorentzVector position(0, 0, 10000.0, 0);
  sp->set_position(position);
  const DataStructure::Global::TrackPoint* tp1 =
      new const DataStructure::Global::TrackPoint(sp);
  position.SetZ(10001.1);
  sp->set_position(position);
  const DataStructure::Global::TrackPoint* tp2 =
      new const DataStructure::Global::TrackPoint(sp);

  EXPECT_TRUE(GlobalTools::TrackPointSort(tp1, tp2));
  EXPECT_FALSE(GlobalTools::TrackPointSort(tp2, tp1));
  EXPECT_FALSE(GlobalTools::TrackPointSort(tp1, tp1));

  delete sp;
  delete tp1;
  delete tp2;
}

} // ~namespace MAUS
