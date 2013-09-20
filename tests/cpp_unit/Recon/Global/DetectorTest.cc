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
#include <string>

#include <cstdlib>

#include "gtest/gtest.h"
#include "json/reader.h"
#include "json/value.h"

#include "Utils/Exception.hh"

Json::Value SetupConfig(int verbose_level);

class DetectorTest : public testing::Test {
 public:
  DetectorTest() {
  }

  ~DetectorTest() {
  }

 protected:
};

// *************************************************
// DetectorTest static const initializations
// *************************************************

// ***********
// test cases
// ***********

TEST_F(DetectorTest, Constructor) {
}

