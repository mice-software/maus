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

#include "src/common_cpp/DataStructure/JobFooter.hh"
#include "src/common_cpp/DataStructure/MAUSEvent.hh"
#include "src/common_cpp/DataStructure/JobFooterData.hh"

namespace MAUS {

JobFooterData::JobFooterData()
        : MAUSEvent<JobFooter>("JobFooter"), _job_footer(NULL) {
}

JobFooterData::JobFooterData(const JobFooterData& data)
        : MAUSEvent<JobFooter>("JobFooter"), _job_footer(NULL) {
    *this = data;
}

JobFooterData::~JobFooterData() {
    if (_job_footer != NULL) {
        delete _job_footer;
        _job_footer = NULL;
    }
}

JobFooterData& JobFooterData::operator=(const JobFooterData& data) {
    if (this == & data) {
        return *this;
    }
    if (data._job_footer == NULL) {
        SetJobFooter(NULL);
    } else {
        SetJobFooter(new JobFooter(*data._job_footer));
    }
    SetEventType(data.GetEventType());
    return *this;
}

void JobFooterData::SetJobFooter(JobFooter* footer) {
    if (_job_footer != NULL) {
        delete _job_footer;
        _job_footer = NULL;
    }
    _job_footer = footer;
}

JobFooter* JobFooterData::GetJobFooter() const {
    return _job_footer;
}

int JobFooterData::GetSizeOf() const {
  JobFooterData head;
  return sizeof(head);
}
}

