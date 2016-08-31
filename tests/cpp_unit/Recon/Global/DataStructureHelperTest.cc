/* This file is part of MAUS: http://  micewww.pp.rl.ac.uk:8080/projects/maus
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
 * along with MAUS.  If not, see <http://  www.gnu.org/licenses/>.
 */

/* Author: Peter Lane
 */

#include <cmath>
#include <iostream>
#include <sstream>
#include <string>

#include <cstdlib>

#include "CLHEP/Vector/ThreeVector.h"
#include "gtest/gtest.h"
#include "json/reader.h"
#include "json/value.h"

#include "Config/MiceModule.hh"
#include "DataStructure/Global/ReconEnums.hh"
#include "DataStructure/Global/TrackPoint.hh"
#include "Globals/GlobalsManager.hh"
#include "Optics/PhaseSpaceVector.hh"
#include "Recon/Global/DataStructureHelper.hh"
#include "Recon/Global/Detector.hh"
#include "Utils/Exception.hh"
#include "Utils/Globals.hh"

using MAUS::recon::global::DataStructureHelper;
namespace Recon = MAUS::recon::global;
namespace GlobalDS = MAUS::DataStructure::Global;
class DataStructureHelperTest : public testing::Test {
 public:
  DataStructureHelperTest() {
  }

  ~DataStructureHelperTest() {
  }

 protected:
};

// *************************************************
// DataStructureHelperTest static const initializations
// *************************************************

// ***********
// test cases
// ***********

TEST_F(DataStructureHelperTest, FindModulesByName) {
  MiceModule root(NULL, "root");
  MiceModule * orwell = new MiceModule(&root, "Orwell");
  MiceModule * george_orwell = new MiceModule(orwell, "George");
  MiceModule * curie = new MiceModule(&root, "Curie");
  MiceModule * marie_curie = new MiceModule(curie, "Marie");
  MiceModule * washington = new MiceModule(&root, "Washington");
  MiceModule * george_washington = new MiceModule(washington, "George");
  MiceModule * nother = new MiceModule(&root, "Nother");
  MiceModule * emmy_nother = new MiceModule(curie, "Nother");
  MiceModule * scott = new MiceModule(&root, "Scott");
  MiceModule * c_scott = new MiceModule(scott, "C");
  MiceModule * george_c_scott = new MiceModule(c_scott, "George");
  root.addDaughter(orwell);
  orwell->addDaughter(george_orwell);
  root.addDaughter(curie);
  curie->addDaughter(marie_curie);
  root.addDaughter(washington);
  washington->addDaughter(george_washington);
  root.addDaughter(nother);
  nother->addDaughter(emmy_nother);
  root.addDaughter(scott);
  scott->addDaughter(c_scott);
  c_scott->addDaughter(george_c_scott);

  const std::vector<const MiceModule *> georges
    = DataStructureHelper::GetInstance().FindModulesByName(&root, "George");
  const size_t number_of_georges = georges.size();
  ASSERT_EQ(number_of_georges, static_cast<size_t>(3));

  const std::string names[3] = {
    "root/Orwell/George",
    "root/Washington/George",
    "root/Scott/C/George"
  };
  for (size_t index = 0; index < 3; ++index) {
    std::string actual_name = georges[index]->fullName();
    std::string expected_name = names[index];
    EXPECT_EQ(expected_name, actual_name);
  }
}

TEST_F(DataStructureHelperTest, GetDetectorZPosition) {
  const GlobalDS::DetectorPoint detector_ids[14] = {
    GlobalDS::kTOF0, GlobalDS::kTOF1,
    GlobalDS::kTracker0_1, GlobalDS::kTracker0_2, GlobalDS::kTracker0_3,
    GlobalDS::kTracker0_4, GlobalDS::kTracker0_5,
    GlobalDS::kTracker1_1, GlobalDS::kTracker1_2, GlobalDS::kTracker1_3,
    GlobalDS::kTracker1_4, GlobalDS::kTracker1_5,
    GlobalDS::kTOF2,
    GlobalDS::kEMR  // used for testing unsupported detector failure
  };

  MAUS::Globals * old_globals = MAUS::Globals::GetInstance();
  // Deep copy since GlobalsManager deletes Globals' old modules
  // on call to SetReconstructionMiceModules()
  MiceModule * old_modules
    = MiceModule::deepCopy(*old_globals->GetReconstructionMiceModules());
  MiceModule * recon_modules = new MiceModule(NULL, "root");

  // *** Test missing module exception ***
  MAUS::GlobalsManager::SetReconstructionMiceModules(recon_modules);
  for (int index = 0; index < 14; ++index) {
    bool success = false;
    try {
      DataStructureHelper::GetInstance().GetDetectorZPosition(
        detector_ids[index]);
    } catch (Exceptions::Exception& exc) {
      success = true;
    }
    EXPECT_TRUE(success);
  }

  // *** Test duplicate module exception ***
  recon_modules = new MiceModule(NULL, "root");
  for (int index = 0; index < 3; ++index) {
    std::stringstream detector_name;
    detector_name << "TOF" << index << ".dat";
    recon_modules->addDaughter(
      new MiceModule(recon_modules, detector_name.str()));
    recon_modules->addDaughter(
      new MiceModule(recon_modules, detector_name.str()));
  }
  for (int index = 0; index < 2; ++index) {
    for (int station = 0; station < 5; ++station) {
      std::stringstream detector_name;
      detector_name << "Tracker";
      if (index > 0) detector_name << index;
      detector_name << "Station" << (station+1) << ".dat";
      recon_modules->addDaughter(
        new MiceModule(recon_modules, detector_name.str()));
      recon_modules->addDaughter(
        new MiceModule(recon_modules, detector_name.str()));
    }
  }
  MAUS::GlobalsManager::SetReconstructionMiceModules(recon_modules);
  for (int index = 0; index < 13; ++index) {
    bool success = false;
    try {
      DataStructureHelper::GetInstance().GetDetectorZPosition(
        detector_ids[index]);
    } catch (Exceptions::Exception& exc) {
      success = true;
    }
    EXPECT_TRUE(success);
  }

  // create a new root module since the next SetReconstructionMiceModules() call
  // will delete the original used during the above missing module tests
  recon_modules = new MiceModule(NULL, "root");

  // Setup a minimal MiceModule tree manually
  MiceModule * tof0 = new MiceModule(recon_modules, "TOF0.dat");
  tof0->addPropertyHep3Vector("Position", Hep3Vector(0., 0., -2000.));
  recon_modules->addDaughter(tof0);

  MiceModule * tof1 = new MiceModule(recon_modules, "TOF1.dat");
  tof1->addPropertyHep3Vector("Position", Hep3Vector(0., 0., -900.));
  recon_modules->addDaughter(tof1);
  MiceModule * tof1_detector = new MiceModule(tof1, "TOF1Detector.dat");
  tof1_detector->addPropertyHep3Vector("Position", Hep3Vector(0., 0., -100.));
  tof1->addDaughter(tof1_detector);

  MiceModule * tracker0 = new MiceModule(recon_modules, "Tracker0.dat");
  tracker0->addPropertyHep3Vector("Position", Hep3Vector(0., 0., 1000.));
  recon_modules->addDaughter(tracker0);
  MiceModule * tracker0_stations[5];
  for (int index = 0; index < 5; ++index) {
    std::stringstream detector_name;
    detector_name << "TrackerStation" << (index+1) << ".dat";
    tracker0_stations[index] = new MiceModule(tracker0,
                                              detector_name.str());
    tracker0_stations[index]->addPropertyHep3Vector("Position",
      Hep3Vector(0., 0., 10*(index-2)));
    tracker0->addDaughter(tracker0_stations[index]);

    MiceModule * tracker_view_w = NULL;
    if (index == 4) {  // Station 5
      tracker_view_w = new MiceModule(tracker0_stations[index],
                                      "Tracker0Station5ViewW.dat");
    } else {
      tracker_view_w = new MiceModule(tracker0_stations[index],
                                      "TrackerViewW.dat");
    }
    tracker_view_w->addPropertyHep3Vector("Position", Hep3Vector(0., 0., 0.));
    tracker0_stations[index]->addDaughter(tracker_view_w);
  }

  MiceModule * tracker1 = new MiceModule(recon_modules, "Tracker1.dat");
  tracker1->addPropertyHep3Vector("Position", Hep3Vector(0., 0., 2000.));
  recon_modules->addDaughter(tracker1);
  MiceModule * tracker1_stations[5];
  for (int index = 0; index < 5; ++index) {
    std::stringstream detector_name;
    detector_name << "Tracker1Station" << (index+1) << ".dat";
    tracker1_stations[index] = new MiceModule(tracker1,
                                              detector_name.str());
    tracker1_stations[index]->addPropertyHep3Vector("Position",
      Hep3Vector(0., 0., 10*(index-2)));
    tracker1->addDaughter(tracker1_stations[index]);

    MiceModule * tracker_view_w = new MiceModule(tracker1_stations[index],
                                                 "TrackerViewW.dat");
    tracker_view_w->addPropertyHep3Vector("Position", Hep3Vector(0., 0., 0.));
    tracker1_stations[index]->addDaughter(tracker_view_w);
  }

  MiceModule * tof2 = new MiceModule(recon_modules, "TOF2.dat");
  tof2->addPropertyHep3Vector("Position", Hep3Vector(0., 0., 3100.));
  recon_modules->addDaughter(tof2);
  MiceModule * tof2_detector = new MiceModule(tof2, "TOF2Detector.dat");
  tof2_detector->addPropertyHep3Vector("Position", Hep3Vector(0., 0., -100.));
  tof2->addDaughter(tof2_detector);

  MAUS::GlobalsManager::SetReconstructionMiceModules(recon_modules);

  // *** Test getting z positions for all TOF and Tracker stations
  const double z_positions[13] = {
    -2000, -1000,                 // TOF0, TOF1
    980, 990, 1000, 1010, 1020,   // Tracker0, Station1-5
    1980, 1990, 2000, 2010, 2020, // Tracker1, Station1-5
    3000                          // TOF2
  };
  for (int index = 0; index < 13; ++index) {
    const double detector_z = DataStructureHelper::GetInstance()
                            .GetDetectorZPosition(detector_ids[index]);
    EXPECT_EQ(z_positions[index], detector_z);
  }

  // Restore the original modules
  MAUS::GlobalsManager::SetReconstructionMiceModules(old_modules);
}

TEST_F(DataStructureHelperTest, GetDetectorAttributes) {
  std::stringstream configuration_string;
  configuration_string << "{"
    << "\"global_recon_detector_attributes\":["
      << "{"
        << "\"id\":2,"
        << "\"uncertainties\":["
          << "[4.0, 0.0, 0.0, 0.0, 0.0, 0.0],"
          << "[0.0, 1.44e7, 0.0, 0.0, 0.0, 0.0],"
          << "[0.0, 0.0, 300.0, 0.0, 0.0, 0.0],"
          << "[0.0, 0.0, 0.0, 1.0e7, 0.0, 0.0],"
          << "[0.0, 0.0, 0.0, 0.0, 300.0, 0.0],"
          << "[0.0, 0.0, 0.0, 0.0, 0.0, 1.0e7]"
        << "]"
      << "},"
      << "{"
        << "\"id\":7,"
        << "\"uncertainties\":["
          << "[4.0, 0.0, 0.0, 0.0, 0.0, 0.0],"
          << "[0.0, 1.44e7, 0.0, 0.0, 0.0, 0.0],"
          << "[0.0, 0.0, 300.0, 0.0, 0.0, 0.0],"
          << "[0.0, 0.0, 0.0, 1.0e7, 0.0, 0.0],"
          << "[0.0, 0.0, 0.0, 0.0, 300.0, 0.0],"
          << "[0.0, 0.0, 0.0, 0.0, 0.0, 1.0e7]"
        << "]"
      << "}]}";
  MAUS::recon::global::DetectorMap detectors;
  bool success = true;
  try {
    const Json::Value configuration
      = JsonWrapper::StringToJson(configuration_string.str());

    DataStructureHelper::GetInstance().GetDetectorAttributes(
        configuration, detectors);
  } catch (Exceptions::Exception& exception) {
    success = false;
  } catch (std::exception& exc) {
    success = false;
  }
  ASSERT_TRUE(success);

  Recon::DetectorMap::iterator detector = detectors.begin();
  const GlobalDS::DetectorPoint ids[2] = {GlobalDS::kTOF0, GlobalDS::kTOF1};
  const double uncertainty_data[36] = {
    4.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 1.44e7, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 300.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 1.0e7, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 300.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 1.0e7
  };
  size_t index = 0;
  while (detector != detectors.end()) {
    const GlobalDS::DetectorPoint id = detector->first;
    EXPECT_EQ(ids[index], id);
    const MAUS::CovarianceMatrix uncertainties
      = detector->second.uncertainties();
    for (size_t row = 0; row < 6; ++row) {
      for (size_t column = 0; column < 6; ++column) {
        EXPECT_EQ(uncertainty_data[row*6+column],
                  uncertainties(row+1, column+1));
      }
    }
    ++index;
    ++detector;
  }

  // *** Test wrong size uncertainty matrices (GetJsonCovariance failure) ***
  std::stringstream bad_rows_string;
  bad_rows_string << "{"
    << "\"global_recon_detector_attributes\":["
      << "{"
        << "\"id\":2,"
        << "\"uncertainties\":["
          << "[4.0, 0.0, 0.0, 0.0, 0.0, 0.0],"
          << "[0.0, 1.44e7, 0.0, 0.0, 0.0, 0.0],"
          << "[0.0, 0.0, 300.0, 0.0, 0.0, 0.0],"
          << "[0.0, 0.0, 0.0, 0.0, 300.0, 0.0],"
          << "[0.0, 0.0, 0.0, 0.0, 0.0, 1.0e7]"
        << "]"
      << "}]}";
  success = false;
  try {
    const Json::Value configuration
      = JsonWrapper::StringToJson(bad_rows_string.str());

    DataStructureHelper::GetInstance().GetDetectorAttributes(
        configuration, detectors);
  } catch (Exceptions::Exception& exception) {
    success = true;
  } catch (std::exception& exc) {
    success = true;
  }
  ASSERT_TRUE(success);

  std::stringstream bad_columns_string;
  bad_columns_string << "{"
    << "\"global_recon_detector_attributes\":["
      << "{"
        << "\"id\":2,"
        << "\"uncertainties\":["
          << "[4.0, 0.0, 0.0, 0.0, 0.0, 0.0],"
          << "[0.0, 1.44e7, 0.0, 0.0, 0.0, 0.0],"
          << "[0.0, 0.0, 300.0, 0.0, 0.0, 0.0],"
          << "[0.0, 0.0, 0.0, 1.0e7, 0.0],"
          << "[0.0, 0.0, 0.0, 0.0, 300.0, 0.0],"
          << "[0.0, 0.0, 0.0, 0.0, 0.0, 1.0e7]"
        << "]"
      << "}]}";
  success = false;
  try {
    const Json::Value configuration
      = JsonWrapper::StringToJson(bad_columns_string.str());

    DataStructureHelper::GetInstance().GetDetectorAttributes(
        configuration, detectors);
  } catch (Exceptions::Exception& exception) {
    success = true;
  } catch (std::exception& exc) {
    success = true;
  }
  ASSERT_TRUE(success);
}

TEST_F(DataStructureHelperTest, TrackPoint2PhaseSpaceVector) {
  // t, E, x, Px, y, Py
  const double coordinates[6] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
  GlobalDS::TrackPoint track_point;
  TLorentzVector position(coordinates[2], coordinates[4], 0., coordinates[0]);
  track_point.set_position(position);
  TLorentzVector momentum(coordinates[3], coordinates[5], 0., coordinates[1]);
  track_point.set_momentum(momentum);
  MAUS::PhaseSpaceVector phase_space_vector = DataStructureHelper::GetInstance()
    .TrackPoint2PhaseSpaceVector(track_point);
  for (int index = 0; index < 6; ++index) {
    std::cout << "Coordinate " << index << "..." << std::endl;
    // Order is t, E, x, Px, y, Py
    EXPECT_EQ(coordinates[index], phase_space_vector[index]);
  }
}

TEST_F(DataStructureHelperTest, PhaseSpaceVector2TrackPoint) {
  // *** Test on-mass-shell conversion ***
  const double positions[4] = {1.0, 3.0, 5.0, 7.0};       // x, y, z, t
  const double momenta[4] = {2.0, 4.0, 792.979388, 800.0}; // Px, Py, Pz, E
  MAUS::PhaseSpaceVector phase_space_vector(positions[3], momenta[3],
                                            positions[0], momenta[0],
                                            positions[1], momenta[1]);
  GlobalDS::TrackPoint track_point = DataStructureHelper::GetInstance()
    .PhaseSpaceVector2TrackPoint(phase_space_vector, positions[2],
                                 GlobalDS::kMuMinus);
  TLorentzVector position = track_point.get_position();
  TLorentzVector momentum = track_point.get_momentum();
  for (int index = 0; index < 4; ++index) {
    // Order is x, y, z, t
    EXPECT_NEAR(positions[index], position[index], 1.e-6);
    EXPECT_NEAR(momenta[index], momentum[index], 1.e-6);
  }

  // *** Test off-mass-shell conversion (Pz set to 0.) ***
  const double bad_momenta[4] = {2.0, 4.0, 0.0, 8.0}; // Px, Py, Pz, E
  phase_space_vector = MAUS::PhaseSpaceVector(positions[3], bad_momenta[3],
                                              positions[0], bad_momenta[0],
                                              positions[1], bad_momenta[1]);
  track_point = DataStructureHelper::GetInstance().PhaseSpaceVector2TrackPoint(
    phase_space_vector, positions[2], GlobalDS::kMuMinus);
  position = track_point.get_position();
  momentum = track_point.get_momentum();
  for (int index = 0; index < 4; ++index) {
    // Order is x, y, z, t
    EXPECT_NEAR(positions[index], position[index], 1.e-6);
    EXPECT_NEAR(bad_momenta[index], momentum[index], 1.e-6);
  }
}
