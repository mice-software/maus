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

#include <string>

#include "gtest/gtest.h"

#include "src/common_cpp/Utils/JsonWrapper.hh"

#include "src/common_cpp/JsonCppProcessors/DBBHitProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/DBBSpillDataProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/EMRDaqProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/DAQDataProcessor.hh"

#include "tests/cpp_unit/JsonCppProcessors/CommonProcessorTest.hh"


namespace MAUS {
namespace DAQProcessorTest {

std::string DBBHIT =
        std::string("{\"channel\":-1,\"geo\":-2,")+
        std::string("\"ldc\":-4,\"leading_edge_time\":5,")+
        std::string("\"trailing_edge_time\":6}");

std::string DBBSPILLDATA =
        std::string("{\"ldc_id\":-1,\"dbb_id\":-2,\"spill_number\":-4,")+
        std::string("\"spill_width\":-5,\"trigger_count\":-6,")+
        std::string("\"hit_count\":-7,\"time_stamp\":-8,\"detector\":\"abc\",")+
        std::string("\"dbb_hits\":["+DBBHIT+","+DBBHIT+"],")+
        std::string("\"dbb_triggers\":["+DBBHIT+","+DBBHIT+"]}");

std::string EMRDAQ =
        std::string("{\"V1731\":[], \"dbb\":[")+DBBSPILLDATA+","+DBBSPILLDATA+
        std::string("]}");


std::string DAQDATA =
        std::string("{\"trigger_request\":[],")+
        std::string("\"tof0\":[],")+
        std::string("\"tof1\":[],")+
        std::string("\"tof2\":[],")+
        std::string("\"ckov\":[],")+
        std::string("\"unknown\":[],")+
        std::string("\"kl\":[],")+
        std::string("\"tag\":[],")+
        std::string("\"trigger\":[],")+
        std::string("\"emr\":"+EMRDAQ+"}");

TEST(DAQProcessorTest, DBBHitProcessorTest) {
    DBBHitProcessor proc;
    ProcessorTest::test_value(&proc, DBBHIT);
}

TEST(DAQProcessorTest, DBBSpillDataProcessorTest) {
    DBBSpillDataProcessor proc;
    ProcessorTest::test_value(&proc, DBBSPILLDATA);
}

TEST(DAQProcessorTest, EMRDaqProcessorTest) {
    EMRDaqProcessor proc;
    ProcessorTest::test_value(&proc, EMRDAQ);
}

// TEST(DAQProcessorTest, DAQDataProcessorTest) {
//    DAQDataProcessor proc;
//    ProcessorTest::test_value(&proc, DAQDATA);
// }
} // DAQPROCESSORTEST
} // MAUS

