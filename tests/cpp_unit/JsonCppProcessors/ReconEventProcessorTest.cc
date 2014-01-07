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

#include "tests/cpp_unit/JsonCppProcessors/CommonProcessorTest.hh"
#include "src/common_cpp/JsonCppProcessors/EMRBarHitProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/EMRBarProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/EMRPlaneHitProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/EMREventProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/ReconEventProcessor.hh"

namespace MAUS {
namespace ReconEventProcessorTest {

std::string EMR_BAR_HIT = "{\"tot\":-1,\"delta_t\":-2}";

std::string EMR_BAR = "{\"bar\":-1,\"emr_bar_hits\":["+EMR_BAR_HIT+","+
                                                       EMR_BAR_HIT+"]}";

std::string EMR_PLANE_HIT =
    std::string("{\"emr_bars\":["+EMR_BAR+","+EMR_BAR+"], \"plane\":-1,")+
    std::string("\"orientation\":-2, \"charge\":-3, \"time\":-4,")+
    std::string("\"spill\":-5, \"trigger\":-6, \"delta_t\":-7}");

std::string EMR_EVENT = "{\"emr_plane_hits\":["+EMR_PLANE_HIT+","+EMR_PLANE_HIT+
                        "]}";

std::string RECON_EVENT =
    std::string("{\"part_event_number\":1, \"emr_event\":"+EMR_EVENT+"}");

TEST(ReconEventProcessorTest, EMRBarHitProcessorTest) {
    EMRBarHitProcessor proc;
    ProcessorTest::test_value(&proc, EMR_BAR_HIT);
}

TEST(ReconEventProcessorTest, EMRBarProcessorTest) {
    EMRBarProcessor proc;
    ProcessorTest::test_value(&proc, EMR_BAR);
}

TEST(ReconEventProcessorTest, EMRPlaneHitProcessorTest) {
    EMRPlaneHitProcessor proc;
    ProcessorTest::test_value(&proc, EMR_PLANE_HIT);
}

TEST(ReconEventProcessorTest, EMREventProcessorTest) {
    EMREventProcessor proc;
    ProcessorTest::test_value(&proc, EMR_EVENT);
}

TEST(ReconEventProcessorTest, ReconEventProcessorTest) {
    ReconEventProcessor proc;
    ProcessorTest::test_value(&proc, RECON_EVENT);
}
}
}

