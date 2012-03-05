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

#include "gtest/gtest.h"

#include "src/common_cpp/DataStructure/ReconEvent.hh"

namespace MAUS {

/** Tests everything in the data structure from spill up to ReconEvent
 */

// all one function - urk
TEST(ReconEventTest, ReconEventTest) {
    TOFEvent* tof_1 = new TOFEvent();
    TOFEvent* tof_2 = new TOFEvent();
    TOFEvent* tof_null = NULL;

    SciFiEvent* scifi_1 = new SciFiEvent();
    SciFiEvent* scifi_2 = new SciFiEvent();
    SciFiEvent* scifi_null = NULL;

    CkovEvent* ckov_1 = new CkovEvent();
    CkovEvent* ckov_2 = new CkovEvent();
    CkovEvent* ckov_null = NULL;

    KLEvent* kl_1 = new KLEvent();
    KLEvent* kl_2 = new KLEvent();
    KLEvent* kl_null = NULL;

    EMREvent* emr_1 = new EMREvent();
    EMREvent* emr_2 = new EMREvent();
    EMREvent* emr_null = NULL;

    TriggerEvent* trigger_1 = new TriggerEvent();
    TriggerEvent* trigger_2 = new TriggerEvent();
    TriggerEvent* trigger_null = NULL;

    GlobalEvent* global_1 = new GlobalEvent();
    GlobalEvent* global_2 = new GlobalEvent();
    GlobalEvent* global_null = NULL;

    ReconEvent my_event;
    // check allocation from NULL okay
    my_event.SetTOFEvent(tof_1);
    my_event.SetSciFiEvent(scifi_1);
    my_event.SetCkovEvent(ckov_1);
    my_event.SetKLEvent(kl_1);
    my_event.SetEMREvent(emr_1);
    my_event.SetTriggerEvent(trigger_1);
    my_event.SetGlobalEvent(global_1);
    EXPECT_EQ(my_event.GetTOFEvent(), tof_1);
    EXPECT_EQ(my_event.GetSciFiEvent(), scifi_1);
    EXPECT_EQ(my_event.GetCkovEvent(), ckov_1);
    EXPECT_EQ(my_event.GetKLEvent(), kl_1);
    EXPECT_EQ(my_event.GetEMREvent(), emr_1);
    EXPECT_EQ(my_event.GetTriggerEvent(), trigger_1);
    EXPECT_EQ(my_event.GetGlobalEvent(), global_1);

    // check reallocation okay
    my_event.SetTOFEvent(tof_2);
    my_event.SetSciFiEvent(scifi_2);
    my_event.SetCkovEvent(ckov_2);
    my_event.SetKLEvent(kl_2);
    my_event.SetEMREvent(emr_2);
    my_event.SetTriggerEvent(trigger_2);
    my_event.SetGlobalEvent(global_2);
    EXPECT_EQ(my_event.GetTOFEvent(), tof_2);
    EXPECT_EQ(my_event.GetSciFiEvent(), scifi_2);
    EXPECT_EQ(my_event.GetCkovEvent(), ckov_2);
    EXPECT_EQ(my_event.GetKLEvent(), kl_2);
    EXPECT_EQ(my_event.GetEMREvent(), emr_2);
    EXPECT_EQ(my_event.GetTriggerEvent(), trigger_2);
    EXPECT_EQ(my_event.GetGlobalEvent(), global_2);

    // check copy constructor (deep copy)
    ReconEvent my_event_copy(my_event);
    EXPECT_NE(my_event.GetTOFEvent(), my_event_copy.GetTOFEvent());
    EXPECT_NE(tof_null, my_event_copy.GetTOFEvent());
    EXPECT_NE(my_event.GetSciFiEvent(), my_event_copy.GetSciFiEvent());
    EXPECT_NE(scifi_null, my_event_copy.GetSciFiEvent());
    EXPECT_NE(my_event.GetCkovEvent(), my_event_copy.GetCkovEvent());
    EXPECT_NE(ckov_null, my_event_copy.GetCkovEvent());
    EXPECT_NE(my_event.GetKLEvent(), my_event_copy.GetKLEvent());
    EXPECT_NE(kl_null, my_event_copy.GetKLEvent());
    EXPECT_NE(my_event.GetEMREvent(), my_event_copy.GetEMREvent());
    EXPECT_NE(emr_null, my_event_copy.GetEMREvent());
    EXPECT_NE(my_event.GetTriggerEvent(), my_event_copy.GetTriggerEvent());
    EXPECT_NE(trigger_null, my_event_copy.GetTriggerEvent());
    EXPECT_NE(my_event.GetGlobalEvent(), my_event_copy.GetGlobalEvent());
    EXPECT_NE(global_null, my_event_copy.GetGlobalEvent());

    // check equality operator (deep copy)
    ReconEvent my_event_equal;
    my_event_equal = my_event;
    EXPECT_NE(my_event.GetTOFEvent(), my_event_equal.GetTOFEvent());
    EXPECT_NE(tof_null, my_event_equal.GetTOFEvent());
    EXPECT_NE(my_event.GetSciFiEvent(), my_event_equal.GetSciFiEvent());
    EXPECT_NE(scifi_null, my_event_equal.GetSciFiEvent());
    EXPECT_NE(my_event.GetCkovEvent(), my_event_equal.GetCkovEvent());
    EXPECT_NE(ckov_null, my_event_equal.GetCkovEvent());
    EXPECT_NE(my_event.GetKLEvent(), my_event_equal.GetKLEvent());
    EXPECT_NE(kl_null, my_event_equal.GetKLEvent());
    EXPECT_NE(my_event.GetEMREvent(), my_event_equal.GetEMREvent());
    EXPECT_NE(emr_null, my_event_equal.GetEMREvent());
    EXPECT_NE(my_event.GetTriggerEvent(), my_event_equal.GetTriggerEvent());
    EXPECT_NE(trigger_null, my_event_equal.GetTriggerEvent());
    EXPECT_NE(my_event.GetGlobalEvent(), my_event_equal.GetGlobalEvent());
    EXPECT_NE(global_null, my_event_equal.GetGlobalEvent());

    // test that we free okay on reallocation
    my_event_equal = my_event;

    // test that equality on myself doesn't cause trouble
    my_event_equal = my_event_equal;

    // check that we handle copy of NULL data okay
    // (i.e. don't attempt to copy!)
    ReconEvent default_event;
    my_event_equal = default_event;
    EXPECT_EQ(tof_null, my_event_equal.GetTOFEvent());
    EXPECT_EQ(scifi_null, my_event_equal.GetSciFiEvent());
    EXPECT_EQ(ckov_null, my_event_equal.GetCkovEvent());
    EXPECT_EQ(kl_null, my_event_equal.GetKLEvent());
    EXPECT_EQ(emr_null, my_event_equal.GetEMREvent());
    EXPECT_EQ(trigger_null, my_event_equal.GetTriggerEvent());
    EXPECT_EQ(global_null, my_event_equal.GetGlobalEvent());
}
}

