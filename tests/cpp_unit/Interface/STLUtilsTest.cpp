// Copyright 2010 Chris Rogers
//
// This file is a part of G4MICE
//
// G4MICE is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// G4MICE is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with G4MICE in the doc folder.  If not, see
// <http://www.gnu.org/licenses/>.

#include "gtest/gtest.h" 

#include "src/common/Interface/STLUtils.hh"

namespace {

class STLUtilsTest : public ::testing::Test {
  protected:
    STLUtilsTest() {}
    virtual ~STLUtilsTest() {}
    virtual void SetUp()    {}
    virtual void TearDown() {}
};

TEST_F(STLUtilsTest, IterableEqualityTest) {
  std::vector<int> a, b;
  EXPECT_TRUE(STLUtils::IterableEquality(a, b));
  EXPECT_TRUE(STLUtils::IterableEquality(a.begin(), a.end(),
                                         b.begin(), b.end()));
  a = std::vector<int>(10, 1);
  b = std::vector<int>(11, 1);
  EXPECT_FALSE(STLUtils::IterableEquality(a, b));
  EXPECT_FALSE(STLUtils::IterableEquality(b, a));
  a = std::vector<int>(11, 1);
  for (size_t i = 0; i < a.size(); i++) a[i] = b[i] = i;
  EXPECT_TRUE(STLUtils::IterableEquality(a, b));
  EXPECT_TRUE(STLUtils::IterableEquality(a.begin(), a.end(),
                                         b.begin(), b.end()));
  for (size_t i = 0; i < a.size(); i++) {
    for (size_t j = 0; j < a.size(); j++) a[j] = j;
    a[i] = i+1;
    EXPECT_FALSE(STLUtils::IterableEquality(a, b));
    EXPECT_FALSE(STLUtils::IterableEquality(b, a));
  }
}

TEST_F(STLUtilsTest, ToStringTest) {
  EXPECT_EQ(STLUtils::ToString(1), std::string("1"))
                                                << STLUtils::ToString(1);
  EXPECT_EQ(STLUtils::ToString("string"), std::string("string"))
                                                << STLUtils::ToString("string");
  EXPECT_EQ(STLUtils::ToString(3.142), std::string("3.142"))
                                                << STLUtils::ToString(3.142);

  EXPECT_EQ(STLUtils::ToStringP(1001, 3), std::string("1001"))
                                       << STLUtils::ToStringP(1001, 3);
  EXPECT_EQ(STLUtils::ToStringP("string", 3), std::string("string"))
                                       << STLUtils::ToString("string");
  EXPECT_EQ(STLUtils::ToStringP(13.14159265359, 3), std::string("13.1"))
                                      << STLUtils::ToStringP(13.14159265359, 3);
}

}

