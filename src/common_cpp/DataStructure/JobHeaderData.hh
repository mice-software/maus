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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_JOBHEADERDATA_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_JOBHEADERDATA_HH_

#include <string>
#include "src/common_cpp/DataStructure/MAUSEvent.hh"

namespace MAUS {

class JobHeader;

/** JobHeaderData is the root of the MAUS job header structure
 *
 *  JobHeaderData class is the root class for the MAUS job header structure. It
 *  just holds a pointer to the header. This is to handle stupid way in which
 *  ROOT does data structure stuff.
 */
class JobHeaderData : public MAUSEvent<JobHeader> {
  public:
    /** Default constructor initialises everything to NULL */
    JobHeaderData();

    /** Copy constructor (deepcopy) */
    JobHeaderData(const JobHeaderData& data);

    /** Deepcopy from data to *this */
    JobHeaderData& operator=(const JobHeaderData& data);

    /** Deletes the job_header data */
    virtual ~JobHeaderData();

    /** Set the job_header data
     *
     *  JobHeaderData now takes ownership of memory allocated to job header
     */
    void SetJobHeader(JobHeader* header);

    /** Get the job header data
     *
     *  JobHeaderData still owns this memory
     */
    JobHeader* GetJobHeader() const;

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
        return MAUSEvent<JobHeader>::GetEventType();
    }

    /** Set Event Type information 
     */
    void SetEventType(std::string type) {
        MAUSEvent<JobHeader>::SetEventType(type);
    }

    /** Overload MAUSEvent function (wraps SetJobHeader)*/
    void SetEvent(JobHeader* header) {SetJobHeader(header);}

    /** Overload MAUSEvent function (wraps GetJobHeader)*/
    JobHeader* GetEvent() const {return GetJobHeader();}

  private:
    JobHeader* _job_header;
    ClassDef(JobHeaderData, 1)
};
}

#endif

