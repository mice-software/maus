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

#include <math.h>

#include <ostream>
#include <string>
#include <iostream>
#include <fstream>

#include "gtest/gtest.h"

#include "src/common_cpp/Utils/Globals.hh"
#include "src/legacy/Interface/Squeak.hh"
#include "Utils/Exception.hh"

namespace {

class SqueakTest : public ::testing::Test {
 protected:
  SqueakTest() {}
  virtual ~SqueakTest() {
    Json::Value& cards = *MAUS::Globals::GetConfigurationCards();
    int verbose_level = cards["verbose_level"].asInt();
    Squeak::setStandardOutputs(verbose_level);
    Squeak::setOutputs(verbose_level);
  }
};

// Commented out as could not get to work with gcc 5
// A. Dobbs, 16/12/2015

// test mout(Squeak::errorLevel) and SetOutput(Squeak::errorLevel)
// TEST_F(SqueakTest, SqueakSetOutputMoutErrorLevelTest) {
//   std::stringstream sstr;
//   // debug, info, warning, error, fatal
//   Squeak::errorLevel err[] = {Squeak::debug, Squeak::info, Squeak::warning,
//                                                   Squeak::error, Squeak::fatal};
//   for (int i = 0; i < 5; ++i) {
//     Squeak::setAnOutput(err[i], sstr);
//     ASSERT_EQ(Squeak::mout(err[i]), sstr) << "errorLevel " << err[i]
//                                                                    << " failed";
//     Squeak::setAnOutput(err[i], std::cout);
//     ASSERT_EQ(Squeak::mout(err[i]), std::cout) << "errorLevel " << err[i]
//                                                                    << " failed";
//   }
// }

// test mout(Squeal::exceptionLevel)
// TEST_F(SqueakTest, SqueakMoutExceptionTest) {
//   std::stringstream sstr;
//   MAUS::Exception::exceptionLevel exc[] = {
//     MAUS::Exception::recoverable, MAUS::Exception::nonRecoverable};
//   Squeak::errorLevel err[] = {Squeak::error, Squeak::fatal};
//   for (int i = 0; i < 2; ++i) {
//     Squeak::setAnOutput(err[i], sstr);
//     ASSERT_EQ(Squeak::mout(exc[i]), sstr);
//     Squeak::setAnOutput(err[i], std::cout);
//     ASSERT_EQ(Squeak::mout(exc[i]), std::cout);
//   }
// }

// test mout()
// TEST_F(SqueakTest, SqueakMoutDefaultTest) {
//   std::stringstream sstr;
//   Squeak::setAnOutput(Squeak::debug, sstr);
//   ASSERT_EQ(Squeak::mout(), sstr);
//   Squeak::setAnOutput(Squeak::debug, std::cout);
//   ASSERT_EQ(Squeak::mout(), std::cout);
// }

void __TestStdOutputs(int test_value) {
    int i = test_value;
    if (i > static_cast<int>(Squeak::debug)) {
      EXPECT_EQ(std::cout.rdbuf(), Squeak::nullOut().rdbuf()) << test_value
                          << " " << i << " " << static_cast<int>(Squeak::debug);
    } else {
      EXPECT_EQ(std::cout.rdbuf(), Squeak::coutOut().rdbuf()) << test_value
                          << " " << i << " " << static_cast<int>(Squeak::debug);
    }
    if (i > static_cast<int>(Squeak::info)) {
      EXPECT_EQ(std::clog.rdbuf(), Squeak::nullOut().rdbuf()) << test_value
                          << " " << i << " " << static_cast<int>(Squeak::info);
    } else {
      EXPECT_EQ(std::clog.rdbuf(), Squeak::clogOut().rdbuf()) << test_value
                          << " " << i << " " << static_cast<int>(Squeak::info);
    }
    if (i > static_cast<int>(Squeak::error)) {
      EXPECT_EQ(std::cerr.rdbuf(), Squeak::nullOut().rdbuf()) << test_value
                          << " " << i << " " << static_cast<int>(Squeak::error);
    } else {
      EXPECT_EQ(std::cerr.rdbuf(), Squeak::cerrOut().rdbuf()) << test_value
                          << " " << i << " " << static_cast<int>(Squeak::error);
    }
}

TEST_F(SqueakTest, ActivateCoutTest) {
  Squeak::activateCout(true);
  EXPECT_EQ(std::cout.rdbuf(), Squeak::coutOut().rdbuf());
  EXPECT_EQ(Squeak::coutIsActive(), true);

  Squeak::activateCout(false);
  EXPECT_EQ(std::cout.rdbuf(), Squeak::nullOut().rdbuf());
  EXPECT_EQ(Squeak::coutIsActive(), false);
  std::cout << "IF YOU SEE THIS THERE IS AN ERROR" << std::endl;

  Squeak::activateCout(true);
  EXPECT_EQ(std::cout.rdbuf(), Squeak::coutOut().rdbuf());
  EXPECT_EQ(Squeak::coutIsActive(), true);
}

TEST_F(SqueakTest, ActivateClogTest) {
  Squeak::activateClog(true);
  EXPECT_EQ(std::clog.rdbuf(), Squeak::clogOut().rdbuf());
  EXPECT_EQ(Squeak::clogIsActive(), true);

  Squeak::activateClog(false);
  EXPECT_EQ(std::clog.rdbuf(), Squeak::nullOut().rdbuf());
  EXPECT_EQ(Squeak::clogIsActive(), false);
  std::clog << "IF YOU SEE THIS THERE IS AN ERROR" << std::endl;

  Squeak::activateClog(true);
  EXPECT_EQ(std::clog.rdbuf(), Squeak::clogOut().rdbuf());
  EXPECT_EQ(Squeak::clogIsActive(), true);
}

TEST_F(SqueakTest, ActivateCerrTest) {
  Squeak::activateCerr(true);
  EXPECT_EQ(std::cerr.rdbuf(), Squeak::cerrOut().rdbuf());
  EXPECT_EQ(Squeak::cerrIsActive(), true);

  Squeak::activateCerr(false);
  EXPECT_EQ(std::cerr.rdbuf(), Squeak::nullOut().rdbuf());
  EXPECT_EQ(Squeak::cerrIsActive(), false);
  std::cerr << "IF YOU SEE THIS THERE IS AN ERROR" << std::endl;

  Squeak::activateCerr(true);
  EXPECT_EQ(std::cerr.rdbuf(), Squeak::cerrOut().rdbuf());
  EXPECT_EQ(Squeak::cerrIsActive(), true);
}

TEST_F(SqueakTest, SetStandardOutputsTest) {
  for (int i = 0; i < 6; ++i) {
    Squeak::setStandardOutputs(i);
    __TestStdOutputs(i);
  }
  for (int i = 6; i > -1; --i) {
    Squeak::setStandardOutputs(i);
    __TestStdOutputs(i);
  }
}

void __TestOutputs(int test_value) {
    int i = test_value;
    if (i > static_cast<int>(Squeak::debug)) {
      EXPECT_EQ(Squeak::mout(Squeak::debug).rdbuf(), Squeak::nullOut().rdbuf())
           << test_value << " " << i << " " << static_cast<int>(Squeak::debug);
    } else {
      EXPECT_EQ(Squeak::mout(Squeak::debug).rdbuf(), Squeak::coutOut().rdbuf())
            << test_value << " " << i << " " << static_cast<int>(Squeak::debug);
    }
    if (i > static_cast<int>(Squeak::info)) {
      EXPECT_EQ(Squeak::mout(Squeak::info).rdbuf(), Squeak::nullOut().rdbuf())
           << test_value << " " << i << " " << static_cast<int>(Squeak::info);
    } else {
      EXPECT_EQ(Squeak::mout(Squeak::info).rdbuf(), Squeak::clogOut().rdbuf())
            << test_value << " " << i << " " << static_cast<int>(Squeak::info);
    }
    if (i > static_cast<int>(Squeak::warning)) {
      EXPECT_EQ(Squeak::mout(Squeak::warning).rdbuf(), Squeak::nullOut().rdbuf())
           << test_value << " " << i << " " << static_cast<int>(Squeak::warning);
    } else {
      EXPECT_EQ(Squeak::mout(Squeak::warning).rdbuf(), Squeak::cerrOut().rdbuf())
            << test_value << " " << i << " " << static_cast<int>(Squeak::warning);
    }
    if (i > static_cast<int>(Squeak::error)) {
      EXPECT_EQ(Squeak::mout(Squeak::error).rdbuf(), Squeak::nullOut().rdbuf())
           << test_value << " " << i << " " << static_cast<int>(Squeak::error);
    } else {
      EXPECT_EQ(Squeak::mout(Squeak::error).rdbuf(), Squeak::cerrOut().rdbuf())
            << test_value << " " << i << " " << static_cast<int>(Squeak::error);
    }
    if (i > static_cast<int>(Squeak::fatal)) {
      EXPECT_EQ(Squeak::mout(Squeak::fatal).rdbuf(), Squeak::nullOut().rdbuf())
           << test_value << " " << i << " " << static_cast<int>(Squeak::fatal);
    } else {
      EXPECT_EQ(Squeak::mout(Squeak::fatal).rdbuf(), Squeak::cerrOut().rdbuf())
            << test_value << " " << i << " " << static_cast<int>(Squeak::fatal);
    }
}

TEST_F(SqueakTest, SetOutputsTest) {
  for (int i = 0; i < 6; ++i) {
    Squeak::setOutputs(i);
    __TestOutputs(i);
  }
  for (int i = 6; i > -1; --i) {
    Squeak::setOutputs(i);
    __TestOutputs(i);
  }
}
}

