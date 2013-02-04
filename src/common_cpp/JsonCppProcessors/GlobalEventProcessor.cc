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

#include "src/common_cpp/JsonCppProcessors/GlobalEventProcessor.hh"

namespace MAUS {

GlobalEventProcessor::GlobalEventProcessor() :
    _global_spacepoint_array_proc(
        new MAUS::Processor::Global::SpacePointProcessor()),
    _global_trackpoint_array_proc(
        new MAUS::Processor::Global::TrackPointProcessor()),
    _global_track_array_proc(
        new MAUS::Processor::Global::TrackProcessor()),
    _global_primarychain_array_proc(
        new MAUS::Processor::Global::PrimaryChainProcessor())
{

  RegisterPointerBranch("spacepoints", &_global_spacepoint_array_proc,
                        &GlobalEvent::get_spacepoints,
                        &GlobalEvent::set_spacepoints, false); 
                        
  RegisterPointerBranch("trackpoints", &_global_trackpoint_array_proc,
                        &GlobalEvent::get_trackpoints,
                        &GlobalEvent::set_trackpoints, false); 
                        
  RegisterPointerBranch("tracks", &_global_track_array_proc,
                        &GlobalEvent::get_tracks,
                        &GlobalEvent::set_tracks, false); 
                        
  RegisterPointerBranch("primarychains", &_global_primarychain_array_proc,
                        &GlobalEvent::get_primarychains,
                        &GlobalEvent::set_primarychains, false); 
                        
}
}  // namespace MAUS


