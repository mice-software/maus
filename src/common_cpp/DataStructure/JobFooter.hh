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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_JOBFOOTER_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_JOBFOOTER_HH_

#include "Rtypes.h"  // ROOT

#include "src/common_cpp/DataStructure/DateTime.hh"

namespace MAUS {

/** @class JobFooter holds footer information for a given code execution (job)
 *
 *  \param end_of_job time stamp that the job finished
 */

class JobFooter {
  public:
    /** Default constructor - initialises to 0/NULL */
    JobFooter();

    /** Copy constructor - any pointers are deep copied */
    JobFooter(const JobFooter& job_footer);

    /** Equality operator - any pointers are deep copied */
    JobFooter& operator=(const JobFooter& job_footer);

    /** Destructor - any member pointers are deleted */
    virtual ~JobFooter();

    /** Get time stamp for when the job finished */
    DateTime GetEndOfJob() const;

    /** Set time stamp for when the job finished */
    void SetEndOfJob(DateTime end);

  private:
    DateTime _end_of_job;

    ClassDef(JobFooter, 1)
};
}

#endif  // _SRC_COMMON_CPP_DATASTRUCTURE_JOBFOOTER_

