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
 *
 */

#include "gtest/gtest.h"

#include "src/common_cpp/Simulation/MAUSGeant4Manager.hh"
#include "src/common_cpp/Simulation/MAUSEventAction.hh"
#include "Utils/Exception.hh"

using MAUS::MAUSEventAction;
using MAUS::MAUSGeant4Manager;

TEST(MAUSEventActionTest, SetGetEventsTest) {
    MAUSEventAction* _eventAct
                           = MAUSGeant4Manager::GetInstance()->GetEventAction();
    EXPECT_THROW(_eventAct->SetEvents(Json::Value(Json::objectValue)), Exception);
    Json::Value event_array = Json::Value(Json::arrayValue);
    event_array.append(Json::Value(Json::objectValue));
    event_array.append(Json::Value(Json::arrayValue));
    EXPECT_THROW(_eventAct->SetEvents(event_array), Exception);
    event_array[1] = Json::Value(Json::objectValue);
    EXPECT_NO_THROW(_eventAct->SetEvents(event_array));
    EXPECT_EQ(event_array, _eventAct->GetEvents());
}

TEST(MAUSEventActionTest, BeginOfEventActionTest) {
    MAUSGeant4Manager* _g4 = MAUSGeant4Manager::GetInstance();
    Json::Value tracks(Json::arrayValue);
    tracks[Json::Value::UInt(0)] = Json::Value(1);
    Json::Value virtual_hits;
    virtual_hits[1] = Json::Value(0);
    _g4->GetTracking()->SetTracks(Json::Value(tracks));
    _g4->GetVirtualPlanes()->SetVirtualHits(Json::Value(virtual_hits));
    // can't set MICEDetectorConstruction/SDHits
    _g4->GetEventAction()->BeginOfEventAction(NULL);
    EXPECT_EQ(_g4->GetTracking()->GetTracks().size(), (unsigned int) 0);
    EXPECT_EQ(_g4->GetVirtualPlanes()->GetVirtualHits().size(),
              (unsigned int) 0);
}

TEST(MAUSEventActionTest, EndOfEventActionTest) {
    MAUSGeant4Manager* _g4 = MAUSGeant4Manager::GetInstance();
    // _events out of range
    Json::Value events = Json::Value(Json::arrayValue);
    _g4->GetEventAction()->SetEvents(events);
    EXPECT_THROW(_g4->GetEventAction()->EndOfEventAction(NULL), Exception);

    Json::Value::UInt zero(0);
    events.append(Json::Value(Json::objectValue));
    events.append(Json::Value(Json::objectValue));
    _g4->GetEventAction()->SetEvents(events);

    // put some data into tracking, virtual planes
    Json::Value tracks(Json::arrayValue);
    tracks[Json::Value::UInt(0)] = Json::Value(1);
    Json::Value virtual_hits;
    virtual_hits[1] = Json::Value(zero);
    _g4->GetTracking()->SetTracks(Json::Value(tracks));
    _g4->GetVirtualPlanes()->SetVirtualHits(Json::Value(virtual_hits));

    // check if virtual planes, tracking off we don't get any data through
    _g4->GetVirtualPlanes()->SetWillUseVirtualPlanes(false);
    _g4->GetTracking()->SetWillKeepTracks(false);
    _g4->GetEventAction()->EndOfEventAction(NULL);
    _g4->GetEventAction()->EndOfEventAction(NULL);

    EXPECT_EQ(_g4->GetEventAction()->GetEvents(), events);
    // _events out of range
    EXPECT_THROW(_g4->GetEventAction()->EndOfEventAction(NULL), Exception);

    // reset _primary index
    _g4->GetEventAction()->SetEvents(events);

    // now try with tracking, virtualplanes switched on
    _g4->GetVirtualPlanes()->SetWillUseVirtualPlanes(true);
    _g4->GetTracking()->SetWillKeepTracks(true);
    _g4->GetEventAction()->EndOfEventAction(NULL);
    _g4->GetEventAction()->EndOfEventAction(NULL);

    // check that this time we do get virtuals/track data through
    events[zero]["tracks"] = tracks;
    events[zero]["virtual_hits"] = virtual_hits;
    events[1]["tracks"] = tracks;
    events[1]["virtual_hits"] = virtual_hits;
    EXPECT_EQ(_g4->GetEventAction()->GetEvents(), events);
}

