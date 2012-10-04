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

#include "json/json.h"

#include "src/common_cpp/DataStructure/JobHeader.hh"

namespace MAUS {
JobHeader::JobHeader() : _start_of_job(), _bzr_configuration(""),
  _bzr_revision(""), _bzr_status(""), _maus_version_number(""),
  _json_configuration("") {
}

JobHeader::JobHeader(const JobHeader& job_header) : _start_of_job(),
  _bzr_configuration(""), _bzr_revision(""), _bzr_status(""),
  _maus_version_number(""), _json_configuration("") {
    *this = job_header;
}

JobHeader& JobHeader::operator=(const JobHeader& job_header) {
    if (this == &job_header) {
        return *this;
    }
    _start_of_job = job_header._start_of_job;
    _bzr_configuration = job_header._bzr_configuration;
    _bzr_revision = job_header._bzr_revision;
    _bzr_status = job_header._bzr_status;
    _maus_version_number = job_header._maus_version_number;
    _json_configuration = job_header._json_configuration;
    return *this;
}

JobHeader::~JobHeader() {
}

DateTime JobHeader::GetStartOfJob() const {
    return _start_of_job;
}

void JobHeader::SetStartOfJob(DateTime timestamp) {
    _start_of_job = timestamp;
}

std::string JobHeader::GetBzrConfiguration() const {
    return _bzr_configuration;
}

void JobHeader::SetBzrConfiguration(std::string bzr_config) {
    _bzr_configuration = bzr_config;
}

std::string JobHeader::GetBzrRevision() const {
    return _bzr_revision;
}

void JobHeader::SetBzrRevision(std::string bzr_revision) {
    _bzr_revision = bzr_revision;
}

std::string JobHeader::GetBzrStatus() const {
    return _bzr_status;
}

void JobHeader::SetBzrStatus(std::string bzr_status) {
    _bzr_status = bzr_status;
}

std::string JobHeader::GetMausVersionNumber() const {
    return _maus_version_number;
}

void JobHeader::SetMausVersionNumber(std::string version_number) {
    _maus_version_number = version_number;
}

std::string JobHeader::GetJsonConfiguration() const {
    return _json_configuration;
}

void JobHeader::SetJsonConfiguration(std::string json_config) {
    _json_configuration = json_config;
}
}

