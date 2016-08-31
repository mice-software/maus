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
#include "Utils/Exception.hh"

namespace MAUS {
namespace ReferenceResolver {
namespace JsonToCpp {

class TestClass {
  public:
    void SetData(double* data) {_data = data;} // not memory safe
    double * _data;
};

TEST(ReferenceResolverJsonToCppTest, ResolverTest) {
    RefManager::Birth();
    double child(99);
    TestClass test;
    FullyTypedResolver<TestClass, double> res
                                          ("#data", &TestClass::SetData, &test);
    RefManager::GetInstance().SetPointerAsValue("#data", &child);
    EXPECT_THROW(RefManager::GetInstance().SetPointerAsValue("#data", &child),
                 MAUS::Exceptions::Exception);
    res.ResolveReferences();
    EXPECT_TRUE(test._data == &child);

    RefManager::Death();
    RefManager::Birth();
    EXPECT_THROW(res.ResolveReferences(), MAUS::Exceptions::Exception);
    EXPECT_TRUE(test._data == NULL);
    RefManager::Death();
}

TEST(ReferenceResolverJsonToCppTest, VectorResolverTest) {
    RefManager::Birth();
    double child(99);
    std::vector<double*> vec;
    vec.push_back(NULL);
    VectorResolver<double> res("#data", &vec, 0);
    RefManager::GetInstance().SetPointerAsValue("#data", &child);
    res.ResolveReferences();
    EXPECT_TRUE(vec[0] == &child);
    RefManager::Death();
    RefManager::Birth();
    VectorResolver<double> res2("#data", &vec, 1);
    EXPECT_THROW(res2.ResolveReferences(), MAUS::Exceptions::Exception);
    EXPECT_THROW(res.ResolveReferences(), MAUS::Exceptions::Exception);
    EXPECT_TRUE(vec[0] == NULL);
    RefManager::Death();
}


TEST(ReferenceResolverJsonToCppTest, RefManagerBirthTest) {
    EXPECT_FALSE(RefManager::HasInstance());
    RefManager::Birth();
    EXPECT_TRUE(RefManager::HasInstance());
    EXPECT_THROW(RefManager::Birth(), MAUS::Exceptions::Exception);
    RefManager::Death();
    EXPECT_FALSE(RefManager::HasInstance());
    EXPECT_THROW(RefManager::Death(), MAUS::Exceptions::Exception);
    EXPECT_THROW(RefManager::GetInstance(), MAUS::Exceptions::Exception);
    RefManager::Birth();
    EXPECT_NO_THROW(RefManager::GetInstance());
    RefManager::Death();
}

TEST(ReferenceResolverJsonToCppTest, RefManagerTest) {
    RefManager::Birth();
    double child(99);
    TestClass test;
    FullyTypedResolver<TestClass, double>* res = new FullyTypedResolver
                       <TestClass, double>("#data", &TestClass::SetData, &test);
    RefManager::GetInstance().AddReference(res);
    RefManager::GetInstance().SetPointerAsValue("#data", &child);
    RefManager::GetInstance().ResolveReferences();
    EXPECT_TRUE(test._data == &child);
    RefManager::Death();
}
}

namespace CppToJson {
TEST(ReferenceResolverCppToJsonTest, TypedResolverTest) {
    RefManager::Birth();
    double child(0);
    double* null = NULL;
    Json::Value parent(Json::objectValue);
    parent["test"] = Json::Value();
    TypedResolver<double> res(&child, "#test");
    RefManager::GetInstance().SetPointerAsValue(&child, "PATH TO BRANCH");
    EXPECT_THROW(RefManager::GetInstance().
                SetPointerAsValue(&child, "PATH TO BRANCH"), MAUS::Exceptions::Exception);
    res.ResolveReferences(parent);

    Json::Value test(Json::objectValue);
    test["$ref"] = Json::Value("PATH TO BRANCH");
    EXPECT_EQ(parent["test"], test);
    RefManager::Death(); // clear static data
    RefManager::Birth();
    parent["test"] = Json::Value("bob");
    EXPECT_THROW(res.ResolveReferences(parent), MAUS::Exceptions::Exception);
    EXPECT_EQ(parent["test"], Json::Value()) << parent["test"];

    // NULL handling
    TypedResolver<double> res_null(null, "#test");
    RefManager::GetInstance().SetPointerAsValue(null, "SHOULD IGNORE");
    RefManager::GetInstance().SetPointerAsValue(null, "SHOULD IGNORE");
    EXPECT_THROW(RefManager::GetInstance().GetPointerAsValue(null), MAUS::Exceptions::Exception);

    RefManager::Death();
}

TEST(ReferenceResolverCppToJsonTest, RefManagerBirthTest) {
    EXPECT_FALSE(RefManager::HasInstance());
    RefManager::Birth();
    EXPECT_TRUE(RefManager::HasInstance());
    EXPECT_THROW(RefManager::Birth(), MAUS::Exceptions::Exception);
    RefManager::Death();
    EXPECT_FALSE(RefManager::HasInstance());
    EXPECT_THROW(RefManager::Death(), MAUS::Exceptions::Exception);
    EXPECT_THROW(RefManager::GetInstance(), MAUS::Exceptions::Exception);
    RefManager::Birth();
    EXPECT_NO_THROW(RefManager::GetInstance());
    RefManager::Death();
}

TEST(ReferenceResolverCppToJsonTest, RefManagerTest) {
    RefManager::Birth();
    Json::Value test(Json::objectValue);
    test["$ref"] = Json::Value("PATH TO BRANCH");

    double child(0);
    Json::Value parent(Json::objectValue);
    parent["test_1"] = Json::Value(1);
    parent["test_2"] = Json::Value(2);
    TypedResolver<double>* res = new TypedResolver<double>(&child, "#test_1");
    RefManager::GetInstance().SetPointerAsValue(&child, "PATH TO BRANCH");
    RefManager::GetInstance().AddReference(res);
    RefManager::GetInstance().ResolveReferences(parent);
    EXPECT_EQ(parent["test_1"], test);
    RefManager::Death(); // res should now be deleted;
}
}
}
}

