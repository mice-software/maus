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

#include "src/common_cpp/JsonCppProcessors/ObjectMapProcessors.hh"
#include "src/common_cpp/JsonCppProcessors/PrimitivesProcessors.hh"

namespace MAUS {

TEST(ObjectMapProcessorsTest, ObjectMapValueConstructorTest) {
    ObjectMapValueProcessor<std::string> test(new StringProcessor());
}

TEST(ObjectMapProcessorsTest, ObjectMapValueJsonToCppTest) {
    ObjectMapValueProcessor<int> test(new IntProcessor());
    Json::Value value_in;
    value_in["one"] = 1;
    value_in["two"] = 2;
    value_in["minus_one"] = -1;
    std::map<std::string, int>* map_out = test.JsonToCpp(value_in);
    EXPECT_EQ((*map_out)["one"], 1);
    EXPECT_EQ((*map_out)["two"], 2);
    EXPECT_EQ((*map_out)["minus_one"], -1);
    delete map_out;
}

TEST(ObjectMapProcessorsTest, ObjectMapValueJsonToCppBadValueTest) {
    ObjectMapValueProcessor<int> test(new IntProcessor());
    Json::Value value_in;
    value_in["one"] = "bob";
    EXPECT_THROW(test.JsonToCpp(value_in), Exceptions::Exception);
}

TEST(ObjectMapProcessorsTest, ObjectMapValueJsonToCppNullTest) {
    ObjectMapValueProcessor<int> test(new IntProcessor());
    Json::Value value_null;
    std::map<std::string, int>* map_out = test.JsonToCpp(value_null);
    EXPECT_EQ(map_out->size(), size_t(0));
    delete map_out;
}

TEST(ObjectMapProcessorsTest, ObjectMapValueJsonToCppBadTypeTest) {
    ObjectMapValueProcessor<int> test(new IntProcessor());
    Json::Value value_in(1);
    EXPECT_THROW(test.JsonToCpp(value_in), Exceptions::Exception);
}

TEST(ObjectMapProcessorsTest, ObjectMapValueCppToJsonTest) {
    ObjectMapValueProcessor<int> test(new IntProcessor());
    std::map<std::string, int> map_in;
    map_in["one"] = 1;
    map_in["two"] = 2;
    map_in["minus_one"] = -1;
    Json::Value* value_out = test.CppToJson(map_in);
    EXPECT_EQ((*value_out)["one"], 1);
    EXPECT_EQ((*value_out)["two"], 2);
    EXPECT_EQ((*value_out)["minus_one"], -1);
    delete value_out;

    value_out = test.CppToJson(map_in, "path");
    EXPECT_EQ(JsonWrapper::Path::GetPath(*value_out), "path");
    EXPECT_EQ(JsonWrapper::Path::GetPath((*value_out)["one"]), "path/one");
    EXPECT_EQ(JsonWrapper::Path::GetPath((*value_out)["two"]), "path/two");
    EXPECT_EQ(JsonWrapper::Path::GetPath((*value_out)["minus_one"]),
              "path/minus_one");
    delete value_out;
}
}

