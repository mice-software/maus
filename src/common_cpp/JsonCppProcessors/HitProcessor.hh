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

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_HITPROCESSOR_HH_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_HITPROCESSOR_HH_

#include "json/value.h"

#include "src/common_cpp/JsonCppProcessors/PrimaryProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/ObjectProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/ProcessorBase.hh"

#include "src/common_cpp/DataStructure/Hit.hh"

namespace MAUS {

/** @class Hit processor for converting json <-> cpp data */
template <class ChannelId>
class HitProcessor : public ObjectProcessor<Hit<ChannelId> > {
 public:
    /** Constructor - registers the branch structure */
    HitProcessor();
    /** delete the channel_id processor */
    ~HitProcessor();

    void RegisterBranches();

 private:
    ProcessorBase<ChannelId>* _channel_id_proc;
    ThreeVectorProcessor _three_vec_proc;
    DoubleProcessor _double_proc;
    IntProcessor _int_proc;
};

typedef HitProcessor<SciFiChannelId> SciFiHitProcessor;
typedef HitProcessor<TOFChannelId> TOFHitProcessor;
typedef HitProcessor<CkovChannelId> CkovHitProcessor;
typedef HitProcessor<KLChannelId> KLHitProcessor;
typedef HitProcessor<EMRChannelId> EMRHitProcessor;
typedef HitProcessor<SpecialVirtualChannelId> SpecialVirtualHitProcessor;

template <class ChannelId>
void HitProcessor<ChannelId>::RegisterBranches() {
    this->RegisterValueBranch("part_ev_id", &_int_proc,
                        &Hit<ChannelId>::GetPartEvId,
                        &Hit<ChannelId>::SetPartEvId, true);
    this->RegisterValueBranch("track_id", &_int_proc,
                        &Hit<ChannelId>::GetTrackId,
                        &Hit<ChannelId>::SetTrackId, true);
    this->RegisterValueBranch("particle_id", &_int_proc,
                        &Hit<ChannelId>::GetParticleId,
                        &Hit<ChannelId>::SetParticleId, true);
    this->RegisterValueBranch("path_length", &_double_proc,
                        &Hit<ChannelId>::GetPathLength,
                        &Hit<ChannelId>::SetPathLength, true);
    this->RegisterValueBranch("energy", &_double_proc,
                        &Hit<ChannelId>::GetEnergy,
                        &Hit<ChannelId>::SetEnergy, true);
    this->RegisterValueBranch("charge", &_double_proc,
                        &Hit<ChannelId>::GetCharge,
                        &Hit<ChannelId>::SetCharge, true);
    this->RegisterValueBranch("mass", &_double_proc,
                        &Hit<ChannelId>::GetMass,
                        &Hit<ChannelId>::SetMass, true);
    this->RegisterValueBranch("time", &_double_proc,
                        &Hit<ChannelId>::GetTime,
                        &Hit<ChannelId>::SetTime, true);
    this->RegisterValueBranch("energy_deposited", &_double_proc,
                        &Hit<ChannelId>::GetEnergyDeposited,
                        &Hit<ChannelId>::SetEnergyDeposited, true);
    this->RegisterValueBranch("position", &_three_vec_proc,
                        &Hit<ChannelId>::GetPosition,
                        &Hit<ChannelId>::SetPosition, true);
    this->RegisterValueBranch("momentum", &_three_vec_proc,
                        &Hit<ChannelId>::GetMomentum,
                        &Hit<ChannelId>::SetMomentum, true);
    this->RegisterPointerBranch("channel_id", _channel_id_proc,
                        &Hit<ChannelId>::GetChannelId,
                        &Hit<ChannelId>::SetChannelId, true);
}

template <class ChannelId>
HitProcessor<ChannelId>::~HitProcessor() {
    if (_channel_id_proc != NULL) {
        delete _channel_id_proc;
    }
}
}

#endif

