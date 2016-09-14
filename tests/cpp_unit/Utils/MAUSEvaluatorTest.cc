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

#include <Python.h>

#include "gtest/gtest.h"

#include "src/common_cpp/Utils/MAUSEvaluator.hh"
#include "Utils/Exception.hh"

TEST(MAUSEvaluatorTest, AllocationTest) {
    // check alloc dealloc
    MAUS::MAUSEvaluator* my_eval_1 = new MAUS::MAUSEvaluator();
    delete my_eval_1;

    // check reverse dealloc order
    MAUS::MAUSEvaluator* my_eval_4 = new MAUS::MAUSEvaluator();
    MAUS::MAUSEvaluator* my_eval_5 = new MAUS::MAUSEvaluator();
    delete my_eval_5;
    delete my_eval_4;

    // check non-reverse dealloc order
    MAUS::MAUSEvaluator* my_eval_2 = new MAUS::MAUSEvaluator();
    MAUS::MAUSEvaluator* my_eval_3 = new MAUS::MAUSEvaluator();
    delete my_eval_2;
    delete my_eval_3;
}

TEST(MAUSEvaluatorTest, EvaluateRawTest) {
    MAUS::MAUSEvaluator my_eval = MAUS::MAUSEvaluator();
    EXPECT_DOUBLE_EQ(my_eval.evaluate("1.+2."), 3);
    EXPECT_DOUBLE_EQ(my_eval.evaluate("1.*cm+2.*m"), 2010.);
    EXPECT_THROW(my_eval.evaluate("bob"), MAUS::Exceptions::Exception); // unknown variable
}

TEST(MAUSEvaluatorTest, EvaluateVariableTest) {
    MAUS::MAUSEvaluator my_eval = MAUS::MAUSEvaluator();
    my_eval.set_variable("test_var", 2.); // basic check
    EXPECT_DOUBLE_EQ(my_eval.evaluate("1.+test_var"), 3.);
    my_eval.set_variable("test_var", 3.); // overwrite
    EXPECT_DOUBLE_EQ(my_eval.evaluate("1.+test_var"), 4.);
    my_eval.set_variable("sin", 4.); // shadows a math func
    EXPECT_DOUBLE_EQ(my_eval.evaluate("1.+sin"), 5);
    EXPECT_DOUBLE_EQ(my_eval.evaluate(""), 1);
    my_eval.set_variable("test var", 3.); // overwrite
}

TEST(MAUSEvaluatorTest, ResetTest) {
    MAUS::MAUSEvaluator my_eval = MAUS::MAUSEvaluator();
    my_eval.set_variable("test_var", 2.); // basic check
    EXPECT_DOUBLE_EQ(my_eval.evaluate("1.+test_var"), 3.);
    my_eval.reset();
    EXPECT_THROW(my_eval.evaluate("1.+test_var"), MAUS::Exceptions::Exception);
}

