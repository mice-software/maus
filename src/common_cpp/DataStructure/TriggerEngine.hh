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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_TriggerEngine_
#define _SRC_COMMON_CPP_DATASTRUCTURE_TriggerEngine_

#include "src/common_cpp/Utils/VersionNumber.hh"
#include "src/common_cpp/DataStructure/ParticleTrigger.hh"

namespace MAUS {

/** @class TriggerEngine comment
 *
 *  @var ldc_id  <--description-->
 *  @var phys_event_number  <--description-->
 *  @var time_stamp  <--description-->
 *  @var geo  <--description-->
 *  @var ptriggers  <--description-->
 */

class TriggerEngine {
  public:
    /** Default constructor - initialises to 0/NULL */
    TriggerEngine();

    /** Copy constructor - any pointers are deep copied */
    TriggerEngine(const TriggerEngine& trigger);

    /** Equality operator - any pointers are deep copied */
    TriggerEngine& operator=(const TriggerEngine& trigger);

    /** Destructor - any member pointers are deleted */
    virtual ~TriggerEngine();


    /** Returns LdcId */
    int GetLdcId() const;

    /** Sets LdcId */
    void SetLdcId(int ldc_id);

    /** Returns PhysEventNumber */
    int GetPhysEventNumber() const;

    /** Sets PhysEventNumber */
    void SetPhysEventNumber(int phys_event_number);

    /** Returns TimeStamp */
    int GetTimeStamp() const;

    /** Sets TimeStamp */
    void SetTimeStamp(int time_stamp);

    /** Returns Geo */
    int GetGeo() const;

    /** Sets Geo */
    void SetGeo(int geo);

    /** Returns the array of particle trigers */
    ParticleTriggerArray GetParticleTriggerArray() const         { return _ptriggers; }

    /** Returns a pointer to the array of particle trigers */
    ParticleTriggerArray* GetParticleTriggerArrayPtr()           { return &_ptriggers;}

    /** Sets the array of of particle trigers */
    void SetParticleTriggerArray(ParticleTriggerArray ptriggers) { _ptriggers = ptriggers; }

    /** Adds a trigger */
    void AddParticleTrigger(ParticleTrigger trigger)             { _ptriggers.push_back(trigger); }

    /** Returns the amount of particle triggers */
    size_t GetParticleTriggerArraySize()                         { return _ptriggers.size(); }

  private:
    int _ldc_id;
    int _phys_event_number;
    int _time_stamp;
    int _geo;
    ParticleTriggerArray  _ptriggers;

    MAUS_VERSIONED_CLASS_DEF(TriggerEngine)
};
}

#endif  // _SRC_COMMON_CPP_DATASTRUCTURE_TriggerEngine_

