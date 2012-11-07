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

#include "src/common_cpp/DataStructure/RunFooter.hh"
#include "src/common_cpp/DataStructure/MAUSEvent.hh"
#include "src/common_cpp/DataStructure/RunFooterData.hh"

namespace MAUS {

RunFooterData::RunFooterData()
        : MAUSEvent<RunFooter>("RunFooter"), _run_footer(NULL) {
}

RunFooterData::RunFooterData(const RunFooterData& data)
        : MAUSEvent<RunFooter>("RunFooter"), _run_footer(NULL) {
    *this = data;
}

RunFooterData::~RunFooterData() {
    if (_run_footer != NULL) {
        delete _run_footer;
        _run_footer = NULL;
    }
}

RunFooterData& RunFooterData::operator=(const RunFooterData& data) {
    if (this == & data) {
        return *this;
    }
    if (data._run_footer == NULL) {
        SetRunFooter(NULL);
    } else {
        SetRunFooter(new RunFooter(*data._run_footer));
    }
    SetEventType(data.GetEventType());
    return *this;
}

void RunFooterData::SetRunFooter(RunFooter* footer) {
    if (_run_footer != NULL) {
        delete _run_footer;
        _run_footer = NULL;
    }
    _run_footer = footer;
}

RunFooter* RunFooterData::GetRunFooter() const {
    return _run_footer;
}

int RunFooterData::GetSizeOf() const {
  RunFooterData foot;
  return sizeof(foot);
}
}

