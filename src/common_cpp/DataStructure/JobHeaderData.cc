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

#include "src/common_cpp/DataStructure/JobHeader.hh"
#include "src/common_cpp/DataStructure/MAUSEvent.hh"
#include "src/common_cpp/DataStructure/JobHeaderData.hh"

namespace MAUS {

JobHeaderData::JobHeaderData()
      : MAUSEvent<JobHeader>("JobHeader"), _job_header(NULL) {
}

JobHeaderData::JobHeaderData(const JobHeaderData& data)
      : MAUSEvent<JobHeader>("JobHeader"), _job_header(NULL) {
    *this = data;
}

JobHeaderData::~JobHeaderData() {
    if (_job_header != NULL) {
        delete _job_header;
        _job_header = NULL;
    }
}

JobHeaderData& JobHeaderData::operator=(const JobHeaderData& data) {
    if (this == & data) {
        return *this;
    }
    if (data._job_header == NULL) {
        SetJobHeader(NULL);
    } else {
        SetJobHeader(new JobHeader(*data._job_header));
    }
    SetEventType(data.GetEventType());
    return *this;
}

void JobHeaderData::SetJobHeader(JobHeader* header) {
    if (_job_header != NULL) {
        delete _job_header;
        _job_header = NULL;
    }
    _job_header = header;
}

JobHeader* JobHeaderData::GetJobHeader() const {
    return _job_header;
}

int JobHeaderData::GetSizeOf() const {
  JobHeaderData head;
  return sizeof(head);
}
}

