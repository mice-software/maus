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

#include <time.h>

#include <fstream>

#include "gtest/gtest.h"

#include "Utils/Exception.hh"
#include "src/common_cpp/Utils/ConvertBinaryToString.hh"

namespace {

TEST(ConvertBinaryToStringTest, AllocationTest) {
    using MAUS::Utils::ConvertBinaryToString;
    // check alloc dealloc
    ConvertBinaryToString* my_eval_1 = new ConvertBinaryToString();
    delete my_eval_1;

    // check reverse dealloc order
    ConvertBinaryToString* my_eval_3 = new ConvertBinaryToString();
    ConvertBinaryToString* my_eval_2 = new ConvertBinaryToString();
    delete my_eval_2;
    delete my_eval_3;

    // check non-reverse dealloc order
    ConvertBinaryToString* my_eval_4 = new ConvertBinaryToString();
    ConvertBinaryToString* my_eval_5 = new ConvertBinaryToString();
    delete my_eval_4;
    delete my_eval_5;
}

TEST(ConvertBinaryToStringTest, ConversionTest) {
    using MAUS::Utils::ConvertBinaryToString;
    std::string fname = "test_binary.dat";
    std::ofstream fout(fname.c_str());
    fout << "1";
    fout.close();
    struct timespec start_time, end_time;
    start_time.tv_sec = 0;
    start_time.tv_nsec = 0;
    end_time.tv_sec = 0;
    end_time.tv_nsec = 10000000;
    nanosleep(&start_time, &end_time);

    ConvertBinaryToString converter;
    EXPECT_EQ(converter.convert(fname, false), "MQ==");
    EXPECT_EQ(converter.convert(fname, true), "MQ==");
    nanosleep(&start_time, &end_time);
    EXPECT_THROW(converter.convert(fname, true), Exceptions::Exception); // file is gone
}
}

