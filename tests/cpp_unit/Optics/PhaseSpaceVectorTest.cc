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
#include "TLorentzVector.h"

#include "DataStructure/ThreeVector.hh"
#include "DataStructure/VirtualHit.hh"
#include "DataStructure/Global/ReconEnums.hh"
#include "src/common_cpp/Optics/PhaseSpaceVector.hh"
#include "Recon/Global/Particle.hh"
#include "Utils/Exception.hh"
#include "Maths/Vector.hh"

namespace GlobalDS = MAUS::DataStructure::Global;
namespace Recon = MAUS::recon::global;

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
  static const double kDoubleData[6];
};

// *************************************************
// PhaseSpaceVectorTest static const initializations
// *************************************************

const double PhaseSpaceVectorTest::kData[10]
  = {3.1, 4.1, 5.9, 2.6, 5.3, 5.8, 9.7, 9.3, 2.3, 8.4};  // 10 elem.
const double PhaseSpaceVectorTest::kDoubleData[6]
  = {6.2, 8.2, 11.8, 5.2, 10.6, 11.6};


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
  } catch (MAUS::Exceptions::Exception exc) {}
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

TEST_F(PhaseSpaceVectorTest, VirtualHitConstructor) {
  MAUS::VirtualHit virtual_hit;
  virtual_hit.SetTime(kData[0]);
  const double mass
    = Recon::Particle::GetInstance().GetMass(GlobalDS::kMuMinus);
  virtual_hit.SetMass(mass);
  MAUS::ThreeVector position(kData[2], kData[4], kData[6]);
  virtual_hit.SetPosition(position);
  MAUS::ThreeVector momentum(kData[3], kData[5], kData[7]);
  virtual_hit.SetMomentum(momentum);
  const double energy = sqrt(mass*mass + momentum.mag()*momentum.mag());
  const PhaseSpaceVector test_psv(virtual_hit);
  const double data[6] = {
    kData[0], energy, kData[2], kData[3], kData[4], kData[5]
  };
  for (size_t index = 0; index < 6; ++index) {
    EXPECT_EQ(test_psv[index], data[index]);
  }
}

TEST_F(PhaseSpaceVectorTest, ThreeVectorConstructor) {
  MAUS::ThreeVector position(kData[2], kData[4], kData[6]);
  MAUS::ThreeVector momentum(kData[3], kData[5], kData[7]);
  const PhaseSpaceVector test_psv(kData[0], kData[1], position, momentum);
  for (size_t index = 0; index < 6; ++index) {
    EXPECT_EQ(test_psv[index], kData[index]);
  }
}

TEST_F(PhaseSpaceVectorTest, TLorentzVectorConstructor) {
  TLorentzVector position(kData[2], kData[4], kData[6], kData[0]);
  TLorentzVector momentum(kData[3], kData[5], kData[7], kData[1]);
  const PhaseSpaceVector test_psv(position, momentum);
  for (size_t index = 0; index < 6; ++index) {
    EXPECT_EQ(test_psv[index], kData[index]);
  }
}

TEST_F(PhaseSpaceVectorTest, VectorAsignmentOperators) {
  const PhaseSpaceVector ps_vector(kData);
  PhaseSpaceVector test_vector;
  test_vector = ps_vector;
  for (size_t index = 0; index < 6; ++index) {
    EXPECT_NEAR(test_vector[index], kData[index], 1.e-6);
  }

  const double multiplier[6] = {2.0, 2.0, 2.0, 2.0, 2.0, 2.0};
  const PhaseSpaceVector multiplier_vector(multiplier);

  test_vector += ps_vector;
  for (size_t index = 0; index < 6; ++index) {
    EXPECT_NEAR(test_vector[index], kDoubleData[index], 1.e-6);
  }

  test_vector -= ps_vector;
  for (size_t index = 0; index < 6; ++index) {
    EXPECT_NEAR(test_vector[index], kData[index], 1.e-6);
  }

  test_vector *= multiplier_vector;
  for (size_t index = 0; index < 6; ++index) {
    EXPECT_NEAR(test_vector[index], kDoubleData[index], 1.e-6);
  }

  test_vector /= multiplier_vector;
  for (size_t index = 0; index < 6; ++index) {
    EXPECT_NEAR(test_vector[index], kData[index], 1.e-6);
  }
}

TEST_F(PhaseSpaceVectorTest, ScalarAssignmentOperators) {
  const PhaseSpaceVector ps_vector(kData);

  const double summand = 1.0;
  const double summand_array[6] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
  const PhaseSpaceVector summand_vector(summand_array);
  PhaseSpaceVector compare_vector(kData);
  compare_vector += summand_vector;
  PhaseSpaceVector test_vector(kData);
  test_vector += summand;
  for (size_t index = 0; index < 6; ++index) {
    EXPECT_NEAR(test_vector[index], compare_vector[index], 1.e-6);
  }

  test_vector -= summand;
  for (size_t index = 0; index < 6; ++index) {
    EXPECT_NEAR(test_vector[index], kData[index], 1.e-6);
  }

  const double multiplier = 2.0;
  test_vector *= multiplier;
  for (size_t index = 0; index < 6; ++index) {
    EXPECT_NEAR(test_vector[index], kDoubleData[index], 1.e-6);
  }

  test_vector /= multiplier;
  for (size_t index = 0; index < 6; ++index) {
    EXPECT_NEAR(test_vector[index], kData[index], 1.e-6);
  }
}

TEST_F(PhaseSpaceVectorTest, VectorAlgebraicOperators) {
  const PhaseSpaceVector ps_vector(kData);
  PhaseSpaceVector test_vector;

  const PhaseSpaceVector summand_vector(kData);
  test_vector = ps_vector + summand_vector;
  for (size_t index = 0; index < 6; ++index) {
    EXPECT_NEAR(test_vector[index], kDoubleData[index], 1.e-6);
  }

  const PhaseSpaceVector zero_vector;
  test_vector = ps_vector - summand_vector;
  for (size_t index = 0; index < 6; ++index) {
    EXPECT_NEAR(test_vector[index], 0.0, 1.e-6);
  }

  const double multiplier[6] = {2.0, 2.0, 2.0, 2.0, 2.0, 2.0};
  const PhaseSpaceVector multiplier_vector(multiplier);
  test_vector = ps_vector * multiplier_vector;
  for (size_t index = 0; index < 6; ++index) {
    EXPECT_NEAR(test_vector[index], kDoubleData[index], 1.e-6);
  }

  const PhaseSpaceVector one_vector;
  test_vector = test_vector / multiplier_vector;
  for (size_t index = 0; index < 6; ++index) {
    EXPECT_NEAR(test_vector[index], kData[index], 1.e-6);
  }
}

TEST_F(PhaseSpaceVectorTest, ScalarAlgebraicOperators) {
  const PhaseSpaceVector ps_vector(kData);

  const double summand = 1.0;
  const double summand_array[6] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
  const PhaseSpaceVector summand_vector(summand_array);
  PhaseSpaceVector compare_vector(kData);
  compare_vector += summand_vector;
  PhaseSpaceVector test_vector;

  test_vector = ps_vector + summand;
  for (size_t index = 0; index < 6; ++index) {
    EXPECT_NEAR(test_vector[index], compare_vector[index], 1.e-6);
  }

  compare_vector = PhaseSpaceVector(kData);
  compare_vector -= summand_vector;
  test_vector = ps_vector - summand;
  for (size_t index = 0; index < 6; ++index) {
    EXPECT_NEAR(test_vector[index], compare_vector[index], 1.e-6);
  }

  const double multiplier = 2.0;
  test_vector = ps_vector * multiplier;
  for (size_t index = 0; index < 6; ++index) {
    EXPECT_NEAR(test_vector[index], kDoubleData[index], 1.e-6);
  }

  test_vector = ps_vector / multiplier;
  compare_vector = PhaseSpaceVector(kData);
  compare_vector /= multiplier;
  for (size_t index = 0; index < 6; ++index) {
    EXPECT_NEAR(test_vector[index], compare_vector[index], 1.e-6);
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
