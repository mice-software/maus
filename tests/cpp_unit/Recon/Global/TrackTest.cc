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
#include "Recon/Global/Track.hh"
#include "Recon/Global/TrackPoint.hh"

using MAUS::CovarianceMatrix;
using MAUS::Vector;
using MAUS::recon::global::Detector;
using MAUS::recon::global::Particle;
using MAUS::recon::global::Track;
using MAUS::recon::global::TrackPoint;

class TrackTest : public ::testing::Test {
 public:
  TrackTest() {
  }
 protected:
  static const double kDetectorPlane;
  static const double kUncertaintyData[36];
  static const CovarianceMatrix kUncertainties;
  static const TrackPoint kPoint;

  virtual void SetUp() { }

  virtual void TearDown() { }
};

// ************************************************//
// TrackTest static const initializations//
// ************************************************//

const double TrackTest::kDetectorPlane = 78.6;

const double TrackTest::kUncertaintyData[36] = {
  +0.,      1.,       2.,       3.,      -5.,      -6.,
  +1.,    -13.,     -21.,      34.5,     55.7,     13.2,
  +2.,    -21.,     -32.5,    -57.5,    -91.2,    -23.4,
  +3.,     34.5,    -57.5,      2.65,    -3.58,    59.9,
  -5.,    -55.7,    -91.2,     -3.58,     3.38,    67.4,
  -6.,     13.2,    -23.4,     59.9,     67.4,      5.12
};

const CovarianceMatrix TrackTest::kUncertainties
  = CovarianceMatrix(TrackTest::kUncertainties);

const TrackPoint TrackTest::kPoint
  = TrackPoint(1.1, 2.2, 3.3, 4.4, 5.5, 6.6,
               TrackTest::kUncertainties, 7.7);

// ***********
// test cases
// ***********

TEST_F(TrackTest, DefaultConstructor) {
  Track empty_track;

  ASSERT_EQ((size_t) 0, empty_track.size());
  ASSERT_EQ(Particle::kNone, empty_track.particle_id());
}

TEST_F(TrackTest, Accessors) {
  Track track;
  track.set_particle_id(Particle::kMuPlus);
  EXPECT_EQ(Particle::kMuPlus, track.particle_id());
}

TEST_F(TrackTest, IdentifiedConstructor) {
  Track track(Particle::kMuMinus);

  ASSERT_EQ((size_t) 0, track.size());
  ASSERT_EQ(Particle::kMuMinus, track.particle_id());
}

TEST_F(TrackTest, ContentConstructor) {
  const TrackPoint track_point_array[2] = {
    TrackPoint(1., 2., 3., 4., 5., 6., TrackTest::kUncertainties, 7.),
    TrackPoint(8., 7., 6., 5., 4., 3., CovarianceMatrix(), 2.)
  };
  std::vector<TrackPoint> track_points(track_point_array, track_point_array+2);

  Track track(track_points, Particle::kMuMinus);

  ASSERT_EQ((size_t) 2, track.size());
  for (int index = 0; index < 2; ++index) {
    ASSERT_EQ(track_point_array[index], track[index]);
  }
  ASSERT_EQ(Particle::kMuMinus, track.particle_id());
}

TEST_F(TrackTest, CopyConstructor) {
  const TrackPoint track_point_array[2] = {
    TrackPoint(1., 2., 3., 4., 5., 6., TrackTest::kUncertainties, 7.),
    TrackPoint(8., 7., 6., 5., 4., 3., CovarianceMatrix(), 2.)
  };
  std::vector<TrackPoint> track_points(track_point_array, track_point_array+2);
  Track track(track_points, Particle::kMuMinus);
  Track track_copy(track);

  for (int index = 0; index < 2; ++index) {
    TrackPoint track_point = track[index];
    TrackPoint track_point_copy = track[index];
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

