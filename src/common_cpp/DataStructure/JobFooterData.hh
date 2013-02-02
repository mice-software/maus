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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_JOBFOOTERDATA_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_JOBFOOTERDATA_HH_

#include <string>
#include "src/common_cpp/Utils/VersionNumber.hh"
#include "src/common_cpp/DataStructure/MAUSEvent.hh"

namespace MAUS {

class JobFooter;

/** JobFooterData is the root of the MAUS job header structure
 *
 *  JobFooterData class is the root class for the MAUS job header structure. It
 *  just holds a pointer to the header. This is to handle stupid way in which
 *  ROOT does data structure stuff.
 */
class JobFooterData : public MAUSEvent<JobFooter> {
  public:
    /** Default constructor initialises everything to NULL */
    JobFooterData();

    /** Copy constructor (deepcopy) */
    JobFooterData(const JobFooterData& data);

    /** Deepcopy from data to *this */
    JobFooterData& operator=(const JobFooterData& data);

    /** Deletes the job_footer data */
    virtual ~JobFooterData();

    /** Set the job_footer data
     *
     *  JobFooterData now takes ownership of memory allocated to job header
     */
    void SetJobFooter(JobFooter* header);

    /** Get the job header data
     *
     *  JobFooterData still owns this memory
     */
    JobFooter* GetJobFooter() const;

    /** Returns sizeof the job header object
     *
     *  When setting up TTrees directly in ROOT, it is necessary to hand ROOT
     *  the size of the class member. This is difficult (impossible?) to access
     *  from python, so we provide a convenience function here.
     */
    int GetSizeOf() const;

    /** Get Event Type information 
     */
    std::string GetEventType() const {
        return MAUSEvent<JobFooter>::GetEventType();
    }

    /** Set Event Type information 
     */
    void SetEventType(std::string type) {
        MAUSEvent<JobFooter>::SetEventType(type);
    }

    /** Overload MAUSEvent function (wraps SetJobFooter)*/
    void SetEvent(JobFooter* header) {SetJobFooter(header);}

    /** Overload MAUSEvent function (wraps GetJobFooter)*/
    JobFooter* GetEvent() const {return GetJobFooter();}

  private:
    JobFooter* _job_footer;
    MAUS_VERSIONED_CLASS_DEF(JobFooterData)
};
}

#endif

