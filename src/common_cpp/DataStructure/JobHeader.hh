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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_JOBHEADER_
#define _SRC_COMMON_CPP_DATASTRUCTURE_JOBHEADER_

#include <string>

#include "src/common_cpp/Utils/VersionNumber.hh"
#include "src/common_cpp/DataStructure/DateTime.hh"

namespace Json {
class Value;
}

namespace MAUS {

/** @class JobHeader holds header information for a given code execution (job)
 *
 *  Note that these data refer to the information stored in the data structure,
 *  which can be different to the information for *this* job, depending on
 *  whether you are looking at an old data structure (e.g. examining some old
 *  reconstructed data).
 *
 *  \param bzr_config Branch configuration information can be found in
 *  .bzr/branch/branch_config- this ids the branch from whence commits come
 *  etc.
 *  \param bzr_config Branch configuration information can be found in
 *  .bzr/branch/branch_config- this ids the branch from whence commits come
 *  etc.
 *  \param bzr_revision Branch revision information can be found in
 *  .bzr/branch/last-revision- this ids the specific commit number.
 *  \param bzr_status Branch status can be found by doing 'bzr status'
 *  this tells us if any code was changed locally - but requires bzr to run.
 *  \param maus_version_number MAUS version number (from datacards).
 *  \param json_configuration Json configuration/datacards information.
 *  This is only used for reference in postprocessing jobs - the datacards
 *  should always be accessed through Globals::GetConfigurationCards(). You
 *  can get in real trouble here because it is possible for the datacards
 *  stored in JobHeader to be different to the datacards stored in Globals,
 *  if e.g. somone is doing a postprocessing job on an existing dataset.
 *
 */

class JobHeader {
  public:
    /** Default constructor - initialises to 0/NULL */
    JobHeader();

    /** Copy constructor - any pointers are deep copied */
    JobHeader(const JobHeader& job_header);

    /** Equality operator - any pointers are deep copied */
    JobHeader& operator=(const JobHeader& job_header);

    /** Destructor - any member pointers are deleted */
    virtual ~JobHeader();

    /** Get time stamp for when the job started */
    DateTime GetStartOfJob() const;

    /** Set time stamp for when the job started */
    void SetStartOfJob(DateTime timestamp);

    /** Get bzr configuration if available or else empty string */
    std::string GetBzrConfiguration() const;

    /** Set bzr configuration if available or else empty string */
    void SetBzrConfiguration(std::string bzr_config);

    /** Get bzr revision if available or else empty string */
    std::string GetBzrRevision() const;

    /** Set bzr revision if available or else empty string */
    void SetBzrRevision(std::string bzr_revision);

    /** Get bzr status if available or else empty string */
    std::string GetBzrStatus() const;

    /** Set bzr status if available or else empty string */
    void SetBzrStatus(std::string bzr_status);

    /** Get maus version number used to run the code (from datacards) */
    std::string GetMausVersionNumber() const;

    /** Set maus version number used to run the code (from datacards) */
    void SetMausVersionNumber(std::string version_number);

    /** Get datacards for the job. */
    std::string GetJsonConfiguration() const;

    /** Set datacards for the job. */
    void SetJsonConfiguration(std::string json_config);

  private:
    DateTime _start_of_job;
    std::string _bzr_configuration;
    std::string _bzr_revision;
    std::string _bzr_status;
    std::string _maus_version_number;
    std::string _json_configuration;

    MAUS_VERSIONED_CLASS_DEF(JobHeader)
};
}

#endif  // _SRC_COMMON_CPP_DATASTRUCTURE_JOBHEADER_

