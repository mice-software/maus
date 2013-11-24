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

#include "src/common_cpp/DataStructure/SciFiMCLookup.hh"
#include "src/common_cpp/DataStructure/SciFiDigit.hh"

namespace MAUS {

SciFiMCLookup::SciFiMCLookup(): _digit_Id(0) {
}

SciFiMCLookup::SciFiMCLookup(const SciFiMCLookup& lookup)
    : _digit_Id(0) {
	*this = lookup;
}

SciFiMCLookup::SciFiMCLookup(const SciFiDigit& digit) {
	_digit_Id = 1 * digit.get_channel() + 1000 * digit.get_plane() +
                10000 * digit.get_station() + 100000 * digit.get_tracker() +
				1000000 * digit.get_event() + 10000000 * digit.get_spill();
}

SciFiMCLookup::SciFiMCLookup(const SciFiDigit* digit) {
	_digit_Id = 1 * digit->get_channel() + 1000 * digit->get_plane() +
                10000 * digit->get_station() + 100000 * digit->get_tracker() +
				1000000 * digit->get_event() + 10000000 * digit->get_spill();
}

SciFiMCLookup& SciFiMCLookup::operator=(const SciFiMCLookup& lookup) {
    if (this == &lookup) {
        return *this;
    }
	_digit_Id = lookup._digit_Id;
    return *this;
}

SciFiMCLookup::~SciFiMCLookup() {
}

} // ~namespace MAUS
