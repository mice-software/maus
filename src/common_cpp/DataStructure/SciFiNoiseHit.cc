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

#include "src/common_cpp/DataStructure/SciFiNoiseHit.hh"

namespace MAUS {

SciFiNoiseHit::SciFiNoiseHit():_spill(0), _event(0),
                               _tracker(0), _station(0), _plane(0),
                               _channel(0), _npe(0.), _time(0.), _used(false),
                               _digit_id(0) {
}

SciFiNoiseHit::SciFiNoiseHit(int spill, int event, int tracker, int station,
                       int plane, int channel, double npe, double time )
                       :_spill(spill), _event(event),
                        _tracker(tracker), _station(station), _plane(plane),
                        _channel(channel), _npe(npe), _time(time), _used(false),
                        _digit_id(0) {
}

SciFiNoiseHit::SciFiNoiseHit(const SciFiNoiseHit& noise)
    : _spill(noise._spill), _event(noise._event),
      _tracker(noise._tracker), _station(noise._station), _plane(noise._plane),
      _channel(noise._channel), _npe(noise._npe), _time(noise._npe), _used(noise._used),
      _digit_id(noise._digit_id) {
}

SciFiNoiseHit::~SciFiNoiseHit() {
}

SciFiNoiseHit& SciFiNoiseHit::operator=(const SciFiNoiseHit& noise) {
    if (this == &noise) {
        return *this;
    }
    _spill = noise._spill;
    _event = noise._event;
    _tracker = noise._tracker;
    _station = noise._station;
    _plane = noise._plane;
    _channel = noise._channel;
    _npe = noise._npe;
    _time = noise._time;
    _used = noise._used;
    _digit_id = noise._digit_id;
    return *this;
}

} // ~namespace MAUS
