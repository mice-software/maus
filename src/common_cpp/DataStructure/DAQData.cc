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
    : _V830(), _trigger_request(), _tof1(), _ckov(), _tof2(), _tof0(), _kl(),
      _tag(), _unknown(), _trigger() {
}

DAQData::DAQData(const DAQData& _daqdata)
    : _V830(), _trigger_request(), _tof1(), _ckov(), _tof2(), _tof0(), _kl(),
      _tag(), _unknown(), _trigger() {
    *this = _daqdata;
}

DAQData& DAQData::operator=(const DAQData& _daqdata) {
    if (this == &_daqdata) {
        return *this;
    }
    SetV830(_daqdata._V830);
    SetTriggerRequestArray(_daqdata._trigger_request);
    SetTOF1Array(_daqdata._tof1);
    SetCkovArray(_daqdata._ckov);
    SetTOF2Array(_daqdata._tof2);
    SetTOF0Array(_daqdata._tof0);
    SetKLArray(_daqdata._kl);
    SetTagArray(_daqdata._tag);
    SetUnknownArray(_daqdata._unknown);
    SetTriggerArray(_daqdata._trigger);
    return *this;
}

DAQData::~DAQData() {
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

TriggerRequest DAQData::GetTriggerRequestArrayElement(size_t index) const{
    return _trigger_request[index];
}

size_t DAQData::GetTriggerRequestArraySize() const {
    return _trigger_request.size();
}

void DAQData::SetTriggerRequestArray(TriggerRequestArray trigger_request) {
    _trigger_request = trigger_request;
}

TOF1Array DAQData::GetTOF1Array() const {
    return _tof1;
}

TOFDaq DAQData::GetTOF1ArrayElement(size_t index) const{
    return _tof1[index];
}

size_t DAQData::GetTOF1ArraySize() const {
    return _tof1.size();
}

void DAQData::SetTOF1Array(TOF1Array tof1) {
    _tof1 = tof1;
}

CkovArray DAQData::GetCkovArray() const {
    return _ckov;
}

CkovDaq DAQData::GetCkovArrayElement(size_t index) const{
    return _ckov[index];
}

size_t DAQData::GetCkovArraySize() const {
    return _ckov.size();
}

void DAQData::SetCkovArray(CkovArray ckov) {
    _ckov = ckov;
}

TOF2Array DAQData::GetTOF2Array() const {
    return _tof2;
}

TOFDaq DAQData::GetTOF2ArrayElement(size_t index) const{
    return _tof2[index];
}

size_t DAQData::GetTOF2ArraySize() const {
    return _tof2.size();
}

void DAQData::SetTOF2Array(TOF2Array tof2) {
    _tof2 = tof2;
}

TOF0Array DAQData::GetTOF0Array() const {
    return _tof0;
}

TOFDaq DAQData::GetTOF0ArrayElement(size_t index) const{
    return _tof0[index];
}

size_t DAQData::GetTOF0ArraySize() const {
    return _tof0.size();
}

void DAQData::SetTOF0Array(TOF0Array tof0) {
    _tof0 = tof0;
}

KLArray DAQData::GetKLArray() const {
    return _kl;
}

KLDaq DAQData::GetKLArrayElement(size_t index) const{
    return _kl[index];
}

size_t DAQData::GetKLArraySize() const {
    return _kl.size();
}

void DAQData::SetKLArray(KLArray kl) {
    _kl = kl;
}

TagArray DAQData::GetTagArray() const {
    return _tag;
}

Tag DAQData::GetTagArrayElement(size_t index) const{
    return _tag[index];
}

size_t DAQData::GetTagArraySize() const {
    return _tag.size();
}

void DAQData::SetTagArray(TagArray tag) {
    _tag = tag;
}

UnknownArray DAQData::GetUnknownArray() const {
    return _unknown;
}

Unknown DAQData::GetUnknownArrayElement(size_t index) const{
    return _unknown[index];
}

size_t DAQData::GetUnknownArraySize() const {
    return _unknown.size();
}

void DAQData::SetUnknownArray(UnknownArray unknown) {
    _unknown = unknown;
}

TriggerArray DAQData::GetTriggerArray() const {
    return _trigger;
}

Trigger DAQData::GetTriggerArrayElement(size_t index) const{
    return _trigger[index];
}

size_t DAQData::GetTriggerArraySize() const {
    return _trigger.size();
}

void DAQData::SetTriggerArray(TriggerArray trigger) {
    _trigger = trigger;
}
}

