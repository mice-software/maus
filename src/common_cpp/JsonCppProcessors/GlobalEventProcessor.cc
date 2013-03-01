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
    _global_space_point_array_proc(
        new MAUS::Processor::Global::SpacePointProcessor()),
    _global_track_point_array_proc(
        new MAUS::Processor::Global::TrackPointProcessor()),
    _global_track_array_proc(
        new MAUS::Processor::Global::TrackProcessor()),
    _global_primary_chain_array_proc(
        new MAUS::Processor::Global::PrimaryChainProcessor())
{

  RegisterPointerBranch("space_points", &_global_space_point_array_proc,
                        &GlobalEvent::get_space_points,
                        &GlobalEvent::set_space_points, false); 
                        
  RegisterPointerBranch("track_points", &_global_track_point_array_proc,
                        &GlobalEvent::get_track_points,
                        &GlobalEvent::set_track_points, false); 
                        
  RegisterPointerBranch("tracks", &_global_track_array_proc,
                        &GlobalEvent::get_tracks,
                        &GlobalEvent::set_tracks, false); 
                        
  RegisterPointerBranch("primary_chains", &_global_primary_chain_array_proc,
                        &GlobalEvent::get_primary_chains,
                        &GlobalEvent::set_primary_chains, false); 
                        
}
}  // namespace MAUS


