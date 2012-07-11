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

#include "src/common_cpp/DataStructure/DAQData.hh"


namespace MAUS {

DAQData::DAQData()
    : _V830(), _trigger_request(), _tof1(), _ckov(), _tof2(), _unknown(),
      _kl(), _tag(), _tof0(), _trigger() {
}

DAQData::DAQData(const DAQData& _daqdata)
    : _V830(), _trigger_request(), _tof1(), _ckov(), _tof2(), _unknown(),
      _kl(), _tag(), _tof0(), _trigger() {
    *this = _daqdata;
}

DAQData& DAQData::operator=(const DAQData& _daqdata) {
    if (this == &_daqdata) {
        return *this;
    }
    SetV830(_daqdata._V830);
    SetTriggerRequestArray(_daqdata._trigger_request);
    SetTOF1DaqArray(_daqdata._tof1);
    SetCkovArray(_daqdata._ckov);
    SetTOF2DaqArray(_daqdata._tof2);
    SetUnknownArray(_daqdata._unknown);
    SetKLArray(_daqdata._kl);
    SetTagArray(_daqdata._tag);
    SetTOF0DaqArray(_daqdata._tof0);
    SetTriggerArray(_daqdata._trigger);
    return *this;
}

DAQData::~DAQData() {
    SetTriggerRequestArray(TriggerRequestArray());
    SetTOF1DaqArray(TOF1DaqArray());
    SetCkovArray(CkovArray());
    SetTOF2DaqArray(TOF2DaqArray());
    SetUnknownArray(UnknownArray());
    SetKLArray(KLArray());
    SetTagArray(TagArray());
    SetTOF0DaqArray(TOF0DaqArray());
    SetTriggerArray(TriggerArray());
}

V830 DAQData::GetV830() const {
    return _V830;
}

void DAQData::SetV830(V830 V830) {
    _V830 = V830;
}

TriggerRequestArray DAQData::GetTriggerRequestArray() const {
    return _trigger_request;
}

TriggerRequest* DAQData::GetTriggerRequestArrayElement(size_t index) const {
    return _trigger_request[index];
}

size_t DAQData::GetTriggerRequestArraySize() const {
    return _trigger_request.size();
}

void DAQData::SetTriggerRequestArray(TriggerRequestArray trigger_request) {
    for (size_t i = 0; i < _trigger_request.size(); ++i) {
        if (_trigger_request[i] != NULL) {
            delete _trigger_request[i];
        }
    }
    _trigger_request = trigger_request;
}

TOF1DaqArray DAQData::GetTOF1DaqArray() const {
    return _tof1;
}

TOFDaq* DAQData::GetTOF1DaqArrayElement(size_t index) const {
    return _tof1[index];
}

size_t DAQData::GetTOF1DaqArraySize() const {
    return _tof1.size();
}

void DAQData::SetTOF1DaqArray(TOF1DaqArray tof1) {
    for (size_t i = 0; i < _tof1.size(); ++i) {
        if (_tof1[i] != NULL) {
            delete _tof1[i];
        }
    }
    _tof1 = tof1;
}

CkovArray DAQData::GetCkovArray() const {
    return _ckov;
}

CkovDaq* DAQData::GetCkovArrayElement(size_t index) const {
    return _ckov[index];
}

size_t DAQData::GetCkovArraySize() const {
    return _ckov.size();
}

void DAQData::SetCkovArray(CkovArray ckov) {
    for (size_t i = 0; i < _ckov.size(); ++i) {
        if (_ckov[i] != NULL) {
            delete _ckov[i];
        }
    }
    _ckov = ckov;
}

TOF2DaqArray DAQData::GetTOF2DaqArray() const {
    return _tof2;
}

TOFDaq* DAQData::GetTOF2DaqArrayElement(size_t index) const {
    return _tof2[index];
}

size_t DAQData::GetTOF2DaqArraySize() const {
    return _tof2.size();
}

void DAQData::SetTOF2DaqArray(TOF2DaqArray tof2) {
    for (size_t i = 0; i < _tof2.size(); ++i) {
        if (_tof2[i] != NULL) {
            delete _tof2[i];
        }
    }
    _tof2 = tof2;
}

UnknownArray DAQData::GetUnknownArray() const {
    return _unknown;
}

Unknown* DAQData::GetUnknownArrayElement(size_t index) const {
    return _unknown[index];
}

size_t DAQData::GetUnknownArraySize() const {
    return _unknown.size();
}

void DAQData::SetUnknownArray(UnknownArray unknown) {
    for (size_t i = 0; i < _unknown.size(); ++i) {
        if (_unknown[i] != NULL) {
            delete _unknown[i];
        }
    }
    _unknown = unknown;
}

KLArray DAQData::GetKLArray() const {
    return _kl;
}

KLDaq* DAQData::GetKLArrayElement(size_t index) const {
    return _kl[index];
}

size_t DAQData::GetKLArraySize() const {
    return _kl.size();
}

void DAQData::SetKLArray(KLArray kl) {
    for (size_t i = 0; i < _kl.size(); ++i) {
        if (_kl[i] != NULL) {
            delete _kl[i];
        }
    }
    _kl = kl;
}

TagArray DAQData::GetTagArray() const {
    return _tag;
}

Tag* DAQData::GetTagArrayElement(size_t index) const {
    return _tag[index];
}

size_t DAQData::GetTagArraySize() const {
    return _tag.size();
}

void DAQData::SetTagArray(TagArray tag) {
    for (size_t i = 0; i < _tag.size(); ++i) {
        if (_tag[i] != NULL) {
            delete _tag[i];
        }
    }
    _tag = tag;
}

TOF0DaqArray DAQData::GetTOF0DaqArray() const {
    return _tof0;
}

TOFDaq* DAQData::GetTOF0DaqArrayElement(size_t index) const {
    return _tof0[index];
}

size_t DAQData::GetTOF0DaqArraySize() const {
    return _tof0.size();
}

void DAQData::SetTOF0DaqArray(TOF0DaqArray tof0) {
    for (size_t i = 0; i < _tof0.size(); ++i) {
        if (_tof0[i] != NULL) {
            delete _tof0[i];
        }
    }
    _tof0 = tof0;
}

TriggerArray DAQData::GetTriggerArray() const {
    return _trigger;
}

Trigger* DAQData::GetTriggerArrayElement(size_t index) const {
    return _trigger[index];
}

size_t DAQData::GetTriggerArraySize() const {
    return _trigger.size();
}

void DAQData::SetTriggerArray(TriggerArray trigger) {
    for (size_t i = 0; i < _trigger.size(); ++i) {
        if (_trigger[i] != NULL) {
            delete _trigger[i];
        }
    }
    _trigger = trigger;
}
}

