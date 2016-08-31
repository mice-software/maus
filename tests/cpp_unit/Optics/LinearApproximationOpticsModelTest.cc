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

#include "gtest/gtest.h"
#include "CLHEP/Vector/Rotation.h"
#include "CLHEP/Vector/ThreeVector.h"

#include "BeamTools/BTTracker.hh"
#include "Utils/Exception.hh"
#include "src/common_cpp/Globals/GlobalsManager.hh"
#include "src/common_cpp/Optics/CovarianceMatrix.hh"
#include "src/common_cpp/Optics/LinearApproximationOpticsModel.hh"
#include "src/common_cpp/Optics/PhaseSpaceVector.hh"
#include "src/common_cpp/Optics/TransferMap.hh"
#include "src/common_cpp/Simulation/MAUSGeant4Manager.hh"
#include "src/common_cpp/Simulation/MAUSPhysicsList.hh"
#include "src/common_cpp/Simulation/VirtualPlanes.hh"
#include "src/common_cpp/Utils/Globals.hh"

#include "json/reader.h"
#include "json/value.h"

using MAUS::LinearApproximationOpticsModel;

Json::Value SetupConfig(int verbose_level);

class LinearApproximationOpticsModelTest : public testing::Test {
 public:
  LinearApproximationOpticsModelTest()
      : default_virtual_planes_(new MAUS::VirtualPlaneManager(
          *MAUS::MAUSGeant4Manager::GetInstance()->GetVirtualPlanes())),
        virtual_planes_(new MAUS::VirtualPlaneManager()) {
    MAUS::MAUSGeant4Manager * simulation
        = MAUS::MAUSGeant4Manager::GetInstance();

    Json::Value * config = MAUS::Globals::GetConfigurationCards();
    orig_verbose_level = (*config)["verbose_level"].asInt();
    (*config)["verbose_level"] = Json::Value(2);
    (*config)["reference_physics_processes"] = Json::Value("none");
    (*config)["physics_processes"] = Json::Value("none");
    (*config)["particle_decay"] = Json::Value(false);
    simulation->GetPhysicsList()->Setup();

    (*config)["simulation_reference_particle"] = JsonWrapper::StringToJson(
      std::string("{\"position\":{\"x\":0.0,\"y\":0.0,\"z\":-1000.0},")+
      std::string("\"momentum\":{\"x\":0.0,\"y\":0.0,\"z\":200.0},")+
      std::string("\"spin\":{\"x\":0.0,\"y\":-0.0,\"z\":1.0},")+
      std::string("\"particle_id\":-13,\"energy\":226.1939223,\"time\":0.0,")+
      std::string("\"random_seed\":2}")
    );

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
    std::string config_string = JsonWrapper::JsonToString(*config);

    MAUS::GlobalsManager::DeleteGlobals();
    MAUS::GlobalsManager::InitialiseGlobals(config_string);

    simulation->SetVirtualPlanes(virtual_planes_);
    MAUS::VirtualPlane start_plane = MAUS::VirtualPlane::BuildVirtualPlane(
        CLHEP::HepRotation(), CLHEP::Hep3Vector(0., 0., kPrimaryPlane), -1,
        true, kPrimaryPlane, BTTracker::z, MAUS::VirtualPlane::ignore, false);
    virtual_planes_->AddPlane(new MAUS::VirtualPlane(start_plane), NULL);

    MAUS::VirtualPlane mid_plane = MAUS::VirtualPlane::BuildVirtualPlane(
        CLHEP::HepRotation(), CLHEP::Hep3Vector(0., 0., kPrimaryPlane+1000.),
        -1, true, kPrimaryPlane+1000., BTTracker::z, MAUS::VirtualPlane::ignore,
        false);
    virtual_planes_->AddPlane(new MAUS::VirtualPlane(mid_plane), NULL);

    MAUS::VirtualPlane end_plane = MAUS::VirtualPlane::BuildVirtualPlane(
        CLHEP::HepRotation(), CLHEP::Hep3Vector(0., 0., kPrimaryPlane+2000.),
        -1, true, kPrimaryPlane+2000., BTTracker::z, MAUS::VirtualPlane::ignore,
        false);
    virtual_planes_->AddPlane(new MAUS::VirtualPlane(end_plane), NULL);
  }

  ~LinearApproximationOpticsModelTest() {
    MAUS::GlobalsManager::DeleteGlobals();
    // SetupConfig() is defined in MAUSUnitTest.cc
    MAUS::GlobalsManager::InitialiseGlobals(
        JsonWrapper::JsonToString(SetupConfig(orig_verbose_level)));

    MAUS::MAUSGeant4Manager::GetInstance()
        ->SetVirtualPlanes(default_virtual_planes_);
    std::cout << "*** Reset Globals ***" << std::endl;
  }

  static const double kPrimaryPlane;
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

  static const double kCovariances[36];
  static const MAUS::CovarianceMatrix kCovarianceMatrix;
 private:
  MAUS::VirtualPlaneManager * const default_virtual_planes_;
  MAUS::VirtualPlaneManager * const virtual_planes_;
  int orig_verbose_level;
};

// *************************************************
// LinearApproximationOpticsModelTest static const initializations
// *************************************************
const double LinearApproximationOpticsModelTest::kPrimaryPlane = -1000;
const double LinearApproximationOpticsModelTest::kCovariances[36] = {
  0., 1., 2., 3., 4., 5.,
  1., 2., 3., 4., 5., 6.,
  2., 3., 4., 5., 6., 7.,
  3., 4., 5., 6., 7., 8.,
  4., 5., 6., 7., 8., 9.,
  5., 6., 7., 8., 9., 10.
};
const MAUS::CovarianceMatrix
LinearApproximationOpticsModelTest::kCovarianceMatrix(
  LinearApproximationOpticsModelTest::kCovariances);

// ***********
// test cases
// ***********

TEST_F(LinearApproximationOpticsModelTest, Constructor) {
  const LinearApproximationOpticsModel optics_model(
      MAUS::Globals::GetConfigurationCards());
}

TEST_F(LinearApproximationOpticsModelTest, Accessors) {
  LinearApproximationOpticsModel optics_model(
      MAUS::Globals::GetConfigurationCards());
  double primary_plane = optics_model.primary_plane();
  ASSERT_DOUBLE_EQ(kPrimaryPlane, primary_plane);

  optics_model.set_primary_plane(0.);
  primary_plane = optics_model.primary_plane();
  ASSERT_DOUBLE_EQ(0., primary_plane);
}

TEST_F(LinearApproximationOpticsModelTest, Transport) {
  LinearApproximationOpticsModel optics_model(
      MAUS::Globals::GetConfigurationCards());
  // The configuration specifies a 2m drift between -1m and +1 m.
  optics_model.Build();

  // Check transport to start plane
  MAUS::PhaseSpaceVector input_vector(0., 226., 1., 0., 3., 0.);
  MAUS::PhaseSpaceVector output_vector = optics_model.Transport(input_vector,
                                                                kPrimaryPlane);
  EXPECT_EQ(input_vector, output_vector);

  MAUS::CovarianceMatrix output_errors
      = optics_model.Transport(kCovarianceMatrix, kPrimaryPlane);
  EXPECT_EQ(kCovarianceMatrix, output_errors);

  // Check transport to end plane
  MAUS::PhaseSpaceVector expected_vector(7.5466, 226., 1., 0., 3., 0.);
  output_vector = optics_model.Transport(input_vector, kPrimaryPlane+2000.);
  for (int index = 0; index < 6; ++index) {
    EXPECT_NEAR(expected_vector[index], output_vector[index], 5.0e-4);
  }

  output_errors = optics_model.Transport(kCovarianceMatrix,
                                         kPrimaryPlane+2000.);
  EXPECT_EQ(kCovarianceMatrix, output_errors);

  // Check transport to mid plane
  expected_vector = MAUS::PhaseSpaceVector(3.7733, 226., 1., 0., 3., 0.);
  output_vector = optics_model.Transport(input_vector, kPrimaryPlane+1000.);
  for (int index = 0; index < 6; ++index) {
    EXPECT_NEAR(expected_vector[index], output_vector[index], 5.0e-4);
  }

  output_errors = optics_model.Transport(kCovarianceMatrix,
                                         kPrimaryPlane+1000.);
  EXPECT_EQ(kCovarianceMatrix, output_errors);

  // Check transport between mid plane and end plane
  output_vector = optics_model.Transport(input_vector,
                                         kPrimaryPlane+1000.,
                                         kPrimaryPlane+2000.);
  for (int index = 0; index < 6; ++index) {
    EXPECT_NEAR(expected_vector[index], output_vector[index], 5.0e-4);
  }

  output_errors = optics_model.Transport(kCovarianceMatrix,
                                         kPrimaryPlane+1000.,
                                         kPrimaryPlane+2000.);
  EXPECT_EQ(kCovarianceMatrix, output_errors);

  // Check failure for off mass shell particle transport
  MAUS::PhaseSpaceVector off_shell_vector(0., 1., 0., 0., 0., 0.);
  bool transport_failed = false;
  try {
    optics_model.Transport(off_shell_vector, kPrimaryPlane);
  } catch (MAUS::Exceptions::Exception exc) {
    transport_failed = true;
  }
  EXPECT_TRUE(transport_failed);
}
