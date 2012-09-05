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

#ifndef _TESTS_CPP_UNIT_JSONCPPPROCESSORS_COMMONPROCESSORTEST_HH_

namespace MAUS {
namespace ProcessorTest {
template <class TYPE>
void test_value(ProcessorBase<TYPE>* proc, std::string test_string) {
    Json::Value json_in;
    ASSERT_NO_THROW(json_in = JsonWrapper::StringToJson(test_string))
                                                                 << test_string;
    TYPE* cpp_type = NULL;
    ASSERT_NO_THROW(cpp_type = proc->JsonToCpp(json_in)) << json_in;
    Json::Value* json_out = NULL;
    ASSERT_NO_THROW(json_out = proc->CppToJson(*cpp_type));
    EXPECT_PRED3(JsonWrapper::AlmostEqual, json_in, *json_out, 1e-9);
    delete cpp_type;
    delete json_out;
}
}
}

#endif

