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
 */

#include "gtest/gtest.h"

#include "src/common_cpp/DataStructure/DateTime.hh"

namespace MAUS {
TEST(DateTimeTest, TestCheck) {
    EXPECT_TRUE(DateTime::CheckFormat("1976-04-04T00:00:00.000000"));
    // length
    EXPECT_FALSE(DateTime::CheckFormat("1976-04-04A00:00:00.0000000"));
    // separators
    EXPECT_TRUE(DateTime::CheckFormat("1976-04-04A00:00:00.000000"));
    EXPECT_FALSE(DateTime::CheckFormat("1976+04-04T00:00:00.000000"));
    EXPECT_FALSE(DateTime::CheckFormat("1976-04+04T00:00:00.000000"));
    EXPECT_FALSE(DateTime::CheckFormat("1976-04-04T00+00:00.000000"));
    EXPECT_FALSE(DateTime::CheckFormat("1976-04-04T00:00+00.000000"));
    EXPECT_FALSE(DateTime::CheckFormat("1976-04-04T00:00:00+000000"));
    EXPECT_FALSE(DateTime::CheckFormat("1976-04-04T00+00:00.000000"));
    // int as character
    EXPECT_FALSE(DateTime::CheckFormat("1976-04-04T00:00:00.00000a"));
    // month
    EXPECT_FALSE(DateTime::CheckFormat("1976-00-04T00:00:00.000000"));
    EXPECT_FALSE(DateTime::CheckFormat("1976-13-04T00:00:00.000000"));
    // days
    EXPECT_FALSE(DateTime::CheckFormat("1976-01-00T00:00:00.000000"));
    EXPECT_FALSE(DateTime::CheckFormat("1976-01-32T00:00:00.000000"));
    EXPECT_FALSE(DateTime::CheckFormat("1976-02-30T00:00:00.000000"));
    EXPECT_FALSE(DateTime::CheckFormat("1976-03-32T00:00:00.000000"));
    EXPECT_FALSE(DateTime::CheckFormat("1976-04-31T00:00:00.000000"));
    EXPECT_FALSE(DateTime::CheckFormat("1976-05-32T00:00:00.000000"));
    EXPECT_FALSE(DateTime::CheckFormat("1976-06-31T00:00:00.000000"));
    EXPECT_FALSE(DateTime::CheckFormat("1976-07-32T00:00:00.000000"));
    EXPECT_FALSE(DateTime::CheckFormat("1976-08-32T00:00:00.000000"));
    EXPECT_FALSE(DateTime::CheckFormat("1976-09-31T00:00:00.000000"));
    EXPECT_FALSE(DateTime::CheckFormat("1976-10-32T00:00:00.000000"));
    EXPECT_FALSE(DateTime::CheckFormat("1976-11-31T00:00:00.000000"));
    EXPECT_FALSE(DateTime::CheckFormat("1976-12-32T00:00:00.000000"));
    // hours
    EXPECT_TRUE(DateTime::CheckFormat("1976-01-04T00:00:00.000000"));
    EXPECT_FALSE(DateTime::CheckFormat("1976-01-04T24:00:00.000000"));
    // minutes
    EXPECT_FALSE(DateTime::CheckFormat("1976-01-04T00:60:00.000000"));
    // seconds
    EXPECT_FALSE(DateTime::CheckFormat("1976-01-04T00:00:60.000000"));
}

TEST(DateTimeTest, TestDefaultConstructor) {
    DateTime dt;
    EXPECT_EQ(dt.GetDateTime(), std::string("1976-04-04T00:00:00.000000"));
}

TEST(DateTimeTest, TestGetSet) {
    DateTime dt;
    EXPECT_EQ(dt.GetDateTime(), std::string("1976-04-04T00:00:00.000000"));
    dt.SetDateTime(std::string("1977-05-06 01:02:03.000005"));
    EXPECT_EQ(dt.GetDateTime(), std::string("1977-05-06 01:02:03.000005"));
}

TEST(DateTimeTest, TestCopyConstructors) {
}
}
