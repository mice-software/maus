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

#include "src/common_cpp/JsonCppProcessors/HitProcessor.hh"

#include "src/common_cpp/JsonCppProcessors/SciFiChannelIdProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/TOFChannelIdProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/CkovChannelIdProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/KLChannelIdProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/EMRChannelIdProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/SpecialVirtualChannelIdProcessor.hh"

namespace MAUS {

template <>
HitProcessor<SciFiChannelId>::HitProcessor()
    : _channel_id_proc(new SciFiChannelIdProcessor()) {
    this->RegisterBranches();
}

template <>
HitProcessor<TOFChannelId>::HitProcessor()
    : _channel_id_proc(new TOFChannelIdProcessor()) {
    this->RegisterBranches();
}

template <>
HitProcessor<CkovChannelId>::HitProcessor()
    : _channel_id_proc(new CkovChannelIdProcessor()) {
    this->RegisterBranches();
}

template <>
HitProcessor<KLChannelId>::HitProcessor()
    : _channel_id_proc(new KLChannelIdProcessor()) {
    this->RegisterBranches();
}

template <>
HitProcessor<EMRChannelId>::HitProcessor()
    : _channel_id_proc(new EMRChannelIdProcessor()) {
    this->RegisterBranches();
}

template <>
HitProcessor<SpecialVirtualChannelId>::HitProcessor()
    : _channel_id_proc(new SpecialVirtualChannelIdProcessor()) {
    this->RegisterBranches();
}
}


