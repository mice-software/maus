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
 *
 */

#include "src/common_cpp/DataStructure/RunHeader.hh"
#include "src/common_cpp/DataStructure/MAUSEvent.hh"
#include "src/common_cpp/DataStructure/RunHeaderData.hh"

namespace MAUS {

RunHeaderData::RunHeaderData() : MAUSEvent("RunHeader"), _run_header(NULL) {
}

RunHeaderData::RunHeaderData(const RunHeaderData& data)
                           : MAUSEvent("RunHeader"), _run_header(NULL) {
    *this = data;
}

RunHeaderData::~RunHeaderData() {
    if (_run_header != NULL) {
        delete _run_header;
        _run_header = NULL;
    }
}

RunHeaderData& RunHeaderData::operator=(const RunHeaderData& data) {
    if (this == & data) {
        return *this;
    }
    if (data._run_header == NULL) {
        SetRunHeader(NULL);
    } else {
        SetRunHeader(new RunHeader(*data._run_header));
    }
    SetEventType(data.GetEventType());
    return *this;
}

void RunHeaderData::SetRunHeader(RunHeader* header) {
    if (_run_header != NULL) {
        delete _run_header;
        _run_header = NULL;
    }
    _run_header = header;
}

RunHeader* RunHeaderData::GetRunHeader() const {
    return _run_header;
}

int RunHeaderData::GetSizeOf() const {
  RunHeaderData head;
  return sizeof(head);
}
}

