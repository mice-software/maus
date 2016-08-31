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

#include <string>

#include "gtest/gtest.h"

#include "src/common_cpp/JsonCppProcessors/TestBranchProcessor.hh"

// Test conversion from cpp -> json -> cap

namespace MAUS {
namespace TestBranchProcessorTest {

class TestBranchProcessorTestClass : public ::testing::Test {
 protected:
  virtual void SetUp() {
    // Fill Cpp TestBranch
    _proc = new TestBranchProcessor();
    _testbranch = new MAUS::TestBranch();
  }

  virtual void TearDown() {
    // Fill Cpp TestBranch
    delete _testbranch;
    delete _proc;
  }

  MAUS::TestBranch *_testbranch;
  TestBranchProcessor *_proc;
};

TEST_F(TestBranchProcessorTestClass, CheckInitialSetup) {
  MAUS::TestChild *testchild = _testbranch->GetChildByValue();
  EXPECT_EQ(testchild, _testbranch->GetRequiredChildByRef());
}

TEST_F(TestBranchProcessorTestClass, CppToJson) {
  Json::Value *json_out = NULL;
  ASSERT_NO_THROW(ReferenceResolver::CppToJson::RefManager::Birth());
  ASSERT_NO_THROW(json_out = _proc->CppToJson(*_testbranch, ""));
  ASSERT_TRUE(json_out);
  ASSERT_NO_THROW(ReferenceResolver::CppToJson::RefManager::GetInstance()
                  .ResolveReferences(*json_out));
  ASSERT_NO_THROW(ReferenceResolver::CppToJson::RefManager::Death());
}

TEST_F(TestBranchProcessorTestClass, CppToJsonWithNULLRequiredRef) {
  _testbranch->SetRequiredChildByRef(NULL);
  _testbranch->SetNotRequiredChildByRef(_testbranch->GetChildByValue());

  Json::Value *json_out = NULL;
  if (json_out) {;} // dummy to kill gcc warning, does nothing

  ASSERT_NO_THROW(ReferenceResolver::CppToJson::RefManager::Birth());
  ASSERT_THROW(json_out = _proc->CppToJson(*_testbranch, ""), Exceptions::Exception);
  ASSERT_NO_THROW(ReferenceResolver::CppToJson::RefManager::Death());
}

TEST_F(TestBranchProcessorTestClass, CppToJsonWithNULLNotRequiredRef) {
  _testbranch->SetRequiredChildByRef(_testbranch->GetChildByValue());
  _testbranch->SetNotRequiredChildByRef(NULL);

  Json::Value *json_out = NULL;

  ASSERT_NO_THROW(ReferenceResolver::CppToJson::RefManager::Birth());
  ASSERT_NO_THROW(json_out = _proc->CppToJson(*_testbranch, ""));
  ASSERT_TRUE(json_out);
  ASSERT_NO_THROW(ReferenceResolver::CppToJson::RefManager::GetInstance().
                                                  ResolveReferences(*json_out));
  ASSERT_NO_THROW(ReferenceResolver::CppToJson::RefManager::Death());
}
}
}
