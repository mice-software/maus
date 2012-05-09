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

#include "src/common_cpp/DataStructure/TriggerEvent.hh"


namespace MAUS {

TriggerEvent::TriggerEvent() {
}

TriggerEvent::TriggerEvent(const TriggerEvent& _triggerevent) {
    *this = _triggerevent;
}

TriggerEvent& TriggerEvent::operator=(const TriggerEvent& _triggerevent) {
    if (this == &_triggerevent) {
        return *this;
    }
    return *this;
}

TriggerEvent::~TriggerEvent() {
}
}

