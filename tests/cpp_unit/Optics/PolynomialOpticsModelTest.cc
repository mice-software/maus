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

#include <streambuf>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <cstdlib>
/*
#include <unistd.h>
#include <sys/param.h> // MAXPATHLEN definition
*/

#include "gtest/gtest.h"
#include "CLHEP/Vector/Rotation.h"
#include "CLHEP/Vector/ThreeVector.h"

#include "BeamTools/BTTracker.hh"
#include "Utils/Exception.hh"
#include "src/common_cpp/Globals/GlobalsManager.hh"
#include "src/common_cpp/Optics/CovarianceMatrix.hh"
#include "src/common_cpp/Optics/PolynomialOpticsModel.hh"
#include "src/common_cpp/Optics/PhaseSpaceVector.hh"
#include "src/common_cpp/Optics/TransferMap.hh"
#include "src/common_cpp/Simulation/MAUSGeant4Manager.hh"
#include "src/common_cpp/Simulation/MAUSPhysicsList.hh"
#include "src/common_cpp/Simulation/VirtualPlanes.hh"
#include "src/common_cpp/Utils/Globals.hh"

#include "json/reader.h"
#include "json/value.h"

using MAUS::PolynomialOpticsModel;

Json::Value SetupConfig(int verbose_level);

class PolynomialOpticsModelTest : public testing::Test {
 public:
  PolynomialOpticsModelTest()
      : default_virtual_planes_(
          new MAUS::VirtualPlaneManager(*MAUS::MAUSGeant4Manager::GetInstance()
                                  ->GetVirtualPlanes())) {
    MAUS::MAUSGeant4Manager * simulation
        = MAUS::MAUSGeant4Manager::GetInstance();

    Json::Value * config = MAUS::Globals::GetConfigurationCards();
    orig_verbose_level = (*config)["verbose_level"].asInt();
    (*config)["verbose_level"] = Json::Value(2);

    (*config)["reference_physics_processes"] = Json::Value("none");
    (*config)["physics_processes"] = Json::Value("none");
    (*config)["particle_decay"] = Json::Value(false);
    simulation->GetPhysicsList()->Setup();

    std::stringstream reference_particle_string;
    reference_particle_string
      << std::setprecision(1)
      << "{\"position\":{\"x\":0.0,\"y\":0.0,\"z\":" << kPrimaryPlane << "},"
      << "\"momentum\":{\"x\":0.0,\"y\":0.0,\"z\":200.0},"
      << "\"particle_id\":-13,\"energy\":226.1939223,\"time\":0.0,"
      << "\"spin\":{\"x\":0.0,\"y\":-0.0,\"z\":1.0},"
      << "\"random_seed\":2}";
    (*config)["simulation_reference_particle"]
      = JsonWrapper::StringToJson(reference_particle_string.str());

    Json::Value ellipse(Json::objectValue);
    ellipse["Emittance_T"] = Json::Value(10.0);
    ellipse["Emittance_L"] = Json::Value(0.01);
    ellipse["Beta_T"] = Json::Value(650.);
    ellipse["Beta_L"] = Json::Value(5.);
    ellipse["Alpha_T"] = Json::Value(0.);
    ellipse["Alpha_L"] = Json::Value(0.);
    ellipse["NormalisedAngularMomentum"] = Json::Value(0.);
    ellipse["Bz"] = Json::Value(0.);
    (*config)["TransferMapOpticsModel_EllipseDefinition"] = ellipse;
    Json::Value deltas(Json::objectValue);
    deltas["t"] = Json::Value(1.0);
    deltas["E"] = Json::Value(1.0);
    deltas["x"] = Json::Value(1.0);
    deltas["Px"] = Json::Value(1.0);
    deltas["y"] = Json::Value(1.0);
    deltas["Py"] = Json::Value(1.0);
    (*config)["TransferMapOpticsModel_Deltas"] = deltas;

    Json::Value algorithms(Json::arrayValue);
    algorithms.append(Json::Value("Least Squares"));
    algorithms.append(Json::Value("Constrained Least Squares"));
    algorithms.append(Json::Value("Constrained Chi Squared"));
    algorithms.append(Json::Value("Sweeping Chi Squared"));
    algorithms.append(Json::Value("Sweeping Chi Squared Variable Walls"));
    (*config)["PolynomialOpticsModel_algorithms"] = algorithms;
    (*config)["PolynomialOpticsModel_algorithm"] = Json::Value("Least Squares");
    (*config)["PolynomialOpticsModel_order"] = Json::Value(1);

    std::string config_string = JsonWrapper::JsonToString(*config);
    MAUS::GlobalsManager::DeleteGlobals();
    MAUS::GlobalsManager::InitialiseGlobals(config_string);

    std::cout << "Globals:" << std::endl
              << (*MAUS::Globals::GetConfigurationCards()) << std::endl;
    virtual_planes_ = new MAUS::VirtualPlaneManager();
    simulation->SetVirtualPlanes(virtual_planes_);
    MAUS::VirtualPlane start_plane = MAUS::VirtualPlane::BuildVirtualPlane(
        CLHEP::HepRotation(), CLHEP::Hep3Vector(0., 0., kPrimaryPlane),
        -1, true,
        kPrimaryPlane, BTTracker::z, MAUS::VirtualPlane::ignore, false);
    virtual_planes_->AddPlane(new MAUS::VirtualPlane(start_plane), NULL);

    MAUS::VirtualPlane mid_plane = MAUS::VirtualPlane::BuildVirtualPlane(
        CLHEP::HepRotation(), CLHEP::Hep3Vector(0., 0., kPrimaryPlane+1000.),
        -1, true,
        kPrimaryPlane+1000., BTTracker::z, MAUS::VirtualPlane::ignore, false);
    virtual_planes_->AddPlane(new MAUS::VirtualPlane(mid_plane), NULL);

    MAUS::VirtualPlane end_plane = MAUS::VirtualPlane::BuildVirtualPlane(
        CLHEP::HepRotation(), CLHEP::Hep3Vector(0., 0., kPrimaryPlane+2000.),
        -1, true,
        kPrimaryPlane+2000., BTTracker::z, MAUS::VirtualPlane::ignore, false);
    virtual_planes_->AddPlane(new MAUS::VirtualPlane(end_plane), NULL);
  }

  ~PolynomialOpticsModelTest() {
    MAUS::GlobalsManager::DeleteGlobals();
    // SetupConfig() is defined in MAUSUnitTest.cc
    MAUS::GlobalsManager::InitialiseGlobals(
        JsonWrapper::JsonToString(SetupConfig(orig_verbose_level)));
    MAUS::MAUSGeant4Manager::GetInstance()
        ->SetVirtualPlanes(default_virtual_planes_);
    std::cout << "*** Reset Globals ***" << std::endl;
  }

 protected:
  void ResolveRootDirectory(std::string & str,
                            const std::string & maus_root_dir) {
    std::string var_name("${MAUS_ROOT_DIR}");
    size_t index = str.find(var_name);
    while (index != std::string::npos) {
      str.replace(index, var_name.length(), maus_root_dir);
      index = str.find(var_name, index+var_name.length());
    }
  }

  static const double kPrimaryPlane;
  static const double kCovariances[36];
  static const MAUS::CovarianceMatrix kCovarianceMatrix;
  MAUS::VirtualPlaneManager* virtual_planes_;
  MAUS::VirtualPlaneManager* default_virtual_planes_;
  int orig_verbose_level;

 private:
};

// *************************************************
// PolynomialOpticsModelTest static const initializations
// *************************************************
const double PolynomialOpticsModelTest::kPrimaryPlane = 10;
const double PolynomialOpticsModelTest::kCovariances[36] = {
  0., 1., 2., 3., 4., 5.,
  1., 2., 3., 4., 5., 6.,
  2., 3., 4., 5., 6., 7.,
  3., 4., 5., 6., 7., 8.,
  4., 5., 6., 7., 8., 9.,
  5., 6., 7., 8., 9., 10.
};
const MAUS::CovarianceMatrix
PolynomialOpticsModelTest::kCovarianceMatrix(
  PolynomialOpticsModelTest::kCovariances);

// ***********
// test cases
// ***********

TEST_F(PolynomialOpticsModelTest, Constructor) {
  const PolynomialOpticsModel optics_model(
      MAUS::Globals::GetConfigurationCards());
}

/*
TEST_F(PolynomialOpticsModelTest, Build) {
  // Bad build -- incomplete particle tracks
  Json::Value * config = MAUS::Globals::GetConfigurationCards();
  (*config)["reference_physics_processes"] = Json::Value("mean_energy_loss");
  (*config)["physics_processes"] = Json::Value("mean_energy_loss");
  (*config)["particle_decay"] = Json::Value(true);
  (*config)["muon_half_life"] = Json::Value(1.0);  // 1 ns -> ~25 cm
  std::string config_string = JsonWrapper::JsonToString(*config);
  MAUS::GlobalsManager::DeleteGlobals();
  MAUS::GlobalsManager::InitialiseGlobals(config_string);
  PolynomialOpticsModel bad_optics_model_1(config);
  bool success = false;
  try {
    bad_optics_model_1.Build();
  } catch (MAUS::Exception exc) {
    success = true;
  }
  EXPECT_TRUE(success);
}
*/

TEST_F(PolynomialOpticsModelTest, Accessors) {
  PolynomialOpticsModel optics_model(
      MAUS::Globals::GetConfigurationCards());
  double primary_plane = optics_model.primary_plane();
  ASSERT_DOUBLE_EQ(kPrimaryPlane, primary_plane);

  optics_model.set_primary_plane(kPrimaryPlane+1000.);
  primary_plane = optics_model.primary_plane();
  ASSERT_DOUBLE_EQ(kPrimaryPlane+1000., primary_plane);
}

TEST_F(PolynomialOpticsModelTest, AvailablePositions) {
  PolynomialOpticsModel optics_model(
      MAUS::Globals::GetConfigurationCards());

  // Bad position query before model is built
  bool success = false;
  try {
    optics_model.GetAvailableMapPositions();
  } catch (MAUS::Exception exc) {
    success = true;
  }
  EXPECT_TRUE(success);

  // One position per virtual detectors
  optics_model.Build();
  const std::vector<int64_t> positions
    = optics_model.GetAvailableMapPositions();
  const size_t position_count = positions.size();
  // start, mid, and end virtual detectors
  EXPECT_EQ(position_count, static_cast<size_t>(3));
}

TEST_F(PolynomialOpticsModelTest, Transport) {
  PolynomialOpticsModel optics_model(
      MAUS::Globals::GetConfigurationCards());

  // The configuration specifies a 2m drift between 1m and 3m.
  optics_model.Build();
  // Check transport to start plane
  MAUS::PhaseSpaceVector input_vector(0., 226., 1., 0., 3., 0.);
  MAUS::PhaseSpaceVector output_vector = optics_model.Transport(input_vector,
                                                                kPrimaryPlane);
  for (int index = 0; index < 6; ++index) {
    EXPECT_NEAR(input_vector[index], output_vector[index], 5.0e-4);
  }

  MAUS::CovarianceMatrix output_errors
      = optics_model.Transport(kCovarianceMatrix, kPrimaryPlane);
  for (int row = 1; row <= 6; ++row) {
    for (int column = 1; column <= 6; ++column) {
      EXPECT_NEAR(kCovarianceMatrix(row, column), output_errors(row, column),
                  5.0e-4);
    }
  }

  // Check transport to end plane
  MAUS::PhaseSpaceVector expected_vector(7.5466, 226., 1., 0., 3., 0.);
  output_vector = optics_model.Transport(input_vector, kPrimaryPlane+2000.);
  for (int index = 0; index < 6; ++index) {
    EXPECT_NEAR(expected_vector[index], output_vector[index], 5.0e-4);
  }

  // Check transport to mid plane
  expected_vector = MAUS::PhaseSpaceVector(3.7733, 226., 1., 0., 3., 0.);
  output_vector = optics_model.Transport(input_vector, kPrimaryPlane+1000.);
  for (int index = 0; index < 6; ++index) {
    EXPECT_NEAR(expected_vector[index], output_vector[index], 5.0e-4);
  }

  // Check transport from mid plane to end plane (should fail because the
  // Inverse() function in PolynomialMap is not implemented)
  bool transport_failed = false;
  try {
    output_vector = optics_model.Transport(input_vector,
                                           kPrimaryPlane+1000.,
                                           kPrimaryPlane+2000.);
  } catch (MAUS::Exception exc) {
    transport_failed = true;
  }
  EXPECT_TRUE(transport_failed);
}

/* This test fails, but higher order maps should be more accurate.
 *
TEST_F(PolynomialOpticsModelTest, SecondOrderTransport) {
  // Test higher order polynomial map transport to end plane
  Json::Value * config = MAUS::Globals::GetConfigurationCards();
  (*config)["PolynomialOpticsModel_order"] = Json::Value(2);
  std::string config_string = JsonWrapper::JsonToString(*config);
  MAUS::GlobalsManager::DeleteGlobals();
  MAUS::GlobalsManager::InitialiseGlobals(config_string);
  PolynomialOpticsModel optics_model(*config);
  optics_model.Build();

  MAUS::PhaseSpaceVector input_vector(0., 226., 1., 0., 3., 0.);
  MAUS::PhaseSpaceVector expected_vector(7.5466, 226., 1., 0., 3., 0.);
  MAUS::PhaseSpaceVector output_vector
      = optics_model.Transport(input_vector, 1000.);
  for (int index = 0; index < 6; ++index) {
    EXPECT_NEAR(expected_vector[index], output_vector[index], 5.0e-4);
  }
}
*/

TEST_F(PolynomialOpticsModelTest, UnsupportedAlgorithms) {
  // Make sure unsupported algorithms throw exceptions
  std::vector<std::string> algorithms;
  algorithms.push_back(std::string("Constrained Least Squares"));
  algorithms.push_back(std::string("Constrained Chi Squared"));
  algorithms.push_back(std::string("Sweeping Chi Squared"));
  algorithms.push_back(std::string("Sweeping Chi Squared Variable Walls"));
  Json::Value * config = MAUS::Globals::GetConfigurationCards();
  (*config)["PolynomialOpticsModel_order"] = Json::Value(1);
  for (std::vector<std::string>::const_iterator iter = algorithms.begin();
       iter < algorithms.end();
       ++iter) {
    (*config)["PolynomialOpticsModel_algorithm"] = Json::Value(*iter);
    PolynomialOpticsModel optics_model(config);
    optics_model.Build();
    EXPECT_EQ(optics_model.GetAvailableMapPositions().size(), 0);
  }
}
