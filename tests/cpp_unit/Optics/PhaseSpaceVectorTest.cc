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
#include <fstream>

#include "gtest/gtest.h"

#include "src/common_cpp/Optics/PhaseSpaceVector.hh"
#include "Maths/Vector.hh"

using MAUS::PhaseSpaceVector;
using MAUS::Vector;
using MAUS::operator ==;
using MAUS::operator !=;
using MAUS::operator +;
using MAUS::operator +=;
using MAUS::operator -;
using MAUS::operator -=;
using MAUS::operator *;
using MAUS::operator *=;
using MAUS::operator /;

class PhaseSpaceVectorTest : public testing::Test {
 public:
  PhaseSpaceVectorTest() {
  }

 protected:
  static const double kData[10];
};

// *************************************************
// PhaseSpaceVectorTest static const initializations
// *************************************************

const double PhaseSpaceVectorTest::kData[10]
  = {3.1, 4.1, 5.9, 2.6, 5.3, 5.8, 9.7, 9.3, 2.3, 8.4};  // 10 elem.


// ***********
// test cases
// ***********

TEST_F(PhaseSpaceVectorTest, DefaultConstructor) {
  const PhaseSpaceVector ps_vector;
  ASSERT_EQ(ps_vector.size(), (size_t) 6);
  for (size_t index = 0; index < 6; ++index) {
    EXPECT_EQ(ps_vector[index], 0.0);
  }
}

TEST_F(PhaseSpaceVectorTest, VectorConstructor) {
  const Vector<double> vector(kData, 10);
  const PhaseSpaceVector ps_vector(vector);
  ASSERT_EQ(ps_vector.size(), (size_t) 6);
  for (size_t index = 0; index < 6; ++index) {
    EXPECT_EQ(ps_vector[index], kData[index]);
  }

  const Vector<double> bad_vector(kData, 4);
  bool testpass = true;
  try {
    const PhaseSpaceVector bad_ps_vector(bad_vector);
    testpass = false;
  } catch (Squeal squeal) {}
  ASSERT_TRUE(testpass);
}

TEST_F(PhaseSpaceVectorTest, ArrayConstructor) {
  const PhaseSpaceVector ps_vector(kData);
  ASSERT_EQ(ps_vector.size(), (size_t) 6);
  for (size_t index = 0; index < 6; ++index) {
    EXPECT_EQ(ps_vector[index], kData[index]);
  }
}

TEST_F(PhaseSpaceVectorTest, CopyConstructor) {
  const PhaseSpaceVector ps_vector(kData);
  const PhaseSpaceVector ps_vector_copy(ps_vector);
  ASSERT_EQ(ps_vector_copy.size(), (size_t) 6);
  for (size_t index = 0; index < 6; ++index) {
    EXPECT_EQ(ps_vector_copy[index], kData[index]);
  }
}

TEST_F(PhaseSpaceVectorTest, ParameterConstructor) {
  const PhaseSpaceVector ps_vector(
    kData[0], kData[1], kData[2],
    kData[3], kData[4], kData[5]);
  ASSERT_EQ(ps_vector.size(), (size_t) 6);
  for (size_t index = 0; index < 6; ++index) {
    EXPECT_EQ(ps_vector[index], kData[index]);
  }
}

TEST_F(PhaseSpaceVectorTest, Accessors) {
  const PhaseSpaceVector ps_vector(kData);
  EXPECT_EQ(ps_vector.time(), kData[0]);
  EXPECT_EQ(ps_vector.t(), kData[0]);
  EXPECT_EQ(ps_vector.energy(), kData[1]);
  EXPECT_EQ(ps_vector.E(), kData[1]);
  EXPECT_EQ(ps_vector.x(), kData[2]);
  EXPECT_EQ(ps_vector.x_momentum(), kData[3]);
  EXPECT_EQ(ps_vector.Px(), kData[3]);
  EXPECT_EQ(ps_vector.y(), kData[4]);
  EXPECT_EQ(ps_vector.y_momentum(), kData[5]);
  EXPECT_EQ(ps_vector.Py(), kData[5]);
}


TEST_F(PhaseSpaceVectorTest, Mutators) {
  PhaseSpaceVector ps_vector;
  ps_vector.set_time(kData[0]);
  ps_vector.set_energy(kData[1]);
  ps_vector.set_x(kData[2]);
  ps_vector.set_x_momentum(kData[3]);
  ps_vector.set_y(kData[4]);
  ps_vector.set_y_momentum(kData[5]);
  for (size_t index = 0; index < 6; ++index) {
    EXPECT_EQ(ps_vector[index], kData[index]);
    ps_vector[index] = 0.0;
  }

  ps_vector.set_t(kData[0]);
  ps_vector.set_E(kData[1]);
  ps_vector.set_x(kData[2]);
  ps_vector.set_Px(kData[3]);
  ps_vector.set_y(kData[4]);
  ps_vector.set_Py(kData[5]);
  for (size_t index = 0; index < 6; ++index) {
    EXPECT_EQ(ps_vector[index], kData[index]);
  }
}
