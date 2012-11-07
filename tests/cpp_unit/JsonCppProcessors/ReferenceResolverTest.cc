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

#include "src/common_cpp/JsonCppProcessors/Common/ReferenceResolverCppToJson.hh"
#include "src/common_cpp/JsonCppProcessors/Common/ReferenceResolverJsonToCpp.hh"
#include "src/legacy/Interface/Squeal.hh"

namespace MAUS {
namespace ReferenceResolver {
namespace JsonToCpp {

class TestClass {
  public:
    void SetData(double* data) {_data = data;} // not memory safe
    double * _data;
};

TEST(ReferenceResolverJsonToCppTest, ResolverTest) {
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

TEST(ReferenceResolverJsonToCppTest, VectorResolverTest) {
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


TEST(ReferenceResolverJsonToCppTest, RefManagerBirthTest) {
    EXPECT_FALSE(RefManager::HasInstance());
    RefManager::Birth();
    EXPECT_TRUE(RefManager::HasInstance());
    EXPECT_THROW(RefManager::Birth(), Squeal);
    RefManager::Death();
    EXPECT_FALSE(RefManager::HasInstance());
    EXPECT_THROW(RefManager::Death(), Squeal);
    EXPECT_THROW(RefManager::GetInstance(), Squeal);
    RefManager::Birth();
    EXPECT_NO_THROW(RefManager::GetInstance());
    RefManager::Death();
}

TEST(ReferenceResolverJsonToCppTest, RefManagerTest) {
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

namespace CppToJson {
TEST(ReferenceResolverCppToJsonTest, TypedResolverTest) {
    double child(0);
    Json::Value parent(Json::objectValue);
    parent["test"] = Json::Value();
    TypedResolver<double> res(&child, "#test");
    TypedResolver<double>::AddData(&child, "PATH TO BRANCH");
    EXPECT_THROW(TypedResolver<double>::
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

TEST(ReferenceResolverCppToJsonTest, RefManagerBirthTest) {
    EXPECT_FALSE(RefManager::HasInstance());
    RefManager::Birth();
    EXPECT_TRUE(RefManager::HasInstance());
    EXPECT_THROW(RefManager::Birth(), Squeal);
    RefManager::Death();
    EXPECT_FALSE(RefManager::HasInstance());
    EXPECT_THROW(RefManager::Death(), Squeal);
    EXPECT_THROW(RefManager::GetInstance(), Squeal);
    RefManager::Birth();
    EXPECT_NO_THROW(RefManager::GetInstance());
    RefManager::Death();
}

TEST(ReferenceResolverCppToJsonTest, RefManagerTest) {
    RefManager* man = new RefManager();
    Json::Value test(Json::objectValue);
    test["$ref"] = Json::Value("PATH TO BRANCH");

    double child(0);
    Json::Value parent(Json::objectValue);
    parent["test_1"] = Json::Value(1);
    parent["test_2"] = Json::Value(2);
    TypedResolver<double>* res = new TypedResolver<double>(&child, "#test_1");
    res->AddData(&child, "PATH TO BRANCH");
    man->AddReference(res);
    man->ResolveReferences(parent);
    EXPECT_EQ(parent["test_1"], test);
    delete man; // res should now be deleted;
}
}
}
}

