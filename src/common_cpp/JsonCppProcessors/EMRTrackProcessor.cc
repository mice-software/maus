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

#include "src/common_cpp/JsonCppProcessors/EMRTrackProcessor.hh"

namespace MAUS {

EMRTrackProcessor::EMRTrackProcessor()
  : _trackpoint_array_proc(new EMRTrackPointProcessor),
    _double_array_proc(new DoubleProcessor) {
    RegisterValueBranch
          ("track_points", &_trackpoint_array_proc, &EMRTrack::GetEMRTrackPointArray,
           &EMRTrack::SetEMRTrackPointArray, true);
    RegisterValueBranch
          ("parx", &_double_array_proc, &EMRTrack::GetParametersX,
           &EMRTrack::SetParametersX, true);
    RegisterValueBranch
          ("pary", &_double_array_proc, &EMRTrack::GetParametersY,
           &EMRTrack::SetParametersY, true);
    RegisterValueBranch
          ("parx_err", &_double_array_proc, &EMRTrack::GetParametersErrorsX,
           &EMRTrack::SetParametersErrorsX, true);
    RegisterValueBranch
          ("pary_err", &_double_array_proc, &EMRTrack::GetParametersErrorsY,
           &EMRTrack::SetParametersErrorsY, true);
    RegisterValueBranch
          ("origin", &_threevector_proc, &EMRTrack::GetOrigin,
           &EMRTrack::SetOrigin, true);
    RegisterValueBranch
          ("origin_err", &_threevector_proc, &EMRTrack::GetOriginErrors,
           &EMRTrack::SetOriginErrors, true);
    RegisterValueBranch
          ("polar", &_double_proc, &EMRTrack::GetPolar,
           &EMRTrack::SetPolar, true);
    RegisterValueBranch
          ("polar_err", &_double_proc, &EMRTrack::GetPolarError,
           &EMRTrack::SetPolarError, true);
    RegisterValueBranch
          ("azimuth", &_double_proc, &EMRTrack::GetAzimuth,
           &EMRTrack::SetAzimuth, true);
    RegisterValueBranch
          ("azimuth_err", &_double_proc, &EMRTrack::GetAzimuthError,
           &EMRTrack::SetAzimuthError, true);
    RegisterValueBranch
          ("chi2", &_double_proc, &EMRTrack::GetChi2,
           &EMRTrack::SetChi2, true);
    RegisterValueBranch
          ("range", &_double_proc, &EMRTrack::GetRange,
           &EMRTrack::SetRange, true);
    RegisterValueBranch
          ("range_err", &_double_proc, &EMRTrack::GetRangeError,
           &EMRTrack::SetRangeError, true);
    RegisterValueBranch
          ("mom", &_double_proc, &EMRTrack::GetMomentum,
           &EMRTrack::SetMomentum, true);
    RegisterValueBranch
          ("mom_err", &_double_proc, &EMRTrack::GetMomentumError,
           &EMRTrack::SetMomentumError, true);
}
}  // namespace MAUS
