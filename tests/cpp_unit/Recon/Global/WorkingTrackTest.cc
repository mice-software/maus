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
#include <vector>

#include "gtest/gtest.h"

#include "src/common_cpp/Optics/PhaseSpaceVector.hh"
#include "src/common_cpp/Optics/CovarianceMatrix.hh"
#include "Recon/Global/Detector.hh"
#include "Recon/Global/Particle.hh"
#include "Recon/Global/WorkingTrack.hh"
#include "Recon/Global/WorkingTrackPoint.hh"

using MAUS::CovarianceMatrix;
using MAUS::Vector;
using MAUS::recon::global::Detector;
using MAUS::recon::global::Particle;
using MAUS::recon::global::WorkingTrack;
using MAUS::recon::global::WorkingTrackPoint;

class WorkingTrackTest : public ::testing::Test {
 public:
  WorkingTrackTest() {
  }
 protected:
  static const double kDetectorPlane;
  static const double kUncertaintyData[36];
  static const CovarianceMatrix kUncertainties;
  static const WorkingTrackPoint kPoint;

  virtual void SetUp() { }

  virtual void TearDown() { }
};

// ************************************************//
// WorkingTrackTest static const initializations//
// ************************************************//

const double WorkingTrackTest::kDetectorPlane = 78.6;

const double WorkingTrackTest::kUncertaintyData[36] = {
  +0.,      1.,       2.,       3.,      -5.,      -6.,
  +1.,    -13.,     -21.,      34.5,     55.7,     13.2,
  +2.,    -21.,     -32.5,    -57.5,    -91.2,    -23.4,
  +3.,     34.5,    -57.5,      2.65,    -3.58,    59.9,
  -5.,    -55.7,    -91.2,     -3.58,     3.38,    67.4,
  -6.,     13.2,    -23.4,     59.9,     67.4,      5.12
};

const CovarianceMatrix WorkingTrackTest::kUncertainties
  = CovarianceMatrix(WorkingTrackTest::kUncertainties);

const WorkingTrackPoint WorkingTrackTest::kPoint
  = WorkingTrackPoint(1.1, 2.2, 3.3, 4.4, 5.5, 6.6,
               WorkingTrackTest::kUncertainties, 7.7);

// ***********
// test cases
// ***********

TEST_F(WorkingTrackTest, DefaultConstructor) {
  WorkingTrack empty_track;

  ASSERT_EQ((size_t) 0, empty_track.size());
  ASSERT_EQ(Particle::kNone, empty_track.particle_id());
}

TEST_F(WorkingTrackTest, Accessors) {
  WorkingTrack track;
  track.set_particle_id(Particle::kMuPlus);
  EXPECT_EQ(Particle::kMuPlus, track.particle_id());
}

TEST_F(WorkingTrackTest, IdentifiedConstructor) {
  WorkingTrack track(Particle::kMuMinus);

  ASSERT_EQ((size_t) 0, track.size());
  ASSERT_EQ(Particle::kMuMinus, track.particle_id());
}

TEST_F(WorkingTrackTest, ContentConstructor) {
  const WorkingTrackPoint track_point_array[2] = {
    WorkingTrackPoint(1., 2., 3., 4., 5., 6., WorkingTrackTest::kUncertainties, 7.),
    WorkingTrackPoint(8., 7., 6., 5., 4., 3., CovarianceMatrix(), 2.)
  };
  std::vector<WorkingTrackPoint> track_points(track_point_array, track_point_array+2);

  WorkingTrack track(track_points, Particle::kMuMinus);

  ASSERT_EQ((size_t) 2, track.size());
  for (int index = 0; index < 2; ++index) {
    ASSERT_EQ(track_point_array[index], track[index]);
  }
  ASSERT_EQ(Particle::kMuMinus, track.particle_id());
}

TEST_F(WorkingTrackTest, CopyConstructor) {
  const WorkingTrackPoint track_point_array[2] = {
    WorkingTrackPoint(1., 2., 3., 4., 5., 6., WorkingTrackTest::kUncertainties, 7.),
    WorkingTrackPoint(8., 7., 6., 5., 4., 3., CovarianceMatrix(), 2.)
  };
  std::vector<WorkingTrackPoint> track_points(track_point_array, track_point_array+2);
  WorkingTrack track(track_points, Particle::kMuMinus);
  WorkingTrack track_copy(track);

  for (int index = 0; index < 2; ++index) {
    WorkingTrackPoint track_point = track[index];
    WorkingTrackPoint track_point_copy = track[index];
    EXPECT_EQ(track_point, track_point_copy);
  }
  EXPECT_EQ(track.particle_id(), track_copy.particle_id());

  // shamelessly useless test designed solely to increase test coverage
  std::stringstream stream_copy;
  stream_copy << track_copy;
  std::stringstream stream;
  stream << track;
  ASSERT_EQ(stream_copy.str(), stream.str());
}

