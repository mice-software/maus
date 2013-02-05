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
#include "src/common_cpp/JsonCppProcessors/GlobalRawTrackProcessor.hh"

namespace MAUS {

GlobalEventProcessor::GlobalEventProcessor()
    : raw_track_array_processor_(new GlobalRawTrackProcessor()) {
  RegisterValueBranch("raw_tracks", &raw_track_array_processor_,
                      &GlobalEvent::raw_tracks,
                      &GlobalEvent::set_raw_tracks, true);
}
}  // namespace MAUS


