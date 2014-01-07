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
#include "stdint.h"

#include "src/common_cpp/Recon/SciFi/SciFiLookup.hh"
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/DataStructure/MCEvent.hh"
#include "src/common_cpp/DataStructure/ReconEvent.hh"
#include "src/common_cpp/DataStructure/Hit.hh"
#include "src/common_cpp/DataStructure/SciFiChannelId.hh"
#include "src/common_cpp/DataStructure/SciFiEvent.hh"
#include "src/common_cpp/DataStructure/SciFiDigit.hh"
#include "src/common_cpp/DataStructure/SciFiNoiseHit.hh"

namespace MAUS {

class SciFiLookupTest : public ::testing::Test {
 protected:
  SciFiLookupTest()  {}
  virtual ~SciFiLookupTest() {}
  virtual void SetUp()    {}
  virtual void TearDown() {}
};

TEST_F(SciFiLookupTest, test_make_hits_map) {

  // Set up a digit with an expected id number of 100012123
  uint64_t id = 100012023;
  int event_num = 100;
  int tracker = 0;
  int station = 1;
  int plane = 2;
  int channel = 23;
  SciFiDigit* dig = new SciFiDigit();
  dig->set_event(event_num);
  dig->set_tracker(tracker);
  dig->set_station(station);
  dig->set_plane(plane);
  dig->set_channel(channel);

  // Set up the corresponding hits
  std::vector<SciFiHit> *hits = new std::vector<SciFiHit>;
  SciFiHit* hit1 = new SciFiHit();
  SciFiHit* hit2 = new SciFiHit();
  SciFiChannelId *sfid1 = new SciFiChannelId();
  SciFiChannelId *sfid2 = new SciFiChannelId();
  sfid1->SetID(static_cast<double>(id));
  sfid2->SetID(static_cast<double>(id));
  hit1->SetChannelId(sfid1);
  hit2->SetChannelId(sfid2);
  hits->push_back(*hit1);
  hits->push_back(*hit2);

  // Set up the correspoding MCEvent
  std::vector<MCEvent*> *mcevts = new std::vector<MCEvent*>;
  MCEvent *mcevt = new MCEvent();
  mcevt->SetSciFiHits(hits);
  mcevts->push_back(mcevt);

  // Set up the correspoding SciFiEvent
  SciFiEvent *sfevt = new SciFiEvent();
  sfevt->add_digit(dig);

  // Set up the correspoding ReconEvent
  std::vector<ReconEvent*> *revts = new std::vector<ReconEvent*>;
  ReconEvent *revt = new ReconEvent();
  revt->SetSciFiEvent(sfevt);
  revts->push_back(revt);

  // Set up the correspoding Spill
  Spill* spill =  new Spill();
  spill->SetMCEvents(mcevts);
  spill->SetReconEvents(revts);

  // Run the Lookup
  SciFiLookup lu;
  lu.make_hits_map(mcevt);
  std::vector<SciFiHit*> out_hits;
  bool success = lu.get_hits(dig, out_hits);

  // Check the results
  ASSERT_TRUE(success);
  ASSERT_EQ(id, lu.get_digit_id(dig));
  ASSERT_EQ(2, static_cast<int>(out_hits.size()));
}

} // ~namespace MAUS
