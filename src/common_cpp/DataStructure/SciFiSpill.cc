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
#include "src/common_cpp/DataStructure/SciFiSpill.hh"

namespace MAUS {

// Default constructor
SciFiSpill::SciFiSpill() {
  _scifievents.resize(0);
}

// Copy constructor
SciFiSpill::SciFiSpill(const SciFiSpill& _scifispill) {
  _scifievents.resize(_scifispill._scifievents.size());
  for (unsigned int i = 0; i < _scifispill._scifievents.size(); ++i) {
    _scifievents[i] = new SciFiEvent(*_scifispill._scifievents[i]);
  }
}

// Assignment operator
SciFiSpill& SciFiSpill::operator=(const SciFiSpill& _scifispill) {
  if (this == &_scifispill) {
      return *this;
  }
  _scifievents.resize(_scifispill._scifievents.size());
  for (unsigned int i = 0; i < _scifispill._scifievents.size(); ++i) {
    _scifievents[i] = new SciFiEvent(*_scifispill._scifievents[i]);
  }
  return *this;
}

// Destructor
SciFiSpill::~SciFiSpill() {}

} // ~namespace MAUS
