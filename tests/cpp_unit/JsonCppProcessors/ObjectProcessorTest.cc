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

#include "src/common_cpp/JsonCppProcessors/ObjectProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/PrimitivesProcessors.hh"

namespace  MAUS {

class TestObject {
  public:
    TestObject() : _a(0.), _b(NULL) {}
    ~TestObject() {
      if (_b != NULL) {
        delete _b;
      }
    }

    double GetA() const {return _a;}
    void SetA(double data) {
      _a = data;
    }

    double* GetB() const {return _b;}
    void SetB(double* data) {
      if (_b != NULL) {
        delete _b;
      }
      _b = data;
    }

  private:
    double _a;
    double* _b;
};

class ObjectProcessorTest : public ::testing::Test {
 protected:
  ObjectProcessorTest() {
    req_proc.RegisterValueBranch("branch_a", &double_proc,
                        &MAUS::TestObject::GetA, &MAUS::TestObject::SetA, true);
    req_proc.RegisterPointerBranch("branch_b", &double_proc, &TestObject::GetB,
                                                      &TestObject::SetB, true);
    req_proc.RegisterConstantBranch("branch_c", Json::Value("const"), true);
    req_proc.RegisterIgnoredBranch("branch_d", true);

    not_req_proc.RegisterValueBranch("branch_a", &double_proc,
                                   &TestObject::GetA, &TestObject::SetA, false);
    not_req_proc.RegisterPointerBranch("branch_b", &double_proc,
                                   &TestObject::GetB, &TestObject::SetB, false);
    not_req_proc.RegisterConstantBranch("branch_c", Json::Value("const"), false);
    not_req_proc.RegisterIgnoredBranch("branch_d", false);
    test.SetA(1.);
    test.SetB(new double(2.));
  }

  MAUS::ObjectProcessor<TestObject> not_req_proc;
  MAUS::ObjectProcessor<TestObject> req_proc;
  MAUS::DoubleProcessor double_proc;
  TestObject test;
};


TEST_F(ObjectProcessorTest, HasUnknownBranches) {
  EXPECT_TRUE(not_req_proc.GetThrowsIfUnknownBranches());
  EXPECT_TRUE(req_proc.GetThrowsIfUnknownBranches());
  req_proc.SetThrowsIfUnknownBranches(false);
  EXPECT_FALSE(req_proc.GetThrowsIfUnknownBranches());
  req_proc.SetThrowsIfUnknownBranches(true);
  EXPECT_TRUE(req_proc.GetThrowsIfUnknownBranches());

  Json::Value json_int = Json::Value(1.);
  EXPECT_THROW(not_req_proc.HasUnknownBranches(json_int), Exceptions::Exception);

  Json::Value json_object_1;
  json_object_1["branch_a"] = Json::Value(1.);
  EXPECT_FALSE(not_req_proc.HasUnknownBranches(json_object_1));

  json_object_1["branch_u"] = Json::Value(3.);
  EXPECT_TRUE(not_req_proc.HasUnknownBranches(json_object_1));

  EXPECT_THROW(not_req_proc.JsonToCpp(json_object_1), Exceptions::Exception);
  not_req_proc.SetThrowsIfUnknownBranches(false);
  TestObject* cpp_object = NULL;
  EXPECT_NO_THROW(cpp_object = not_req_proc.JsonToCpp(json_object_1));
  if (cpp_object != NULL)
    delete cpp_object;
}


TEST_F(ObjectProcessorTest, JsonToCppRequiredTest) {
  Json::Value json_int(1);
  // should throw if this is not an object at all
  EXPECT_THROW(req_proc.JsonToCpp(json_int), Exceptions::Exception);

  Json::Value json_object(Json::objectValue);
  json_object["branch_a"] = Json::Value(1.);
  json_object["branch_b"] = Json::Value(2.);
  json_object["branch_c"] = Json::Value("const");
  json_object["branch_d"] = Json::Value(1.); // could be int, string or anything

  // should handle processing correctly
  TestObject* cpp_object = req_proc.JsonToCpp(json_object);
  EXPECT_DOUBLE_EQ(cpp_object->GetA(), 1.);
  EXPECT_DOUBLE_EQ(*cpp_object->GetB(), 2.);
  delete cpp_object;

  // should throw on NULL
  json_object["branch_a"] = Json::Value(1.);
  json_object["branch_b"] = Json::Value();
  json_object["branch_c"] = Json::Value("const");
  json_object["branch_d"] = Json::Value("const");
  EXPECT_THROW(req_proc.JsonToCpp(json_object), Exceptions::Exception);

  // should throw if object is missing
  Json::Value json_object_missing_a(Json::objectValue);
  json_object_missing_a["branch_b"] = Json::Value(2.);
  json_object_missing_a["branch_c"] = Json::Value("const");
  json_object_missing_a["branch_d"] = Json::Value("const");
  EXPECT_THROW(req_proc.JsonToCpp(json_object_missing_a), Exceptions::Exception);

  Json::Value json_object_missing_b(Json::objectValue);
  json_object_missing_b["branch_a"] = Json::Value(2.);
  json_object_missing_b["branch_c"] = Json::Value("const");
  json_object_missing_b["branch_d"] = Json::Value("const");
  EXPECT_THROW(req_proc.JsonToCpp(json_object_missing_b), Exceptions::Exception);

  Json::Value json_object_missing_c(Json::objectValue);
  json_object_missing_c["branch_a"] = Json::Value(2.);
  json_object_missing_c["branch_b"] = Json::Value(1.);
  json_object_missing_c["branch_d"] = Json::Value("const");
  EXPECT_THROW(req_proc.JsonToCpp(json_object_missing_c), Exceptions::Exception);

  Json::Value json_object_missing_d(Json::objectValue);
  json_object_missing_d["branch_a"] = Json::Value(1.);
  json_object_missing_d["branch_b"] = Json::Value();
  json_object_missing_d["branch_c"] = Json::Value("const");
  EXPECT_THROW(req_proc.JsonToCpp(json_object_missing_d), Exceptions::Exception);

  // should throw if object is present but can't be processed
  json_object["branch_b"] = Json::Value("string");
  EXPECT_THROW(req_proc.JsonToCpp(json_object), Exceptions::Exception);

  json_object["branch_b"] = Json::Value(2.);
  json_object["branch_a"] = Json::Value("string");
  EXPECT_THROW(req_proc.JsonToCpp(json_object), Exceptions::Exception);

  json_object["branch_c"] = Json::Value("not_const");
  json_object["branch_a"] = Json::Value(2.);
  EXPECT_THROW(req_proc.JsonToCpp(json_object), Exceptions::Exception);
}

TEST_F(ObjectProcessorTest, JsonToCppNotRequiredTest) {
  Json::Value json_object(Json::objectValue);
  json_object["branch_a"] = Json::Value(1.);
  json_object["branch_b"] = Json::Value(2.);
  json_object["branch_c"] = Json::Value("const");
  json_object["branch_d"] = Json::Value("const");

  // should handle processing correctly
  TestObject* cpp_object = not_req_proc.JsonToCpp(json_object);
  EXPECT_DOUBLE_EQ(cpp_object->GetA(), 1.);
  EXPECT_DOUBLE_EQ(*cpp_object->GetB(), 2.);
  delete cpp_object;

  // should not throw if object is missing
  Json::Value json_object_missing_all(Json::objectValue);
  cpp_object = not_req_proc.JsonToCpp(json_object_missing_all);
  delete cpp_object;

  // should throw if object is present but can't be processed
  json_object["branch_b"] = Json::Value("string");
  EXPECT_THROW(req_proc.JsonToCpp(json_object), Exceptions::Exception);

  json_object["branch_b"] = Json::Value(2.);
  json_object["branch_a"] = Json::Value("string");
  EXPECT_THROW(not_req_proc.JsonToCpp(json_object), Exceptions::Exception);

  json_object["branch_c"] = Json::Value("not_const");
  json_object["branch_a"] = Json::Value(1.);
  EXPECT_THROW(not_req_proc.JsonToCpp(json_object), Exceptions::Exception);

  // should return NULL on nullValue
  double* null_value = NULL;
  json_object["branch_a"] = Json::Value(1.);
  json_object["branch_b"] = Json::Value();
  json_object["branch_c"] = Json::Value("const");
  cpp_object = not_req_proc.JsonToCpp(json_object);
  EXPECT_EQ(not_req_proc.JsonToCpp(json_object)->GetB(), null_value);
  delete cpp_object;
}

TEST_F(ObjectProcessorTest, CppToJsonRequiredTest) {
  // Check allocates values okay
  Json::Value* json_value = req_proc.CppToJson(test, "");
  EXPECT_EQ((*json_value)["branch_a"], test.GetA());
  EXPECT_EQ((*json_value)["branch_b"], *(test.GetB()));
  EXPECT_EQ((*json_value)["branch_c"], Json::Value("const"));
  delete json_value;

  // Check throw on NULL
  test.SetB(NULL);
  EXPECT_THROW(req_proc.CppToJson(test, ""), Exceptions::Exception);
}

TEST_F(ObjectProcessorTest, CppToJsonNotRequiredTest) {
  // Check allocates values okay
  Json::Value* json_value = not_req_proc.CppToJson(test, "");
  EXPECT_EQ((*json_value)["branch_a"].asDouble(), test.GetA());
  EXPECT_EQ((*json_value)["branch_b"].asDouble(), *(test.GetB()));
  EXPECT_EQ((*json_value)["branch_c"], Json::Value("const"));
  delete json_value;

  // Check that branch_b is not allocated
  test.SetB(NULL);
  json_value = not_req_proc.CppToJson(test, "");
  EXPECT_FALSE(json_value->isMember("branch_b"));
  delete json_value;
}

TEST_F(ObjectProcessorTest, PathTest) {
  Json::Value* json_value = req_proc.CppToJson(test, "#path");
  EXPECT_EQ(JsonWrapper::Path::GetPath((*json_value)), "#path");
  EXPECT_EQ(JsonWrapper::Path::GetPath((*json_value)["branch_a"]),
            "#path/branch_a");
  EXPECT_EQ(JsonWrapper::Path::GetPath((*json_value)["branch_b"]),
            "#path/branch_b");
  EXPECT_EQ(JsonWrapper::Path::GetPath((*json_value)["branch_c"]),
            "#path/branch_c");
}
}

