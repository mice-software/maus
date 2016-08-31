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

#include <string>
#include <vector>

#include "gtest/gtest.h"

#include "src/common_cpp/Utils/Exception.hh"
#include "src/legacy/Interface/STLUtils.hh"

namespace {

TEST(STLUtilsTest, IterableEqualityTest) {
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

TEST(STLUtilsTest, ToStringTest) {
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

TEST(STLUtilsTest, FromStringTest) {
  EXPECT_EQ(STLUtils::FromString<int>("1"), 1);
  EXPECT_EQ(STLUtils::FromString<std::string>("1"), std::string("1"));
  EXPECT_THROW(STLUtils::FromString<int>("a"), MAUS::Exceptions::Exception);
}

TEST(STLUtilsTest, ReplaceVariablesTest) {
    char* mrd_c = getenv("MAUS_ROOT_DIR");
    if (mrd_c == NULL) {
        ASSERT_EQ(1, 0) << "Need to define $MAUS_ROOT_DIR for env var test";
    }
    std::string mrd(mrd_c);

    EXPECT_EQ(STLUtils::ReplaceVariables("some_string"), "some_string");
    EXPECT_EQ(STLUtils::ReplaceVariables("some_${MAUS_ROOT_DIR}_string"),
                                         "some_"+mrd+"_string");
    EXPECT_EQ(STLUtils::ReplaceVariables("${MAUS_ROOT_DIR}_some_string"),
                                         mrd+"_some_string");
    EXPECT_EQ(STLUtils::ReplaceVariables("some_string_${MAUS_ROOT_DIR}"),
                                         "some_string_"+mrd);
    EXPECT_THROW(STLUtils::ReplaceVariables
                      ("some_string_${UNDEFINED_ENV_VAR_ZSSDFDSASD}"),
                       MAUS::Exceptions::Exception);
    EXPECT_THROW(STLUtils::ReplaceVariables
                      ("some_string_$MAUS_ROOT_DIR}"), MAUS::Exceptions::Exception);
    EXPECT_THROW(STLUtils::ReplaceVariables
                      ("some_string_${MAUS_ROOT_DIR"), MAUS::Exceptions::Exception);
    EXPECT_THROW(STLUtils::ReplaceVariables
                      ("some_string_$MAUS_ROOT_DIR"), MAUS::Exceptions::Exception);
}
}

