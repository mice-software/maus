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
    void SetA(double data) {_a = data;}

    double* GetB() const {return _b;}
    void SetB(double* data) {_b = data;}

  private:
    double _a;
    double* _b;
};

class ObjectProcessorTest : public ::testing::Test {
 protected:
  ObjectProcessorTest() {
  req_proc.RegisterValueBranch("branch_a", &double_proc, &MAUS::TestObject::GetA,
                                                    &MAUS::TestObject::SetA, true);
  req_proc.RegisterPointerBranch("branch_b", &double_proc, &TestObject::GetB,
                                                    &TestObject::SetB, true);

  not_req_proc.RegisterValueBranch("branch_a", &double_proc, &TestObject::GetA,
                                                    &TestObject::SetA, false);
  not_req_proc.RegisterPointerBranch("branch_b", &double_proc, 
                                  &TestObject::GetB, &TestObject::SetB, false);

  }

  MAUS::ObjectProcessor<TestObject> not_req_proc;
  MAUS::ObjectProcessor<TestObject> req_proc;
  MAUS::DoubleProcessor double_proc;
};

TEST_F(ObjectProcessorTest, JsonToCppRequiredTest) {
  Json::Value json_object(Json::objectValue);
  json_object["branch_a"] = Json::Value(1.);
  json_object["branch_b"] = Json::Value(2.);

  TestObject* cpp_object = req_proc.JsonToCpp(json_object);
  EXPECT_DOUBLE_EQ(cpp_object->GetA(), 1.);
  EXPECT_DOUBLE_EQ(*cpp_object->GetB(), 2.);
  delete cpp_object;

  Json::Value json_object_missing_a(Json::objectValue);
  json_object_missing_a["branch_b"] = Json::Value(2.);
  EXPECT_THROW(req_proc.JsonToCpp(json_object_missing_a), Squeal);

  Json::Value json_object_missing_b(Json::objectValue);
  json_object_missing_b["branch_a"] = Json::Value(2.);
  EXPECT_THROW(req_proc.JsonToCpp(json_object_missing_b), Squeal);
}

}


