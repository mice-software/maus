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

#include "src/common_cpp/DataStructure/EpicsInterface.hh"


namespace MAUS {

EpicsInterface::EpicsInterface()
    : _ldc_id(0), _phys_event_number(0), _geo(0) {
}

EpicsInterface::EpicsInterface(const EpicsInterface& ei)
    : _ldc_id(0), _phys_event_number(0), _geo(0) {
    *this = ei;
}

EpicsInterface& EpicsInterface::operator=(const EpicsInterface& ei) {
    if (this == &ei) {
        return *this;
    }

    SetLdcId(ei._ldc_id);
    SetPhysEventNumber(ei._phys_event_number);
    SetGeo(ei._geo);
    SetPvIdArray(ei._pv_id);
    SetPvValueArray(ei._pv_value);

    return *this;
}

EpicsInterface::~EpicsInterface() {
}

int EpicsInterface::GetLdcId() const {
    return _ldc_id;
}

void EpicsInterface::SetLdcId(int ldc_id) {
    _ldc_id = ldc_id;
}

int EpicsInterface::GetPhysEventNumber() const {
    return _phys_event_number;
}

void EpicsInterface::SetPhysEventNumber(int phys_event_number) {
    _phys_event_number = phys_event_number;
}

int EpicsInterface::GetGeo() const {
    return _geo;
}

void EpicsInterface::SetGeo(int geo) {
    _geo = geo;
}

void EpicsInterface::AddHallProbe(int pvId, double v) {
    _pv_id.push_back(pvId);
    _pv_value.push_back(v);
}
}

