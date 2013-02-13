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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_TESTBRANCH_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_TESTBRANCH_HH_

#include <vector>

#include "src/common_cpp/Utils/VersionNumber.hh"
#include "src/common_cpp/DataStructure/TestChild.hh"

namespace MAUS {

typedef std::vector<MAUS::TestChild*> TestBranchArray;
/** For integration testing pointer-by-value and pointer-by-reference
 */
class TestBranch {
 public:
  /** Initialise - for convenience we initialise pointers here also (as this
   *  data is just for testing
   */
  TestBranch();

  /** Deep copy child_by_value - shallow copy other data
   */
  TestBranch(const TestBranch& md);

  /** Deep copy child_by_value - shallow copy other data
   */
  TestBranch& operator=(const TestBranch& md);

  /** Destructor only cleans child_by_value */
  virtual ~TestBranch();

  /** Return child_by_value - memory still owned by TestBranch */
  TestChild* GetChildByValue() const;

  /** Set child_by_value - memory now owned by TestBranch */
  void SetChildByValue(TestChild* test);

  /** Return child_by_reference - memory owned externally */
  TestChild* GetRequiredChildByRef() const;

  /** Set child_by_reference - memory owned by caller still */
  void SetRequiredChildByRef(TestChild* test);

  /** Return child_by_reference - memory owned externally */
  TestChild* GetNotRequiredChildByRef() const;

  /** Set child_by_reference - memory owned by caller still */
  void SetNotRequiredChildByRef(TestChild* test);

  /** Return test_pointer_array - memory of the vector owned by TestBranch;
   *  memory of the vector data owned by caller
   */
  std::vector<TestChild*>* GetChildArray() const;

  /** Set test_pointer_array - memory of the vector owned by TestBranch;
   *  memory of the vector data owned by caller
   */
  void SetChildArray(std::vector<TestChild*>* test);

 private:
  TestChild* _not_required_child_by_reference;
  TestChild* _required_child_by_reference;
  TestChild* _child_by_value;
  std::vector<TestChild*>* _test_pointer_array;

  MAUS_VERSIONED_CLASS_DEF(TestBranch)
};
}

#endif
