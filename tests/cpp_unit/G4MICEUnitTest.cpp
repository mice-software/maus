/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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
 * along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

// Will run tests in all files included below
// If you just want to run against a subset of files
// comment the rest out and recompile

#include <string>

#include "gtest/gtest.h"

/////////// Needed until persistency move is done /////////////
#include "src/common/Interface/dataCards.hh"
#include "src/common/Interface/MICEEvent.hh"
#include "src/common/Interface/MICERun.hh"
#include "src/common/Interface/JsonWrapper.hh"


#include "src/common/Config/MiceModule.hh"
#include "src/common/Interface/MiceMaterials.hh"
#include "src/common/Simulation/FillMaterials.hh"
dataCards MyDataCards(0);
MICEEvent simEvent;
/////////// Needed until persistency move is done //////////////

/////////// Needed until I clean up legacy tests to gtest framework //////////
#include "src/common/Interface/Squeak.hh"
/////////// Needed until I clean up legacy tests to gtest framework //////////

std::string jsonconfig =
"{\"maximum_number_of_steps\":10000}";


int main(int argc, char **argv) {
  ///// Try to keep static set up to a minimum (not very unit testy)
  MICERun::getInstance()->DataCards = &MyDataCards;
  MICERun::getInstance()->miceModule = new MiceModule("Test.dat"); // delete me!
  MICERun::getInstance()->miceMaterials = new MiceMaterials(); // delete me!
  fillMaterials(*MICERun::getInstance());
  MICERun::getInstance()->jsonConfiguration =
    new Json::Value(JsonWrapper::StringToJson(jsonconfig)); // delete me!
  Squeak::setOutput(Squeak::debug, Squeak::nullOut());

  ::testing::InitGoogleTest(&argc, argv);
  int test_out = RUN_ALL_TESTS();
  std::cerr << "WARNING - MiceModule test was disabled - see #368" << std::endl;
  return test_out;
}


