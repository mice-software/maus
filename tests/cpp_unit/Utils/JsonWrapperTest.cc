// Copyright 2011 Chris Rogers
//
// This file is a part of MAUS
//
// MAUS is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// MAUS is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with MAUS in the doc folder.  If not, see
// <http://www.gnu.org/licenses/>.

#include "gtest/gtest.h"

#include "src/common_cpp/Utils/JsonWrapper.hh"

namespace {

TEST(JsonWrapperTest, StringToJson) {
  std::string good_json = "{}";
  Json::Value val = JsonWrapper::StringToJson(good_json);
  EXPECT_EQ(val, Json::Value(Json::objectValue));
  std::string bad_json = "{";
  EXPECT_THROW(JsonWrapper::StringToJson(bad_json), Exceptions::Exception);
}

TEST(JsonWrapperTest, JsonToString) {
  Json::Value json = Json::Value("bob");
  std::string val = JsonWrapper::JsonToString(json);
  EXPECT_EQ(val, std::string("\"bob\""));
}

TEST(JsonWrapperTest, GetItemTest) {
  std::string good_json = "[2, 3, 4]";
  Json::Value good_val = JsonWrapper::StringToJson(good_json);
  for (unsigned int i = 0; i < 2; ++i) {
    EXPECT_EQ(Json::Value(static_cast<int>(i+2)),
      JsonWrapper::GetItem(good_val, i, JsonWrapper::intValue));
    EXPECT_EQ(Json::Value(static_cast<int>(i+2)),
      JsonWrapper::GetItem(good_val, i, JsonWrapper::anyValue));
    EXPECT_THROW( // bad type
     JsonWrapper::GetItem(good_val, i, JsonWrapper::stringValue), Exceptions::Exception);
  }
  Json::Value empty_array =  JsonWrapper::StringToJson("[]");
  EXPECT_THROW( // empty value
    JsonWrapper::GetItem(empty_array, 0, JsonWrapper::anyValue), Exceptions::Exception);
  EXPECT_THROW( // out of range item
    JsonWrapper::GetItem(good_val, 4, JsonWrapper::anyValue), Exceptions::Exception);

}

TEST(JsonWrapperTest, GetPropertyTest) {
  Json::Value test;
  std::string good_json =
      std::string("{\"real\":2.5, \"int\":-3, \"null\":null, ")+
      std::string("\"bool\":true, \"object\":{}, \"array\":[]}");
  std::string gets [] = {"real", "int", "null", "bool", "object", "array"};
  JsonWrapper::JsonType types[] =
   {JsonWrapper::realValue, JsonWrapper::intValue,
    JsonWrapper::nullValue, JsonWrapper::booleanValue, JsonWrapper::objectValue,
    JsonWrapper::arrayValue};
  Json::Value good_val = JsonWrapper::StringToJson(good_json);
  good_val["uint"] = Json::Value(Json::UInt(4));
  for (unsigned int i = 0; i < 5; ++i)
    for (unsigned int j = 0; j < 5; ++j) {
      if ( i == j || (i == 1 && j == 0)) {
          // it's okay
          EXPECT_NO_THROW(JsonWrapper::GetProperty(good_val, gets[i], types[j]))
                          << i << " " << j << " " << gets[i];
      } else {
          EXPECT_THROW(JsonWrapper::GetProperty(good_val, gets[i], types[j]),
                       Exceptions::Exception)  << i << " " << j << " " << gets[i];
      }
    }
  EXPECT_EQ(Json::Value(2.5),
            JsonWrapper::GetProperty(good_val, gets[0], types[0]));
  EXPECT_EQ(Json::Value(-3),
            JsonWrapper::GetProperty(good_val, gets[1], types[1]));
   EXPECT_EQ(Json::Value(Json::nullValue),
            JsonWrapper::GetProperty(good_val, gets[2], types[2]));
  EXPECT_EQ(Json::Value(true),
            JsonWrapper::GetProperty(good_val, gets[3], types[3]));
  EXPECT_EQ(Json::Value(Json::objectValue),
            JsonWrapper::GetProperty(good_val, gets[4], types[4]));
  EXPECT_EQ(Json::Value(Json::arrayValue),
            JsonWrapper::GetProperty(good_val, gets[5], types[5]));
  Json::Value emptyProp =  JsonWrapper::StringToJson("{}");
  EXPECT_THROW(JsonWrapper::GetProperty(emptyProp, "a", JsonWrapper::anyValue),
               Exceptions::Exception);
}

TEST(JsonWrapperTest, TypeConversionTest) {
   JsonWrapper::JsonType wr_tp[] = {JsonWrapper::nullValue,
     JsonWrapper::uintValue, JsonWrapper::intValue, JsonWrapper::realValue,
     JsonWrapper::stringValue, JsonWrapper::booleanValue,
     JsonWrapper::arrayValue, JsonWrapper::objectValue};
   Json::ValueType js_tp[] = {Json::nullValue,
     Json::uintValue, Json::intValue, Json::realValue,
     Json::stringValue, Json::booleanValue,
     Json::arrayValue, Json::objectValue};
  for (size_t i = 0; i < 8; ++i) {
    EXPECT_EQ(JsonWrapper::JsonTypeToValueType(wr_tp[i]), js_tp[i]);
    EXPECT_EQ(JsonWrapper::ValueTypeToJsonType(js_tp[i]), wr_tp[i]);
  };
  EXPECT_THROW(JsonWrapper::JsonTypeToValueType(JsonWrapper::anyValue), Exceptions::Exception);
}

TEST(JsonWrapperTest, SimilarTypeTest) {
   JsonWrapper::JsonType wr_tp[] = {JsonWrapper::nullValue,
     JsonWrapper::uintValue, JsonWrapper::intValue, JsonWrapper::realValue,
     JsonWrapper::stringValue, JsonWrapper::booleanValue,
     JsonWrapper::arrayValue, JsonWrapper::objectValue, JsonWrapper::anyValue};
  for (size_t i = 0; i < 9; ++i)
    for (size_t j = 0; j < 9; ++j) {
      bool is_okay = i == j || i == 8 || j == 8 || (i == 1 && j == 3) ||
                     (i == 2 && j == 3) || (i == 1 && j == 2);
      EXPECT_EQ(JsonWrapper::SimilarType(wr_tp[i], wr_tp[j]), is_okay) 
                << i << " " << j << std::endl;
    }
}

TEST(JsonWrapperTest, PrintTest) {
  std::stringstream my_ss;
  Json::Value val_in("Mary had a little lamb");
  JsonWrapper::Print(my_ss, val_in);
  Json::Value val_out = JsonWrapper::StringToJson(my_ss.str());
  EXPECT_EQ(val_in, val_out);
}

TEST(JsonWrapperTest, AlmostEqualTest) {
  Json::Value real_1(1.), real_2(1.+1e-8);
  EXPECT_TRUE(JsonWrapper::AlmostEqual(real_1, real_1, 1e-9));
  EXPECT_TRUE(JsonWrapper::AlmostEqual(real_1, real_2, 2e-8));
  EXPECT_FALSE(JsonWrapper::AlmostEqual(real_1, real_2, 1e-9));

  Json::Value null_1, null_2;
  EXPECT_TRUE(JsonWrapper::AlmostEqual(null_1, null_1, 1e-9));

  Json::Value bool_1(true), bool_2(false);
  EXPECT_TRUE(JsonWrapper::AlmostEqual(bool_1, bool_1, 1e-9));
  EXPECT_FALSE(JsonWrapper::AlmostEqual(bool_1, bool_2, 1e-9));

  Json::Value string_1("a"), string_2("b");
  EXPECT_TRUE(JsonWrapper::AlmostEqual(string_1, string_1, 1e-9));
  EXPECT_FALSE(JsonWrapper::AlmostEqual(string_1, string_2, 1e-9));

  Json::Value int_1(1), int_2(2);
  EXPECT_TRUE(JsonWrapper::AlmostEqual(int_1, int_1, 1e-9));
  EXPECT_FALSE(JsonWrapper::AlmostEqual(int_1, int_2, 1e-9));

  Json::Value uint_1(Json::UInt(1)), uint_2(Json::UInt(2));
  EXPECT_TRUE(JsonWrapper::AlmostEqual(uint_1, uint_1, 1e-9));
  EXPECT_FALSE(JsonWrapper::AlmostEqual(uint_1, uint_2, 1e-9));

  EXPECT_FALSE(JsonWrapper::AlmostEqual(uint_1, int_1, 1e-9));  // type checking

  // more array tests below
  Json::Value arr_1(Json::arrayValue), arr_2(Json::arrayValue);
  arr_1.append(int_1);
  arr_2.append(int_2);
  EXPECT_TRUE(JsonWrapper::AlmostEqual(arr_1, arr_1, 1e-9));
  EXPECT_FALSE(JsonWrapper::AlmostEqual(arr_1, arr_2, 1e-9));

  Json::Value arr_3, arr_4, arr_5;
  arr_3.append(int_1);
  arr_4.append(uint_1);
  arr_5.append(real_1);
  EXPECT_TRUE(JsonWrapper::AlmostEqual(arr_3, arr_4, 1e-9, true));
  EXPECT_FALSE(JsonWrapper::AlmostEqual(arr_3, arr_4, 1e-9, false));
  EXPECT_FALSE(JsonWrapper::AlmostEqual(arr_3, arr_5, 1e-9, true));

  // more object tests below
  Json::Value obj_1(Json::objectValue), obj_2(Json::objectValue);
  obj_1["1"] = int_1;
  obj_2["1"] = int_2;
  EXPECT_TRUE(JsonWrapper::AlmostEqual(obj_1, obj_1, 1e-9));
  EXPECT_FALSE(JsonWrapper::AlmostEqual(obj_1, obj_2, 1e-9));

  Json::Value obj_3(Json::objectValue), obj_4(Json::objectValue);
  obj_3["1"] = int_1;
  obj_4["1"] = uint_1;
  EXPECT_TRUE(JsonWrapper::AlmostEqual(obj_3, obj_4, 1e-9, true));
  EXPECT_FALSE(JsonWrapper::AlmostEqual(obj_3, obj_4, 1e-9, false));
}

TEST(JsonWrapperTest, ArrayEqualTest) {
  Json::Value int_1(1), int_2(2);
  Json::Value arr_1(Json::arrayValue), arr_2(Json::arrayValue);
  arr_1.append(int_1);
  arr_1.append(int_2);
  arr_1.append(int_1);
  arr_2.append(int_1);
  arr_2.append(int_2);
  EXPECT_TRUE(JsonWrapper::AlmostEqual(arr_1, arr_1, 1e-9));
  EXPECT_FALSE(JsonWrapper::AlmostEqual(arr_1, arr_2, 1e-9));  // test size
  arr_2.append(int_2);
  EXPECT_FALSE(JsonWrapper::AlmostEqual(arr_1, arr_2, 1e-9));  // test value
  arr_1.append(arr_2);
  arr_1.append(int_1);
  arr_1.append(arr_1);
  EXPECT_TRUE(JsonWrapper::AlmostEqual(arr_1, arr_1, 1e-9));  // test recursion
  arr_2 = arr_1;
  arr_2[5][3].append(int_2);
  EXPECT_FALSE(JsonWrapper::AlmostEqual(arr_1, arr_2, 1e-9));  // test recursion
}

TEST(JsonWrapperTest, ObjectEqualTest) {
  Json::Value int_1(1), int_2(2);
  Json::Value obj_1(Json::objectValue), obj_2(Json::objectValue);
  obj_1["1"] = int_1;
  obj_1["2"] = int_2;
  obj_1["3"] = int_1;
  obj_2["1"] = int_1;
  obj_2["2"] = int_2;
  EXPECT_TRUE(JsonWrapper::AlmostEqual(obj_1, obj_1, 1e-9));
  EXPECT_FALSE(JsonWrapper::AlmostEqual(obj_1, obj_2, 1e-9));  // test size
  obj_1["3"] = int_2;
  EXPECT_FALSE(JsonWrapper::AlmostEqual(obj_1, obj_2, 1e-9));  // test value

  obj_1["branch"] = obj_1;
  obj_1["top branch"] = obj_1;
  EXPECT_TRUE(JsonWrapper::AlmostEqual(obj_1, obj_1, 1e-9));

  obj_2 = obj_1;
  obj_2["top branch"]["branch"]["3"] = int_1;
  EXPECT_FALSE(JsonWrapper::AlmostEqual(obj_1, obj_2, 1e-9)) << obj_1 << "\n\n"
                                                          << obj_2 << std::endl;
}

TEST(JsonWrapperTest, ObjectMergeTest) {
  Json::Value int_1(1);
  Json::Value object_1(Json::objectValue), object_2(Json::objectValue);
  Json::Value array_1(Json::arrayValue);
  array_1.append(Json::Value("array_value"));
  object_1["string"] = "string_value";
  object_1["array"] = array_1;
  object_2["string_alt"] = "string_value_alt";
  object_2["array"] = array_1;
  object_1["array_alt_1"] = array_1;
  object_2["array_alt_2"] = array_1;
  Json::Value object_merged = JsonWrapper::ObjectMerge(object_1, object_2);
  // check that merge was correct
  EXPECT_EQ(object_merged["string"], Json::Value("string_value"));
  EXPECT_EQ(object_merged["string_alt"], Json::Value("string_value_alt"));
  EXPECT_EQ(object_merged["array_alt_1"].size(), size_t(1));
  EXPECT_EQ(object_merged["array_alt_2"].size(), size_t(1));
  EXPECT_EQ(object_merged["array"].size(), size_t(2));
  EXPECT_EQ(object_merged["string"], Json::Value("string_value"));
  // throw because common non-array properties
  EXPECT_THROW(JsonWrapper::ObjectMerge(object_1, object_1), Exceptions::Exception);
  // throw because non-objects
  EXPECT_THROW(JsonWrapper::ObjectMerge(int_1, object_1), Exceptions::Exception);
  EXPECT_THROW(JsonWrapper::ObjectMerge(object_1, int_1), Exceptions::Exception);
}

TEST(JsonWrapperTest, ArrayMergeTest) {
  Json::Value int_1(1);
  Json::Value array_1(Json::arrayValue), array_2(Json::arrayValue);
  array_1.append(1);
  array_1.append(2);
  array_2.append("bob");
  array_2.append(3);
  Json::Value array_merged = JsonWrapper::ArrayMerge(array_1, array_2);
  // check that the merge was okay
  EXPECT_EQ(array_merged[Json::UInt(0)], Json::Value(1));
  EXPECT_EQ(array_merged[1], Json::Value(2));
  EXPECT_EQ(array_merged[2], Json::Value("bob"));
  EXPECT_EQ(array_merged[3], Json::Value(3));
  // non-array in merge
  EXPECT_THROW(JsonWrapper::ArrayMerge(int_1, array_1), Exceptions::Exception);
  EXPECT_THROW(JsonWrapper::ArrayMerge(array_1, int_1), Exceptions::Exception);
}

TEST(JsonWrapperTest, JsonValueTypeToStringTest) {
  // check one works. Not sure I can really do much more
  EXPECT_EQ(JsonWrapper::ValueTypeToString(Json::nullValue), "nullValue");
}

TEST(JsonWrapperTest, PathHandlingTest) {
  Json::Value test("test_value");
  EXPECT_EQ(JsonWrapper::Path::GetPath(test), "");
  JsonWrapper::Path::SetPath(test, "#test");
  EXPECT_EQ(JsonWrapper::Path::GetPath(test), "#test");
  JsonWrapper::Path::AppendPath(test, "append");
  EXPECT_EQ(JsonWrapper::Path::GetPath(test), "#test/append");
  JsonWrapper::Path::AppendPath(test, 1);
  EXPECT_EQ(JsonWrapper::Path::GetPath(test), "#test/append/1");
  EXPECT_THROW(JsonWrapper::Path::AppendPath(test, "append/"), Exceptions::Exception);
  Json::Value test_2;
  JsonWrapper::Path::AppendPath(test_2, "append");
  EXPECT_EQ(JsonWrapper::Path::GetPath(test_2), "#append");
  
}

TEST(JsonWrapperTest, PathDereferenceTest) {
  Json::Value test(Json::objectValue);
  test["object_0"] = Json::Value(Json::arrayValue);
  test["object_0"][Json::Value::UInt(0)] = Json::Value("test_0");
  test["object_0"][Json::Value::UInt(1)] = Json::Value("test_1");
  test["object_1"] = Json::Value(Json::arrayValue);
  test["object_1"][Json::Value::UInt(0)] = Json::Value();
  test["object_1"][Json::Value::UInt(1)] = Json::Value("test_2");
  EXPECT_EQ(JsonWrapper::Path::DereferencePath(test, ""), test);
  EXPECT_EQ(JsonWrapper::Path::DereferencePath(test, "#object_0/0"),
            Json::Value("test_0"));
  EXPECT_EQ(JsonWrapper::Path::DereferencePath(test, "object_0/0"),
            Json::Value("test_0"));
  EXPECT_EQ(JsonWrapper::Path::DereferencePath(test, "#object_0/0/"),
            Json::Value("test_0"));
  EXPECT_EQ(JsonWrapper::Path::DereferencePath(test, "#object_0/1"),
            Json::Value("test_1"));
  EXPECT_EQ(JsonWrapper::Path::DereferencePath(test, "#object_1/0"),
            Json::Value());
  EXPECT_EQ(JsonWrapper::Path::DereferencePath(test, "#object_1/1"),
            Json::Value("test_2"));
  // not an object or array
  Json::Value not_branch(1);
  EXPECT_THROW(JsonWrapper::Path::DereferencePath(not_branch, "#1"), Exceptions::Exception);
  // branch does not exist
  EXPECT_THROW(JsonWrapper::Path::DereferencePath(test, "#no_branch"), Exceptions::Exception);
  // array element does not exist
  EXPECT_THROW(JsonWrapper::Path::DereferencePath(test, "#object_0/2"), Exceptions::Exception);
}

TEST(JsonWrapperTest, SetPathRecursiveTest) {
  Json::Value test(Json::objectValue);
  test["object_0"] = Json::Value(Json::objectValue);
  test["object_0"]["a"] = Json::Value("test_0");
  test["object_0"]["b"] = Json::Value("test_1");
  test["object_1"] = Json::Value(Json::arrayValue);
  test["object_1"][Json::Value::UInt(0)] = Json::Value();
  test["object_1"][Json::Value::UInt(1)] = Json::Value("test_2");
  JsonWrapper::Path::SetPathRecursive(test, "");
  EXPECT_EQ(JsonWrapper::Path::GetPath(test), "");
  EXPECT_EQ(JsonWrapper::Path::GetPath(test["object_0"]), "#object_0");
  EXPECT_EQ(JsonWrapper::Path::GetPath(test["object_0"]["a"]), "#object_0/a");
  EXPECT_EQ(JsonWrapper::Path::GetPath(test["object_0"]["b"]), "#object_0/b");
  EXPECT_EQ(JsonWrapper::Path::GetPath(test["object_1"][0u]), "#object_1/0");
  EXPECT_EQ(JsonWrapper::Path::GetPath(test["object_1"][1]), "#object_1/1");

  JsonWrapper::Path::StripPathRecursive(test);

  EXPECT_EQ(test.getComment(Json::commentAfter), "");
  EXPECT_EQ(test["object_0"].getComment(Json::commentAfter), "");
  EXPECT_EQ(test["object_0"]["a"].getComment(Json::commentAfter), "");
  EXPECT_EQ(test["object_0"]["b"].getComment(Json::commentAfter), "");
  EXPECT_EQ(test["object_1"].getComment(Json::commentAfter), "");
  EXPECT_EQ(test["object_1"][0u].getComment(Json::commentAfter), "");
  EXPECT_EQ(test["object_1"][1].getComment(Json::commentAfter), "");
}
}

