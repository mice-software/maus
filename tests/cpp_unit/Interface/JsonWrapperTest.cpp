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

#include "src/cpp_core/Utils/JsonWrapper.hh"

TEST(JsonWrapper, StringToJson) {
  std::string good_json = "{}";
  Json::Value val = JsonWrapper::StringToJson(good_json);
  EXPECT_EQ(val, Json::Value(Json::objectValue));
  std::string bad_json = "{";
  EXPECT_THROW(JsonWrapper::StringToJson(bad_json), Squeal);
}

TEST(JsonWrapper, GetItemTest) {
  std::string good_json = "[2, 3, 4]";
  Json::Value good_val = JsonWrapper::StringToJson(good_json);
  for (unsigned int i = 0; i < 2; ++i) {
    EXPECT_EQ(Json::Value(static_cast<int>(i+2)),
      JsonWrapper::GetItem(good_val, i, JsonWrapper::intValue));
    EXPECT_EQ(Json::Value(static_cast<int>(i+2)),
      JsonWrapper::GetItem(good_val, i, JsonWrapper::anyValue));
    EXPECT_THROW( // bad type
     JsonWrapper::GetItem(good_val, i, JsonWrapper::stringValue), Squeal);
  }
  Json::Value empty_array =  JsonWrapper::StringToJson("[]");
  EXPECT_THROW( // empty value
    JsonWrapper::GetItem(empty_array, 0, JsonWrapper::anyValue), Squeal);
  EXPECT_THROW( // out of range item
    JsonWrapper::GetItem(good_val, 4, JsonWrapper::anyValue), Squeal);

}

TEST(JsonWrapper, GetPropertyTest) {
  Json::Value test;
  std::string good_json =
      std::string("{\"real\":2.5, \"int\":3, \"null\":null, \"bool\":true, ")+
      std::string("\"object\":{}, \"array\":[]}");
  std::string gets [] = {"real", "int", "null", "bool", "object", "array"};
  JsonWrapper::JsonType types[] =
   {JsonWrapper::realValue, JsonWrapper::intValue, JsonWrapper::nullValue,
    JsonWrapper::booleanValue, JsonWrapper::objectValue,
    JsonWrapper::arrayValue};
  Json::Value good_val = JsonWrapper::StringToJson(good_json);
  for (unsigned int i = 0; i < 5; ++i)
    for (unsigned int j = 0; j < 5; ++j) {
      if ( i != j )
      EXPECT_THROW(
           JsonWrapper::GetProperty(good_val, gets[i], types[j]), Squeal)
        << i << " " << j << " " << gets[i];
     }
  EXPECT_EQ(Json::Value(2.5), 
                   JsonWrapper::GetProperty(good_val, gets[0], types[0]));
  EXPECT_EQ(Json::Value(3), 
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
  EXPECT_THROW( // non-existent property
    JsonWrapper::GetProperty(emptyProp, "a", JsonWrapper::anyValue), Squeal);
}

TEST(JsonWrapper, TypeConversionTest) {
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
  EXPECT_THROW(JsonWrapper::JsonTypeToValueType(JsonWrapper::anyValue), Squeal);  
}

TEST(JsonWrapper, SimilarTypeTest) {
   JsonWrapper::JsonType wr_tp[] = {JsonWrapper::nullValue, 
     JsonWrapper::uintValue, JsonWrapper::intValue, JsonWrapper::realValue, 
     JsonWrapper::stringValue, JsonWrapper::booleanValue, 
     JsonWrapper::arrayValue, JsonWrapper::objectValue, JsonWrapper::anyValue};
  for (size_t i = 0; i < 9; ++i)
    for (size_t j = 0; j < 9; ++j) {
      EXPECT_EQ(JsonWrapper::SimilarType(wr_tp[i], wr_tp[j]), 
                i == j || i == 8 || j == 8 );
    }
}

