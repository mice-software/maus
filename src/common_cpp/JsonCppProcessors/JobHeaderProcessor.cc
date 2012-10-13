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

#include "src/common_cpp/JsonCppProcessors/JobHeaderProcessor.hh"

namespace MAUS {

JobHeaderProcessor::JobHeaderProcessor()
    : _string_proc(), _date_time_proc() {

    RegisterValueBranch("start_of_job", &_date_time_proc,
                        &JobHeader::GetStartOfJob,
                        &JobHeader::SetStartOfJob, true);
    RegisterValueBranch("bzr_configuration", &_string_proc,
                        &JobHeader::GetBzrConfiguration,
                        &JobHeader::SetBzrConfiguration, true);
    RegisterValueBranch("bzr_revision", &_string_proc,
                        &JobHeader::GetBzrRevision,
                        &JobHeader::SetBzrRevision, true);
    RegisterValueBranch("bzr_status", &_string_proc,
                        &JobHeader::GetBzrStatus,
                        &JobHeader::SetBzrStatus, true);
    RegisterValueBranch("maus_version", &_string_proc,
                        &JobHeader::GetMausVersionNumber,
                        &JobHeader::SetMausVersionNumber, true);
    RegisterValueBranch("json_configuration", &_string_proc,
                        &JobHeader::GetJsonConfiguration,
                        &JobHeader::SetJsonConfiguration, true);
    RegisterConstantBranch("maus_event_type", Json::Value("JobHeader"), true);
}
}  // namespace MAUS


