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

#include <string>

#include "gtest/gtest.h"

#include "src/common_cpp/Utils/Exception.hh"
#include "src/common_cpp/Utils/ExceptionLevel.hh"

namespace MAUS {

TEST(ExceptionTest, ConstructorAccessorTest) {
    // just check it doesnt do anything daft
    Exceptions::Exception* exception = new Exceptions::Exception();
    delete exception;

    exception = new Exceptions::Exception(Exceptions::recoverable, "error", "location");
    EXPECT_EQ(exception->GetErrorLevel(), Exceptions::recoverable);
    EXPECT_EQ(exception->GetMessage(), "error");
    EXPECT_EQ(exception->GetLocation(), "location");
    EXPECT_EQ(std::string(exception->what()), std::string("error at location"));

    delete exception;
}

TEST(ExceptionTest, SetGetMessageTest) {
    Exceptions::Exception exception(Exceptions::recoverable, "error", "location");
    exception.SetMessage("new error");
    EXPECT_EQ(exception.GetMessage(), "new error");
    EXPECT_EQ(std::string(exception.what()), std::string("new error at location"));
}

std::string test_make_stack_trace(size_t skips, int recursion_level) {
    if (recursion_level <= 0) {
        return Exceptions::Exception::MakeStackTrace(skips);
    } else {
        return test_make_stack_trace(skips, recursion_level-1);
    }
}

TEST(ExceptionTest, MakeStackTraceTest) {
    std::string stack_trace = test_make_stack_trace(2, 4);
    // one skip is the MakeStackTrace;
    // one skip is a call to _test_make_stack_trace
    // should be 3 more occurences of test_make_stack_trace in the stack
    int n_occurrences = 0;
    size_t pos = stack_trace.find(stack_trace, 0);
    while (pos != std::string::npos && n_occurrences < 10) {
        ++n_occurrences;
        pos = stack_trace.find("test_cpp_unit", pos+1);
    }
    // better to test n_occurrences = some_value
    EXPECT_GE(n_occurrences, 0) << stack_trace;
}

TEST(ExceptionTest, PrintTest) {
    // just run the code; probably if I was being careful I would check the
    // output by redirecting Squeak...
    Exceptions::Exception exception(Exceptions::recoverable, "error", "location");
    exception.Print();
}

TEST(ExceptionTest, TestWillDoStackTrace) {
    Exceptions::Exception::SetWillDoStackTrace(true);
    EXPECT_TRUE(Exceptions::Exception::GetWillDoStackTrace());
    Exceptions::Exception exception_1(Exceptions::recoverable, "error", "location");
    EXPECT_TRUE(exception_1.GetStackTrace() != "");

    Exceptions::Exception::SetWillDoStackTrace(false);
    EXPECT_FALSE(Exceptions::Exception::GetWillDoStackTrace());
    Exceptions::Exception exception_2(Exceptions::recoverable, "error", "location");
    EXPECT_TRUE(exception_2.GetStackTrace() == "");

    Exceptions::Exception::SetWillDoStackTrace(true);
    EXPECT_TRUE(Exceptions::Exception::GetWillDoStackTrace());
    Exceptions::Exception exception_3(Exceptions::recoverable, "error", "location");
    EXPECT_TRUE(exception_3.GetStackTrace() != "");
}
}

