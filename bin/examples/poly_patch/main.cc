#include "gtest/gtest.h"


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    std::cout << "Running tests" << std::endl;
    int test_out = RUN_ALL_TESTS();
    return test_out;
}

