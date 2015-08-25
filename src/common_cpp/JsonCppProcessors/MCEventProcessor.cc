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

#include "src/common_cpp/JsonCppProcessors/MCEventProcessor.hh"

namespace MAUS {

MCEventProcessor::MCEventProcessor()
    : _sci_fi_hit_proc(new SciFiHitProcessor()),
      _sci_fi_noise_hit_proc(new SciFiNoiseHitProcessor()),
      _tof_hit_proc(new TOFHitProcessor()),
      _ckov_hit_proc(new CkovHitProcessor()),
      _kl_hit_proc(new KLHitProcessor()),
      _emr_hit_proc(new EMRHitProcessor()),
      _special_virtual_hit_proc(new SpecialVirtualHitProcessor()),
      _track_proc(new TrackProcessor()),
      _virtual_hit_proc(new VirtualHitProcessor()) {
    RegisterPointerBranch
        ("primary", &_primary_proc, &MCEvent::GetPrimary,
                                    &MCEvent::SetPrimary, true);
    RegisterPointerBranch
        ("sci_fi_hits", &_sci_fi_hit_proc, &MCEvent::GetSciFiHits,
                                           &MCEvent::SetSciFiHits, false);
    RegisterPointerBranch
        ("sci_fi_noise_hits", &_sci_fi_noise_hit_proc, &MCEvent::GetSciFiNoiseHits,
                                                       &MCEvent::SetSciFiNoiseHits, false);
    RegisterPointerBranch
        ("tof_hits", &_tof_hit_proc, &MCEvent::GetTOFHits,
                                     &MCEvent::SetTOFHits, false);
    RegisterPointerBranch
        ("ckov_hits", &_ckov_hit_proc, &MCEvent::GetCkovHits,
                                       &MCEvent::SetCkovHits, false);
    RegisterPointerBranch
        ("kl_hits", &_kl_hit_proc, &MCEvent::GetKLHits,
                                     &MCEvent::SetKLHits, false);
    RegisterPointerBranch
        ("emr_hits", &_emr_hit_proc, &MCEvent::GetEMRHits,
                                     &MCEvent::SetEMRHits, false);
    RegisterPointerBranch("special_virtual_hits", &_special_virtual_hit_proc,
                          &MCEvent::GetSpecialVirtualHits,
                          &MCEvent::SetSpecialVirtualHits, false);
    RegisterPointerBranch
       ("tracks", &_track_proc, &MCEvent::GetTracks, &MCEvent::SetTracks, false);
    RegisterPointerBranch("virtual_hits", &_virtual_hit_proc,
                      &MCEvent::GetVirtualHits, &MCEvent::SetVirtualHits, false);
}
}
