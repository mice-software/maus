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
#include "src/common_cpp/JsonCppProcessors/ThreeVectorProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/EMRTrackPointProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/EMRTrackProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/EMRSpacePointProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/EMRBarHitProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/EMRPlaneHitProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/EMREventTrackProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/EMREventProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/EMRSpillDataProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/ReconEventProcessor.hh"

namespace MAUS {
namespace ReconEventProcessorTest {

std::string THREE_VECTOR =
    std::string("{\"x\":-1., \"y\":-2., \"z\":-3.}");

std::string EMR_TRACK_POINT =
    std::string("{\"pos\":"+THREE_VECTOR+", \"gpos\":"+THREE_VECTOR+", ")+
    std::string("\"pos_err\":"+THREE_VECTOR+", \"ch\":-1, \"resx\":-2., ")+
    std::string("\"resy\":-3., \"chi2\":-4.}");

std::string EMR_TRACK =
    std::string("{\"track_points\":["+EMR_TRACK_POINT+","+EMR_TRACK_POINT+"], ")+
    std::string("\"parx\":[-1.,-2.], \"pary\":[-1.,-2.], \"parx_err\":[-1.,-2.], ")+
    std::string("\"pary_err\":[-1.,-2.], \"origin\":"+THREE_VECTOR+", ")+
    std::string("\"origin_err\":"+THREE_VECTOR+", \"polar\":-1., \"polar_err\":-2., ")+
    std::string("\"azimuth\":-3., \"azimuth_err\":-4., \"chi2\":-5., \"range\":-6., ")+
    std::string("\"range_err\":-7., \"mom\":-8., \"mom_err\":-9.}");

std::string EMR_SPACE_POINT =
    std::string("{\"pos\":"+THREE_VECTOR+", \"gpos\":"+THREE_VECTOR+", ")+
    std::string("\"pos_err\":"+THREE_VECTOR+", \"ch\":-1, \"time\":-2., \"deltat\":-3., ")+
    std::string("\"charge_ma\":-4., \"charge_sa\":-5.}");

std::string EMR_BAR_HIT =
    std::string("{\"ch\":-1, \"tot\":-2, \"time\":-3, \"deltat\":-4}");

std::string EMR_PLANE_HIT =
    std::string("{\"bar_hits\":["+EMR_BAR_HIT+","+EMR_BAR_HIT+"], ")+
    std::string("\"plane\":-1, \"orientation\":-2, \"time\":-3, ")+
    std::string("\"deltat\":-4, \"charge\":-5., \"pedestal_area\":-6., ")+
    std::string("\"samples\":[-1,-2,-3]}");

std::string EMR_EVENT_TRACK =
    std::string("{\"plane_hits\":["+EMR_PLANE_HIT+","+EMR_PLANE_HIT+"], ")+
    std::string("\"space_points\":["+EMR_SPACE_POINT+","+EMR_SPACE_POINT+"], ")+
    std::string("\"track\":"+EMR_TRACK+", \"type\":\"none\", \"id\":-1, \"time\":-2., ")+
    std::string("\"plane_density_ma\":-3., \"plane_density_sa\":-4., \"total_charge_ma\":-5., ")+
    std::string("\"total_charge_sa\":-6., \"charge_ratio_ma\":-7., \"charge_ratio_sa\":-8.}");

std::string EMR_EVENT =
    std::string("{\"event_tracks\":["+EMR_EVENT_TRACK+","+EMR_EVENT_TRACK+"], ")+
    std::string("\"vertex\":"+THREE_VECTOR+", \"vertex_err\":"+THREE_VECTOR+", \"deltat\":1., ")+
    std::string("\"distance\":2., \"polar\":3., \"azimuth\":4.}");

std::string RECON_EVENT =
    std::string("{\"part_event_number\":1, \"emr_event\":"+EMR_EVENT+"}");

TEST(ReconEventProcessorTest, EMRTrackPointProcessorTest) {
    EMRTrackPointProcessor proc;
    ProcessorTest::test_value(&proc, EMR_TRACK_POINT);
}

TEST(ReconEventProcessorTest, EMRTrackProcessorTest) {
    EMRTrackProcessor proc;
    ProcessorTest::test_value(&proc, EMR_TRACK);
}

TEST(ReconEventProcessorTest, EMRSpacePointProcessorTest) {
    EMRSpacePointProcessor proc;
    ProcessorTest::test_value(&proc, EMR_SPACE_POINT);
}

TEST(ReconEventProcessorTest, EMRBarHitProcessorTest) {
    EMRBarHitProcessor proc;
    ProcessorTest::test_value(&proc, EMR_BAR_HIT);
}

TEST(ReconEventProcessorTest, EMRPlaneHitProcessorTest) {
    EMRPlaneHitProcessor proc;
    ProcessorTest::test_value(&proc, EMR_PLANE_HIT);
}

TEST(ReconEventProcessorTest, EMREventTrackProcessorTest) {
    EMREventTrackProcessor proc;
    ProcessorTest::test_value(&proc, EMR_EVENT_TRACK);
}

TEST(ReconEventProcessorTest, EMREventProcessorTest) {
    EMREventProcessor proc;
    ProcessorTest::test_value(&proc, EMR_EVENT);
}

TEST(ReconEventProcessorTest, ReconEventProcessorTest) {
    ReconEventProcessor proc;
    ProcessorTest::test_value(&proc, RECON_EVENT);
}
} // namespace ReconEventProcessorTest
} // namespace MAUS
