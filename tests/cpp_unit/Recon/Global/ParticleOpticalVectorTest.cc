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
#include "json/reader.h"
#include "json/value.h"

#include "Utils/Exception.hh"
#include "Optics/PhaseSpaceVector.hh"
#include "Recon/Global/ParticleOpticalVector.hh"

Json::Value SetupConfig(int verbose_level);
using MAUS::recon::global::ParticleOpticalVector;
using MAUS::operator ==;
using MAUS::operator !=;
using MAUS::operator +;
using MAUS::operator +=;
using MAUS::operator -;
using MAUS::operator -=;
using MAUS::operator *;
using MAUS::operator *=;
using MAUS::operator /;

class ParticleOpticalVectorTest : public testing::Test {
 public:
  ParticleOpticalVectorTest() { }

  ~ParticleOpticalVectorTest() { }

 protected:

  static const double kK;   // -beta0 * c * gamma0 / (1 + gamma0)
  static const double kT0;
  static const double kE0;
  static const double kP0;
  static const double kPhaseSpaceData[6];
  static const double kParticleOpticalData[6];
  static const double kParticleOpticalDoubledData[6];
};

// *************************************************
// ParticleOpticalVectorTest static const initializations
// *************************************************
const double ParticleOpticalVectorTest::kT0 = 2.0;
const double ParticleOpticalVectorTest::kE0 = 226.193922;
const double ParticleOpticalVectorTest::kP0 = 200.0;
// t, E, x, Px, y, Py
const double ParticleOpticalVectorTest::kPhaseSpaceData[6] = {
  1., 200., 3., 4., 5., 6.
};
// l = k * (t - t0), delta = (E - E0) / E0, x, a = Px/P0, y, b = Py/P0
const double ParticleOpticalVectorTest::kParticleOpticalData[6] = {
  -0.602677777, -0.115802945, 3.0, 0.02, 5., 0.03
};
const double ParticleOpticalVectorTest::kParticleOpticalDoubledData[6] = {
  -1.20535555, -0.231605889, 6.0, 0.04, 10., 0.06
};

// ***********
// test cases
// ***********

TEST_F(ParticleOpticalVectorTest, Constructor) {
  // *** parametric ***
  const ParticleOpticalVector zero_vector(0., 0., 0., 0., 0., 0.);
  for (int index = 0; index < 6; ++index) {
    EXPECT_EQ(0., zero_vector[index]);
  }

  // *** default ***
  const ParticleOpticalVector default_vector;
  EXPECT_EQ(zero_vector, default_vector);

  // *** array ***
  const ParticleOpticalVector vector(kParticleOpticalData);
  for (int index = 0; index < 6; ++index) {
    EXPECT_EQ(kParticleOpticalData[index], vector[index]);
  }

  // *** copy ***
  const ParticleOpticalVector copy_vector(vector);
  for (int index = 0; index < 6; ++index) {
    EXPECT_EQ(kParticleOpticalData[index], copy_vector[index]);
  }

  // *** conversion from PhaseSpaceVector ***
  const MAUS::PhaseSpaceVector phase_space_vector(kPhaseSpaceData);
  const ParticleOpticalVector converted_vector(phase_space_vector,
                                               kT0, kE0, kP0);
  for (int index = 0; index < 6; ++index) {
    EXPECT_NEAR(kParticleOpticalData[index], converted_vector[index], 1.e-6);
  }
}

TEST_F(ParticleOpticalVectorTest, Accessors) {
  const ParticleOpticalVector vector(kParticleOpticalData);
  EXPECT_EQ(kParticleOpticalData[0], vector.l());
  EXPECT_EQ(kParticleOpticalData[1], vector.delta());
  EXPECT_EQ(kParticleOpticalData[2], vector.x());
  EXPECT_EQ(kParticleOpticalData[3], vector.a());
  EXPECT_EQ(kParticleOpticalData[4], vector.y());
  EXPECT_EQ(kParticleOpticalData[5], vector.b());
}

TEST_F(ParticleOpticalVectorTest, Mutators) {
  ParticleOpticalVector vector;
  vector.set_l(kParticleOpticalData[0]);
  vector.set_delta(kParticleOpticalData[1]);
  vector.set_x(kParticleOpticalData[2]);
  vector.set_a(kParticleOpticalData[3]);
  vector.set_y(kParticleOpticalData[4]);
  vector.set_b(kParticleOpticalData[5]);
  for (int index = 0; index < 6; ++index) {
    EXPECT_EQ(kParticleOpticalData[index], vector[index]);
  }
}

TEST_F(ParticleOpticalVectorTest, VectorAsignmentOperators) {
  const ParticleOpticalVector vector(kParticleOpticalData);
  ParticleOpticalVector test_vector;
  test_vector = vector;
  for (size_t index = 0; index < 6; ++index) {
    EXPECT_NEAR(kParticleOpticalData[index], test_vector[index], 1.e-6);
  }

  const double multiplier[6] = {2.0, 2.0, 2.0, 2.0, 2.0, 2.0};
  const ParticleOpticalVector multiplier_vector(multiplier);

  test_vector += vector;
  for (size_t index = 0; index < 6; ++index) {
    EXPECT_NEAR(kParticleOpticalDoubledData[index], test_vector[index], 1.e-6);
  }

  test_vector -= vector;
  for (size_t index = 0; index < 6; ++index) {
    EXPECT_NEAR(kParticleOpticalData[index], test_vector[index], 1.e-6);
  }

  test_vector *= multiplier_vector;
  for (size_t index = 0; index < 6; ++index) {
    EXPECT_NEAR(kParticleOpticalDoubledData[index], test_vector[index], 1.e-6);
  }

  test_vector /= multiplier_vector;
  for (size_t index = 0; index < 6; ++index) {
    EXPECT_NEAR(kParticleOpticalData[index], test_vector[index], 1.e-6);
  }
}

TEST_F(ParticleOpticalVectorTest, ScalarAssignmentOperators) {
  const ParticleOpticalVector vector(kParticleOpticalData);

  const double summand = 1.0;
  const double summand_array[6] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
  const ParticleOpticalVector summand_vector(summand_array);
  ParticleOpticalVector compare_vector(kParticleOpticalData);
  compare_vector += summand_vector;
  ParticleOpticalVector test_vector(kParticleOpticalData);
  test_vector += summand;
  for (size_t index = 0; index < 6; ++index) {
    EXPECT_NEAR(compare_vector[index], test_vector[index], 1.e-6);
  }

  test_vector -= summand;
  for (size_t index = 0; index < 6; ++index) {
    EXPECT_NEAR(kParticleOpticalData[index], test_vector[index], 1.e-6);
  }

  const double multiplier = 2.0;
  test_vector *= multiplier;
  for (size_t index = 0; index < 6; ++index) {
    EXPECT_NEAR(kParticleOpticalDoubledData[index], test_vector[index], 1.e-6);
  }

  test_vector /= multiplier;
  for (size_t index = 0; index < 6; ++index) {
    EXPECT_NEAR(kParticleOpticalData[index], test_vector[index], 1.e-6);
  }
}

TEST_F(ParticleOpticalVectorTest, VectorAlgebraicOperators) {
  const ParticleOpticalVector vector(kParticleOpticalData);
  ParticleOpticalVector test_vector;

  const ParticleOpticalVector summand_vector(kParticleOpticalData);
  test_vector = vector + summand_vector;
  for (size_t index = 0; index < 6; ++index) {
    EXPECT_NEAR(kParticleOpticalDoubledData[index], test_vector[index], 1.e-6);
  }

  const ParticleOpticalVector zero_vector;
  test_vector = vector - summand_vector;
  for (size_t index = 0; index < 6; ++index) {
    EXPECT_NEAR(0.0, test_vector[index], 1.e-6);
  }

  const double multiplier[6] = {2.0, 2.0, 2.0, 2.0, 2.0, 2.0};
  const ParticleOpticalVector multiplier_vector(multiplier);
  test_vector = vector * multiplier_vector;
  for (size_t index = 0; index < 6; ++index) {
    EXPECT_NEAR(kParticleOpticalDoubledData[index], test_vector[index], 1.e-6);
  }

  const ParticleOpticalVector one_vector;
  test_vector = test_vector / multiplier_vector;
  for (size_t index = 0; index < 6; ++index) {
    EXPECT_NEAR(kParticleOpticalData[index], test_vector[index], 1.e-6);
  }
}

TEST_F(ParticleOpticalVectorTest, ScalarAlgebraicOperators) {
  const ParticleOpticalVector vector(kParticleOpticalData);

  const double summand = 1.0;
  const double summand_array[6] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
  const ParticleOpticalVector summand_vector(summand_array);
  ParticleOpticalVector compare_vector(kParticleOpticalData);
  compare_vector += summand_vector;
  ParticleOpticalVector test_vector;

  test_vector = vector + summand;
  for (size_t index = 0; index < 6; ++index) {
    EXPECT_NEAR(compare_vector[index], test_vector[index], 1.e-6);
  }

  compare_vector = ParticleOpticalVector(kParticleOpticalData);
  compare_vector -= summand_vector;
  test_vector = vector - summand;
  for (size_t index = 0; index < 6; ++index) {
    EXPECT_NEAR(compare_vector[index], test_vector[index], 1.e-6);
  }

  const double multiplier = 2.0;
  test_vector = vector * multiplier;
  for (size_t index = 0; index < 6; ++index) {
    EXPECT_NEAR(kParticleOpticalDoubledData[index], test_vector[index], 1.e-6);
  }

  test_vector = vector / multiplier;
  compare_vector = ParticleOpticalVector(kParticleOpticalData);
  compare_vector /= multiplier;
  for (size_t index = 0; index < 6; ++index) {
    EXPECT_NEAR(compare_vector[index], test_vector[index], 1.e-6);
  }
}

