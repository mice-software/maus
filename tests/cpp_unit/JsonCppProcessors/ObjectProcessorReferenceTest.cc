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

namespace MAUS {

class TestObject_1;

class TestObject_2 {
  public:
    TestObject_2() : _d(NULL) {}
    ~TestObject_2();

    TestObject_1* GetD() const {return _d;}
    void SetD(TestObject_1* data);

  private:
    TestObject_1* _d;
};

class TestObject_1 {
  public:
    TestObject_1() : _a(NULL), _b(NULL), _c(NULL) {}
    ~TestObject_1() {
      if (_a != NULL)
        delete _a;
      if (_b != NULL)
        delete _b;
      if (_c != NULL)
        delete _c;
    }

    TestObject_2* GetA() const {return _a;}
    void SetA(TestObject_2* data) {
      _a = data;
    }

    TestObject_2* GetB() const {return _b;}
    void SetB(TestObject_2* data) {
      if (_b != NULL) {
        delete _b;
      }
      _b = data;
    }

    TestObject_2* GetC() const {return _c;}
    void SetC(TestObject_2* data) {
      if (_c != NULL) {
        delete _c;
      }
      _c = data;
    }

  private:
    TestObject_2* _a;
    TestObject_2* _b;
    TestObject_2* _c;
};

TestObject_2::~TestObject_2() {
  if (_d != NULL)
    delete _d;
}

void TestObject_2::SetD(TestObject_1* data) {
  if (_d != NULL) {
    delete _d;
  }
  _d = data;
}


class TestObject_4 {
  public:
    TestObject_4() : _reference(NULL) {}
    ~TestObject_4() {}

    double* GetReference() const {return _reference;}
    void SetReference(double* data) {
        _reference = data; // Note: don't own this memory
    }

  private:
    double* _reference;
};

class TestObject_3 {
  public:
    TestObject_3() : _pointer(NULL), _child_1(NULL), _child_2(NULL) {}
    ~TestObject_3() {
        if (_pointer != NULL) delete _pointer;
        if (_child_1 != NULL) delete _child_1;
        if (_child_2 != NULL) delete _child_2;
    }

    double* GetPointer() const {return _pointer;}
    void SetPointer(double* data) {
        if (_pointer != NULL) {
          delete _pointer;
        }
        _pointer = data;
    }

    TestObject_4* GetChild1() const {return _child_1;}
    void SetChild1(TestObject_4* data) {
        if (_child_1 != NULL) {
          delete _child_1;
        }
        _child_1 = data;
    }

    TestObject_4* GetChild2() const {return _child_2;}
    void SetChild2(TestObject_4* data) {
        if (_child_2 != NULL) {
          delete _child_2;
        }
        _child_2 = data;
    }

  private:
    double* _pointer;
    TestObject_4* _child_1;
    TestObject_4* _child_2;
};

TEST(ObjectProcessorReferenceTest, SimpleTreeTest) {
  ReferenceResolver::JsonToCpp::RefManager::Birth();
  ReferenceResolver::CppToJson::RefManager::Birth();
  // setup C++ tree
  TestObject_3 test_3;
  test_3.SetPointer(new double(1));
  test_3.SetChild1(new TestObject_4());
  test_3.GetChild1()->SetReference(test_3.GetPointer());
  test_3.SetChild2(new TestObject_4());
  test_3.GetChild2()->SetReference(test_3.GetPointer());

  // setup processors
  DoubleProcessor double_proc;
  ObjectProcessor<TestObject_4> test_4_proc;
  test_4_proc.RegisterPointerReference("reference", &double_proc,
                &TestObject_4::GetReference, &TestObject_4::SetReference, true);
  ObjectProcessor<TestObject_3> test_3_proc;
  test_3_proc.RegisterPointerBranch("child_1", &test_4_proc,
                &TestObject_3::GetChild1, &TestObject_3::SetChild1, true);
  test_3_proc.RegisterPointerBranch("child_2", &test_4_proc,
                &TestObject_3::GetChild2, &TestObject_3::SetChild2, true);
  test_3_proc.RegisterPointerBranch("pointer", &double_proc,
                &TestObject_3::GetPointer, &TestObject_3::SetPointer, true);
  
  // test C++ to Json conversion
  Json::Value* test_3_json = test_3_proc.CppToJson(test_3);
  ReferenceResolver::CppToJson::RefManager::
                                 GetInstance().ResolveReferences(*test_3_json);
  EXPECT_EQ((*test_3_json)["child_1"]["reference"]["$ref"],
            Json::Value("#pointer"));
  EXPECT_EQ((*test_3_json)["child_2"]["reference"]["$ref"],
            Json::Value("#pointer"));

  // check Json to C++ conversion
  TestObject_3* test_3_out = test_3_proc.JsonToCpp(*test_3_json);
  ReferenceResolver::JsonToCpp::RefManager::GetInstance().ResolveReferences();
  EXPECT_EQ(test_3_out->GetChild1()->GetReference(),
            test_3_out->GetPointer());
  EXPECT_EQ(test_3_out->GetChild2()->GetReference(),
            test_3_out->GetPointer());
  EXPECT_EQ(*(test_3_out->GetPointer()), *(test_3.GetPointer())); 

  delete test_3_json;
  delete test_3_out;
  ReferenceResolver::CppToJson::RefManager::Death();
  ReferenceResolver::JsonToCpp::RefManager::Death();
}

TEST(ObjectProcessorReferenceTest, RequiredTest) {
  DoubleProcessor double_proc;
  ObjectProcessor<TestObject_4> test_4_req_proc;
  test_4_req_proc.RegisterPointerReference("reference", &double_proc,
               &TestObject_4::GetReference, &TestObject_4::SetReference, true);
  ObjectProcessor<TestObject_4> test_4_not_proc;
  test_4_not_proc.RegisterPointerReference("reference", &double_proc,
               &TestObject_4::GetReference, &TestObject_4::SetReference, false);

  TestObject_4 test_4_cpp;
  test_4_cpp.SetReference(NULL);
  EXPECT_THROW(test_4_req_proc.CppToJson(test_4_cpp), Squeal);
  delete test_4_not_proc.CppToJson(test_4_cpp);  // shouldnt throw

  Json::Value test_4_json(Json::objectValue);
  EXPECT_THROW(test_4_req_proc.JsonToCpp(test_4_json), Squeal);
  delete test_4_not_proc.JsonToCpp(test_4_json);  // shouldnt throw

  test_4_json["reference"] = Json::Value();
  EXPECT_THROW(test_4_req_proc.JsonToCpp(test_4_json), Squeal);
  delete test_4_not_proc.JsonToCpp(test_4_json);  // shouldnt throw

  test_4_json["reference"]["$ref"] = Json::Value();
  EXPECT_THROW(test_4_req_proc.JsonToCpp(test_4_json), Squeal);
  delete test_4_not_proc.JsonToCpp(test_4_json);  // shouldnt throw
}

MAUS::ObjectProcessor<TestObject_2> GetTest2Proc
                           (MAUS::ObjectProcessor<TestObject_1>* test_1_proc) {
  MAUS::ObjectProcessor<TestObject_2> test_2_proc;
  test_2_proc.RegisterPointerReference("d", test_1_proc, &TestObject_2::GetD,
                                                     &TestObject_2::SetD, true);
  return test_2_proc;
}

TEST(ObjectProcessorReferenceTest, CircularTest) {
  ASSERT_TRUE(false);
  TestObject_1 test_1;
  test_1.SetA(new TestObject_2());
  test_1.SetB(test_1.GetA());
  test_1.SetC(test_1.GetA());

  MAUS::ObjectProcessor<TestObject_1> test_1_proc;
  MAUS::ObjectProcessor<TestObject_2> test_2_proc = GetTest2Proc(&test_1_proc); 
  test_1_proc.RegisterPointerBranch("a", &test_2_proc, &TestObject_1::GetA,
                                                     &TestObject_1::SetA, true);
  test_1_proc.RegisterPointerBranch("b", &test_2_proc, &TestObject_1::GetB,
                                                     &TestObject_1::SetB, true);
  test_1_proc.RegisterPointerBranch("c", &test_2_proc, &TestObject_1::GetC,
                                                     &TestObject_1::SetC, true);

  Json::Value* test_1_json = test_1_proc.CppToJson(test_1);

  std::cerr << JsonWrapper::JsonToString(*test_1_json) << std::endl;
}

}

