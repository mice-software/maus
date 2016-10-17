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

#include "src/common_cpp/DataStructure/TriggerEngine.hh"


namespace MAUS {

TriggerEngine::TriggerEngine()
    : _ldc_id(0), _phys_event_number(0),
      _time_stamp(0), _geo(0), _ptriggers() {
}

TriggerEngine::TriggerEngine(const TriggerEngine& trigger)
    : _ldc_id(0), _phys_event_number(0),
      _time_stamp(0), _geo(0), _ptriggers() {
    *this = trigger;
}

TriggerEngine& TriggerEngine::operator=(const TriggerEngine& trigger) {
    if (this == &trigger) {
        return *this;
    }

    SetLdcId(trigger._ldc_id);
    SetPhysEventNumber(trigger._phys_event_number);
    SetTimeStamp(trigger._time_stamp);
    SetGeo(trigger._geo);
    SetParticleTriggerArray(trigger._ptriggers);

    return *this;
}

TriggerEngine::~TriggerEngine() {
}

int TriggerEngine::GetLdcId() const {
    return _ldc_id;
}

void TriggerEngine::SetLdcId(int ldc_id) {
    _ldc_id = ldc_id;
}

int TriggerEngine::GetPhysEventNumber() const {
    return _phys_event_number;
}

void TriggerEngine::SetPhysEventNumber(int phys_event_number) {
    _phys_event_number = phys_event_number;
}

int TriggerEngine::GetTimeStamp() const {
    return _time_stamp;
}

void TriggerEngine::SetTimeStamp(int time_stamp) {
    _time_stamp = time_stamp;
}

int TriggerEngine::GetGeo() const {
    return _geo;
}

void TriggerEngine::SetGeo(int geo) {
    _geo = geo;
}
}

