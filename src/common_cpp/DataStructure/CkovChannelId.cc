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

#include "src/common_cpp/DataStructure/CkovChannelId.hh"

namespace MAUS {

CkovChannelId::CkovChannelId()
    : _station(0), _pmt(0) {
}

CkovChannelId::CkovChannelId(const CkovChannelId& channel)
    :  _station(channel._station), _pmt(channel._pmt) {
}

CkovChannelId& CkovChannelId::operator=(const CkovChannelId& channel) {
    if (this == &channel) {
        return *this;
    }
    _station = channel._station;
    _pmt = channel._pmt;
    return *this;
}

CkovChannelId::~CkovChannelId() {
}


int CkovChannelId::GetStation() const {
    return _station;
}

void CkovChannelId::SetStation(int station) {
    _station = station;
}

int CkovChannelId::GetPMT() const {
    return _pmt;
}

void CkovChannelId::SetPMT(int PMT) {
    _pmt = PMT;
}
}
