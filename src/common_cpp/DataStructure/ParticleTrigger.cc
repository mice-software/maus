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

#include "src/common_cpp/DataStructure/ParticleTrigger.hh"


namespace MAUS {

ParticleTrigger::ParticleTrigger()
    : _ptrigger_number(0), _time_stamp(0), _tof0Pattern(0), _tof1Pattern(0), _tof2Pattern(0) {
}

ParticleTrigger::ParticleTrigger(const ParticleTrigger& trigger)
    : _ptrigger_number(0), _time_stamp(0), _tof0Pattern(0), _tof1Pattern(0), _tof2Pattern(0) {
    *this = trigger;
}

ParticleTrigger& ParticleTrigger::operator=(const ParticleTrigger& trigger) {
    if (this == &trigger) {
        return *this;
    }

    SetParticleTriggerNumber(trigger._ptrigger_number);
    SetTimeStamp(trigger._time_stamp);
    SetTof0Pattern(trigger._tof0Pattern);
    SetTof1Pattern(trigger._tof1Pattern);
    SetTof2Pattern(trigger._tof2Pattern);

    return *this;
}

ParticleTrigger::~ParticleTrigger() {
}

int ParticleTrigger::GetParticleTriggerNumber() const {
  return _ptrigger_number;
}

void ParticleTrigger::SetParticleTriggerNumber(int ptrigger_number) {
  _ptrigger_number = ptrigger_number;
}

int ParticleTrigger::GetTimeStamp() const {
    return _time_stamp;
}

void ParticleTrigger::SetTimeStamp(int time_stamp) {
    _time_stamp = time_stamp;
}

void ParticleTrigger::SetTof0Pattern(int pattern) {
  _tof0Pattern = pattern;
}

int ParticleTrigger::GetTof0Pattern() const {
  return _tof0Pattern;
}

void ParticleTrigger::SetTof1Pattern(int pattern) {
  _tof1Pattern = pattern;
}

int ParticleTrigger::GetTof1Pattern() const {
  return _tof1Pattern;
}

void ParticleTrigger::SetTof2Pattern(int pattern) {
  _tof2Pattern = pattern;
}

int ParticleTrigger::GetTof2Pattern() const {
  return _tof2Pattern;
}
}

