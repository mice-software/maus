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

#include "src/common_cpp/DataStructure/SciFiDigit.hh"


namespace MAUS {

SciFiDigit::SciFiDigit():_spill(0), _event(0),
                         _tracker(0), _station(0), _plane(0),
                         _channel(0), _npe(0.), _time(0.), _adc(0), _used(false) {
  // Do nothing
}

SciFiDigit::SciFiDigit(int spill, int event, int tracker, int station,
                       int plane, int channel, double npe, double time )
                       :_spill(spill), _event(event),
                        _tracker(tracker), _station(station), _plane(plane),
                        _channel(channel), _npe(npe), _time(time), _adc(0), _used(false) {
  // Do nothing
}

SciFiDigit::SciFiDigit(const SciFiDigit& dig)
          : _spill(0), _event(0), _tracker(0), _station(0), _plane(0), _channel(0),
            _npe(0.0), _time(0.0), _adc(0), _used(false) {
  *this = dig;
}

SciFiDigit& SciFiDigit::operator=(const SciFiDigit& _scifidigit) {
    if (this == &_scifidigit) {
        return *this;
    }
    _spill   = _scifidigit._spill;
    _event   = _scifidigit._event;
    _tracker = _scifidigit._tracker;
    _station = _scifidigit._station;
    _plane   = _scifidigit._plane;
    _channel = _scifidigit._channel;
    _npe     = _scifidigit._npe;
    _time    = _scifidigit._time;
    _adc     = _scifidigit._adc;
    _used    = _scifidigit._used;
    return *this;
}

SciFiDigit::~SciFiDigit() {
}

} // ~namespace MAUS
