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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_RUNHEADERDATA_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_RUNHEADERDATA_HH_

#include <string>
#include "src/common_cpp/DataStructure/MAUSEvent.hh"

namespace MAUS {

class RunHeader;

/** RunHeaderData is the root of the MAUS run header structure
 *
 *  RunHeaderData class is the root class for the MAUS run header structure. It
 *  just holds a pointer to the header. This is to handle stupid way in which
 *  ROOT does data structure stuff.
 */
class RunHeaderData : public MAUSEvent<RunHeader> {
  public:
    /** Default constructor initialises everything to NULL */
    RunHeaderData();

    /** Copy constructor (deepcopy) */
    RunHeaderData(const RunHeaderData& data);

    /** Deepcopy from data to *this */
    RunHeaderData& operator=(const RunHeaderData& data);

    /** Deletes the run_header data */
    virtual ~RunHeaderData();

    /** Set the run_header data
     *
     *  RunHeaderData now takes ownership of memory allocated to run header
     */
    void SetRunHeader(RunHeader* header);

    /** Get the run header data
     *
     *  RunHeaderData still owns this memory
     */
    RunHeader* GetRunHeader() const;

    /** Returns sizeof the run header object
     *
     *  When setting up TTrees directly in ROOT, it is necessary to hand ROOT
     *  the size of the class member. This is difficult (impossible?) to access
     *  from python, so we provide a convenience function here.
     */
    int GetSizeOf() const;

    /** Get Event Type information 
     */
    std::string GetEventType() const {return MAUSEvent::GetEventType();}

    /** Set Event Type information 
     */
    void SetEventType(std::string type) {MAUSEvent::SetEventType(type);}

    /** Overload MAUSEvent function (wraps SetRunHeader)*/
    void SetEvent(RunHeader* header) {SetRunHeader(header);}

    /** Overload MAUSEvent function (wraps GetRunHeader)*/
    RunHeader* GetEvent() const {return GetRunHeader();}

  private:
    RunHeader* _run_header;
    ClassDef(RunHeaderData, 1)
};
}

#endif

