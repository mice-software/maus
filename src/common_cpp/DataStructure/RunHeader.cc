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

#include "src/common_cpp/DataStructure/RunHeader.hh"


namespace MAUS {

RunHeader::RunHeader() : _run_number(0) {
}

RunHeader::RunHeader(const RunHeader& _run_header) : _run_number(0) {
    *this = _run_header;
}

RunHeader& RunHeader::operator=(const RunHeader& run_header) {
    if (this == &run_header) {
        return *this;
    }
    _run_number = run_header._run_number;
    return *this;
}

RunHeader::~RunHeader() {
}

int RunHeader::GetRunNumber() const {
    return _run_number;
}

void RunHeader::SetRunNumber(int run_number) {
  _run_number = run_number;
}
}

