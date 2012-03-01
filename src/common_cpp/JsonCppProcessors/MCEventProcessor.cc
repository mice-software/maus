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

#include <iostream>

#include "src/common_cpp/JsonCppProcessors/VirtualHitProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/TrackProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/HitProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/MCEventProcessor.hh"

namespace MAUS {

MCEventProcessor::MCEventProcessor()
    : _hit_proc(new HitProcessor()), _track_proc(new TrackProcessor()),
      _virtual_hit_proc(new VirtualHitProcessor()) {
    RegisterPointerBranch("primary", &_primary_proc, &MCEvent::GetPrimary,
                                                    &MCEvent::SetPrimary, true);
    RegisterPointerBranch
               ("hits", &_hit_proc, &MCEvent::GetHits, &MCEvent::SetHits, true);
    RegisterPointerBranch
       ("tracks", &_track_proc, &MCEvent::GetTracks, &MCEvent::SetTracks, true);
    RegisterPointerBranch("virtual_hits", &_virtual_hit_proc,
                      &MCEvent::GetVirtualHits, &MCEvent::SetVirtualHits, true);
}
}

