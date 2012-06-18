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

#include "API/APIExceptions.hh"

namespace MAUS {

  TEST(APIExceptionsTest, TestNullInputExceptionConstructor ) {
    NullInputException npe("MyClass");

    ASSERT_FALSE(strcmp("MyClass", npe._classname.c_str()))
      << "Fail: Constructor failed, Classname not set properly"
      << std::endl;
  }


  TEST(APIExceptionsTest, TestNullInputExceptionWhat ) {
    NullInputException npe("MyClass");

    std::string ret = "The input to '";
    ret            += npe._classname;
    ret            += "' was a null pointer";

    ASSERT_FALSE(strcmp(ret.c_str(), npe.what()))
      << "Fail: What() failed, didn't return the expected string"
      << std::endl;
  }

  TEST(APIExceptionsTest, TestUnhandledExceptionConstructor ) {
    UnhandledException ue("MyClass");

    ASSERT_FALSE(strcmp("MyClass", ue._classname.c_str()))
      << "Fail: Constructor failed, Classname not set properly"
      << std::endl;
  }


  TEST(APIExceptionsTest, TestUnhandledExceptionWhat ) {
    UnhandledException ue("MyClass");

    std::string ret = "An unhandled exception was thrown by '";
    ret            += ue._classname;
    ret            += "' which was not of type 'Squeal' or deriving from 'std::exception'";

    ASSERT_FALSE(strcmp(ret.c_str(), ue.what()))
      << "Fail: What() failed, didn't return the expected string"
      << std::endl;
  }


}// end of namespace
