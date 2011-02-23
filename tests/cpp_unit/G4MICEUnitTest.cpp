#include "gtest/gtest.h" 

// Will run tests in all files included below
// If you just want to run against a subset of files
// comment the rest out and recompile


/////////// Needed until persistency move is done /////////////
#include "src/common/Interface/dataCards.hh"
#include "src/common/Interface/MICEEvent.hh"
dataCards MyDataCards;
MICEEvent simEvent;
/////////// Needed until persistency move is done //////////////

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}


