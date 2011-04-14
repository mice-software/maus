#include "gtest/gtest.h" 

// Will run tests in all files included below
// If you just want to run against a subset of files
// comment the rest out and recompile


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
"{\"MaxNumberOfSteps\":10000}";


int main(int argc, char **argv) {
  ///// Try to keep static set up to a minimum (not very unit testy)
  MICERun::getInstance()->DataCards = &MyDataCards;
  MICERun::getInstance()->miceModule = new MiceModule("Test.dat"); //delete me!
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


