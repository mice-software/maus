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

#include "DataStructure/DAQData.hh"


namespace MAUS {

DAQData::DAQData()
    : _event_size(0), _V830(), _trigger_request(), _tof1(), _ckov(), _tof2(), _unknown(),
      _kl(), _tag(), _tof0(), _trigger(), _emr(), _tracker0(), _tracker1() {
}

DAQData::DAQData(const DAQData& daqdata)
    : _event_size(0), _V830(), _trigger_request(), _tof1(), _ckov(), _tof2(), _unknown(),
      _kl(), _tag(), _tof0(), _trigger(), _emr(), _tracker0(), _tracker1() {
    *this = daqdata;
}

DAQData& DAQData::operator=(const DAQData& daqdata) {
    using DAQDataHelper::VectorDeepcopy;
    if (this == &daqdata) {
        return *this;
    }
    _event_size = daqdata._event_size;
    SetV830(daqdata._V830);
    SetTriggerRequestArray(VectorDeepcopy<>(daqdata._trigger_request));
    SetTOF1DaqArray(VectorDeepcopy<>(daqdata._tof1));
    SetCkovArray(VectorDeepcopy<>(daqdata._ckov));
    SetTOF2DaqArray(VectorDeepcopy<>(daqdata._tof2));
    SetUnknownArray(VectorDeepcopy<>(daqdata._unknown));
    SetKLArray(VectorDeepcopy<>(daqdata._kl));
    SetTagArray(VectorDeepcopy<>(daqdata._tag));
    SetTOF0DaqArray(VectorDeepcopy<>(daqdata._tof0));
    SetTriggerArray(VectorDeepcopy<>(daqdata._trigger));
    SetEMRDaq(daqdata._emr);
    SetTracker0DaqArray(VectorDeepcopy<>(daqdata._tracker0));
    SetTracker1DaqArray(VectorDeepcopy<>(daqdata._tracker1));

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
    SetTracker0DaqArray(Tracker0DaqArray());
    SetTracker1DaqArray(Tracker1DaqArray());
}

unsigned int DAQData::GetEventSize() const {
    return _event_size;
}

void DAQData::SetEventSize(unsigned int size) {
    _event_size = size;
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

TriggerRequestArray* DAQData::GetTriggerRequestArrayPtr() {
    return &_trigger_request;
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

TOF1DaqArray* DAQData::GetTOF1DaqArrayPtr() {
    return &_tof1;
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

CkovArray* DAQData::GetCkovArrayPtr() {
    return &_ckov;
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

TOF2DaqArray* DAQData::GetTOF2DaqArrayPtr() {
    return &_tof2;
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

Tracker0DaqArray DAQData::GetTracker0DaqArray() const {
    return _tracker0;
}

Tracker0DaqArray* DAQData::GetTracker0DaqArrayPtr() {
    return &_tracker0;
}

TrackerDaq* DAQData::GetTracker0DaqArrayElement(size_t index) const {
    return _tracker0[index];
}

size_t DAQData::GetTracker0DaqArraySize() const {
    return _tracker0.size();
}

void DAQData::SetTracker0DaqArray(Tracker0DaqArray tracker0) {
    for (size_t i = 0; i < _tracker0.size(); ++i) {
        if (_tracker0[i] != NULL) {
            delete _tracker0[i];
        }
    }
    _tracker0 = tracker0;
}

Tracker1DaqArray DAQData::GetTracker1DaqArray() const {
    return _tracker1;
}

Tracker1DaqArray* DAQData::GetTracker1DaqArrayPtr() {
    return &_tracker1;
}

TrackerDaq* DAQData::GetTracker1DaqArrayElement(size_t index) const {
    return _tracker1[index];
}

size_t DAQData::GetTracker1DaqArraySize() const {
    return _tracker1.size();
}

void DAQData::SetTracker1DaqArray(Tracker1DaqArray tracker1) {
    for (size_t i = 0; i < _tracker1.size(); ++i) {
        if (_tracker1[i] != NULL) {
            delete _tracker1[i];
        }
    }
    _tracker1 = tracker1;
}

UnknownArray DAQData::GetUnknownArray() const {
    return _unknown;
}

UnknownArray* DAQData::GetUnknownArrayPtr() {
    return &_unknown;
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

KLArray* DAQData::GetKLArrayPtr() {
    return &_kl;
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

TagArray* DAQData::GetTagArrayPtr() {
    return &_tag;
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

TOF0DaqArray* DAQData::GetTOF0DaqArrayPtr() {
    return &_tof0;
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

TriggerArray* DAQData::GetTriggerArrayPtr() {
    return &_trigger;
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

EMRDaq DAQData::GetEMRDaq() const {
    return _emr;
}

EMRDaq* DAQData::GetEMRDaqPtr() {
  return &_emr;
}

void DAQData::SetEMRDaq(EMRDaq emr) {
    _emr = emr;
}
}

