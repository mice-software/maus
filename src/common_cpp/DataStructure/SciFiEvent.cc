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

#include "src/common_cpp/DataStructure/SciFiEvent.hh"


namespace MAUS {

SciFiEvent::SciFiEvent() {
  _scifihits.resize(0);
  _scifidigits.resize(0);
}

SciFiEvent::SciFiEvent(const SciFiEvent& _scifievent) {
    _scifihits.resize(_scifievent._scifihits.size());
    for (unsigned int i = 0; i < _scifievent._scifihits.size(); ++i) {
      _scifihits[i] = new SciFiHit(*_scifievent._scifihits[i]);
    }
    _scifidigits.resize(_scifievent._scifidigits.size());
    for (unsigned int i = 0; i < _scifievent._scifidigits.size(); ++i) {
      _scifidigits[i] = new SciFiDigit(*_scifievent._scifidigits[i]);
    }
    // *this = _scifievent;
}

SciFiEvent& SciFiEvent::operator=(const SciFiEvent& _scifievent) {
    if (this == &_scifievent) {
        return *this;
    }
    _scifihits.resize(_scifievent._scifihits.size());
    for (unsigned int i = 0; i < _scifievent._scifihits.size(); ++i) {
      _scifihits[i] = new SciFiHit(*_scifievent._scifihits[i]);
    }
    _scifidigits.resize(_scifievent._scifidigits.size());
    for (unsigned int i = 0; i < _scifievent._scifidigits.size(); ++i) {
      _scifidigits[i] = new SciFiDigit(*_scifievent._scifidigits[i]);
    }
    /*
    if ( _scifidigits.size() != 0) {
        for (size_t i = 0; i < _scifidigits.size(); ++i) {
            delete _scifidigits[i];
        }
        _scifidigits.resize(0);
    }
    if (_scifievent._scifidigits.size() == 0) {
         _scifidigits.resize(0);
    } else {
        _scifidigits = SciFiDigitPArray(_scifievent._scifidigits);
    }
    */
    return *this;
}

SciFiEvent::~SciFiEvent() {
  std::vector<SciFiHit*>::iterator hit;
  for (hit = _scifihits.begin(); hit!= _scifihits.end(); ++hit) {
    delete (*hit);
  }
  std::vector<SciFiDigit*>::iterator digit;
  for (digit = _scifidigits.begin(); digit!= _scifidigits.end(); ++digit) {
    delete (*digit);
  }
}

} // ~namespace MAUS
