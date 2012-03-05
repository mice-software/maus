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

#include "src/common_cpp/DataStructure/Step.hh"
#include "src/common_cpp/DataStructure/Track.hh"
#include "src/common_cpp/DataStructure/VirtualHit.hh"
#include "src/common_cpp/DataStructure/Primary.hh"
#include "src/common_cpp/DataStructure/Hit.hh"
#include "src/common_cpp/DataStructure/MCEvent.hh"

/** Tests for everything in the MCEvent (i.e. Hits etc also)
 */

namespace MAUS {

TEST(MCEventTest, StepTest) {
    // Accessors/mutators
    Step my_step;
    my_step.SetPosition(ThreeVector(1., 2., 3.));
    my_step.SetMomentum(ThreeVector(-1, -2, -3));
    my_step.SetProperTime(10.);
    my_step.SetPathLength(11.);
    my_step.SetEnergy(12.);
    my_step.SetEnergyDeposited(13.);
    EXPECT_EQ(my_step.GetPosition(), ThreeVector(1., 2., 3.));
    EXPECT_EQ(my_step.GetMomentum(), ThreeVector(-1., -2., -3.));
    EXPECT_EQ(my_step.GetProperTime(), 10.);
    EXPECT_EQ(my_step.GetPathLength(), 11.);
    EXPECT_EQ(my_step.GetEnergy(), 12.);
    EXPECT_EQ(my_step.GetEnergyDeposited(), 13.);

    // copy_constructor
    Step my_step_copy(my_step);
    EXPECT_EQ(my_step.GetPosition(), my_step_copy.GetPosition());
    EXPECT_EQ(my_step.GetMomentum(), my_step_copy.GetMomentum());
    EXPECT_EQ(my_step.GetPathLength(), my_step_copy.GetPathLength());
    EXPECT_EQ(my_step.GetEnergy(), my_step_copy.GetEnergy());
    EXPECT_EQ(my_step.GetEnergyDeposited(), my_step_copy.GetEnergyDeposited());

    // equality operator
    Step my_step_equal;
    my_step_equal = my_step;
    EXPECT_EQ(my_step.GetPosition(), my_step_equal.GetPosition());
    EXPECT_EQ(my_step.GetMomentum(), my_step_equal.GetMomentum());
    EXPECT_EQ(my_step.GetPathLength(), my_step_equal.GetPathLength());
    EXPECT_EQ(my_step.GetEnergy(), my_step_equal.GetEnergy());
    EXPECT_EQ(my_step.GetEnergyDeposited(), my_step_equal.GetEnergyDeposited());

    // test that equality on myself doesn't cause trouble
    my_step = my_step;
}

TEST(MCEventTest, TrackTest) {
    std::vector<Step>* steps1 = new std::vector<Step>(2, Step());
    std::vector<Step>* steps2 = new std::vector<Step>(2, Step());
    std::vector<Step>* steps_null = NULL;
    // Accessors/mutators
    Track my_track;
    my_track.SetInitialPosition(ThreeVector(1., 2., 3.));
    my_track.SetFinalPosition(ThreeVector(-1, -2, -3));
    my_track.SetInitialMomentum(ThreeVector(4., 5., 6.));
    my_track.SetFinalMomentum(ThreeVector(-4, -5, -6));
    my_track.SetParticleId(10);
    my_track.SetTrackId(11);
    my_track.SetParentTrackId(12);
    my_track.SetSteps(steps1);
    EXPECT_EQ(my_track.GetInitialPosition(), ThreeVector(1., 2., 3.));
    EXPECT_EQ(my_track.GetFinalPosition(), ThreeVector(-1., -2., -3.));
    EXPECT_EQ(my_track.GetInitialMomentum(), ThreeVector(4., 5., 6.));
    EXPECT_EQ(my_track.GetFinalMomentum(), ThreeVector(-4., -5., -6.));
    EXPECT_EQ(my_track.GetParticleId(), 10);
    EXPECT_EQ(my_track.GetTrackId(), 11);
    EXPECT_EQ(my_track.GetParentTrackId(), 12);
    EXPECT_EQ(my_track.GetSteps(), steps1);

    // check reallocation (free original memory, then allocate)
    my_track.SetSteps(steps2);
    EXPECT_EQ(my_track.GetSteps(), steps2);

    // copy_constructor
    Track my_track_copy(my_track);
    EXPECT_EQ(my_track.GetInitialPosition(), my_track_copy.GetInitialPosition());
    EXPECT_EQ(my_track.GetFinalPosition(), my_track_copy.GetFinalPosition());
    EXPECT_EQ(my_track.GetInitialMomentum(), my_track_copy.GetInitialMomentum());
    EXPECT_EQ(my_track.GetFinalMomentum(), my_track_copy.GetFinalMomentum());
    EXPECT_EQ(my_track.GetParticleId(), my_track_copy.GetParticleId());
    EXPECT_EQ(my_track.GetTrackId(), my_track_copy.GetTrackId());
    EXPECT_EQ(my_track.GetParentTrackId(), my_track_copy.GetParentTrackId());
    // deep copy => value should be the same, but not address
    EXPECT_EQ(my_track_copy.GetSteps()->size(), size_t(2));
    EXPECT_NE(my_track.GetSteps(), my_track_copy.GetSteps());

    // equality operator
    Track my_track_equal;
    my_track_equal = my_track;
    EXPECT_EQ(my_track.GetInitialPosition(), my_track_equal.GetInitialPosition());
    EXPECT_EQ(my_track.GetFinalPosition(), my_track_equal.GetFinalPosition());
    EXPECT_EQ(my_track.GetInitialMomentum(), my_track_equal.GetInitialMomentum());
    EXPECT_EQ(my_track.GetFinalMomentum(), my_track_equal.GetFinalMomentum());
    EXPECT_EQ(my_track.GetParticleId(), my_track_equal.GetParticleId());
    EXPECT_EQ(my_track.GetTrackId(), my_track_equal.GetTrackId());
    EXPECT_EQ(my_track.GetParentTrackId(), my_track_equal.GetParentTrackId());
    // deep copy => value should be the same, but not address
    EXPECT_EQ(my_track_equal.GetSteps()->size(), size_t(2));
    EXPECT_NE(my_track.GetSteps(), my_track_equal.GetSteps());

    // check we free correctly then reallocate (needs valgrind)
    my_track_equal = my_track;

    // test that equality on myself doesn't cause trouble
    my_track = my_track;

    // test that NULL values are handled okay in copy
    Track default_track;
    my_track = default_track;
    EXPECT_NE(steps_null, my_track_equal.GetSteps());
}

TEST(MCEventTest, VirtualHitTest) {
    // Accessors/mutators
    VirtualHit my_vhit;
    my_vhit.SetPosition(ThreeVector(1., 2., 3.));
    my_vhit.SetMomentum(ThreeVector(-1, -2, -3));
    my_vhit.SetBField(ThreeVector(4., 5., 6.));
    my_vhit.SetEField(ThreeVector(-4, -5, -6));
    my_vhit.SetParticleId(10);
    my_vhit.SetTrackId(11);
    my_vhit.SetStationId(12);
    my_vhit.SetTime(20.);
    my_vhit.SetMass(21.);
    my_vhit.SetCharge(22.);
    my_vhit.SetProperTime(23.);
    my_vhit.SetPathLength(24.);
    EXPECT_EQ(my_vhit.GetPosition(), ThreeVector(1., 2., 3.));
    EXPECT_EQ(my_vhit.GetMomentum(), ThreeVector(-1., -2., -3.));
    EXPECT_EQ(my_vhit.GetBField(), ThreeVector(4., 5., 6.));
    EXPECT_EQ(my_vhit.GetEField(), ThreeVector(-4., -5., -6.));
    EXPECT_EQ(my_vhit.GetParticleId(), 10);
    EXPECT_EQ(my_vhit.GetTrackId(), 11);
    EXPECT_EQ(my_vhit.GetStationId(), 12);
    EXPECT_EQ(my_vhit.GetTime(), 20.);
    EXPECT_EQ(my_vhit.GetMass(), 21.);
    EXPECT_EQ(my_vhit.GetCharge(), 22.);
    EXPECT_EQ(my_vhit.GetProperTime(), 23.);
    EXPECT_EQ(my_vhit.GetPathLength(), 24.);

    // copy_constructor
    VirtualHit my_vhit_copy(my_vhit);
    EXPECT_EQ(my_vhit.GetPosition(), my_vhit_copy.GetPosition());
    EXPECT_EQ(my_vhit.GetMomentum(), my_vhit_copy.GetMomentum());
    EXPECT_EQ(my_vhit.GetBField(), my_vhit_copy.GetBField());
    EXPECT_EQ(my_vhit.GetEField(), my_vhit_copy.GetEField());
    EXPECT_EQ(my_vhit.GetParticleId(), my_vhit_copy.GetParticleId());
    EXPECT_EQ(my_vhit.GetTrackId(), my_vhit_copy.GetTrackId());
    EXPECT_EQ(my_vhit.GetStationId(), my_vhit_copy.GetStationId());
    EXPECT_EQ(my_vhit.GetTime(), my_vhit_copy.GetTime());
    EXPECT_EQ(my_vhit.GetMass(), my_vhit_copy.GetMass());
    EXPECT_EQ(my_vhit.GetCharge(), my_vhit_copy.GetCharge());
    EXPECT_EQ(my_vhit.GetProperTime(), my_vhit_copy.GetProperTime());
    EXPECT_EQ(my_vhit.GetPathLength(), my_vhit_copy.GetPathLength());

    // equality operator
    VirtualHit my_vhit_equal;
    my_vhit_equal = my_vhit;
    EXPECT_EQ(my_vhit.GetPosition(), my_vhit_equal.GetPosition());
    EXPECT_EQ(my_vhit.GetMomentum(), my_vhit_equal.GetMomentum());
    EXPECT_EQ(my_vhit.GetBField(), my_vhit_equal.GetBField());
    EXPECT_EQ(my_vhit.GetEField(), my_vhit_equal.GetEField());
    EXPECT_EQ(my_vhit.GetParticleId(), my_vhit_equal.GetParticleId());
    EXPECT_EQ(my_vhit.GetTrackId(), my_vhit_equal.GetTrackId());
    EXPECT_EQ(my_vhit.GetStationId(), my_vhit_equal.GetStationId());
    EXPECT_EQ(my_vhit.GetTime(), my_vhit_equal.GetTime());
    EXPECT_EQ(my_vhit.GetMass(), my_vhit_equal.GetMass());
    EXPECT_EQ(my_vhit.GetCharge(), my_vhit_equal.GetCharge());
    EXPECT_EQ(my_vhit.GetProperTime(), my_vhit_equal.GetProperTime());
    EXPECT_EQ(my_vhit.GetPathLength(), my_vhit_equal.GetPathLength());

    // test that equality on myself doesn't cause trouble
    my_vhit_equal = my_vhit_equal;
}

TEST(MCEventTest, PrimaryTest) {
    // Accessors/mutators
    Primary my_primary;
    my_primary.SetPosition(ThreeVector(1., 2., 3.));
    my_primary.SetMomentum(ThreeVector(-1, -2, -3));
    my_primary.SetParticleId(10);
    my_primary.SetTime(20.);
    my_primary.SetEnergy(21.);
    EXPECT_EQ(my_primary.GetPosition(), ThreeVector(1., 2., 3.));
    EXPECT_EQ(my_primary.GetMomentum(), ThreeVector(-1., -2., -3.));
    EXPECT_EQ(my_primary.GetParticleId(), 10);
    EXPECT_EQ(my_primary.GetTime(), 20.);
    EXPECT_EQ(my_primary.GetEnergy(), 21.);

    // copy_constructor
    Primary my_primary_copy(my_primary);
    EXPECT_EQ(my_primary.GetPosition(), my_primary_copy.GetPosition());
    EXPECT_EQ(my_primary.GetMomentum(), my_primary_copy.GetMomentum());
    EXPECT_EQ(my_primary.GetParticleId(), my_primary_copy.GetParticleId());
    EXPECT_EQ(my_primary.GetTime(), my_primary_copy.GetTime());
    EXPECT_EQ(my_primary.GetEnergy(), my_primary_copy.GetEnergy());

    // equality operator
    Primary my_primary_equal;
    my_primary_equal = my_primary;
    EXPECT_EQ(my_primary.GetPosition(), my_primary_equal.GetPosition());
    EXPECT_EQ(my_primary.GetMomentum(), my_primary_equal.GetMomentum());
    EXPECT_EQ(my_primary.GetParticleId(), my_primary_equal.GetParticleId());
    EXPECT_EQ(my_primary.GetTime(), my_primary_equal.GetTime());
    EXPECT_EQ(my_primary.GetEnergy(), my_primary_equal.GetEnergy());

    // test that equality on myself doesn't cause trouble
    my_primary_equal = my_primary_equal;
}

TEST(MCEventTest, HitTest) {
    ChannelId* channel_id_1 = new ChannelId();
    ChannelId* channel_id_2 = new ChannelId();
    ChannelId* channel_id_null = NULL;

    // Accessors/mutators
    Hit my_hit;
    my_hit.SetPosition(ThreeVector(1., 2., 3.));
    my_hit.SetMomentum(ThreeVector(-1, -2, -3));
    my_hit.SetParticleId(10);
    my_hit.SetTrackId(11);
    my_hit.SetTime(20.);
    my_hit.SetEnergy(21.);
    my_hit.SetCharge(22.);
    my_hit.SetEnergyDeposited(23.);
    my_hit.SetChannelId(channel_id_1);
    EXPECT_EQ(my_hit.GetPosition(), ThreeVector(1., 2., 3.));
    EXPECT_EQ(my_hit.GetMomentum(), ThreeVector(-1., -2., -3.));
    EXPECT_EQ(my_hit.GetParticleId(), 10);
    EXPECT_EQ(my_hit.GetTrackId(), 11);
    EXPECT_EQ(my_hit.GetTime(), 20.);
    EXPECT_EQ(my_hit.GetEnergy(), 21.);
    EXPECT_EQ(my_hit.GetCharge(), 22.);
    EXPECT_EQ(my_hit.GetEnergyDeposited(), 23.);
    EXPECT_EQ(my_hit.GetChannelId(), channel_id_1);

    my_hit.SetChannelId(channel_id_2);
    EXPECT_EQ(my_hit.GetChannelId(), channel_id_2);

    // copy_constructor
    Hit my_hit_copy(my_hit);
    EXPECT_EQ(my_hit.GetPosition(), my_hit_copy.GetPosition());
    EXPECT_EQ(my_hit.GetMomentum(), my_hit_copy.GetMomentum());
    EXPECT_EQ(my_hit.GetParticleId(), my_hit_copy.GetParticleId());
    EXPECT_EQ(my_hit.GetTrackId(), my_hit_copy.GetTrackId());
    EXPECT_EQ(my_hit.GetTime(), my_hit_copy.GetTime());
    EXPECT_EQ(my_hit.GetEnergy(), my_hit_copy.GetEnergy());
    EXPECT_EQ(my_hit.GetCharge(), my_hit_copy.GetCharge());
    EXPECT_EQ(my_hit.GetEnergyDeposited(), my_hit_copy.GetEnergyDeposited());
    EXPECT_NE(my_hit.GetChannelId(), my_hit_copy.GetChannelId());
    EXPECT_NE(channel_id_null, my_hit_copy.GetChannelId());

    // equality operator
    Hit my_hit_equal;
    my_hit_equal = my_hit;
    EXPECT_EQ(my_hit.GetPosition(), my_hit_equal.GetPosition());
    EXPECT_EQ(my_hit.GetMomentum(), my_hit_equal.GetMomentum());
    EXPECT_EQ(my_hit.GetParticleId(), my_hit_equal.GetParticleId());
    EXPECT_EQ(my_hit.GetTime(), my_hit_equal.GetTime());
    EXPECT_EQ(my_hit.GetEnergy(), my_hit_equal.GetEnergy());
    EXPECT_EQ(my_hit.GetEnergyDeposited(), my_hit_equal.GetEnergyDeposited());
    EXPECT_NE(my_hit.GetChannelId(), my_hit_equal.GetChannelId());
    EXPECT_NE(channel_id_null, my_hit_equal.GetChannelId());

    // test that we free okay on reallocation
    my_hit_equal = my_hit;

    // test that equality on myself doesn't cause trouble
    my_hit_equal = my_hit_equal;

    // check that we handle copy of NULL channel id okay
    // (i.e. don't attempt to copy!)
    Hit default_hit;
    my_hit_equal = default_hit;
    EXPECT_EQ(channel_id_null, my_hit_equal.GetChannelId());
}

TEST(MCEventTest, MCEventTest) {
    Primary* prim_1 = new Primary();
    Primary* prim_2 = new Primary();
    Primary* prim_null = NULL;

    std::vector<VirtualHit>* virt_1 = new std::vector<VirtualHit>();
    std::vector<VirtualHit>* virt_2 = new std::vector<VirtualHit>();
    std::vector<VirtualHit>* virt_null = NULL;

    std::vector<Hit>* hit_1 = new std::vector<Hit>();
    std::vector<Hit>* hit_2 = new std::vector<Hit>();
    std::vector<Hit>* hit_null = NULL;

    std::vector<Track>* track_1 = new std::vector<Track>();
    std::vector<Track>* track_2 = new std::vector<Track>();
    std::vector<Track>* track_null = NULL;

    MCEvent my_event;
    // check allocation from NULL okay
    my_event.SetPrimary(prim_1);
    my_event.SetVirtualHits(virt_1);
    my_event.SetHits(hit_1);
    my_event.SetTracks(track_1);
    EXPECT_EQ(my_event.GetPrimary(), prim_1);
    EXPECT_EQ(my_event.GetVirtualHits(), virt_1);
    EXPECT_EQ(my_event.GetHits(), hit_1);
    EXPECT_EQ(my_event.GetTracks(), track_1);

    // check reallocation okay
    my_event.SetPrimary(prim_2);
    my_event.SetVirtualHits(virt_2);
    my_event.SetHits(hit_2);
    my_event.SetTracks(track_2);
    EXPECT_EQ(my_event.GetPrimary(), prim_2);
    EXPECT_EQ(my_event.GetVirtualHits(), virt_2);
    EXPECT_EQ(my_event.GetHits(), hit_2);
    EXPECT_EQ(my_event.GetTracks(), track_2);

    // check copy constructor (deep copy)
    MCEvent my_event_copy(my_event);
    EXPECT_NE(my_event.GetPrimary(), my_event_copy.GetPrimary());
    EXPECT_NE(my_event.GetVirtualHits(), my_event_copy.GetVirtualHits());
    EXPECT_NE(my_event.GetHits(), my_event_copy.GetHits());
    EXPECT_NE(my_event.GetTracks(), my_event_copy.GetTracks());
    EXPECT_NE(prim_null, my_event_copy.GetPrimary());
    EXPECT_NE(virt_null, my_event_copy.GetVirtualHits());
    EXPECT_NE(hit_null, my_event_copy.GetHits());
    EXPECT_NE(track_null, my_event_copy.GetTracks());

    // check equality operator (deep copy)
    MCEvent my_event_equal;
    my_event_equal = my_event;
    EXPECT_NE(my_event.GetPrimary(), my_event_equal.GetPrimary());
    EXPECT_NE(my_event.GetVirtualHits(), my_event_equal.GetVirtualHits());
    EXPECT_NE(my_event.GetHits(), my_event_equal.GetHits());
    EXPECT_NE(my_event.GetTracks(), my_event_equal.GetTracks());
    EXPECT_NE(prim_null, my_event_equal.GetPrimary());
    EXPECT_NE(virt_null, my_event_equal.GetVirtualHits());
    EXPECT_NE(hit_null, my_event_equal.GetHits());
    EXPECT_NE(track_null, my_event_equal.GetTracks());

    // test that we free okay on reallocation
    my_event_equal = my_event;

    // test that equality on myself doesn't cause trouble
    my_event_equal = my_event_equal;

    // check that we handle copy of NULL data okay
    // (i.e. don't attempt to copy!)
    MCEvent default_event;
    my_event_equal = default_event;
    EXPECT_EQ(prim_null, my_event_equal.GetPrimary());
    EXPECT_EQ(virt_null, my_event_equal.GetVirtualHits());
    EXPECT_EQ(hit_null, my_event_equal.GetHits());
    EXPECT_EQ(track_null, my_event_equal.GetTracks());
}
}

