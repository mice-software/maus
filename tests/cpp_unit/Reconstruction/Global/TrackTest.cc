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
#include "Reconstruction/Global/Detector.hh"
#include "Reconstruction/Global/Particle.hh"
#include "Reconstruction/Global/Track.hh"
#include "Reconstruction/Global/TrackPoint.hh"

using MAUS::CovarianceMatrix;
using MAUS::Detector;
using MAUS::Particle;
using MAUS::Track;
using MAUS::TrackPoint;
using MAUS::Vector;

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
  = TrackPoint(1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8,
               TrackTest::kUncertainties);

// ***********
// test cases
// ***********

TEST_F(TrackTest, DefaultConstructor) {
  Track empty_track;

  ASSERT_EQ((size_t) 0, empty_track.size());
  ASSERT_EQ(Particle::kNone, empty_track.particle_id());
}

TEST_F(TrackTest, IdentifiedConstructor) {
  Track track(Particle::kMuMinus);

  ASSERT_EQ((size_t) 0, track.size());
  ASSERT_EQ(Particle::kMuMinus, track.particle_id());
}

TEST_F(TrackTest, ContentConstructor) {
  const TrackPoint track_point_array[2] = {
    TrackPoint(1., 2., 3., 4., 5., 6., 7., 8., TrackTest::kUncertainties),
    TrackPoint(8., 7., 6., 5., 4., 3., 2., 1., CovarianceMatrix())
  };
  std::vector<TrackPoint> track_points(track_point_array, track_point_array+2);

  Track track(track_points, Particle::kMuMinus);

  ASSERT_EQ((size_t) 2, track.size());
  for (int index = 0; index < 2; ++index) {
    ASSERT_EQ(track_point_array[index], track[index]);
  }
  ASSERT_EQ(Particle::kMuMinus, track.particle_id());
}

/* TODO(plane1@hawk.iit.edu)
TEST_F(TrackTest, CopyConstructor) {
}

TEST_F(TrackTest, ParticleIDAccessor) {
}
*/

