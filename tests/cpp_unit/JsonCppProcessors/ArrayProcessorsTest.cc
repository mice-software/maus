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

#include <vector>

#include "gtest/gtest.h"

#include "src/common_cpp/JsonCppProcessors/PrimitivesProcessors.hh"
#include "src/common_cpp/JsonCppProcessors/ArrayProcessors.hh"

namespace MAUS {

TEST(ArrayProcessorsTest, PointerArrayConstructorDestructorTest) {
    PointerArrayProcessor<double>* proc =
                       new PointerArrayProcessor<double>(new DoubleProcessor());
    delete proc;  // I guess look for segv, and run through valgrind to check
                  // no memory leaks
}

// test empty vector is handled okay in JsonToCpp
TEST(ArrayProcessorsTest, PointerArrayJsonToCppEmptyVecTest) {
    PointerArrayProcessor<double> proc(new DoubleProcessor());
    Json::Value json_array(Json::arrayValue);
    std::vector<double*>* vec = proc.JsonToCpp(json_array);
    ASSERT_EQ(vec->size(), size_t(0));
    delete vec;
}

// test normal vector gets correct values in JsonToCpp
TEST(ArrayProcessorsTest, PointerArrayJsonToCppTest) {
    PointerArrayProcessor<double> proc(new DoubleProcessor());
    Json::Value json_array(Json::arrayValue);
    for (int i = 0; i < 3; ++i) {
        json_array.append(Json::Value(static_cast<double>(i+1)));
    }
    std::vector<double*>* vec = proc.JsonToCpp(json_array);
    ASSERT_EQ(json_array.size(), vec->size());
    for (int i = 0; i < 3; ++i) {
        EXPECT_DOUBLE_EQ( (*(*vec)[i]), json_array[i].asDouble() );
        delete (*vec)[i];
    }
    delete vec;
}

// test we pass the exception up correctly for type errors in JsonToCpp
TEST(ArrayProcessorsTest, PointerArrayJsonToCppWrongTypeTest) {
    PointerArrayProcessor<double> proc(new DoubleProcessor());
    Json::Value json_array(Json::arrayValue);
    json_array.append(Json::Value("string type"));
    EXPECT_THROW(proc.JsonToCpp(json_array), MAUS::Exceptions::Exception); // and should clean up
                                                      // memory allocation
}

// test we pass the exception up correctly for type errors in JsonToCpp
TEST(ArrayProcessorsTest, PointerArrayJsonToCppNullTest) {
    PointerArrayProcessor<double> proc(new DoubleProcessor());
    Json::Value json_array(Json::arrayValue);
    json_array.append(Json::Value());
    std::vector<double*>* vec_out = proc.JsonToCpp(json_array);
    EXPECT_EQ(vec_out->size(), size_t(1));
    double* null_double = NULL;
    EXPECT_EQ((*vec_out)[0], null_double);
    delete vec_out;
}

// test detect not an array type
TEST(ArrayProcessorsTest, PointerArrayJsonToCppNotArrayTest) {
    PointerArrayProcessor<double> proc(new DoubleProcessor());
    Json::Value json_string("string_type");
    EXPECT_THROW(proc.JsonToCpp(json_string), MAUS::Exceptions::Exception); // and should clean up
}

// test empty vector is handled okay in CppToJson
TEST(ArrayProcessorsTest, PointerArrayCppToJsonEmptyVecTest) {
    PointerArrayProcessor<double> proc(new DoubleProcessor());
    std::vector<double*> vec;
    Json::Value* json_array  = proc.CppToJson(vec);
    EXPECT_EQ(vec.size(), json_array->size());
    delete json_array;
}

// test normal vector gets correct values in CppToJson
TEST(ArrayProcessorsTest, PointerArrayCppToJsonTest) {
    PointerArrayProcessor<double> proc(new DoubleProcessor());
    std::vector<double*> vec;
    for (int i = 0; i < 3; ++i) {
        vec.push_back(new double(static_cast<double>(i)));
    }
    Json::Value* json_array = proc.CppToJson(vec);
    ASSERT_EQ(vec.size(), json_array->size());
    for (int i = 0; i < 3; ++i) {
        EXPECT_DOUBLE_EQ( *(vec[i]), (*json_array)[i].asDouble() );
    }
    delete json_array;
    json_array = proc.CppToJson(vec, "path");
    EXPECT_EQ(JsonWrapper::Path::GetPath(*json_array), "path");
    for (int i = 0; i < 3; ++i) {
        std::string path = "path/"+STLUtils::ToString(i);
        EXPECT_EQ(JsonWrapper::Path::GetPath((*json_array)[i]), path);
        delete vec[i];
    }
    delete json_array;
}

// test detect null values and return json null
TEST(ArrayProcessorsTest, PointerArrayCppToJsonNullTest) {
    PointerArrayProcessor<double> proc(new DoubleProcessor());
    std::vector<double*> vec;
    for (int i = 0; i < 3; ++i) {
        vec.push_back(NULL);
    }
    Json::Value* test_value = proc.CppToJson(vec);
    for (int i = 0; i < 3; ++i) {
        EXPECT_EQ((*test_value)[i], Json::Value());
    }
    delete test_value;
    test_value = proc.CppToJson(vec, "path");
    EXPECT_EQ(JsonWrapper::Path::GetPath(*test_value), "path");
    for (int i = 0; i < 3; ++i) {
        std::string path = "path/"+STLUtils::ToString(i);
        EXPECT_EQ(JsonWrapper::Path::GetPath((*test_value)[i]), path);
        delete vec[i];
    }
    delete test_value;
}

////////////////

TEST(ArrayProcessorsTest, ValueArrayConstructorDestructorTest) {
    ValueArrayProcessor<double>* proc =
                       new ValueArrayProcessor<double>(new DoubleProcessor());
    delete proc;  // I guess look for segv, and run through valgrind to check
                  // no memory leaks
}

// test empty vector is handled okay in JsonToCpp
TEST(ArrayProcessorsTest, ValueArrayJsonToCppEmptyVecTest) {
    ValueArrayProcessor<double> proc(new DoubleProcessor());
    Json::Value json_array(Json::arrayValue);
    std::vector<double>* vec = proc.JsonToCpp(json_array);
    ASSERT_EQ(vec->size(), size_t(0));
    delete vec;
}

// test normal vector gets correct values in JsonToCpp
TEST(ArrayProcessorsTest, ValueArrayJsonToCppTest) {
    ValueArrayProcessor<double> proc(new DoubleProcessor());
    Json::Value json_array(Json::arrayValue);
    for (int i = 0; i < 3; ++i) {
        json_array.append(Json::Value(static_cast<double>(i+1)));
    }
    std::vector<double>* vec = proc.JsonToCpp(json_array);
    ASSERT_EQ(json_array.size(), vec->size());
    for (int i = 0; i < 3; ++i) {
        EXPECT_DOUBLE_EQ( (*vec)[i], json_array[i].asDouble() );
    }
    delete vec;
}

// test we pass the exception up correctly for type errors in JsonToCpp
TEST(ArrayProcessorsTest, ValueArrayJsonToCppWrongTypeTest) {
    ValueArrayProcessor<double> proc(new DoubleProcessor());
    Json::Value json_array(Json::arrayValue);
    json_array.append(Json::Value("string type"));
    EXPECT_THROW(proc.JsonToCpp(json_array), MAUS::Exceptions::Exception); // and should clean up
                                                      // memory allocation
}

// test detect not an array type
TEST(ArrayProcessorsTest, ValueArrayJsonToCppNotArrayTest) {
    ValueArrayProcessor<double> proc(new DoubleProcessor());
    Json::Value json_string("string_type");
    EXPECT_THROW(proc.JsonToCpp(json_string), MAUS::Exceptions::Exception); // and should clean up
}

// test empty vector is handled okay in CppToJson
TEST(ArrayProcessorsTest, ValueArrayCppToJsonEmptyVecTest) {
    ValueArrayProcessor<double> proc(new DoubleProcessor());
    std::vector<double> vec;
    Json::Value* json_array  = proc.CppToJson(vec);
    EXPECT_EQ(vec.size(), json_array->size());
    delete json_array;
}

// test normal vector gets correct values in CppToJson
TEST(ArrayProcessorsTest, ValueArrayCppToJsonTest) {
    ValueArrayProcessor<double> proc(new DoubleProcessor());
    std::vector<double> vec;
    for (int i = 0; i < 3; ++i) {
        vec.push_back(static_cast<double>(i));
    }
    Json::Value* json_array = proc.CppToJson(vec);
    ASSERT_EQ(vec.size(), json_array->size());
    for (int i = 0; i < 3; ++i) {
        EXPECT_DOUBLE_EQ( vec[i], (*json_array)[i].asDouble() );
    }
    delete json_array;
    json_array = proc.CppToJson(vec, "path");
    EXPECT_EQ(JsonWrapper::Path::GetPath(*json_array), "path");
    for (int i = 0; i < 3; ++i) {
        std::string path = "path/"+STLUtils::ToString(i);
        EXPECT_EQ(JsonWrapper::Path::GetPath((*json_array)[i]), path);
    }
    delete json_array;
}

//////////
TEST(ArrayProcessorsTest, RefArrayConstructorDestructorTest) {
    ReferenceArrayProcessor<int>* proc = new ReferenceArrayProcessor<int>();
    delete proc;  // I guess look for segv, and run through valgrind to check
                  // no memory leaks
}

TEST(ArrayProcessorsTest, RefArrayCppToJsonTest) {
    ReferenceResolver::CppToJson::RefManager::Birth();
    ReferenceResolver::JsonToCpp::RefManager::Birth();
    std::vector<int*> data_array;
    std::vector<int*> ref_array;
    int data[] = {1, 2, 3};
    data_array.push_back(&data[0]);
    data_array.push_back(&data[1]);
    data_array.push_back(&data[2]);
    ref_array.push_back(data_array[0]);
    ref_array.push_back(data_array[1]);
    ref_array.push_back(data_array[1]);
    ref_array.push_back(data_array[0]);
    ReferenceArrayProcessor<int> ref_proc;
    PointerArrayProcessor<int> pointer_proc(new IntProcessor());
    Json::Value* pointers = pointer_proc.CppToJson(data_array, "#pointers");
    Json::Value* refs = ref_proc.CppToJson(ref_array, "#ref/1");
    Json::Value obj(Json::objectValue);
    obj["pointers"] = *pointers;
    obj["ref"][1] = *refs;
    JsonWrapper::Path::SetPath(obj["ref"][1], JsonWrapper::Path::GetPath(*refs));
    ReferenceResolver::CppToJson::RefManager::GetInstance().ResolveReferences(obj);
    std::string ref_paths[] = {"0", "1", "1", "0"};
    for (int i = 0; i < Json::Value::ArrayIndex(refs->size()); ++i)
        EXPECT_EQ(obj["ref"][1][i]["$ref"], "#pointers/"+ref_paths[i]);

    std::vector<int*>* data_out = pointer_proc.JsonToCpp(*pointers);
    std::vector<int*>* ref_out = ref_proc.JsonToCpp(obj["ref"][1]);
    ReferenceResolver::JsonToCpp::RefManager::GetInstance().ResolveReferences();

    int ref_data[] = {0, 1, 1, 0};
    for (int i = 0; i < Json::Value::ArrayIndex(ref_out->size()); ++i)
        EXPECT_EQ((*ref_out)[i], (*data_out)[ref_data[i]]);
    delete refs;
    delete pointers;
    ReferenceResolver::JsonToCpp::RefManager::Death();
    ReferenceResolver::CppToJson::RefManager::Death();
}
}

