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
class ReconEventTestDS : public ::testing::Test {
 protected:
  
  ReconEventTestDS() {}
  virtual ~ReconEventTestDS() {}
  virtual void SetUp() {}
  virtual void TearDown() {}
};

// all one function - urk
TEST_F(ReconEventTestDS, test_all_constructors) {
    ReconEvent* recon_1 = new ReconEvent();
    ReconEvent* recon_null = NULL;

    TOFEvent* tof_1 = new TOFEvent();
    TOFEvent* tof_null = NULL;

    SciFiEvent* scifi_1 = new SciFiEvent();
    SciFiEvent* scifi_null = NULL;

    CkovEvent* ckov_1 = new CkovEvent();
    CkovEvent* ckov_null = NULL;

    KLEvent* kl_1 = new KLEvent();
    KLEvent* kl_null = NULL;

    EMREvent* emr_1 = new EMREvent();
    EMREvent* emr_null = NULL;

    TriggerEvent* trigger_1 = new TriggerEvent();
    TriggerEvent* trigger_null = NULL;

    GlobalEvent* global_1 = new GlobalEvent();
    GlobalEvent* global_null = NULL;

    EXPECT_TRUE(recon_1 != NULL);
    EXPECT_TRUE(tof_1 != NULL);
    EXPECT_TRUE(scifi_1 != NULL);
    EXPECT_TRUE(ckov_1 != NULL);
    EXPECT_TRUE(kl_1 != NULL);
    EXPECT_TRUE(emr_1 != NULL);
    EXPECT_TRUE(trigger_1 != NULL);
    EXPECT_TRUE(global_1 != NULL);

    EXPECT_TRUE(recon_null == NULL);
    EXPECT_TRUE(tof_null == NULL);
    EXPECT_TRUE(scifi_null == NULL);
    EXPECT_TRUE(ckov_null == NULL);
    EXPECT_TRUE(kl_null == NULL);
    EXPECT_TRUE(emr_null == NULL);
    EXPECT_TRUE(trigger_null == NULL);
    EXPECT_TRUE(global_null == NULL);

}

TEST_F(ReconEventTestDS, test_all_allocators) {
    ReconEvent my_event;

    TOFEvent* tof_1 = new TOFEvent();
    SciFiEvent* scifi_1 = new SciFiEvent();
    CkovEvent* ckov_1 = new CkovEvent();
    KLEvent* kl_1 = new KLEvent();
    EMREvent* emr_1 = new EMREvent();
    TriggerEvent* trigger_1 = new TriggerEvent();
    GlobalEvent* global_1 = new GlobalEvent();

    // check allocation
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

}

TEST_F(ReconEventTestDS, test_all_reallocators) {
    ReconEvent my_event;

    TOFEvent* tof_1 = new TOFEvent();
    SciFiEvent* scifi_1 = new SciFiEvent();
    CkovEvent* ckov_1 = new CkovEvent();
    KLEvent* kl_1 = new KLEvent();
    EMREvent* emr_1 = new EMREvent();
    TriggerEvent* trigger_1 = new TriggerEvent();
    GlobalEvent* global_1 = new GlobalEvent();

    TOFEvent* tof_2 = new TOFEvent();
    SciFiEvent* scifi_2 = new SciFiEvent();
    CkovEvent* ckov_2 = new CkovEvent();
    KLEvent* kl_2 = new KLEvent();
    EMREvent* emr_2 = new EMREvent();
    TriggerEvent* trigger_2 = new TriggerEvent();
    GlobalEvent* global_2 = new GlobalEvent();

    // Allocate once
    my_event.SetTOFEvent(tof_1);
    my_event.SetSciFiEvent(scifi_1);
    my_event.SetCkovEvent(ckov_1);
    my_event.SetKLEvent(kl_1);
    my_event.SetEMREvent(emr_1);
    my_event.SetTriggerEvent(trigger_1);
    my_event.SetGlobalEvent(global_1);

    // Reallocate
    my_event.SetTOFEvent(tof_2);
    my_event.SetSciFiEvent(scifi_2);
    my_event.SetCkovEvent(ckov_2);
    my_event.SetKLEvent(kl_2);
    my_event.SetEMREvent(emr_2);
    my_event.SetTriggerEvent(trigger_2);
    my_event.SetGlobalEvent(global_2);

    // Check allocations worked
    EXPECT_EQ(my_event.GetTOFEvent(), tof_2);
    EXPECT_EQ(my_event.GetSciFiEvent(), scifi_2);
    EXPECT_EQ(my_event.GetCkovEvent(), ckov_2);
    EXPECT_EQ(my_event.GetKLEvent(), kl_2);
    EXPECT_EQ(my_event.GetEMREvent(), emr_2);
    EXPECT_EQ(my_event.GetTriggerEvent(), trigger_2);
    EXPECT_EQ(my_event.GetGlobalEvent(), global_2);

}

TEST_F(ReconEventTestDS, test_copy_constructor) {
  // Prepare first event
    ReconEvent my_event;

    TOFEvent* tof_1 = new TOFEvent();
    SciFiEvent* scifi_1 = new SciFiEvent();
    CkovEvent* ckov_1 = new CkovEvent();
    KLEvent* kl_1 = new KLEvent();
    EMREvent* emr_1 = new EMREvent();
    TriggerEvent* trigger_1 = new TriggerEvent();
    GlobalEvent* global_1 = new GlobalEvent();
    GlobalEvent* global_null = NULL;

    my_event.SetTOFEvent(tof_1);
    my_event.SetSciFiEvent(scifi_1);
    my_event.SetCkovEvent(ckov_1);
    my_event.SetKLEvent(kl_1);
    my_event.SetEMREvent(emr_1);
    my_event.SetTriggerEvent(trigger_1);
    my_event.SetGlobalEvent(global_null);

    ReconEvent my_event_copy(my_event);
    // EXPECT_NE(my_event.GetTOFEvent(), my_event_copy.GetTOFEvent());
    // EXPECT_NE(my_event.GetSciFiEvent(), my_event_copy.GetSciFiEvent());
    // EXPECT_NE(my_event.GetCkovEvent(), my_event_copy.GetCkovEvent());
    // EXPECT_NE(my_event.GetKLEvent(), my_event_copy.GetKLEvent());
    // EXPECT_NE(my_event.GetEMREvent(), my_event_copy.GetEMREvent());
    // EXPECT_NE(my_event.GetTriggerEvent(), my_event_copy.GetTriggerEvent());
    // EXPECT_NE(my_event.GetGlobalEvent(), my_event_copy.GetGlobalEvent());

    // EXPECT_TRUE(my_event_copy.GetTOFEvent() != NULL);
    // EXPECT_TRUE(my_event_copy.GetSciFiEvent() != NULL);
    // EXPECT_TRUE(my_event_copy.GetCkovEvent() != NULL);
    // EXPECT_TRUE(my_event_copy.GetKLEvent() != NULL);
    // EXPECT_TRUE(my_event_copy.GetEMREvent() != NULL);
    // EXPECT_TRUE(my_event_copy.GetTriggerEvent() != NULL);
    // EXPECT_TRUE(my_event_copy.GetGlobalEvent() != NULL);
}

TEST_F(ReconEventTestDS, test_equality_operator) {
  // Prepare first event
    ReconEvent my_event;

    TOFEvent* tof_1 = new TOFEvent();
    SciFiEvent* scifi_1 = new SciFiEvent();
    CkovEvent* ckov_1 = new CkovEvent();
    KLEvent* kl_1 = new KLEvent();
    EMREvent* emr_1 = new EMREvent();
    TriggerEvent* trigger_1 = new TriggerEvent();
    GlobalEvent* global_1 = new GlobalEvent();

    my_event.SetTOFEvent(tof_1);
    my_event.SetSciFiEvent(scifi_1);
    my_event.SetCkovEvent(ckov_1);
    my_event.SetKLEvent(kl_1);
    my_event.SetEMREvent(emr_1);
    my_event.SetTriggerEvent(trigger_1);
    my_event.SetGlobalEvent(global_1);

    ReconEvent my_event_copy(my_event);
    EXPECT_NE(my_event.GetTOFEvent(), my_event_copy.GetTOFEvent());
    EXPECT_NE(my_event.GetSciFiEvent(), my_event_copy.GetSciFiEvent());
    EXPECT_NE(my_event.GetCkovEvent(), my_event_copy.GetCkovEvent());
    EXPECT_NE(my_event.GetKLEvent(), my_event_copy.GetKLEvent());
    EXPECT_NE(my_event.GetEMREvent(), my_event_copy.GetEMREvent());
    EXPECT_NE(my_event.GetTriggerEvent(), my_event_copy.GetTriggerEvent());
    EXPECT_NE(my_event.GetGlobalEvent(), my_event_copy.GetGlobalEvent());

    EXPECT_TRUE(my_event_copy.GetTOFEvent() != NULL);
    EXPECT_TRUE(my_event_copy.GetSciFiEvent() != NULL);
    EXPECT_TRUE(my_event_copy.GetCkovEvent() != NULL);
    EXPECT_TRUE(my_event_copy.GetKLEvent() != NULL);
    EXPECT_TRUE(my_event_copy.GetEMREvent() != NULL);
    EXPECT_TRUE(my_event_copy.GetTriggerEvent() != NULL);
    EXPECT_TRUE(my_event_copy.GetGlobalEvent() != NULL);
}

TEST_F(ReconEventTestDS, test_null_copy) {
  // Prepare first event
    ReconEvent my_event;

    TOFEvent* tof_1 = new TOFEvent();
    SciFiEvent* scifi_1 = new SciFiEvent();
    CkovEvent* ckov_1 = new CkovEvent();
    KLEvent* kl_1 = new KLEvent();
    EMREvent* emr_1 = new EMREvent();
    TriggerEvent* trigger_1 = new TriggerEvent();
    GlobalEvent* global_1 = new GlobalEvent();

    my_event.SetTOFEvent(tof_1);
    my_event.SetSciFiEvent(scifi_1);
    my_event.SetCkovEvent(ckov_1);
    my_event.SetKLEvent(kl_1);
    my_event.SetEMREvent(emr_1);
    my_event.SetTriggerEvent(trigger_1);
    my_event.SetGlobalEvent(global_1);

    // Test that we free okay on reallocation
    ReconEvent my_event_equal = my_event;

    // Test that equality on myself doesn't cause trouble
    my_event_equal = my_event_equal;

    // Check that we handle copy of NULL data okay
    ReconEvent default_event;
    my_event_equal = default_event;
    EXPECT_TRUE(my_event_equal.GetTOFEvent() == NULL);
    EXPECT_TRUE(my_event_equal.GetSciFiEvent() == NULL);
    EXPECT_TRUE(my_event_equal.GetCkovEvent() == NULL);
    EXPECT_TRUE(my_event_equal.GetKLEvent() == NULL);
    EXPECT_TRUE(my_event_equal.GetEMREvent() == NULL);
    EXPECT_TRUE(my_event_equal.GetTriggerEvent() == NULL);
    EXPECT_TRUE(my_event_equal.GetGlobalEvent() == NULL);
}

}

