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
 *
 */
#include <cstring>
#include "gtest/gtest.h"
#include "src/common_cpp/API/MAUSExceptionBase.hh"

namespace MAUS {

  TEST(MAUSExceptionBaseTest, TestMausExceptionBaseConstructor) {
    MAUSExceptionBase me("MyClass");

    ASSERT_FALSE(strcmp("MyClass", me._classname.c_str()))
      << "Fail: Constructor failed, Classname not set properly"
      << std::endl;
  }


  TEST(MAUSExceptionBaseTest, TestMausExceptionBase_What) {
    MAUSExceptionBase me("MyClass");

    std::string ret = "A MAUSException was thrown by 'MyClass'";

    ASSERT_FALSE(strcmp(ret.c_str(), me._what()))
      << "Fail: _what() failed, didn't return the expected string"
      << std::endl;
  }

  TEST(MAUSExceptionBaseTest, TestMausExceptionBaseWhat) {
    MAUSExceptionBase me("MyClass");

    ASSERT_FALSE(strcmp(me.what(), me._what()))
      << "Fail: what() failed, didn't wrap _what properly"
      << std::endl;
  }
}
