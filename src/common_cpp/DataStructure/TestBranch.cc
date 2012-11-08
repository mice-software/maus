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

#include <iostream>

#include "src/common_cpp/DataStructure/TestBranch.hh"

namespace MAUS {
TestBranch::TestBranch() : _double_by_reference(NULL), _double_by_value(NULL),
                           _test_pointer_array(NULL) {
    _double_by_value = new TestChild();
    _double_by_reference = _double_by_value;
    _test_pointer_array = new std::vector<TestChild*>();
    _test_pointer_array->push_back(NULL);
    _test_pointer_array->push_back(_double_by_value);
    _test_pointer_array->push_back(NULL);
    _test_pointer_array->push_back(_double_by_value);
    _test_pointer_array->push_back(_double_by_value);
}

TestBranch::TestBranch(const TestBranch& md) : _double_by_reference(NULL),
                           _double_by_value(NULL), _test_pointer_array(NULL) {
    *this = md;
}

TestBranch& TestBranch::operator=(const TestBranch& md) {
    if (&md == this)
        return *this;
    SetDoubleByValue(new TestChild(*md._double_by_value));
    SetDoubleArray(new std::vector<TestChild*>(*md._test_pointer_array));
    SetDoubleByRef(md._double_by_reference);  // we don't own this memory
    return *this;
}

TestBranch::~TestBranch() {
    // only delete _double_by_value - we don't own any other memory
    if (_double_by_value != NULL)
        delete _double_by_value;
    if (_test_pointer_array != NULL)
        delete _test_pointer_array;
}

TestChild* TestBranch::GetDoubleByRef() const {
    return _double_by_reference;
}

void TestBranch::SetDoubleByRef(TestChild* test) {
    _double_by_reference = test;
}

TestChild* TestBranch::GetDoubleByValue() const {
    return _double_by_value;
}

void TestBranch::SetDoubleByValue(TestChild* test) {
    if (_double_by_value != NULL)
        delete _double_by_value;
    _double_by_value = test;
}

std::vector<TestChild*>* TestBranch::GetDoubleArray() const {
    return _test_pointer_array;
}

void TestBranch::SetDoubleArray(std::vector<TestChild*>* test) {
    if (_test_pointer_array != NULL)
        delete _test_pointer_array;
    _test_pointer_array = test;
}
}

