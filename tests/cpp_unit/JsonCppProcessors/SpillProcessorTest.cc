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

#include "src/common_cpp/JsonCppProcessors/ThreeVectorProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/StepProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/TrackProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/PrimaryProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/SciFiChannelIdProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/TOFChannelIdProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/CkovChannelIdProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/SpecialVirtualChannelIdProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/HitProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/VirtualHitProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/MCEventProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/ReconEventProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/SpillProcessor.hh"

#include "tests/cpp_unit/JsonCppProcessors/CommonProcessorTest.hh"

// Rogers: critique is that we never check that all data was filled; so, for
//         example, if we never touch "energy" or something, it never gets used.
//         But checking this is a lot of work...

// Tests the MC branch and the Spill overall
// Recon branch should go in ReconEventProcessorTest (this is getting unwieldy)

namespace MAUS {
namespace SpillProcessorTest {

std::string THREE_VEC = "{\"x\":1.,\"y\":2.,\"z\":3.}";

std::string STEP =
        std::string("{\"proper_time\":1.,\"path_length\":2.,\"time\":3.,")+
        std::string("\"energy\":4.,\"energy_deposited\":5.,")+
        std::string("\"position\":{\"x\":6.,\"y\":7.,\"z\":8.},")+
        std::string("\"spin\":{\"x\":12.,\"y\":13.,\"z\":14.},")+
        std::string("\"momentum\":{\"x\":9.,\"y\":10.,\"z\":11.},")+
        std::string("\"b_field\":{\"x\":11.,\"y\":12.,\"z\":13.},")+
        std::string("\"e_field\":{\"x\":14.,\"y\":15.,\"z\":16.},")+
        std::string("\"volume\":\"vol\", \"material\":\"mat\"}");

std::string TRACK_SEED =
        std::string("{\"particle_id\":1,\"track_id\":2,\"kill_reason\":\"Z\",")+
        std::string("\"parent_track_id\":3,")+
        std::string("\"initial_position\":{\"x\":6.,\"y\":7.,\"z\":8.},")+
        std::string("\"initial_momentum\":{\"x\":9.,\"y\":10.,\"z\":11.},")+
        std::string("\"final_position\":{\"x\":6.,\"y\":7.,\"z\":8.},")+
        std::string("\"final_momentum\":{\"x\":9.,\"y\":10.,\"z\":11.}");

std::string TRACK_NO_STEP = TRACK_SEED+"}";

std::string TRACK_STEP = TRACK_SEED+",\"steps\":["+STEP+"]}";

std::string PRIMARY =
    std::string("{\"particle_id\":1,\"random_seed\":2,\"time\":3.,")+
    std::string("\"energy\":4.,")+
    std::string("\"position\":{\"x\":6.,\"y\":7.,\"z\":8.},")+
    std::string("\"spin\":{\"x\":12.,\"y\":13.,\"z\":14.},")+
    std::string("\"momentum\":{\"x\":9.,\"y\":10.,\"z\":11.}}");

std::string SF_CHANNEL_ID =
    std::string("{\"digit_id\":0.0,")+
    std::string("\"fibre_number\":1,\"station_number\":2,\"plane_number\":3,")+
    std::string("\"tracker_number\":4}");

std::string TOF_CHANNEL_ID =
    std::string("{\"slab\":1,\"station_number\":2,\"plane\":3}");

std::string CKOV_CHANNEL_ID =
    std::string("{\"station_number\":1}");

std::string SV_CHANNEL_ID =
    std::string("{\"station\":1}");

std::string HIT_SEED =
    std::string("{\"particle_id\":1,\"charge\":2.,\"mass\":105.65840,\"time\":3.,")+
    std::string("\"energy\":4.,\"energy_deposited\":5.,\"track_id\":6,\"path_length\":12.,")+
    std::string("\"position\":{\"x\":6.,\"y\":7.,\"z\":8.},")+
    std::string("\"momentum\":{\"x\":9.,\"y\":10.,\"z\":11.},")+
    std::string("\"momentum\":{\"x\":9.,\"y\":10.,\"z\":11.},")+
    std::string("\"channel_id\":");

std::string SF_HIT = HIT_SEED+SF_CHANNEL_ID+"}";

std::string TOF_HIT = HIT_SEED+TOF_CHANNEL_ID+"}";

std::string CKOV_HIT = HIT_SEED+CKOV_CHANNEL_ID+"}";

std::string SV_HIT = HIT_SEED+SV_CHANNEL_ID+"}";

std::string TEST_BRANCH(std::string path) {
    std::string ref = "{\"$ref\":\"#"+path+"test_child_by_value\"}";
    return std::string("{\"test_child_by_value\":{}, ")+
           std::string("\"test_required_child_by_ref\":")+ref+", "+
           std::string("\"test_not_req_child_by_ref\":")+ref+", "+
           std::string("\"test_child_array\":[")+ref+", "+ref+", null]}";
}

std::string VIRTUAL_HIT =
    std::string("{\"particle_id\":1,\"track_id\":2,\"station_id\":3,")+
    std::string("\"time\":5.,\"mass\":6.,\"charge\":7.,")+
    std::string("\"proper_time\":8.,\"path_length\":9.,")+
    std::string("\"position\":{\"x\":10.,\"y\":11.,\"z\":12.},")+
    std::string("\"momentum\":{\"x\":13.,\"y\":14.,\"z\":15.},")+
    std::string("\"spin\":{\"x\":22.,\"y\":23.,\"z\":24.},")+
    std::string("\"b_field\":{\"x\":16.,\"y\":17.,\"z\":18.},")+
    std::string("\"e_field\":{\"x\":19.,\"y\":20.,\"z\":21.}}");

std::string MC_EVENT =
    std::string("{\"primary\":")+PRIMARY+","+
    std::string("\"sci_fi_hits\":[")+SF_HIT+","+SF_HIT+"],"+
    std::string("\"tof_hits\":[")+TOF_HIT+","+TOF_HIT+"],"+
    std::string("\"ckov_hits\":[")+CKOV_HIT+","+CKOV_HIT+"],"+
    std::string("\"special_virtual_hits\":[")+SV_HIT+","+SV_HIT+"],"+
    std::string("\"virtual_hits\":[")+VIRTUAL_HIT+"],"+
    std::string("\"tracks\":[")+TRACK_STEP+","+TRACK_NO_STEP+"]}";

std::string RECON_EVENT =
    std::string("{\"part_event_number\":1}");

std::string SPILL_SEED =
    std::string("{\"spill_number\":1, \"run_number\":-1, ")+
    std::string("\"daq_event_type\":\"physics_event\", ")+
    std::string("\"errors\":{\"an_error\":\"message\"}, ")+
    std::string("\"maus_event_type\":\"Spill\"");

std::string SPILL_ALL = SPILL_SEED+","+
    std::string("\"scalars\":{}, ")+
    std::string("\"mc_events\":[], \"recon_events\":[], \"test_branch\":"+
                TEST_BRANCH("test_branch/")+"}");

std::string SPILL_MINIMAL = SPILL_SEED+"}";

TEST(SpillProcessorTest, ThreeVectorProcessorTest) {
    ThreeVectorProcessor proc;
    ProcessorTest::test_value(&proc, THREE_VEC);
}

TEST(SpillProcessorTest, StepProcessorTest) {
    StepProcessor proc;
    ProcessorTest::test_value(&proc, STEP);
}

TEST(SpillProcessorTest, TrackProcessorTest) {
    TrackProcessor proc;
    ProcessorTest::test_value(&proc, TRACK_NO_STEP);
    ProcessorTest::test_value(&proc, TRACK_STEP);
}

TEST(SpillProcessorTest, TOFChannelIdProcessorTest) {
    TOFChannelIdProcessor proc;
    ProcessorTest::test_value(&proc, TOF_CHANNEL_ID);
}

TEST(SpillProcessorTest, CkovChannelIdProcessorTest) {
    CkovChannelIdProcessor proc;
    ProcessorTest::test_value(&proc, CKOV_CHANNEL_ID);
}

TEST(SpillProcessorTest, SciFiChannelIdProcessorTest) {
    SciFiChannelIdProcessor proc;
    ProcessorTest::test_value(&proc, SF_CHANNEL_ID);
}

TEST(SpillProcessorTest, SpecialVirtualChannelIdProcessorTest) {
    SpecialVirtualChannelIdProcessor proc;
    ProcessorTest::test_value(&proc, SV_CHANNEL_ID);
}

TEST(SpillProcessorTest, SciFiHitProcessorTest) {
    SciFiHitProcessor proc;
    ProcessorTest::test_value(&proc, SF_HIT);
}

TEST(SpillProcessorTest, TOFHitProcessorTest) {
    TOFHitProcessor proc;
    ProcessorTest::test_value(&proc, TOF_HIT);
}

TEST(SpillProcessorTest, CkovHitProcessorTest) {
    CkovHitProcessor proc;
    ProcessorTest::test_value(&proc, CKOV_HIT);
}

TEST(SpillProcessorTest, SpecialVirtualHitProcessorTest) {
    SpecialVirtualHitProcessor proc;
    ProcessorTest::test_value(&proc, SV_HIT);
}

TEST(SpillProcessorTest, VirtualHitProcessorTest) {
    VirtualHitProcessor proc;
    ProcessorTest::test_value(&proc, VIRTUAL_HIT);
}

TEST(SpillProcessorTest, PrimaryProcessorTest) {
    PrimaryProcessor proc;
    ProcessorTest::test_value(&proc, PRIMARY);
}

TEST(SpillProcessorTest, MCEventProcessorTest) {
    MCEventProcessor proc;
    ProcessorTest::test_value(&proc, MC_EVENT);
}

TEST(SpillProcessorTest, ReconEventProcessorTest) {
    ReconEventProcessor proc;  // just a minimal recon event here
    ProcessorTest::test_value(&proc, RECON_EVENT);
}

TEST(SpillProcessorTest, TestBranchProcessorTest) {
    TestBranchProcessor proc;  // see how we do evaluating references
    ProcessorTest::test_value(&proc, TEST_BRANCH(""));
}

TEST(SpillProcessorTest, SpillProcessorTest) {
    SpillProcessor proc;
    ProcessorTest::test_value(&proc, SPILL_MINIMAL);
    std::cerr << SPILL_MINIMAL << std::endl;
    ProcessorTest::test_value(&proc, SPILL_ALL);
}
}
}
