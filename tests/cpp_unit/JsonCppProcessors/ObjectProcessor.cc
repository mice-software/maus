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

#include "src/common_cpp/JsonCppProcessor/ObjectProcessor.hh"
#include "src/common_cpp/JsonCppProcessor/PrimitivesProcessors.hh"

namespace {
class TestObject {
  public:
    TestObject() : _a(0.), _b(NULL) {}

    double GetA() {return _a;}
    void SetA(double data);

    double* GetB();
    void SetB(double* data);

  private:
    double _a;
    double* _b;
};

TEST(ObjectProcessorTest, JsonToCppTest) {
    ObjectProcessor<TestObject> strict_test_object_proc;
    DoubleProcessor double_proc;
    proc.RegisterValueBranch("branch_a", &_double_proc, &TestObject::GetA,
                                                      &TestObject::SetA, true);
    proc.RegisterPointerBranch("branch_b", &_double_proc, &TestObject::GetB,
                                                      &TestObject::SetB, true);
    
}

