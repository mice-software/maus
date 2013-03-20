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


#include <cstdlib>
#include <ctime>
#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>

#include "gtest/gtest.h"

#include "src/common_cpp/Optics/PhaseSpaceVector.hh"
#include "src/common_cpp/Optics/CovarianceMatrix.hh"
#include "Recon/Global/Detector.hh"
#include "Recon/Global/Particle.hh"
#include "Recon/Global/TrackPoint.hh"

using MAUS::recon::global::Detector;
using MAUS::recon::global::Particle;
using MAUS::recon::global::TrackPoint;

class TrackPointTest : public ::testing::Test {
 public:
  TrackPointTest() {
  }
 protected:
  static const double kDetectorPlane;
  static const double kUncertaintyData[36];
  static const MAUS::CovarianceMatrix kUncertainties;

  virtual void SetUp() { }

  virtual void TearDown() { }
};

// ************************************************//
// TrackPointTest static const initializations//
// ************************************************//

const double TrackPointTest::kDetectorPlane = 78.6;

const double TrackPointTest::kUncertaintyData[36] = {
  +0.,      1.,       2.,       3.,      -5.,      -6.,
  +1.,    -13.,     -21.,      34.5,     55.7,     13.2,
  +2.,    -21.,     -32.5,    -57.5,    -91.2,    -23.4,
  +3.,     34.5,    -57.5,      2.65,    -3.58,    59.9,
  -5.,    -55.7,    -91.2,     -3.58,     3.38,    67.4,
  -6.,     13.2,    -23.4,     59.9,     67.4,      5.12
};

const MAUS::CovarianceMatrix TrackPointTest::kUncertainties
  = MAUS::CovarianceMatrix(TrackPointTest::kUncertaintyData);

// ***********
// test cases
// ***********

TEST_F(TrackPointTest, DefaultConstructor) {
  TrackPoint explicit_default_point;
  explicit_default_point.set_t(0.0);
  explicit_default_point.set_E(0.0);
  explicit_default_point.set_x(0.0);
  explicit_default_point.set_Px(0.0);
  explicit_default_point.set_y(0.0);
  explicit_default_point.set_Py(0.0);
  explicit_default_point.set_z(0.0);
  explicit_default_point.set_detector_id(
      MAUS::DataStructure::Global::kUndefined);
  const MAUS::CovarianceMatrix null_uncertainties;
  explicit_default_point.set_uncertainties(null_uncertainties);

  const TrackPoint default_point;

  // verify with getters
  ASSERT_EQ(0.0, default_point.t());
  ASSERT_EQ(0.0, default_point.E());
  ASSERT_EQ(0.0, default_point.x());
  ASSERT_EQ(0.0, default_point.Px());
  ASSERT_EQ(0.0, default_point.y());
  ASSERT_EQ(0.0, default_point.Py());
  ASSERT_EQ(0.0, default_point.z());
  ASSERT_EQ(MAUS::DataStructure::Global::kUndefined,
            default_point.detector_id());
  const MAUS::CovarianceMatrix uncertainties = default_point.uncertainties();
  ASSERT_EQ(null_uncertainties, uncertainties);

  bool non_particle = false;
  try {
    default_point.Pz();
  } catch (Squeal squeal) {
    non_particle = true;
  }
  ASSERT_TRUE(non_particle);
}

TEST_F(TrackPointTest, CopyConstructor) {
  TrackPoint test_point;
  test_point.set_t(0.0);
  test_point.set_E(1.1);
  test_point.set_x(2.2);
  test_point.set_Px(3.3);
  test_point.set_y(4.4);
  test_point.set_Py(5.5);
  test_point.set_z(6.6);
  test_point.set_detector_id(MAUS::DataStructure::Global::kTOF1_1);
  ASSERT_EQ(static_cast<size_t>(6), kUncertainties.size());
  test_point.set_uncertainties(kUncertainties);

  const TrackPoint point(test_point);

  ASSERT_EQ(test_point, point);
}

TEST_F(TrackPointTest, ArrayConstructor) {
  TrackPoint test_point;
  test_point.set_t(0.0);
  test_point.set_E(1.1);
  test_point.set_x(2.2);
  test_point.set_Px(3.3);
  test_point.set_y(4.4);
  test_point.set_Py(5.5);
  test_point.set_z(6.6);
  test_point.set_detector_id(MAUS::DataStructure::Global::kUndefined);
  MAUS::CovarianceMatrix null_uncertainties;
  test_point.set_uncertainties(null_uncertainties);

  const double coordinates[6] = { 0.0, 1.1, 2.2, 3.3, 4.4, 5.5 };
  const TrackPoint point(coordinates, 6.6);

  ASSERT_EQ(test_point, point);
}

TEST_F(TrackPointTest, ReconstructedConstructor) {
  TrackPoint test_point;
  test_point.set_t(0.0);
  test_point.set_E(1.1);
  test_point.set_x(2.2);
  test_point.set_Px(3.3);
  test_point.set_y(4.4);
  test_point.set_Py(5.5);
  test_point.set_z(6.6);
  test_point.set_uncertainties(kUncertainties);

  const TrackPoint point(0.0, 1.1, 2.2, 3.3, 4.4, 5.5, kUncertainties, 6.6);

  ASSERT_EQ(test_point, point);
}

TEST_F(TrackPointTest, DetectorConstructor) {
  TrackPoint test_point;
  test_point.set_t(0.0);
  test_point.set_E(1.1);
  test_point.set_x(2.2);
  test_point.set_Px(3.3);
  test_point.set_y(4.4);
  test_point.set_Py(5.5);
  test_point.set_z(6.6);
  test_point.set_detector_id(MAUS::DataStructure::Global::kTracker2_3);
  test_point.set_uncertainties(kUncertainties);

  const Detector detector(MAUS::DataStructure::Global::kTracker2_3,
                          kDetectorPlane,
                          kUncertainties);
  const TrackPoint point(0.0, 1.1, 2.2, 3.3, 4.4, 5.5, detector, 6.6);

  ASSERT_EQ(test_point, point);
}

TEST_F(TrackPointTest, VectorConstructor) {
  TrackPoint test_point;
  test_point.set_t(0.0);
  test_point.set_E(1.1);
  test_point.set_x(2.2);
  test_point.set_Px(3.3);
  test_point.set_y(4.4);
  test_point.set_Py(5.5);
  test_point.set_z(6.6);
  test_point.set_detector_id(MAUS::DataStructure::Global::kUndefined);
  MAUS::CovarianceMatrix null_uncertainties;
  test_point.set_uncertainties(null_uncertainties);

  const double coordinates[6] = { 0.0, 1.1, 2.2, 3.3, 4.4, 5.5 };
  const MAUS::Vector<double> coordinate_vector(coordinates, 6);
  const TrackPoint point(coordinate_vector, 6.6);

  ASSERT_EQ(test_point, point);
}

TEST_F(TrackPointTest, PhaseSpaceVectorConstructor) {
  TrackPoint test_point;
  test_point.set_t(0.0);
  test_point.set_E(1.1);
  test_point.set_x(2.2);
  test_point.set_Px(3.3);
  test_point.set_y(4.4);
  test_point.set_Py(5.5);
  test_point.set_z(6.6);
  test_point.set_particle_id(MAUS::DataStructure::Global::kMuMinus);

  const double coordinates[6] = { 0.0, 1.1, 2.2, 3.3, 4.4, 5.5 };
  const MAUS::PhaseSpaceVector coordinate_vector(coordinates);
  const TrackPoint point(coordinate_vector, 6.6,
                         MAUS::DataStructure::Global::kMuMinus);

  ASSERT_EQ(test_point, point);
}

TEST_F(TrackPointTest, Equality) {
  TrackPoint test_point;
  test_point.set_t(0.0);
  test_point.set_E(1000.);  // avoid being off mass shell
  test_point.set_x(2.2);
  test_point.set_Px(3.3);
  test_point.set_y(4.4);
  test_point.set_Py(5.5);
  test_point.set_z(6.6);
  test_point.set_detector_id(MAUS::DataStructure::Global::kTOF1_1);
  test_point.set_particle_id(MAUS::DataStructure::Global::kMuMinus);
  test_point.set_uncertainties(kUncertainties);

  ASSERT_EQ(test_point, test_point);

  TrackPoint point;

  // Test equality with a null point
  const TrackPoint null_point;
  EXPECT_EQ(null_point, point);

  point.set_t(0.0);
  point.set_E(1000.);  // avoid being off mass shell
  point.set_x(2.2);
  point.set_Px(3.3);
  point.set_y(4.4);
  point.set_Py(5.5);
  point.set_z(6.6);
  point.set_detector_id(MAUS::DataStructure::Global::kTOF1_1);
  point.set_particle_id(MAUS::DataStructure::Global::kMuMinus);
  point.set_uncertainties(kUncertainties);

  // Test both operator==() and setters
  ASSERT_EQ(test_point, point);

  // Test assignment
  point.set_t(1.0);
  point = test_point;
  ASSERT_EQ(test_point, point);

  // shamelessly useless test designed solely to increase test coverage
  std::stringstream test_stream;
  test_stream << test_point;
  std::stringstream stream;
  stream << point;
  ASSERT_EQ(test_stream.str(), stream.str());
}

TEST_F(TrackPointTest, Inequality) {
  const double coordinates[6] = {0.0, 1000., 2.2, 3.3, 4.4, 5.5};
  TrackPoint test_point(coordinates, 6.6);
  test_point.set_detector_id(MAUS::DataStructure::Global::kTOF1_1);
  test_point.set_particle_id(MAUS::DataStructure::Global::kMuMinus);
  test_point.set_uncertainties(kUncertainties);

  // Test inequality with a null point
  const TrackPoint null_point;
  EXPECT_NE(test_point, null_point);

  // Test coordinate inequality
  TrackPoint point(test_point);
  test_point.set_t(1.0);
  EXPECT_NE(test_point, point);
  test_point.set_t(0.0);

  test_point.set_z(1.0);
  EXPECT_NE(test_point, point);
  EXPECT_LT(test_point, point);
  EXPECT_GT(point, test_point);
  test_point.set_z(6.6);

  // Test particle ID inequality
  test_point.set_particle_id(MAUS::DataStructure::Global::kPi0);
  EXPECT_NE(test_point, point);
  test_point.set_particle_id(MAUS::DataStructure::Global::kMuMinus);

  // Test detector ID inequality
  test_point.set_detector_id(MAUS::DataStructure::Global::kCherenkov2);
  EXPECT_NE(test_point, point);
  test_point.set_detector_id(MAUS::DataStructure::Global::kTOF1_1);

  // Test uncertainties inequality
  MAUS::CovarianceMatrix uncertainties;
  test_point.set_uncertainties(uncertainties);
  EXPECT_NE(test_point, point);
}

TEST_F(TrackPointTest, OffMassShellException) {
  const double coordinates[6] = { 0.0, 1.1, 2.2, 3.3, 4.4, 5.5 };
  const MAUS::PhaseSpaceVector coordinate_vector(coordinates);
  const TrackPoint point(coordinate_vector, 6.6,
                         MAUS::DataStructure::Global::kMuMinus);

  bool off_mass_shell = false;
  try {
    point.Pz();
  } catch (Squeal squeal) {
    off_mass_shell = true;
  }
  ASSERT_TRUE(off_mass_shell);
}
