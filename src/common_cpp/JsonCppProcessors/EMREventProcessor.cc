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

#include "src/common_cpp/JsonCppProcessors/EMRPlaneHitProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/EMREventProcessor.hh"

namespace MAUS {

EMREventProcessor::EMREventProcessor()
      : _plane_hit_array_proc(new EMRPlaneHitProcessor) {
    RegisterValueBranch
	  ("emr_plane_hits", &_plane_hit_array_proc, &EMREvent::GetEMRPlaneHitArray,
           &EMREvent::SetEMRPlaneHitArray, false);
    RegisterValueBranch
          ("initial_trigger", &_bool_proc, &EMREvent::GetInitialTrigger,
          &EMREvent::SetInitialTrigger, false);
    RegisterValueBranch
          ("has_primary", &_bool_proc, &EMREvent::GetHasPrimary,
          &EMREvent::SetHasPrimary, false);
    RegisterValueBranch
          ("range_primary", &_double_proc, &EMREvent::GetRangePrimary,
          &EMREvent::SetRangePrimary, false);
    RegisterValueBranch
          ("has_secondary", &_bool_proc, &EMREvent::GetHasSecondary,
          &EMREvent::SetHasSecondary, false);
    RegisterValueBranch
          ("range_secondary", &_double_proc, &EMREvent::GetRangeSecondary,
          &EMREvent::SetRangeSecondary, false);
    RegisterValueBranch
          ("secondary_to_primary_track_distance", &_double_proc,
	  &EMREvent::GetSecondaryToPrimaryTrackDistance,
          &EMREvent::SetSecondaryToPrimaryTrackDistance, false);
    RegisterValueBranch
          ("total_charge_MA", &_double_proc, &EMREvent::GetTotalChargeMA,
          &EMREvent::SetTotalChargeMA, false);
    RegisterValueBranch
          ("total_charge_SA", &_double_proc, &EMREvent::GetTotalChargeSA,
          &EMREvent::SetTotalChargeSA, false);
    RegisterValueBranch
          ("charge_ratio_MA", &_double_proc, &EMREvent::GetChargeRatioMA,
          &EMREvent::SetChargeRatioMA, false);
    RegisterValueBranch
          ("charge_ratio_SA", &_double_proc, &EMREvent::GetChargeRatioSA,
          &EMREvent::SetChargeRatioSA, false);
    RegisterValueBranch
          ("plane_density_MA", &_double_proc, &EMREvent::GetPlaneDensityMA,
          &EMREvent::SetPlaneDensityMA, false);
    RegisterValueBranch
          ("plane_density_SA", &_double_proc, &EMREvent::GetPlaneDensitySA,
          &EMREvent::SetPlaneDensitySA, false);
    RegisterValueBranch
          ("chi2", &_double_proc, &EMREvent::GetChi2,
          &EMREvent::SetChi2, false);
}
}  // namespace MAUS

