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

#include "src/common_cpp/JsonCppProcessors/SciFiTrackProcessor.hh"

namespace MAUS {

SciFiTrackProcessor::SciFiTrackProcessor()
                         : _scifi_trackpoint_array_proc(new SciFiTrackPointProcessor),
                           _vector_proc(new DoubleProcessor) {
    RegisterValueBranch("tracker", &_int_proc,
                        &SciFiTrack::tracker,
                        &SciFiTrack::set_tracker, true);
    RegisterValueBranch("algorithm", &_int_proc,
                        &SciFiTrack::GetAlgorithmUsed,
                        &SciFiTrack::SetAlgorithmUsed, true);
    RegisterValueBranch("charge", &_int_proc,
                        &SciFiTrack::charge,
                        &SciFiTrack::set_charge, true);
    RegisterValueBranch("chi2", &_double_proc,
                        &SciFiTrack::chi2,
                        &SciFiTrack::set_chi2, true);
    RegisterValueBranch("ndf", &_int_proc,
                        &SciFiTrack::ndf,
                        &SciFiTrack::set_ndf, true);
    RegisterValueBranch("P_value", &_double_proc,
                        &SciFiTrack::P_value,
                        &SciFiTrack::set_P_value, true);
    RegisterValueBranch("rating", &_int_proc,
                        &SciFiTrack::GetRating,
                        &SciFiTrack::SetRating, false);
    RegisterIgnoredBranch("trackpoints", false);
    RegisterValueBranch("trackpoints", &_scifi_trackpoint_array_proc,
                        &SciFiTrack::scifitrackpoints,
                        &SciFiTrack::set_scifitrackpoints, true);
    RegisterValueBranch("seed_position", &_threevector_proc,
                        &SciFiTrack::GetSeedPosition,
                        &SciFiTrack::SetSeedPosition, false);
    RegisterValueBranch("seed_momentum", &_threevector_proc,
                        &SciFiTrack::GetSeedMomentum,
                        &SciFiTrack::SetSeedMomentum, false);
    RegisterValueBranch("seed_covariance", &_vector_proc,
                        &SciFiTrack::GetSeedCovariance,
                        &SciFiTrack::SetSeedCovariance, false);
    ObjectProcessor<SciFiTrack>::RegisterTRef("pr_track",
        &SciFiTrack::pr_track_tobject, &SciFiTrack::set_pr_track_tobject, false);
    ObjectProcessor<SciFiTrack>::RegisterTRef("scifi_seed",
        &SciFiTrack::scifi_seed_tobject, &SciFiTrack::set_scifi_seed_tobject, false);
}
} // ~namespace MAUS
