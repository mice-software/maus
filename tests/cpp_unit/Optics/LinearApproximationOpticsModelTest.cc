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
#include <fstream>
#include <iostream>
#include <sstream>
#include <streambuf>
#include <string>

#include <cstdlib>
/*
#include <unistd.h>
#include <sys/param.h> // MAXPATHLEN definition
*/

#include "gtest/gtest.h"

#include "Interface/Squeal.hh"
#include "src/common_cpp/Optics/LinearApproximationOpticsModel.hh"

#include "json/reader.h"
#include "json/value.h"

using MAUS::LinearApproximationOpticsModel;

class LinearApproximationOpticsModelTest : public testing::Test {
 public:
  LinearApproximationOpticsModelTest() {
    /*
    char path1[MAXPATHLEN]; // This is a buffer for the text
    getcwd(path1, MAXPATHLEN);
    std::cout << "CWD: " << path1 << std::endl;
    */
    const std::string maus_root_dir(getenv("MAUS_ROOT_DIR"));
    const std::string filename = maus_root_dir + "/" + kConfigFilename;

    std::ifstream config_file(filename.c_str());
    if (!config_file.good()) {
      throw(Squeal(Squeal::nonRecoverable,
                  "Unable to load configuration file.",
                  "LinearApproximationOpticsModelTest()"));
    }
    std::string config_string((std::istreambuf_iterator<char>(config_file)),
                    std::istreambuf_iterator<char>());
    Json::Reader reader;
    bool successful = reader.parse(config_string, configuration_);
    if (!successful) {
      throw(Squeal(Squeal::nonRecoverable,
                  "Unable to parse JSON configuration.",
                  "LinearApproximationOpticsModelTest()"));
    }
  }

 protected:
  Json::Value configuration_;
  static const std::string kConfigFilename;
};

// *************************************************
// LinearApproximationOpticsModelTest static const initializations
// *************************************************
const std::string LinearApproximationOpticsModelTest::kConfigFilename
  = std::string("tests/cpp_unit/Optics/drift_config");

// ***********
// test cases
// ***********

TEST_F(LinearApproximationOpticsModelTest, Constructor) {
  const LinearApproximationOpticsModel optics_model(configuration_);
}
