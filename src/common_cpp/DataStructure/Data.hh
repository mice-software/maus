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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_MAUS_DATA_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_MAUS_DATA_HH_

#include <string>

#include "TObject.h" // ROOT

#include "src/common_cpp/Utils/VersionNumber.hh"
#include "src/common_cpp/DataStructure/MAUSEvent.hh"

namespace MAUS {

class Spill;

/** Data is the root of the MAUS physics data structure
 *
 *  Data class is the root class for the MAUS physics data structure. It
 *  just holds a pointer to the spill.
 */
class Data : public MAUSEvent<Spill> {
  public:
    /** Default constructor initialises everything to NULL */
    Data();

    /** Copy constructor (deepcopy) */
    Data(const Data& data);

    /** Deepcopy from data to *this */
    Data& operator=(const Data& data);

    /** Deletes the spill data */
    virtual ~Data();

    /** Set the spill data
     *
     *  Data now takes ownership of memory allocated to spill
     */
    void SetSpill(Spill* spill);

    /** Get the spill data
     *
     *  Data still owns this memory
     */
    Spill* GetSpill() const;

    /** Returns sizeof the spill object
     *
     *  When setting up TTrees directly in ROOT, it is necessary to hand ROOT
     *  the size of the class member. This is difficult (impossible?) to access
     *  from python, so we provide a convenience function here.
     */
    int GetSizeOf() const;

    /** Get Event Type information 
     *
     *  Need to bring into base class for JsonCppProcessor
     */
    std::string GetEventType() const {return MAUSEvent<Spill>::GetEventType();}

    /** Set Event Type information 
     *
     *  Need to bring into base class for JsonCppProcessor
     */
    void SetEventType(std::string type) {MAUSEvent<Spill>::SetEventType(type);}

    /** Wrapper for MAUSEvent
     */
    Spill* GetEvent() const {return GetSpill();}

    /** Wrapper for MAUSEvent
     */
    void SetEvent(Spill* spill) {SetSpill(spill);}

    std::string getRecordName() const {return std::string("data");}

    static int GetReferenceCount() {return _reference_count;}

    static void SetMaxReferenceCount(int max_ref_count) {
        _max_reference_count = max_ref_count;
    }

    static int GetMaxReferenceCount() {return _max_reference_count;}

  private:
    // Count number of references to data and make a stack trace at construction
    // time so we know who built it
    void IncreaseRefCount();
    void DecreaseRefCount();
    static int _reference_count;
    static int _max_reference_count;
    Spill* _spill;
    std::string _event_type;
    MAUS_VERSIONED_CLASS_DEF(Data)
};
}

#endif

