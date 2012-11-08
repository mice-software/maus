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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_TESTCHILD_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_TESTCHILD_HH_

#include "Rtypes.h"

#include <vector>

namespace MAUS {

/** Empty test object - ROOT doesnt like pointers to primitive types apparently
 */
class TestChild {
 public:
  TestChild() {}
  TestChild(const TestChild& md) {}
  TestChild& operator=(const TestChild& md) {return *this;}
  virtual ~TestChild() {}
 private:
  ClassDef(TestChild, 1)
};
}

#endif

