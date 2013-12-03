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

#include <string>
#include <limits>

#include "gtest/gtest.h"

#include "json/value.h"

#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/JsonCppProcessors/PrimitivesProcessors.hh"

namespace MAUS {

typedef LLUIntProcessor::lluint lluint;

TEST(PrimitivesProcessorsTest, DoubleJsonToCpp) {
  DoubleProcessor proc;
  double* value = proc.JsonToCpp(Json::Value(1.));
  EXPECT_EQ(*value, 1.);
  delete value;
  EXPECT_THROW(proc.JsonToCpp(Json::Value("string")), Squeal);
}

TEST(PrimitivesProcessorsTest, DoubleCppToJson) {
  DoubleProcessor proc;
  double value_cpp = 1.;
  Json::Value* value_json = proc.CppToJson(value_cpp);
  EXPECT_EQ(value_json->asDouble(), value_cpp);
  delete value_json;
  value_json = proc.CppToJson(value_cpp, "path");
  EXPECT_EQ(JsonWrapper::Path::GetPath(*value_json), "path");
  delete value_json;
}

TEST(PrimitivesProcessorsTest, BoolJsonToCpp) {
  BoolProcessor proc;
  bool* value = proc.JsonToCpp(Json::Value(true));
  EXPECT_EQ(*value, true);
  delete value;
  EXPECT_THROW(proc.JsonToCpp(Json::Value("string")), Squeal);
}

TEST(PrimitivesProcessorsTest, BoolCppToJson) {
  BoolProcessor proc;
  bool value_cpp = true;
  Json::Value* value_json = proc.CppToJson(value_cpp);
  EXPECT_EQ(value_json->asBool(), value_cpp);
  delete value_json;
  value_json = proc.CppToJson(value_cpp, "path");
  EXPECT_EQ(JsonWrapper::Path::GetPath(*value_json), "path");
  delete value_json;
}

TEST(PrimitivesProcessorsTest, StringJsonToCpp) {
  StringProcessor proc;
  std::string* value = proc.JsonToCpp(Json::Value("string"));
  EXPECT_EQ(*value, "string");
  delete value;
  EXPECT_THROW(proc.JsonToCpp(Json::Value(1.)), Squeal);
}

TEST(PrimitivesProcessorsTest, StringCppToJson) {
  StringProcessor proc;
  std::string value_cpp = "string";
  Json::Value* value_json = proc.CppToJson(value_cpp);
  EXPECT_EQ(value_json->asString(), value_cpp);
  delete value_json;
  value_json = proc.CppToJson(value_cpp, "path");
  EXPECT_EQ(JsonWrapper::Path::GetPath(*value_json), "path");
  delete value_json;
}

TEST(PrimitivesProcessorsTest, IntJsonToCpp) {
  IntProcessor proc;
  int* value = proc.JsonToCpp(Json::Value(1));
  EXPECT_EQ(*value, 1);
  delete value;
  EXPECT_THROW(proc.JsonToCpp(Json::Value("string")), Squeal);
}

TEST(PrimitivesProcessorsTest, IntCppToJson) {
  IntProcessor proc;
  int value_cpp = 1;
  Json::Value* value_json = proc.CppToJson(value_cpp);
  EXPECT_EQ(value_json->asInt(), value_cpp);
  delete value_json;
  value_json = proc.CppToJson(value_cpp, "path");
  EXPECT_EQ(JsonWrapper::Path::GetPath(*value_json), "path");
  delete value_json;
}

TEST(PrimitivesProcessorsTest, UIntJsonToCpp) {
  UIntProcessor proc;
  unsigned int* value;

  value = proc.JsonToCpp(Json::Value(1)); // int
  EXPECT_EQ(*value, static_cast<unsigned int>(1));
  delete value;

  value = proc.JsonToCpp(Json::Value(0)); // int
  EXPECT_EQ(*value, static_cast<unsigned int>(0));
  delete value;

  EXPECT_THROW(proc.JsonToCpp(Json::Value(-1)), Squeal); // negative int

  unsigned int uint_in = 1;
  value = proc.JsonToCpp(Json::Value(uint_in)); // uint
  EXPECT_EQ(*value, static_cast<unsigned int>(1));
  delete value;

  uint_in = 0;
  value = proc.JsonToCpp(Json::Value(uint_in)); // uint
  EXPECT_EQ(*value, static_cast<unsigned int>(0));
  delete value;

  uint_in = 2147483648;
  value = proc.JsonToCpp(Json::Value(uint_in)); // uint
  EXPECT_EQ(*value, static_cast<unsigned int>(2147483648));
  delete value;

  EXPECT_THROW(proc.JsonToCpp(Json::Value("string")), Squeal);
}

TEST(PrimitivesProcessorsTest, UIntCppToJson) {
  UIntProcessor proc;
  unsigned int value_cpp = 1.;
  Json::Value* value_json = proc.CppToJson(value_cpp);
  EXPECT_EQ(value_json->asUInt(), value_cpp);
  delete value_json;
  value_json = proc.CppToJson(value_cpp, "path");
  EXPECT_EQ(JsonWrapper::Path::GetPath(*value_json), "path");
  delete value_json;
}

TEST(PrimitivesProcessorsTest, LLUIntCppToJson) {
  LLUIntProcessor proc;
  lluint value_cpp = std::numeric_limits<lluint>::max();
  Json::Value* value_json = proc.CppToJson(value_cpp);
  lluint* value_cpp_2 = proc.JsonToCpp(*value_json);
  EXPECT_EQ(value_cpp, *value_cpp_2);
  delete value_cpp_2;
  delete value_json;
  value_json = proc.CppToJson(value_cpp, "path");
  EXPECT_EQ(JsonWrapper::Path::GetPath(*value_json), "path");
  delete value_json;
}

TEST(PrimitivesProcessorsTest, LLUIntJsonToCpp) {
  LLUIntProcessor proc;
  Json::Value value_json;

  value_json = Json::Value("0");
  lluint* value_cpp = proc.JsonToCpp(value_json);
  EXPECT_EQ(*value_cpp, lluint(0));

  value_json = Json::Value("-0");
  EXPECT_THROW(proc.JsonToCpp(value_json), Squeal);

  value_json = Json::Value("abc");
  EXPECT_THROW(proc.JsonToCpp(value_json), Squeal);

  value_json = Json::Value(1);
  EXPECT_THROW(proc.JsonToCpp(value_json), Squeal);
}
}

