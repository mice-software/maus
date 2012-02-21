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

#include "src/common_cpp/DataStructure/Track.hh"
#include "src/common_cpp/DataStructure/Hit.hh"
#include "src/common_cpp/DataStructure/VirtualHit.hh"
#include "src/common_cpp/DataStructure/Primary.hh"

#include "src/common_cpp/DataStructure/MCEvent.hh"

namespace MAUS {

MCEvent::MCEvent()
       : _primary(NULL), _virtuals(NULL), _hits(NULL), _tracks(NULL) {
}

MCEvent::MCEvent(const MCEvent& md)
       : _primary(NULL), _virtuals(NULL), _hits(NULL), _tracks(NULL) {
  *this = md;
}
       
MCEvent& MCEvent::operator=(const MCEvent& md) {
    if (this == &md) {
        return *this;
    }
    _tracks = new TrackArray(*md._tracks);
    _virtuals = new VirtualHitArray(*md._virtuals);
    _hits = new HitArray(*md._hits);
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
    if (_hits != NULL) {
        delete _hits;
        _hits = NULL;
    }
    if (_primary != NULL) {
        delete _primary;
        _primary = NULL;
    }
}

TrackArray* MCEvent::GetTracks() const {
    return _tracks;
}

void MCEvent::SetTracks(TrackArray* tracks) {
    _tracks = tracks;
}

VirtualHitArray* MCEvent::GetVirtualHits() const {
    return _virtuals;
}

void MCEvent::SetVirtualHits(VirtualHitArray* hits) {
    _virtuals = hits;
}

HitArray* MCEvent::GetHits() const {
    return _hits;
}

void MCEvent::SetHits(HitArray* hits) {
    _hits = hits;
}

Primary* MCEvent::GetPrimary() const {
    return _primary;
}

void MCEvent::SetPrimary(Primary* primary) {
    _primary = primary;
}


}

