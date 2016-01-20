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

#include "JsonCppProcessors/EMREventTrackProcessor.hh"

namespace MAUS {

EMREventTrackProcessor::EMREventTrackProcessor()
      : _plane_hit_array_proc(new EMRPlaneHitProcessor),
	_space_point_array_proc(new EMRSpacePointProcessor) {
    RegisterValueBranch
	  ("plane_hits", &_plane_hit_array_proc, &EMREventTrack::GetEMRPlaneHitArray,
           &EMREventTrack::SetEMRPlaneHitArray, false);
    RegisterValueBranch
	  ("space_points", &_space_point_array_proc, &EMREventTrack::GetEMRSpacePointArray,
           &EMREventTrack::SetEMRSpacePointArray, false);
    RegisterValueBranch
	  ("track", &_track_proc, &EMREventTrack::GetEMRTrack,
           &EMREventTrack::SetEMRTrack, false);
    RegisterValueBranch
	  ("type", &_string_proc, &EMREventTrack::GetType,
           &EMREventTrack::SetType, false);
    RegisterValueBranch
	  ("id", &_int_proc, &EMREventTrack::GetTrackId,
           &EMREventTrack::SetTrackId, false);
    RegisterValueBranch
	  ("time", &_double_proc, &EMREventTrack::GetTime,
           &EMREventTrack::SetTime, false);
    RegisterValueBranch
	  ("plane_density_ma", &_double_proc, &EMREventTrack::GetPlaneDensityMA,
           &EMREventTrack::SetPlaneDensityMA, false);
    RegisterValueBranch
	  ("plane_density_sa", &_double_proc, &EMREventTrack::GetPlaneDensitySA,
           &EMREventTrack::SetPlaneDensitySA, false);
    RegisterValueBranch
	  ("total_charge_ma", &_double_proc, &EMREventTrack::GetTotalChargeMA,
           &EMREventTrack::SetTotalChargeMA, false);
    RegisterValueBranch
	  ("total_charge_sa", &_double_proc, &EMREventTrack::GetTotalChargeSA,
           &EMREventTrack::SetTotalChargeSA, false);
    RegisterValueBranch
	  ("charge_ratio_ma", &_double_proc, &EMREventTrack::GetChargeRatioMA,
           &EMREventTrack::SetChargeRatioMA, false);
    RegisterValueBranch
	  ("charge_ratio_sa", &_double_proc, &EMREventTrack::GetChargeRatioSA,
           &EMREventTrack::SetChargeRatioSA, false);
}
}  // namespace MAUS
