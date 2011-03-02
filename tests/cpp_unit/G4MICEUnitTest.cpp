#include "gtest/gtest.h" 

// Will run tests in all files included below
// If you just want to run against a subset of files
// comment the rest out and recompile


/////////// Needed until persistency move is done /////////////
#include "src/common/Interface/dataCards.hh"
#include "src/common/Interface/MICEEvent.hh"
#include "src/common/Interface/MICERun.hh"
dataCards MyDataCards(0);
MICEEvent simEvent;
/////////// Needed until persistency move is done //////////////

/////////// Needed until I clean up legacy tests to gtest framework //////////
#include "src/common/Interface/Squeak.hh"
/////////// Needed until I clean up legacy tests to gtest framework //////////


int main(int argc, char **argv) {
  MICERun::getInstance()->DataCards = &MyDataCards;
  Squeak::setOutput(Squeak::debug, Squeak::nullOut());
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}


