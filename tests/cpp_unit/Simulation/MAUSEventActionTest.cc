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
using MAUS::MCEvent;
using MAUS::Track;
using MAUS::VirtualHit;

TEST(MAUSEventActionTest, SetGetEventsTest) {
    MAUSEventAction* _eventAct
                           = MAUSGeant4Manager::GetInstance()->GetEventAction();
    std::vector<MCEvent*>* events = new std::vector<MCEvent*>();
    EXPECT_NO_THROW(_eventAct->SetEvents(events));
    EXPECT_EQ(events, _eventAct->GetEvents());
}

TEST(MAUSEventActionTest, BeginOfEventActionTest) {
    // check that we clear the hits at BeginOfEventAction
    MAUSGeant4Manager* _g4 = MAUSGeant4Manager::GetInstance();
    _g4->GetEventAction()->SetEvents(new std::vector<MCEvent*>());
    std::vector<Track>* tracks = new std::vector<Track>(1, Track());
    std::vector<VirtualHit>* vhits = new std::vector<VirtualHit>(1, VirtualHit());
    _g4->GetTracking()->SetTracks(tracks);
    _g4->GetVirtualPlanes()->SetVirtualHits(vhits);
    // can't set MICEDetectorConstruction/SDHits
    _g4->GetEventAction()->BeginOfEventAction(NULL);
    EXPECT_EQ(_g4->GetTracking()->GetTracks()->size(), 0);
    EXPECT_EQ(_g4->GetVirtualPlanes()->GetVirtualHits()->size(), 0);
}

void SetVirtualsAndTracks() {
    MAUSGeant4Manager* _g4 = MAUSGeant4Manager::GetInstance();
    // put some data into tracking, virtual planes
    std::vector<Track>* tracks = new std::vector<Track>(2);
    std::vector<VirtualHit>* vhits = new std::vector<VirtualHit>(1);
    _g4->GetTracking()->SetTracks(tracks);
    _g4->GetVirtualPlanes()->SetVirtualHits(vhits);
}

TEST(MAUSEventActionTest, EndOfEventActionTest) {
    MAUSGeant4Manager* _g4 = MAUSGeant4Manager::GetInstance();
    // _events out of range
    std::vector<MCEvent*>* events = new std::vector<MCEvent*>();
    _g4->GetEventAction()->SetEvents(events);
    EXPECT_THROW(_g4->GetEventAction()->EndOfEventAction(NULL), MAUS::Exceptions::Exception);

    events->push_back(new MCEvent());
    events->push_back(new MCEvent());
    _g4->GetEventAction()->SetEvents(events);


    // check if virtual planes, tracking off we don't get any data through
    _g4->GetVirtualPlanes()->SetWillUseVirtualPlanes(false);
    _g4->GetTracking()->SetWillKeepTracks(false);
    _g4->GetEventAction()->BeginOfEventAction(NULL);
    SetVirtualsAndTracks();
    _g4->GetEventAction()->EndOfEventAction(NULL);
    _g4->GetEventAction()->BeginOfEventAction(NULL);
    SetVirtualsAndTracks();
    _g4->GetEventAction()->EndOfEventAction(NULL);

    EXPECT_EQ(_g4->GetEventAction()->GetEvents(), events);
    EXPECT_EQ(events->at(0)->GetVirtualHits()->size(), 0);
    EXPECT_EQ(events->at(0)->GetTracks()->size(), 0);
    // _events out of range
    EXPECT_THROW(_g4->GetEventAction()->EndOfEventAction(NULL), MAUS::Exceptions::Exception);

    // reset _primary index
    events = new std::vector<MCEvent*>();
    events->push_back(new MCEvent());
    events->push_back(new MCEvent());
    _g4->GetEventAction()->SetEvents(events);

    // now try with tracking, virtualplanes switched on
    _g4->GetVirtualPlanes()->SetWillUseVirtualPlanes(true);
    _g4->GetTracking()->SetWillKeepTracks(true);
    _g4->GetEventAction()->BeginOfEventAction(NULL);
    SetVirtualsAndTracks();
    _g4->GetEventAction()->EndOfEventAction(NULL);
    _g4->GetEventAction()->BeginOfEventAction(NULL);
    SetVirtualsAndTracks();
    _g4->GetEventAction()->EndOfEventAction(NULL);

    // check that this time we do get virtuals/track data through
    EXPECT_EQ(_g4->GetEventAction()->GetEvents(), events);
    EXPECT_EQ(events->size(), 2);
    EXPECT_EQ(events->at(0)->GetVirtualHits()->size(), 1);
    EXPECT_EQ(events->at(0)->GetTracks()->size(), 2);
    // we never called begin of event action so the buffers weren't cleared
    // so we should get the same data in second event
    EXPECT_EQ(events->at(1)->GetVirtualHits()->size(), 1);
    EXPECT_EQ(events->at(1)->GetTracks()->size(), 2);
}

