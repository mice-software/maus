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

#include "src/common_cpp/DataStructure/MCTrack.hh"
#include "src/common_cpp/DataStructure/SDHit.hh"
#include "src/common_cpp/DataStructure/VirtualHit.hh"
#include "src/common_cpp/DataStructure/Primary.hh"

#include "src/common_cpp/DataStructure/MCEvent.hh"

namespace MAUS {

MCEvent::MCEvent()
       : _primary(NULL), _virtuals(NULL), _sdhits(NULL), _tracks(NULL) {
}

MCEvent::MCEvent(const MCEvent& md)
       : _primary(NULL), _virtuals(NULL), _sdhits(NULL), _tracks(NULL) {
  *this = md;
}
       
MCEvent& MCEvent::operator=(const MCEvent& md) {
    if (this == &md) {
        return *this;
    }
    _tracks = new TrackArray(*md._tracks);
    _virtuals = new VirtualHitArray(*md._virtuals);
    _sdhits = new SDHitArray(*md._sdhits);
    _primary = new Primary(*md._primary);
    return *this;    
}

MCEvent::~MCEvent() {
    if (_tracks != NULL) {
        delete _tracks;
        _tracks = NULL;
    }
    if (_virtuals != NULL) {
        delete _virtuals;
        _virtuals = NULL;
    }
    if (_sdhits != NULL) {
        delete _sdhits;
        _sdhits = NULL;
    }
    if (_primary != NULL) {
        delete _primary;
        _primary = NULL;
    }
}

TrackArray* MCEvent::GetTracks() {
    return _tracks;
}

void MCEvent::SetTracks(TrackArray* tracks) {
    _tracks = tracks;
}

VirtualHitArray* MCEvent::GetVirtualHits() {
    return _virtuals;
}

void MCEvent::SetVirtualHits(VirtualHitArray* hits) {
    _virtuals = hits;
}

SDHitArray* MCEvent::GetSDHits() {
    return _sdhits;
}

void MCEvent::SetSDHits(SDHitArray* hits) {
    _sdhits = hits;
}

Primary* MCEvent::GetPrimary() {
    return _primary;
}

void MCEvent::SetPrimary(Primary* primary) {
    _primary = primary;
}


}

