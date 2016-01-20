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

#include "JsonCppProcessors/EMRSpillDataProcessor.hh"

namespace MAUS {

EMRSpillDataProcessor::EMRSpillDataProcessor()
      : _bar_hit_array_proc(new EMRBarHitProcessor),
	_event_track_array_proc(new EMREventTrackProcessor) {
    RegisterValueBranch
	  ("bar_hits", &_bar_hit_array_proc, &EMRSpillData::GetEMRBarHitArray,
           &EMRSpillData::SetEMRBarHitArray, false);
    RegisterValueBranch
	  ("event_tracks", &_event_track_array_proc, &EMRSpillData::GetEMREventTrackArray,
           &EMRSpillData::SetEMREventTrackArray, false);
}
} // namespace MAUS
