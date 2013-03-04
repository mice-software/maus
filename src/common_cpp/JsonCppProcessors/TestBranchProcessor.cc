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

#include "src/common_cpp/JsonCppProcessors/TestBranchProcessor.hh"

namespace MAUS {

TestBranchProcessor::TestBranchProcessor() {
    RegisterPointerBranch
           ("test_child_by_value", &_child_proc, &TestBranch::GetChildByValue,
            &TestBranch::SetChildByValue, true);
    RegisterPointerReference
               ("test_required_child_by_ref",
                &TestBranch::GetRequiredChildByRef,
                &TestBranch::SetRequiredChildByRef,
                true);
    RegisterPointerReference
               ("test_not_req_child_by_ref",
                &TestBranch::GetNotRequiredChildByRef,
                &TestBranch::SetNotRequiredChildByRef,
                false);
    RegisterPointerBranch("test_child_array",
                &_child_array_proc,
                &TestBranch::GetChildArray,
                &TestBranch::SetChildArray,
                true);
}
}

