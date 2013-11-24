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

#include "src/common_cpp/JsonCppProcessors/SciFiNoiseHitProcessor.hh"

namespace MAUS {

SciFiNoiseHitProcessor::SciFiNoiseHitProcessor() {
    RegisterValueBranch("spill", &_int_proc,
                        &SciFiNoiseHit::GetSpill,
                        &SciFiNoiseHit::SetSpill, true);
    RegisterValueBranch("event", &_int_proc,
                        &SciFiNoiseHit::GetEvent,
                        &SciFiNoiseHit::SetEvent, true);
    RegisterValueBranch("station", &_int_proc,
                        &SciFiNoiseHit::GetStation,
                        &SciFiNoiseHit::SetStation, true);
    RegisterValueBranch("tracker", &_int_proc,
                        &SciFiNoiseHit::GetTracker,
                        &SciFiNoiseHit::SetTracker, true);
    RegisterValueBranch("plane", &_int_proc,
                        &SciFiNoiseHit::GetPlane,
                        &SciFiNoiseHit::SetPlane, true);
    RegisterValueBranch("channel", &_int_proc,
                        &SciFiNoiseHit::GetChannel,
                        &SciFiNoiseHit::SetChannel, true);
    RegisterValueBranch("npe", &_double_proc,
                        &SciFiNoiseHit::GetNPE,
                        &SciFiNoiseHit::SetNPE, true);
    RegisterValueBranch("time", &_double_proc,
                        &SciFiNoiseHit::GetTime,
                        &SciFiNoiseHit::SetTime, true);
    RegisterValueBranch("used", &_bool_proc,
                        &SciFiNoiseHit::GetUsed,
                        &SciFiNoiseHit::SetUsed, true);
    RegisterValueBranch("digit_id", &_double_proc,
                        &SciFiNoiseHit::GetID,
                        &SciFiNoiseHit::SetID, true);
}
}
