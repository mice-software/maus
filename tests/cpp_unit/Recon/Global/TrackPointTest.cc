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
#include "Recon/Global/TrackPoint.hh"

using MAUS::CovarianceMatrix;
using MAUS::recon::global::Detector;
using MAUS::recon::global::TrackPoint;
using MAUS::Vector;

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
  = MAUS::CovarianceMatrix(TrackPointTest::kUncertainties);

// ***********
// test cases
// ***********

TEST_F(TrackPointTest, DefaultConstructor) {
  const CovarianceMatrix null_uncertainties;
  TrackPoint explicit_default_point;
  explicit_default_point.set_t(0.0);
  explicit_default_point.set_E(0.0);
  explicit_default_point.set_x(0.0);
  explicit_default_point.set_Px(0.0);
  explicit_default_point.set_y(0.0);
  explicit_default_point.set_Py(0.0);
  explicit_default_point.set_z(0.0);
  explicit_default_point.set_detector_id(Detector::kNone);
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
  ASSERT_EQ(0.0, default_point.Pz());
  ASSERT_EQ(0.0, default_point.z_momentum());
  ASSERT_EQ(Detector::kNone, default_point.detector_id());
  const CovarianceMatrix uncertainties = default_point.uncertainties();
  ASSERT_EQ(null_uncertainties, uncertainties);
}

TEST_F(TrackPointTest, Equality) {
  TrackPoint test_point;
  test_point.set_t(0.0);
  test_point.set_E(1.1);
  test_point.set_x(2.2);
  test_point.set_Px(3.3);
  test_point.set_y(4.4);
  test_point.set_Py(5.5);
  test_point.set_z(6.6);
  test_point.set_detector_id(Detector::kTOF1_1);
  test_point.set_uncertainties(TrackPointTest::kUncertainties);

  ASSERT_EQ(test_point, test_point);

  TrackPoint point;
  point.set_t(0.0);
  point.set_E(1.1);
  point.set_x(2.2);
  point.set_Px(3.3);
  point.set_y(4.4);
  point.set_Py(5.5);
  point.set_z(6.6);
  point.set_detector_id(Detector::kTOF1_1);
  point.set_uncertainties(TrackPointTest::kUncertainties);

  // Test both operator==() and setters
  ASSERT_EQ(test_point, point);

  // Test inequality with a null point
  const TrackPoint default_point;
  ASSERT_NE(default_point, point);

  // Test general inequality
  test_point.set_t(1.0);
  test_point.set_E(2.1);
  test_point.set_x(3.2);
  test_point.set_Px(5.3);
  test_point.set_y(7.4);
  test_point.set_Py(11.5);
  test_point.set_z(13.6);
  test_point.set_detector_id(Detector::kCherenkov2);
  CovarianceMatrix uncertainties(TrackPointTest::kUncertainties);
  uncertainties *= 2;
  test_point.set_uncertainties(uncertainties);
  ASSERT_NE(test_point, point);
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
  test_point.set_detector_id(Detector::kTOF1_1);
  test_point.set_uncertainties(TrackPointTest::kUncertainties);

  const TrackPoint point(test_point);

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
  test_point.set_uncertainties(TrackPointTest::kUncertainties);

  const TrackPoint point(0.0, 1.1, 2.2, 3.3, 4.4, 5.5,
                         TrackPointTest::kUncertainties, 6.6);

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
  test_point.set_detector_id(Detector::kTracker2_3);
  test_point.set_uncertainties(TrackPointTest::kUncertainties);

  const Detector detector(Detector::kTracker2_3,
                          TrackPointTest::kDetectorPlane,
                          TrackPointTest::kUncertainties);
  const TrackPoint point(0.0, 1.1, 2.2, 3.3, 4.4, 5.5, detector, 6.6);

  ASSERT_EQ(test_point, point);
}

