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

#include "src/common_cpp/DataStructure/TOFEvent.hh"


namespace MAUS {

TOFEvent::TOFEvent() {
}

TOFEvent::TOFEvent(const TOFEvent& _tofevent) {
    *this = _tofevent;
}

TOFEvent& TOFEvent::operator=(const TOFEvent& _tofevent) {
    if (this == &_tofevent) {
        return *this;
    }
    return *this;
}

TOFEvent::~TOFEvent() {
}
}

