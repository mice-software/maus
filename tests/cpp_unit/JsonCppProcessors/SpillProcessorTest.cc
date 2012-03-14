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
#include "src/common_cpp/JsonCppProcessors/SpecialVirtualChannelIdProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/HitProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/VirtualHitProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/MCEventProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/ReconEventProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/SpillProcessor.hh"

// Rogers: critique is that we never check that all data was filled; so, for
//         example, if we never touch "energy" or something, it never gets used.
//         But checking this is a lot of work...

namespace MAUS {
namespace SpillProcessorTest {

template <class TYPE>
void test_value(ProcessorBase<TYPE>* proc, std::string test_string) {
    Json::Value json_in;
    ASSERT_NO_THROW(json_in = JsonWrapper::StringToJson(test_string))
                                                                 << test_string;
    TYPE* cpp_type = NULL;
 //   ASSERT_NO_THROW(cpp_type = proc->JsonToCpp(json_in)) << json_in;
    cpp_type = proc->JsonToCpp(json_in);
    Json::Value* json_out = NULL;
    ASSERT_NO_THROW(json_out = proc->CppToJson(*cpp_type));
    EXPECT_PRED3(JsonWrapper::AlmostEqual, json_in, *json_out, 1e-9);
    delete cpp_type;
    delete json_out;
}

std::string THREE_VEC = "{\"x\":1.,\"y\":2.,\"z\":3.}";

std::string STEP =
        std::string("{\"proper_time\":1.,\"path_length\":2.,\"time\":3.,")+
        std::string("\"energy\":4.,\"energy_deposited\":5.,")+
        std::string("\"position\":{\"x\":6.,\"y\":7.,\"z\":8.},")+
        std::string("\"momentum\":{\"x\":9.,\"y\":10.,\"z\":11.}}");

std::string TRACK_SEED =
        std::string("{\"particle_id\":1,\"track_id\":2,")+
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
    std::string("\"momentum\":{\"x\":9.,\"y\":10.,\"z\":11.}}");

std::string SF_CHANNEL_ID =
    std::string("{\"fiber_number\":1,\"station_number\":2,\"plane_number\":3,")+
    std::string("\"tracker_number\":4}");

std::string TOF_CHANNEL_ID =
    std::string("{\"slab\":1,\"station\":2,\"plane\":3}");

std::string SV_CHANNEL_ID =
    std::string("{\"station\":1}");

std::string HIT_SEED =
    std::string("{\"particle_id\":1,\"charge\":2.,\"time\":3.,")+
    std::string("\"energy\":4.,\"energy_deposited\":5.,\"track_id\":6,")+
    std::string("\"position\":{\"x\":6.,\"y\":7.,\"z\":8.},")+
    std::string("\"momentum\":{\"x\":9.,\"y\":10.,\"z\":11.},")+
    std::string("\"channel_id\":");

std::string SF_HIT = HIT_SEED+SF_CHANNEL_ID+"}";

std::string TOF_HIT = HIT_SEED+TOF_CHANNEL_ID+"}";

std::string SV_HIT = HIT_SEED+SV_CHANNEL_ID+"}";

std::string VIRTUAL_HIT =
    std::string("{\"particle_id\":1,\"track_id\":2,\"station_id\":3,")+
    std::string("\"time\":5.,\"mass\":6.,\"charge\":7.,")+
    std::string("\"proper_time\":8.,\"path_length\":9.,")+
    std::string("\"position\":{\"x\":10.,\"y\":11.,\"z\":12.},")+
    std::string("\"momentum\":{\"x\":13.,\"y\":14.,\"z\":15.},")+
    std::string("\"b_field\":{\"x\":16.,\"y\":17.,\"z\":18.},")+
    std::string("\"e_field\":{\"x\":19.,\"y\":20.,\"z\":21.}}");

std::string MC_EVENT =
    std::string("{\"primary\":")+PRIMARY+","+
    std::string("\"sci_fi_hits\":[")+SF_HIT+","+SF_HIT+"],"+
    std::string("\"tof_hits\":[")+TOF_HIT+","+TOF_HIT+"],"+
    std::string("\"special_virtual_hits\":[")+SV_HIT+","+SV_HIT+"],"+
    std::string("\"virtual_hits\":[")+VIRTUAL_HIT+"],"+
    std::string("\"tracks\":[")+TRACK_STEP+","+TRACK_NO_STEP+"]}";

std::string RECON_EVENT =
    std::string("{\"tof_event\":{},\"scifi_event\":{},\"ckov_event\":{},")+
    std::string("\"kl_event\":{},\"emr_event\":{},\"trigger_event\":{},")+
    std::string("\"global_event\":{}}");

std::string SPILL_SEED =
    std::string("{\"scalars\":{},\"daq_data\":{},\"emr_spill_data\":{},")+
    std::string("\"recon_events\":[")+RECON_EVENT+","+RECON_EVENT+"],"+
    std::string("\"spill_number\":1");

std::string SPILL_MC = SPILL_SEED+","+
    std::string("\"mc_events\":[")+MC_EVENT+","+MC_EVENT+"]}";

std::string SPILL_NO_MC = SPILL_SEED+"}";


TEST(SpillProcessorTest, ThreeVectorProcessorTest) {
    ThreeVectorProcessor proc;
    test_value(&proc, THREE_VEC);
}

TEST(SpillProcessorTest, StepProcessorTest) {
    StepProcessor proc;
    test_value(&proc, STEP);
}

TEST(SpillProcessorTest, TrackProcessorTest) {
    TrackProcessor proc;
    test_value(&proc, TRACK_NO_STEP);
    test_value(&proc, TRACK_STEP);
}

TEST(SpillProcessorTest, TOFChannelIdProcessorTest) {
    TOFChannelIdProcessor proc;
    test_value(&proc, TOF_CHANNEL_ID);
}

TEST(SpillProcessorTest, SciFiChannelIdProcessorTest) {
    SciFiChannelIdProcessor proc;
    test_value(&proc, SF_CHANNEL_ID);
}

TEST(SpillProcessorTest, SpecialVirtualChannelIdProcessorTest) {
    SpecialVirtualChannelIdProcessor proc;
    test_value(&proc, SV_CHANNEL_ID);
}

TEST(SpillProcessorTest, SciFiHitProcessorTest) {
    SciFiHitProcessor proc;
    test_value(&proc, SF_HIT);
}

TEST(SpillProcessorTest, TOFHitProcessorTest) {
    TOFHitProcessor proc;
    test_value(&proc, TOF_HIT);
}

TEST(SpillProcessorTest, SpecialVirtualHitProcessorTest) {
    SpecialVirtualHitProcessor proc;
    test_value(&proc, SV_HIT);
}

TEST(SpillProcessorTest, VirtualHitProcessorTest) {
    VirtualHitProcessor proc;
    test_value(&proc, VIRTUAL_HIT);
}

TEST(SpillProcessorTest, PrimaryProcessorTest) {
    PrimaryProcessor proc;
    test_value(&proc, PRIMARY);
}

TEST(SpillProcessorTest, MCEventProcessorTest) {
    MCEventProcessor proc;
    test_value(&proc, MC_EVENT);
}

TEST(SpillProcessorTest, ReconEventProcessorTest) {
    ReconEventProcessor proc;
    test_value(&proc, RECON_EVENT);
}

TEST(SpillProcessorTest, SpillProcessorTest) {
    SpillProcessor proc;
    test_value(&proc, SPILL_NO_MC);
    test_value(&proc, SPILL_MC);
}
}
}
