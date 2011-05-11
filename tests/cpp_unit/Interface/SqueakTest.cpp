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

#include "gtest/gtest.h"

#include "src/common/Interface/MICERun.hh"
#include "src/common/Interface/Squeak.hh"
#include "src/common/Interface/Squeal.hh"

namespace {

// test mout(Squeak::errorLevel) and SetOutput(Squeak::errorLevel)
TEST(SqueakTest, SqueakSetOutputMoutErrorLevelTest) {
  std::stringstream sstr;
  // debug, info, warning, error, fatal
  Squeak::errorLevel err[] = {Squeak::debug, Squeak::info, Squeak::warning,
                                                  Squeak::error, Squeak::fatal};
  for (int i = 0; i < 5; ++i) {
    Squeak::setOutput(err[i], sstr);
    ASSERT_EQ(Squeak::mout(err[i]), sstr) << "errorLevel " << err[i]
                                                                   << " failed";
    Squeak::setOutput(err[i], std::cout);
    ASSERT_EQ(Squeak::mout(err[i]), std::cout) << "errorLevel " << err[i]
                                                                   << " failed";
  }
}

// test mout(Squeal::exceptionLevel)
TEST(SqueakTest, SqueakMoutExceptionTest) {
  std::stringstream sstr;
  Squeal::exceptionLevel exc[] = {Squeal::recoverable, Squeal::nonRecoverable};
  Squeak::errorLevel err[] = {Squeak::error, Squeak::fatal};
  for (int i = 0; i < 2; ++i) {
    Squeak::setOutput(err[i], sstr);
    ASSERT_EQ(Squeak::mout(exc[i]), sstr);
    Squeak::setOutput(err[i], std::cout);
    ASSERT_EQ(Squeak::mout(exc[i]), std::cout);
  }
}

// test mout()
TEST(SqueakTest, SqueakMoutDefaultTest) {
  std::stringstream sstr;
  Squeak::setOutput(Squeak::debug, sstr);
  ASSERT_EQ(Squeak::mout(), sstr);
  Squeak::setOutput(Squeak::debug, std::cout);
  ASSERT_EQ(Squeak::mout(), std::cout);
}

void __TestStdOutputs(int i) {
    if (i < 0)
      i = MICERun::getInstance()->DataCards->fetchValueInt("VerboseLevel");
    if (i > static_cast<int>(Squeak::debug)) {
      EXPECT_EQ(std::cout.rdbuf(), Squeak::nullOut().rdbuf()) << i << " "
                                             << static_cast<int>(Squeak::debug);
    } else {
      EXPECT_EQ(std::cout.rdbuf(), Squeak::coutOut().rdbuf()) << i << " "
                                             << static_cast<int>(Squeak::debug);
    }
    if (i > static_cast<int>(Squeak::info)) {
      EXPECT_EQ(std::clog.rdbuf(), Squeak::nullOut().rdbuf()) << i << " "
                                              << static_cast<int>(Squeak::info);
    } else {
      EXPECT_EQ(std::clog.rdbuf(), Squeak::clogOut().rdbuf()) << i << " "
                                              << static_cast<int>(Squeak::info);
    }
    if (i > static_cast<int>(Squeak::warning)) {
      EXPECT_EQ(std::cerr.rdbuf(), Squeak::nullOut().rdbuf()) << i << " "
                                           << static_cast<int>(Squeak::warning);
    } else {
      EXPECT_EQ(std::cerr.rdbuf(), Squeak::cerrOut().rdbuf()) << i << " "
                                           << static_cast<int>(Squeak::warning);
    }
}

TEST(SqueakTest, GetOutputsTest) {
  __TestStdOutputs(0);
}

TEST(SqueakTest, ActivateCoutTest) {
  Squeak::activateCout(true);
  EXPECT_EQ(std::cout.rdbuf(), Squeak::coutOut().rdbuf());
  Squeak::activateCout(false);
  EXPECT_EQ(std::cout.rdbuf(), Squeak::nullOut().rdbuf());
  std::cout << "IF YOU SEE THIS THERE IS AN ERROR" << std::endl;
  Squeak::activateCout(true);
  EXPECT_EQ(std::cout.rdbuf(), Squeak::coutOut().rdbuf());
}

TEST(SqueakTest, ActivateClogTest) {
  Squeak::activateClog(true);
  EXPECT_EQ(std::clog.rdbuf(), Squeak::clogOut().rdbuf());
  Squeak::activateClog(false);
  EXPECT_EQ(std::clog.rdbuf(), Squeak::nullOut().rdbuf());
  std::clog << "IF YOU SEE THIS THERE IS AN ERROR" << std::endl;
  Squeak::activateClog(true);
  EXPECT_EQ(std::clog.rdbuf(), Squeak::clogOut().rdbuf());
}

TEST(SqueakTest, ActivateCerrTest) {
  Squeak::activateCerr(true);
  EXPECT_EQ(std::cerr.rdbuf(), Squeak::cerrOut().rdbuf());
  Squeak::activateCerr(false);
  EXPECT_EQ(std::cerr.rdbuf(), Squeak::nullOut().rdbuf());
  std::cerr << "IF YOU SEE THIS THERE IS AN ERROR" << std::endl;
  Squeak::activateCerr(true);
  EXPECT_EQ(std::cerr.rdbuf(), Squeak::cerrOut().rdbuf());
}

TEST(SqueakTest, SetStandardOutputs) {
  for (int i = -1; i < 6; ++i) {
    Squeak::setStandardOutputs(i);
    __TestStdOutputs(i);
  }
  for (int i = 6; i > -2; --i) {
    Squeak::setStandardOutputs(i);
    __TestStdOutputs(i);
  }
}
}

