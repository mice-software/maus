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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_RUNFOOTERDATA_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_RUNFOOTERDATA_HH_

#include <string>
#include "src/common_cpp/Utils/VersionNumber.hh"
#include "src/common_cpp/DataStructure/MAUSEvent.hh"

namespace MAUS {

class RunFooter;

/** RunFooterData is the root of the MAUS run footer structure
 *
 *  RunFooterData class is the root class for the MAUS run footer structure. It
 *  just holds a pointer to the footer. This is to handle stupid way in which
 *  ROOT does data structure stuff.
 */
class RunFooterData : public MAUSEvent<RunFooter> {
  public:
    /** Default constructor initialises everything to NULL */
    RunFooterData();

    /** Copy constructor (deepcopy) */
    RunFooterData(const RunFooterData& data);

    /** Deepcopy from data to *this */
    RunFooterData& operator=(const RunFooterData& data);

    /** Deletes the run_footer data */
    virtual ~RunFooterData();

    /** Set the run_footer data
     *
     *  RunFooterData now takes ownership of memory allocated to run footer
     */
    void SetRunFooter(RunFooter* footer);

    /** Get the run footer data
     *
     *  RunFooterData still owns this memory
     */
    RunFooter* GetRunFooter() const;

    /** Returns sizeof the run footer object
     *
     *  When setting up TTrees directly in ROOT, it is necessary to hand ROOT
     *  the size of the class member. This is difficult (impossible?) to access
     *  from python, so we provide a convenience function here.
     */
    int GetSizeOf() const;

    /** Get Event Type information 
     */
    std::string GetEventType() const {
        return MAUSEvent<RunFooter>::GetEventType();
    }

    /** Set Event Type information 
     */
    void SetEventType(std::string type) {
        MAUSEvent<RunFooter>::SetEventType(type);
    }

    /** Overload MAUSEvent function (wraps SetRunFooter)*/
    void SetEvent(RunFooter* footer) {SetRunFooter(footer);}

    /** Overload MAUSEvent function (wraps GetRunFooter)*/
    RunFooter* GetEvent() const {return GetRunFooter();}

  private:
    RunFooter* _run_footer;
    MAUS_VERSIONED_CLASS_DEF(RunFooterData)
};
}

#endif

