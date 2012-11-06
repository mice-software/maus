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
#include "src/legacy/Interface/Squeal.hh"

namespace MAUS {
namespace ReferenceResolver {

TEST(ReferenceResolverTest, TypedCppToJsonResolver) {
    double child(0);
    Json::Value parent(Json::objectValue);
    parent["test"] = Json::Value();
    CppToJson::TypedResolver<double> res(&child, "#test");
    CppToJson::TypedResolver<double>::AddData(&child, "PATH TO BRANCH");
    EXPECT_THROW(CppToJson::TypedResolver<double>::
                                     AddData(&child, "PATH TO BRANCH"), Squeal);
    res.ResolveReferences(parent);
     
    Json::Value test(Json::objectValue);
    test["$ref"] = Json::Value("PATH TO BRANCH");
    EXPECT_EQ(parent["test"], test);
    res.ClearData();
    parent["test"] = Json::Value("bob");
    EXPECT_THROW(res.ResolveReferences(parent), Squeal);
    EXPECT_EQ(parent["test"], Json::Value()) << parent["test"];
}

class TestClass {
  public:
    void SetData(double* data) {_data = data;} // not memory safe
    double * _data;
};

TEST(ReferenceResolverTest, JsonToCppResolver) {
    using namespace JsonToCpp;
    double child(99);
    TestClass test;
    FullyTypedResolver<TestClass, double> res
                                          ("#data", &TestClass::SetData, &test);
    ChildTypedResolver<double>::AddData("#data", &child);
    EXPECT_THROW(ChildTypedResolver<double>::AddData("#data", &child), Squeal);
    res.ResolveReferences();
    EXPECT_TRUE(test._data == &child);
    res.ClearData();
    EXPECT_THROW(res.ResolveReferences(), Squeal);
    EXPECT_TRUE(test._data == NULL);
}

TEST(ReferenceResolverTest, JsonToCppVectorResolver) {
    using namespace JsonToCpp;
    double child(99);
    std::vector<double*> vec;
    vec.push_back(NULL);
    VectorResolver<double> res("#data", vec, 0);
    ChildTypedResolver<double>::AddData("#data", &child);
    res.ResolveReferences();
    EXPECT_TRUE(vec[0] == &child);
    VectorResolver<double> res2("#data", vec, 1);
    EXPECT_THROW(res2.ResolveReferences(), Squeal);
    res.ClearData();
    EXPECT_THROW(res.ResolveReferences(), Squeal);
    EXPECT_TRUE(vec[0] == NULL);
}


TEST(ReferenceResolverTest, CppToJsonManagerBirth) {
    EXPECT_FALSE(CppToJson::RefManager::HasInstance());
    CppToJson::RefManager::Birth();
    EXPECT_TRUE(CppToJson::RefManager::HasInstance());
    EXPECT_THROW(CppToJson::RefManager::Birth(), Squeal);
    CppToJson::RefManager::Death();
    EXPECT_FALSE(CppToJson::RefManager::HasInstance());
    EXPECT_THROW(CppToJson::RefManager::Death(), Squeal);
    EXPECT_THROW(CppToJson::RefManager::GetInstance(), Squeal);
    CppToJson::RefManager::Birth();
    EXPECT_NO_THROW(CppToJson::RefManager::GetInstance());
    CppToJson::RefManager::Death();   
}

TEST(ReferenceResolverTest, CppToJsonManager) {
    CppToJson::RefManager* man = new CppToJson::RefManager();
    Json::Value test(Json::objectValue);
    test["$ref"] = Json::Value("PATH TO BRANCH");

    double child(0);
    Json::Value parent(Json::objectValue);
    parent["test_1"] = Json::Value(1);
    parent["test_2"] = Json::Value(2);
    CppToJson::TypedResolver<double>* res =
                        new CppToJson::TypedResolver<double>(&child, "#test_1");
    res->AddData(&child, "PATH TO BRANCH");
    man->AddReference(res);
    man->ResolveReferences(parent);
    EXPECT_EQ(parent["test_1"], test);
    delete man; // res should now be deleted;
}

TEST(ReferenceResolverTest, JsonToCppManagerBirth) {
    EXPECT_FALSE(JsonToCpp::RefManager::HasInstance());
    JsonToCpp::RefManager::Birth();
    EXPECT_TRUE(JsonToCpp::RefManager::HasInstance());
    EXPECT_THROW(JsonToCpp::RefManager::Birth(), Squeal);
    JsonToCpp::RefManager::Death();
    EXPECT_FALSE(JsonToCpp::RefManager::HasInstance());
    EXPECT_THROW(JsonToCpp::RefManager::Death(), Squeal);
    EXPECT_THROW(JsonToCpp::RefManager::GetInstance(), Squeal);
    JsonToCpp::RefManager::Birth();
    EXPECT_NO_THROW(JsonToCpp::RefManager::GetInstance());
    JsonToCpp::RefManager::Death();   
}

TEST(ReferenceResolverTest, JsonToCppManager) {
    using namespace JsonToCpp;
    RefManager* man = new RefManager();
    double child(99);
    TestClass test;
    FullyTypedResolver<TestClass, double>* res = new FullyTypedResolver
                       <TestClass, double>("#data", &TestClass::SetData, &test);
    man->AddReference(res);
    ChildTypedResolver<double>::AddData("#data", &child);
    man->ResolveReferences();
    EXPECT_TRUE(test._data == &child);
    delete man;  // res should now be deleted
}
}
}

