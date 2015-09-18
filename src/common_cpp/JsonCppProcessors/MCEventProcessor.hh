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

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_MCEVENTPROCESSOR_HH_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_MCEVENTPROCESSOR_HH_

#include "json/value.h"

#include "src/common_cpp/JsonCppProcessors/PrimaryProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/ObjectProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/ArrayProcessors.hh"

#include "src/common_cpp/JsonCppProcessors/HitProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/VirtualHitProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/TrackProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/SciFiNoiseHitProcessor.hh"

#include "src/common_cpp/DataStructure/MCEvent.hh"

namespace MAUS {

/** @class MCEventProcessor for converting json <-> cpp MCEvent */
class MCEventProcessor : public ObjectProcessor<MCEvent> {
 public:
    /** Constructor - registers the branch structure */
    MCEventProcessor();

 private:
    ValueArrayProcessor<SciFiHit> _sci_fi_hit_proc;
    ValueArrayProcessor<SciFiNoiseHit> _sci_fi_noise_hit_proc;
    ValueArrayProcessor<TOFHit> _tof_hit_proc;
    ValueArrayProcessor<CkovHit> _ckov_hit_proc;
    ValueArrayProcessor<KLHit> _kl_hit_proc;
    ValueArrayProcessor<EMRHit> _emr_hit_proc;
    ValueArrayProcessor<SpecialVirtualHit> _special_virtual_hit_proc;
    ValueArrayProcessor<Track> _track_proc;
    ValueArrayProcessor<VirtualHit> _virtual_hit_proc;
    PrimaryProcessor _primary_proc;
};
}

#endif
