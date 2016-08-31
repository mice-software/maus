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
#include "Utils/Squeak.hh"
#include "Utils/Exception.hh"

namespace {

class SqueakTest : public ::testing::Test {
 protected:
  SqueakTest() {}
  virtual ~SqueakTest() {
    Json::Value& cards = *MAUS::Globals::GetConfigurationCards();
    int verbose_level = cards["verbose_level"].asInt();
    MAUS::Squeak::setStandardOutputs(verbose_level);
    MAUS::Squeak::setOutputs(verbose_level);
  }
};

// Commented out as could not get to work with gcc 5
// A. Dobbs, 16/12/2015

// test mout(MAUS::Squeak::errorLevel) and SetOutput(MAUS::Squeak::errorLevel)
// TEST_F(SqueakTest, SqueakSetOutputMoutErrorLevelTest) {
//   std::stringstream sstr;
//   // debug, info, warning, error, fatal
//   MAUS::Squeak::errorLevel err[] = {MAUS::Squeak::debug, MAUS::Squeak::info,
//       MAUS::Squeak::warning, MAUS::Squeak::error, MAUS::Squeak::fatal};
//   for (int i = 0; i < 5; ++i) {
//     MAUS::Squeak::setAnOutput(err[i], sstr);
//     ASSERT_EQ(MAUS::Squeak::mout(err[i]), sstr) << "errorLevel " << err[i]
//                                                                    << " failed";
//     MAUS::Squeak::setAnOutput(err[i], std::cout);
//     ASSERT_EQ(MAUS::Squeak::mout(err[i]), std::cout) << "errorLevel " << err[i]
//                                                                    << " failed";
//   }
// }

// test mout(Squeal::exceptionLevel)
// TEST_F(SqueakTest, SqueakMoutExceptionTest) {
//   std::stringstream sstr;
//   MAUS::Exceptions::Exception::exceptionLevel exc[] = {
//     Exceptions::recoverable, Exceptions::nonRecoverable};
//   MAUS::Squeak::errorLevel err[] = {MAUS::Squeak::error, MAUS::Squeak::fatal};
//   for (int i = 0; i < 2; ++i) {
//     MAUS::Squeak::setAnOutput(err[i], sstr);
//     ASSERT_EQ(MAUS::Squeak::mout(exc[i]), sstr);
//     MAUS::Squeak::setAnOutput(err[i], std::cout);
//     ASSERT_EQ(MAUS::Squeak::mout(exc[i]), std::cout);
//   }
// }

// test mout()
// TEST_F(SqueakTest, SqueakMoutDefaultTest) {
//   std::stringstream sstr;
//   MAUS::Squeak::setAnOutput(MAUS::Squeak::debug, sstr);
//   ASSERT_EQ(MAUS::Squeak::mout(), sstr);
//   MAUS::Squeak::setAnOutput(MAUS::Squeak::debug, std::cout);
//   ASSERT_EQ(MAUS::Squeak::mout(), std::cout);
// }

void __TestStdOutputs(int test_value) {
    int i = test_value;
    if (i > static_cast<int>(MAUS::Squeak::debug)) {
      EXPECT_EQ(std::cout.rdbuf(), MAUS::Squeak::nullOut().rdbuf()) << test_value
                          << " " << i << " " << static_cast<int>(MAUS::Squeak::debug);
    } else {
      EXPECT_EQ(std::cout.rdbuf(), MAUS::Squeak::coutOut().rdbuf()) << test_value
                          << " " << i << " " << static_cast<int>(MAUS::Squeak::debug);
    }
    if (i > static_cast<int>(MAUS::Squeak::info)) {
      EXPECT_EQ(std::clog.rdbuf(), MAUS::Squeak::nullOut().rdbuf()) << test_value
                          << " " << i << " " << static_cast<int>(MAUS::Squeak::info);
    } else {
      EXPECT_EQ(std::clog.rdbuf(), MAUS::Squeak::clogOut().rdbuf()) << test_value
                          << " " << i << " " << static_cast<int>(MAUS::Squeak::info);
    }
    if (i > static_cast<int>(MAUS::Squeak::error)) {
      EXPECT_EQ(std::cerr.rdbuf(), MAUS::Squeak::nullOut().rdbuf()) << test_value
                          << " " << i << " " << static_cast<int>(MAUS::Squeak::error);
    } else {
      EXPECT_EQ(std::cerr.rdbuf(), MAUS::Squeak::cerrOut().rdbuf()) << test_value
                          << " " << i << " " << static_cast<int>(MAUS::Squeak::error);
    }
}

TEST_F(SqueakTest, ActivateCoutTest) {
  MAUS::Squeak::activateCout(true);
  EXPECT_EQ(std::cout.rdbuf(), MAUS::Squeak::coutOut().rdbuf());
  EXPECT_EQ(MAUS::Squeak::coutIsActive(), true);

  MAUS::Squeak::activateCout(false);
  EXPECT_EQ(std::cout.rdbuf(), MAUS::Squeak::nullOut().rdbuf());
  EXPECT_EQ(MAUS::Squeak::coutIsActive(), false);
  std::cout << "IF YOU SEE THIS THERE IS AN ERROR" << std::endl;

  MAUS::Squeak::activateCout(true);
  EXPECT_EQ(std::cout.rdbuf(), MAUS::Squeak::coutOut().rdbuf());
  EXPECT_EQ(MAUS::Squeak::coutIsActive(), true);
}

TEST_F(SqueakTest, ActivateClogTest) {
  MAUS::Squeak::activateClog(true);
  EXPECT_EQ(std::clog.rdbuf(), MAUS::Squeak::clogOut().rdbuf());
  EXPECT_EQ(MAUS::Squeak::clogIsActive(), true);

  MAUS::Squeak::activateClog(false);
  EXPECT_EQ(std::clog.rdbuf(), MAUS::Squeak::nullOut().rdbuf());
  EXPECT_EQ(MAUS::Squeak::clogIsActive(), false);
  std::clog << "IF YOU SEE THIS THERE IS AN ERROR" << std::endl;

  MAUS::Squeak::activateClog(true);
  EXPECT_EQ(std::clog.rdbuf(), MAUS::Squeak::clogOut().rdbuf());
  EXPECT_EQ(MAUS::Squeak::clogIsActive(), true);
}

TEST_F(SqueakTest, ActivateCerrTest) {
  MAUS::Squeak::activateCerr(true);
  EXPECT_EQ(std::cerr.rdbuf(), MAUS::Squeak::cerrOut().rdbuf());
  EXPECT_EQ(MAUS::Squeak::cerrIsActive(), true);

  MAUS::Squeak::activateCerr(false);
  EXPECT_EQ(std::cerr.rdbuf(), MAUS::Squeak::nullOut().rdbuf());
  EXPECT_EQ(MAUS::Squeak::cerrIsActive(), false);
  std::cerr << "IF YOU SEE THIS THERE IS AN ERROR" << std::endl;

  MAUS::Squeak::activateCerr(true);
  EXPECT_EQ(std::cerr.rdbuf(), MAUS::Squeak::cerrOut().rdbuf());
  EXPECT_EQ(MAUS::Squeak::cerrIsActive(), true);
}

TEST_F(SqueakTest, SetStandardOutputsTest) {
  for (int i = 0; i < 6; ++i) {
    MAUS::Squeak::setStandardOutputs(i);
    __TestStdOutputs(i);
  }
  for (int i = 6; i > -1; --i) {
    MAUS::Squeak::setStandardOutputs(i);
    __TestStdOutputs(i);
  }
}

void __TestOutputs(int test_value) {
    int i = test_value;
    if (i > static_cast<int>(MAUS::Squeak::debug)) {
      EXPECT_EQ(MAUS::Squeak::mout(MAUS::Squeak::debug).rdbuf(), MAUS::Squeak::nullOut().rdbuf())
           << test_value << " " << i << " " << static_cast<int>(MAUS::Squeak::debug);
    } else {
      EXPECT_EQ(MAUS::Squeak::mout(MAUS::Squeak::debug).rdbuf(), MAUS::Squeak::coutOut().rdbuf())
            << test_value << " " << i << " " << static_cast<int>(MAUS::Squeak::debug);
    }
    if (i > static_cast<int>(MAUS::Squeak::info)) {
      EXPECT_EQ(MAUS::Squeak::mout(MAUS::Squeak::info).rdbuf(), MAUS::Squeak::nullOut().rdbuf())
           << test_value << " " << i << " " << static_cast<int>(MAUS::Squeak::info);
    } else {
      EXPECT_EQ(MAUS::Squeak::mout(MAUS::Squeak::info).rdbuf(), MAUS::Squeak::clogOut().rdbuf())
            << test_value << " " << i << " " << static_cast<int>(MAUS::Squeak::info);
    }
    if (i > static_cast<int>(MAUS::Squeak::warning)) {
      EXPECT_EQ(MAUS::Squeak::mout(MAUS::Squeak::warning).rdbuf(), MAUS::Squeak::nullOut().rdbuf())
           << test_value << " " << i << " " << static_cast<int>(MAUS::Squeak::warning);
    } else {
      EXPECT_EQ(MAUS::Squeak::mout(MAUS::Squeak::warning).rdbuf(), MAUS::Squeak::cerrOut().rdbuf())
            << test_value << " " << i << " " << static_cast<int>(MAUS::Squeak::warning);
    }
    if (i > static_cast<int>(MAUS::Squeak::error)) {
      EXPECT_EQ(MAUS::Squeak::mout(MAUS::Squeak::error).rdbuf(), MAUS::Squeak::nullOut().rdbuf())
           << test_value << " " << i << " " << static_cast<int>(MAUS::Squeak::error);
    } else {
      EXPECT_EQ(MAUS::Squeak::mout(MAUS::Squeak::error).rdbuf(), MAUS::Squeak::cerrOut().rdbuf())
            << test_value << " " << i << " " << static_cast<int>(MAUS::Squeak::error);
    }
    if (i > static_cast<int>(MAUS::Squeak::fatal)) {
      EXPECT_EQ(MAUS::Squeak::mout(MAUS::Squeak::fatal).rdbuf(), MAUS::Squeak::nullOut().rdbuf())
           << test_value << " " << i << " " << static_cast<int>(MAUS::Squeak::fatal);
    } else {
      EXPECT_EQ(MAUS::Squeak::mout(MAUS::Squeak::fatal).rdbuf(), MAUS::Squeak::cerrOut().rdbuf())
            << test_value << " " << i << " " << static_cast<int>(MAUS::Squeak::fatal);
    }
}

TEST_F(SqueakTest, SetOutputsTest) {
  for (int i = 0; i < 6; ++i) {
    MAUS::Squeak::setOutputs(i);
    __TestOutputs(i);
  }
  for (int i = 6; i > -1; --i) {
    MAUS::Squeak::setOutputs(i);
    __TestOutputs(i);
  }
}
}

