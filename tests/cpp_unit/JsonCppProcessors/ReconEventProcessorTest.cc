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

std::string EMR_BAR_HIT =
    std::string("{\"tot\":-1, \"delta_t\":-2, \"hittime\":-3, ")+
    std::string("\"x\":-4., \"y\":-6., \"z\":-8., \"ex\":-5., \"ey\":-7., \"ez\":-9., ")+
    std::string("\"x_global\":-10., \"y_global\":-11., \"z_global\":-12., ")+
    std::string("\"charge_corrected\":-13., \"total_charge_corrected\":-14., ")+
    std::string("\"path_length\":-15.}");

std::string EMR_BAR = "{\"bar\":-1, \"emr_bar_hits\":["+EMR_BAR_HIT+","+EMR_BAR_HIT+"]}";

std::string EMR_PLANE_HIT =
    std::string("{\"emr_bars\":["+EMR_BAR+","+EMR_BAR+"], ")+
    std::string("\"emr_bars_primary\":["+EMR_BAR+","+EMR_BAR+"], ")+
    std::string("\"emr_bars_secondary\":["+EMR_BAR+","+EMR_BAR+"], ")+
    std::string("\"plane\":-1, \"orientation\":-2, \"charge\":-3., ")+
    std::string("\"charge_corrected\":-4., \"pedestal_area\":-5., \"time\":-6, ")+
    std::string("\"spill\":-7, \"trigger\":-8, \"run\":-9, \"delta_t\":-10, ")+
    std::string("\"samples\":[1,2,3]}");

std::string EMR_EVENT =
    std::string("{\"emr_plane_hits\":["+EMR_PLANE_HIT+","+EMR_PLANE_HIT+"], ")+
    std::string("\"has_primary\":true, ")+
    std::string("\"range_primary\":-1., \"has_secondary\":true, ")+
    std::string("\"range_secondary\":-2., \"secondary_to_primary_track_distance\":-3., ")+
    std::string("\"total_charge_MA\":-4., \"charge_ratio_MA\":-5., ")+
    std::string("\"total_charge_SA\":-6., \"charge_ratio_SA\":-7., ")+
    std::string("\"plane_density_MA\":-8., \"plane_density_SA\":-9., \"chi2\":-10.}");

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

