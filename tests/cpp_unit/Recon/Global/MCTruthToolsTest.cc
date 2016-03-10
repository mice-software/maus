/* This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
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

#include "gtest/gtest.h"
#include "src/common_cpp/Recon/Global/MCTruthTools.hh"

namespace MAUS {

class MCTruthToolsTest : public ::testing::Test {
 protected:
  MCTruthToolsTest()  {}
  virtual ~MCTruthToolsTest() {}
  virtual void SetUp()    {
    _mc_event = new MCEvent;

    ThreeVector position(0.0, 0.0, 0.0);

    TOFChannelId* tof_channel_id = new TOFChannelId;
    tof_channel_id->SetStation(1);
    tof_channel_id->SetPlane(0);
    TOFHit tof_hit;
    tof_hit.SetChannelId(tof_channel_id);
    TOFHit tof1_1_hit(tof_hit);
    position.setZ(1000.0);
    tof1_1_hit.SetPosition(position);
    tof_channel_id->SetPlane(1);
    TOFHit tof1_2_hit(tof_hit);
    position.setZ(1010.0);
    tof1_2_hit.SetPosition(position);
    tof_channel_id->SetStation(2);
    TOFHit tof2_2_hit(tof_hit);
    position.setZ(4000.0);
    tof2_2_hit.SetPosition(position);
    TOFHitArray* tof_hit_array =  new TOFHitArray
        {tof1_1_hit, tof1_2_hit, tof2_2_hit}; // NOLINT(readability/braces)
    _mc_event->SetTOFHits(tof_hit_array);

    SciFiChannelId* scifi_channel_id = new SciFiChannelId;
    scifi_channel_id->SetTrackerNumber(1);
    scifi_channel_id->SetStationNumber(1);
    scifi_channel_id->SetPlaneNumber(1);
    SciFiHit scifi_hit;
    scifi_hit.SetChannelId(scifi_channel_id);
    SciFiHit tracker1_1_1_hit(scifi_hit);
    position.setZ(3200.0);
    tracker1_1_1_hit.SetPosition(position);
    scifi_channel_id->SetStationNumber(2);
    scifi_channel_id->SetPlaneNumber(2);
    SciFiHit tracker1_2_2_hit(scifi_hit);
    position.setZ(3230.0);
    tracker1_2_2_hit.SetPosition(position);
    scifi_channel_id->SetStationNumber(5);
    scifi_channel_id->SetPlaneNumber(0);
    SciFiHit tracker1_5_0_hit(scifi_hit);
    position.setZ(3300.0);
    tracker1_5_0_hit.SetPosition(position);
    SciFiHitArray* scifi_hit_array = new SciFiHitArray
        {tracker1_1_1_hit, tracker1_2_2_hit, tracker1_5_0_hit}; // NOLINT(readability/braces)
    _mc_event->SetSciFiHits(scifi_hit_array);

    KLHit kl_hit1;
    position.SetZ(3500.0);
    kl_hit1.SetPosition(position);
    KLHit kl_hit2;
    position.SetZ(3502.0);
    kl_hit2.SetPosition(position);
    KLHitArray* kl_hit_array = new KLHitArray {kl_hit1, kl_hit2}; // NOLINT(readability/braces)
    _mc_event->SetKLHits(kl_hit_array);

    EMRHit emr_hit1;
    position.SetZ(3700.0);
    emr_hit1.SetPosition(position);
    EMRHit emr_hit2;
    position.SetZ(3720.0);
    emr_hit2.SetPosition(position);
    EMRHitArray* emr_hit_array = new EMRHitArray {emr_hit1, emr_hit2}; // NOLINT(readability/braces)
    _mc_event->SetEMRHits(emr_hit_array);
  }
  virtual void TearDown() {
    delete _mc_event;
  }

 public:
  MCEvent* _mc_event;
};

TEST_F(MCTruthToolsTest, GetMCDetectors) {
  auto mc_detectors = MCTruthTools::GetMCDetectors(_mc_event);
  size_t num_exist = 0;
  for (auto detector_iter = mc_detectors.begin();
       detector_iter != mc_detectors.end(); detector_iter++) {
    if (detector_iter->second) {
      num_exist++;
    }
  }

  EXPECT_EQ(num_exist, 11);
  EXPECT_TRUE(mc_detectors.at(DataStructure::Global::kTOF1));
  EXPECT_TRUE(mc_detectors.at(DataStructure::Global::kTOF1_1));
  EXPECT_TRUE(mc_detectors.at(DataStructure::Global::kTOF1_2));
  EXPECT_TRUE(mc_detectors.at(DataStructure::Global::kTracker1));
  EXPECT_TRUE(mc_detectors.at(DataStructure::Global::kTracker1_1));
  EXPECT_TRUE(mc_detectors.at(DataStructure::Global::kTracker1_2));
  EXPECT_TRUE(mc_detectors.at(DataStructure::Global::kTracker1_5));
  EXPECT_TRUE(mc_detectors.at(DataStructure::Global::kTOF2));
  EXPECT_TRUE(mc_detectors.at(DataStructure::Global::kTOF2_2));
  EXPECT_TRUE(mc_detectors.at(DataStructure::Global::kCalorimeter));
  EXPECT_TRUE(mc_detectors.at(DataStructure::Global::kEMR));
}

TEST_F(MCTruthToolsTest, GetTOFHits) {
  EXPECT_EQ(MCTruthTools::GetTOFHits(_mc_event,
            DataStructure::Global::kTOF1_1)->size(), 1);
  EXPECT_EQ(MCTruthTools::GetTOFHits(_mc_event,
            DataStructure::Global::kTOF1_2)->size(), 1);
  EXPECT_EQ(MCTruthTools::GetTOFHits(_mc_event,
            DataStructure::Global::kTOF1)->size(), 2);
  EXPECT_EQ(MCTruthTools::GetTOFHits(_mc_event,
            DataStructure::Global::kTOF2_1)->size(), 0);
  EXPECT_EQ(MCTruthTools::GetTOFHits(_mc_event,
            DataStructure::Global::kTOF2_2)->size(), 1);
}

TEST_F(MCTruthToolsTest, GetTrackerHits) {
  EXPECT_EQ(MCTruthTools::GetTrackerHits(_mc_event,
            DataStructure::Global::kTracker0)->size(), 0);
  EXPECT_EQ(MCTruthTools::GetTrackerHits(_mc_event,
            DataStructure::Global::kTracker1_1)->size(), 1);
  EXPECT_EQ(MCTruthTools::GetTrackerHits(_mc_event,
            DataStructure::Global::kTracker1_2)->size(), 1);
  EXPECT_EQ(MCTruthTools::GetTrackerHits(_mc_event,
            DataStructure::Global::kTracker1_3)->size(), 0);
  EXPECT_EQ(MCTruthTools::GetTrackerHits(_mc_event,
            DataStructure::Global::kTracker1_5)->size(), 1);
  EXPECT_EQ(MCTruthTools::GetTrackerHits(_mc_event,
            DataStructure::Global::kTracker1)->size(), 3);
}

TEST_F(MCTruthToolsTest, GetTrackerPlaneHit) {
  SciFiHit* hit1_1_1 = MCTruthTools::GetTrackerPlaneHit(_mc_event, 1, 1, 1);
  SciFiHit* hit1_2_2 = MCTruthTools::GetTrackerPlaneHit(_mc_event, 1, 2, 2);
  SciFiHit* hit0_4_1 = MCTruthTools::GetTrackerPlaneHit(_mc_event, 0, 4, 1);

  EXPECT_EQ(hit1_1_1->GetChannelId()->GetTrackerNumber(), 1);
  EXPECT_EQ(hit1_1_1->GetChannelId()->GetStationNumber(), 1);
  EXPECT_EQ(hit1_1_1->GetChannelId()->GetPlaneNumber(), 1);
  EXPECT_EQ(hit1_2_2->GetChannelId()->GetTrackerNumber(), 1);
  EXPECT_EQ(hit1_2_2->GetChannelId()->GetStationNumber(), 2);
  EXPECT_EQ(hit1_2_2->GetChannelId()->GetPlaneNumber(), 2);
  EXPECT_FALSE(hit0_4_1);
}

TEST_F(MCTruthToolsTest, GetNearestZHit) {
  TOFHitArray* tof_hits = _mc_event->GetTOFHits();
  KLHitArray* kl_hits = _mc_event->GetKLHits();
  EMRHitArray* emr_hits = _mc_event->GetEMRHits();

  TLorentzVector position(0.0, 0.0, 1011.0, 0.0);
  TOFHit tof_hit = MCTruthTools::GetNearestZHit(tof_hits, position);
  EXPECT_FLOAT_EQ(tof_hit.GetPosition().z(), 1010.0);

  position.SetZ(3500.8);
  KLHit kl_hit = MCTruthTools::GetNearestZHit(kl_hits, position);
  EXPECT_FLOAT_EQ(kl_hit.GetPosition().z(), 3500.0);

  position.SetZ(3711.0);
  EMRHit emr_hit = MCTruthTools::GetNearestZHit(emr_hits, position);
  EXPECT_FLOAT_EQ(emr_hit.GetPosition().z(), 3720.0);
}

} // ~namespace MAUS
