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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_MAUSEVENT_
#define _SRC_COMMON_CPP_DATASTRUCTURE_MAUSEVENT_

#include <string>
#include <typeinfo>

#include "src/common_cpp/Utils/VersionNumber.hh"
#include "src/common_cpp/DataStructure/MAUSEvent.hh"

namespace MAUS {

/** @class MAUSEvent is a base class for top level data
 *
 *  MAUS supports a few different event types - at present:
 *     JobHeader
 *     RunHeader
 *     RunFooter
 *     JobFooter
 *     Data (DAQ or MC data)
 *
 *  ROOT likes to have a memory address where data is stored, and each time we
 *  put new data into a ttree (Tree.Fill()) we update the data stored at that
 *  address; so we make a thin wrapper that stores this address. 
 */
template <class EventT>
class MAUSEvent {
  public:
    /** Default constructor - initialises event_type to "" */
    MAUSEvent<EventT>() : _event_type("") {}

    /** type is the type of EventT - tried using typeinfo but it's making garbage */
    MAUSEvent<EventT>(std::string type)  : _event_type(type) {}

    /** Copy constructor - copy _event_type */
    MAUSEvent(const MAUSEvent& event) : _event_type(typeid(EventT).name()) {}

    /** Equality operator - any pointers are deep copied */
    virtual MAUSEvent& operator=(const MAUSEvent& event) {return *this;}

    /** Destructor - any member pointers are deleted */
    virtual ~MAUSEvent() {}

    /** Get event type. */
    virtual std::string GetEventType() const {
        return _event_type;
    }

    /** Set event type. */
    virtual void SetEventType(std::string event_type) {
        _event_type = event_type;
    }

    /** Get the Event wrapped by the MAUSEvent */
    virtual EventT* GetEvent() const = 0;

    /** Set the Event wrapped by the MAUSEvent */
    virtual void SetEvent(EventT* event) = 0;

    /** Returns sizeof the JobHeader object
     *
     *  When setting up TTrees directly in ROOT, it is necessary to hand ROOT
     *  the size of the class member. This is difficult (impossible?) to access
     *  from python, so we provide a convenience function here.
     */
    virtual int GetSizeOf() const = 0;

    /** Overloads ROOT::TObject ClassName(); returns EventType as a C string
     *
     *  There may be a way to achieve this functionality using RTypes macros
     */
    virtual const char*	ClassName() const {
        return _event_type.c_str();
    }

  private:
    std::string _event_type;

    MAUS_VERSIONED_CLASS_DEF(MAUSEvent<EventT>)
};
}

#endif  // _SRC_COMMON_CPP_DATASTRUCTURE_MAUSEVENT_

