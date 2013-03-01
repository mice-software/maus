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
TestBranch::TestBranch() : _not_required_child_by_reference(NULL),
                           _required_child_by_reference(NULL),
                           _child_by_value(NULL),
                           _test_pointer_array(NULL) {
    _child_by_value = new TestChild();
    _required_child_by_reference = _child_by_value;
    _not_required_child_by_reference = _child_by_value;
    _test_pointer_array = new std::vector<TestChild*>();
    _test_pointer_array->push_back(NULL);
    _test_pointer_array->push_back(_child_by_value);
    _test_pointer_array->push_back(NULL);
    _test_pointer_array->push_back(_child_by_value);
    _test_pointer_array->push_back(_child_by_value);
}

TestBranch::TestBranch(const TestBranch& md)
             :  _not_required_child_by_reference(NULL),
                _required_child_by_reference(NULL),
                _child_by_value(NULL), _test_pointer_array(NULL) {
    *this = md;
}

TestBranch& TestBranch::operator=(const TestBranch& md) {
    if (&md == this)
        return *this;
    SetChildByValue(new TestChild(*md._child_by_value));
    SetChildArray(new std::vector<TestChild*>(*md._test_pointer_array));
    SetRequiredChildByRef(md._required_child_by_reference);
    SetNotRequiredChildByRef(md._not_required_child_by_reference);
    return *this;
}

TestBranch::~TestBranch() {
    // only delete _child_by_value - we don't own any other memory
    if (_child_by_value != NULL)
        delete _child_by_value;
    if (_test_pointer_array != NULL)
        delete _test_pointer_array;
}

TestChild* TestBranch::GetRequiredChildByRef() const {
    return _required_child_by_reference;
}

void TestBranch::SetRequiredChildByRef(TestChild* test) {
    _required_child_by_reference = test;
}

TestChild* TestBranch::GetNotRequiredChildByRef() const {
    return _not_required_child_by_reference;
}

void TestBranch::SetNotRequiredChildByRef(TestChild* test) {
    _not_required_child_by_reference = test;
}

TestChild* TestBranch::GetChildByValue() const {
    return _child_by_value;
}

void TestBranch::SetChildByValue(TestChild* test) {
    if (_child_by_value != NULL)
        delete _child_by_value;
    _child_by_value = test;
}

std::vector<TestChild*>* TestBranch::GetChildArray() const {
    return _test_pointer_array;
}

void TestBranch::SetChildArray(std::vector<TestChild*>* test) {
    if (_test_pointer_array != NULL)
        delete _test_pointer_array;
    _test_pointer_array = test;
}
}

