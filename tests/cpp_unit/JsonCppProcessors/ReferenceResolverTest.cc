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

#include "src/common_cpp/JsonCppProcessors/Common/ReferenceResolver.hh"

namespace MAUS {
namespace ReferenceResolver {

TEST(ReferenceResolverTest, TypedCppToJsonResolver) {
    double child(0);
    Json::Value parent(Json::objectValue);
    parent["test"] = Json::Value();
    TypedCppToJsonResolver<double> res(&child, "#test");
    TypedCppToJsonResolver<double>::AddData(&child, "PATH TO BRANCH");
    res.ResolveReferences(parent);
     
    Json::Value test(Json::objectValue);
    test["$ref"] = Json::Value("PATH TO BRANCH");
    // the actual test...
    EXPECT_EQ(parent["test"], test);
    res.ClearData();
    parent["test"] = Json::Value("bob");
    res.ResolveReferences(parent);
    EXPECT_EQ(parent["test"], Json::Value()) << parent["test"];
}

class TestClass {
  public:
    void SetData(double* data) {_data = data;} // not memory safe
    double * _data;
};

TEST(ReferenceResolverTest, JsonToCppResolver) {
    double child(99);
    TestClass test;
    FullyTypedJsonToCppResolver<TestClass, double> res("#data", &TestClass::SetData, &test);
    ChildTypedJsonToCppResolver<double>::AddData("#data", &child);
    res.ResolveReferences();
    EXPECT_TRUE(test._data == &child);
    res.ClearData();
    res.ResolveReferences();
    EXPECT_TRUE(test._data == NULL);
}

TEST(ReferenceResolverTest, CppToJsonManager) {
    delete &CppToJsonManager::GetInstance();
    CppToJsonManager* man = new CppToJsonManager();
    Json::Value test(Json::objectValue);
    test["$ref"] = Json::Value("PATH TO BRANCH");

    double child(0);
    Json::Value parent(Json::objectValue);
    parent["test_1"] = Json::Value(1);
    parent["test_2"] = Json::Value(2);
    TypedCppToJsonResolver<double>* res =
              new TypedCppToJsonResolver<double>(&child, "#test_1");
    res->AddData(&child, "PATH TO BRANCH");
    man->AddReference(res);
    man->ResolveReferences(parent);
    EXPECT_EQ(parent["test_1"], test);
    delete man; // res should now be deleted;

    parent["test_1"] = Json::Value(1); // reset
    man = &CppToJsonManager::GetInstance();
    man->ResolveReferences(parent);
    // we don't add the reference this time; should be cleared by delete so
    // ResolveReferences should fail (no change to branch)
    EXPECT_EQ(parent["test_1"], Json::Value(1));
    // this resolver should do nothing (should be deleted immediately)
    res = new TypedCppToJsonResolver<double>(&child, "#test_1");
    delete man;

    man = &CppToJsonManager::GetInstance();
    res = new TypedCppToJsonResolver<double>(&child, "#test_2");
    res->AddData(&child, "PATH TO BRANCH");
    man->AddReference(res);
    man->ResolveReferences(parent);
    // we added the reference this time; so should fill the data okay - but the
    // manager should have been deleted so test_2 branch not filled
    EXPECT_EQ(parent["test_1"], Json::Value(1));
    EXPECT_EQ(parent["test_2"], test);
    delete man;
}

TEST(ReferenceResolverTest, JsonToCppManager) {
    delete &JsonToCppManager::GetInstance();
    JsonToCppManager* man = new JsonToCppManager();
    double child(99);
    TestClass test;
    FullyTypedJsonToCppResolver<TestClass, double>* res = new 
                                  FullyTypedJsonToCppResolver<TestClass, double>
                                          ("#data", &TestClass::SetData, &test);
    man->AddReference(res);
    ChildTypedJsonToCppResolver<double>::AddData("#data", &child);
    man->ResolveReferences();
    EXPECT_TRUE(test._data == &child);
    delete man;  // res should now be deleted

    man = &JsonToCppManager::GetInstance();
    res = new FullyTypedJsonToCppResolver<TestClass, double>
                                          ("#data", &TestClass::SetData, &test);
    man->AddReference(res);
    man->ResolveReferences();
    EXPECT_TRUE(test._data == NULL); // we never called AddData - should fail
    delete man; // _instance should now be NULL;

    res = new FullyTypedJsonToCppResolver<TestClass, double>
                                          ("#data", &TestClass::SetData, &test);
    man = &JsonToCppManager::GetInstance();
    man->AddReference(res);
    ChildTypedJsonToCppResolver<double>::AddData("#data", &child);
    man->ResolveReferences();
    EXPECT_TRUE(test._data == &child);
    delete man;
    
}
}
}

