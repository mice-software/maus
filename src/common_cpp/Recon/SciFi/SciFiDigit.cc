/* This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
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
 *
 */

#include "src/common_cpp/Recon/SciFi/SciFiDigit.hh"

// namespace MAUS {

SciFiDigit::SciFiDigit():_used(false) {}

SciFiDigit::SciFiDigit(int spill, int event, int tracker, int station,
                       int plane, int channel, double npe, double time )
                       :_spill(spill), _event(event),
                        _tracker(tracker), _station(station), _plane(plane),
                        _channel(channel), _npe(npe), _time(time), _used(false) {
}

SciFiDigit::~SciFiDigit() {}

SciFiDigit::SciFiDigit(const SciFiDigit &digit) {
  _position = digit.get_true_position();
  _p        = digit.get_true_momentum();
  _spill    = digit.get_spill();
  _event    = digit.get_event();
  // _adc      = digit.get_adc();
  // _tdc      = digit.get_tdc();
  _tracker  = digit.get_tracker();
  _station  = digit.get_station();
  _plane    = digit.get_plane();
  _channel  = digit.get_channel();
  _npe      = digit.get_npe();
  _time     = digit.get_time();
  _used     = digit.is_used();
}

SciFiDigit& SciFiDigit::operator=(const SciFiDigit &digit) {
  _position = digit.get_true_position();
  _p        = digit.get_true_momentum();
  _spill    = digit.get_spill();
  _event    = digit.get_event();
  // _adc      = digit.get_adc();
  // _tdc      = digit.get_tdc();
  _tracker  = digit.get_tracker();
  _station  = digit.get_station();
  _plane    = digit.get_plane();
  _channel  = digit.get_channel();
  _npe      = digit.get_npe();
  _time     = digit.get_time();
  _used     = digit.is_used();
}

// } // ~namespace MAUS

