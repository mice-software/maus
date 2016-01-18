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

#include "JsonCppProcessors/EMREventProcessor.hh"

namespace MAUS {

EMREventProcessor::EMREventProcessor()
      : _event_track_array_proc(new EMREventTrackProcessor) {
    RegisterValueBranch
	  ("event_tracks", &_event_track_array_proc, &EMREvent::GetEMREventTrackArray,
           &EMREvent::SetEMREventTrackArray, false);
    RegisterValueBranch
	  ("vertex", &_threevector_proc, &EMREvent::GetVertex,
           &EMREvent::SetVertex, false);
    RegisterValueBranch
	  ("vertex_err", &_threevector_proc, &EMREvent::GetVertexErrors,
           &EMREvent::SetVertexErrors, false);
    RegisterValueBranch
          ("deltat", &_double_proc,
	   &EMREvent::GetDeltaT, &EMREvent::SetDeltaT, false);
    RegisterValueBranch
          ("distance", &_double_proc,
	   &EMREvent::GetDistance, &EMREvent::SetDistance, false);
    RegisterValueBranch
          ("polar", &_double_proc,
	   &EMREvent::GetPolar, &EMREvent::SetPolar, false);
    RegisterValueBranch
          ("azimuth", &_double_proc,
	   &EMREvent::GetAzimuth, &EMREvent::SetAzimuth, false);
}
}  // namespace MAUS
