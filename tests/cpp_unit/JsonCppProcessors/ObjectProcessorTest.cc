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

    not_req_proc.RegisterValueBranch("branch_a", &double_proc,
                                   &TestObject::GetA, &TestObject::SetA, false);
    not_req_proc.RegisterPointerBranch("branch_b", &double_proc,
                                   &TestObject::GetB, &TestObject::SetB, false);
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
  EXPECT_THROW(not_req_proc.HasUnknownBranches(json_int), Squeal);

  Json::Value json_object_1;
  json_object_1["branch_a"] = Json::Value(1.);
  EXPECT_FALSE(not_req_proc.HasUnknownBranches(json_object_1));

  json_object_1["branch_c"] = Json::Value(3.);
  EXPECT_TRUE(not_req_proc.HasUnknownBranches(json_object_1));

  EXPECT_THROW(not_req_proc.JsonToCpp(json_object_1), Squeal);
  not_req_proc.SetThrowsIfUnknownBranches(false);
  TestObject* cpp_object = NULL;
  EXPECT_NO_THROW(cpp_object = not_req_proc.JsonToCpp(json_object_1));
  if (cpp_object != NULL)
    delete cpp_object;
}


TEST_F(ObjectProcessorTest, JsonToCppRequiredTest) {
  Json::Value json_int(1);
  // should throw if this is not an object at all
  EXPECT_THROW(req_proc.JsonToCpp(json_int), Squeal);

  Json::Value json_object(Json::objectValue);
  json_object["branch_a"] = Json::Value(1.);
  json_object["branch_b"] = Json::Value(2.);

  // should handle processing correctly
  TestObject* cpp_object = req_proc.JsonToCpp(json_object);
  EXPECT_DOUBLE_EQ(cpp_object->GetA(), 1.);
  EXPECT_DOUBLE_EQ(*cpp_object->GetB(), 2.);
  delete cpp_object;

  // should throw on NULL
  json_object["branch_a"] = Json::Value(1.);
  json_object["branch_b"] = Json::Value();
  EXPECT_THROW(req_proc.JsonToCpp(json_object), Squeal);

  // should throw if object is missing
  Json::Value json_object_missing_a(Json::objectValue);
  json_object_missing_a["branch_b"] = Json::Value(2.);
  EXPECT_THROW(req_proc.JsonToCpp(json_object_missing_a), Squeal);

  Json::Value json_object_missing_b(Json::objectValue);
  json_object_missing_b["branch_a"] = Json::Value(2.);
  EXPECT_THROW(req_proc.JsonToCpp(json_object_missing_b), Squeal);

  // should throw if object is present but can't be processed
  json_object["branch_b"] = Json::Value("string");
  EXPECT_THROW(req_proc.JsonToCpp(json_object), Squeal);

  json_object["branch_b"] = Json::Value(2.);
  json_object["branch_a"] = Json::Value("string");
  EXPECT_THROW(req_proc.JsonToCpp(json_object), Squeal);
}

TEST_F(ObjectProcessorTest, JsonToCppNotRequiredTest) {
  Json::Value json_object(Json::objectValue);
  json_object["branch_a"] = Json::Value(1.);
  json_object["branch_b"] = Json::Value(2.);

  // should handle processing correctly
  TestObject* cpp_object = not_req_proc.JsonToCpp(json_object);
  EXPECT_DOUBLE_EQ(cpp_object->GetA(), 1.);
  EXPECT_DOUBLE_EQ(*cpp_object->GetB(), 2.);
  delete cpp_object;

  // should not throw if object is missing
  Json::Value json_object_missing_a(Json::objectValue);
  json_object_missing_a["branch_b"] = Json::Value(2.);
  cpp_object = not_req_proc.JsonToCpp(json_object_missing_a);
  delete cpp_object;

  Json::Value json_object_missing_b(Json::objectValue);
  json_object_missing_b["branch_a"] = Json::Value(2.);
  cpp_object = not_req_proc.JsonToCpp(json_object_missing_b);
  delete cpp_object;

  // should throw if object is present but can't be processed
  json_object["branch_b"] = Json::Value("string");
  EXPECT_THROW(req_proc.JsonToCpp(json_object), Squeal);

  json_object["branch_b"] = Json::Value(2.);
  json_object["branch_a"] = Json::Value("string");
  EXPECT_THROW(not_req_proc.JsonToCpp(json_object), Squeal);

  // should return NULL on nullValue
  double* null_value = NULL;
  json_object["branch_a"] = Json::Value(1.);
  json_object["branch_b"] = Json::Value();
  cpp_object = not_req_proc.JsonToCpp(json_object);
  EXPECT_EQ(not_req_proc.JsonToCpp(json_object)->GetB(), null_value);
  delete cpp_object;
}

TEST_F(ObjectProcessorTest, CppToJsonRequiredTest) {
  // Check allocates values okay
  Json::Value* json_value = req_proc.CppToJson(test);
  EXPECT_EQ((*json_value)["branch_a"], test.GetA());
  EXPECT_EQ((*json_value)["branch_b"], *(test.GetB()));
  delete json_value;

  // Check throw on NULL
  test.SetB(NULL);
  EXPECT_THROW(req_proc.CppToJson(test), Squeal);
}

TEST_F(ObjectProcessorTest, CppToJsonNotRequiredTest) {
  // Check allocates values okay
  Json::Value* json_value = not_req_proc.CppToJson(test);
  EXPECT_EQ((*json_value)["branch_a"].asDouble(), test.GetA());
  EXPECT_EQ((*json_value)["branch_b"].asDouble(), *(test.GetB()));
  delete json_value;

  // Check that branch_b is not allocated
  test.SetB(NULL);
  json_value = not_req_proc.CppToJson(test);
  EXPECT_FALSE(json_value->isMember("branch_b"));
  delete json_value;
}
}

