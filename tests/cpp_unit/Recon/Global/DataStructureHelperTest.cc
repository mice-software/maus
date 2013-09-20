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

#include "Config/MiceModule.hh"
#include "Utils/Exception.hh"
#include "Recon/Global/DataStructureHelper.hh"

using MAUS::recon::global::DataStructureHelper;

Json::Value SetupConfig(int verbose_level);

class DataStructureHelperTest : public testing::Test {
 public:
  DataStructureHelperTest() {
  }

  ~DataStructureHelperTest() {
  }

 protected:
};

// *************************************************
// DataStructureHelperTest static const initializations
// *************************************************

// ***********
// test cases
// ***********

TEST_F(DataStructureHelperTest, FindModulesByName) {
  MiceModule root(NULL, "root");
  MiceModule * orwell = new MiceModule(&root, "Orwell");
  MiceModule * george_orwell = new MiceModule(orwell, "George");
  MiceModule * curie = new MiceModule(&root, "Curie");
  MiceModule * marie_curie = new MiceModule(curie, "Marie");
  MiceModule * washington = new MiceModule(&root, "Washington");
  MiceModule * george_washington = new MiceModule(washington, "George");
  MiceModule * nother = new MiceModule(&root, "Nother");
  MiceModule * emmy_nother = new MiceModule(curie, "Nother");
  MiceModule * scott = new MiceModule(&root, "Scott");
  MiceModule * c_scott = new MiceModule(scott, "C");
  MiceModule * george_c_scott = new MiceModule(c_scott, "George");
  root.addDaughter(orwell);
  orwell->addDaughter(george_orwell);
  root.addDaughter(curie);
  curie->addDaughter(marie_curie);
  root.addDaughter(washington);
  washington->addDaughter(george_washington);
  root.addDaughter(nother);
  nother->addDaughter(emmy_nother);
  root.addDaughter(scott);
  scott->addDaughter(c_scott);
  c_scott->addDaughter(george_c_scott);

  const std::vector<const MiceModule *> georges
    = DataStructureHelper::GetInstance().FindModulesByName(&root, "George");
  const size_t number_of_georges = georges.size();
  ASSERT_EQ(number_of_georges, static_cast<size_t>(3));

  const std::string names[3] = {
    "root/Orwell/George",
    "root/Washington/George",
    "root/Scott/C/George"
  };
  for (size_t index = 0; index < 3; ++index) {
    std::string actual_name = georges[index]->fullName();
    std::string expected_name = names[index];
    EXPECT_EQ(actual_name, expected_name);
  }
}

