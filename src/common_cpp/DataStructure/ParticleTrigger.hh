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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_ParticleTrigger_
#define _SRC_COMMON_CPP_DATASTRUCTURE_ParticleTrigger_

#include <vector>

#include "src/common_cpp/Utils/VersionNumber.hh"

namespace MAUS {

/** @class ParticleTrigger comment
 *
 *  @var ptrigger_number  <--description-->
 *  @var time_stamp  <--description-->
 *  @var tof0Pattern  <--description-->
 *  @var tof1Pattern  <--description-->
 *  @var tof2Pattern  <--description-->
 */

class ParticleTrigger {
  public:
    /** Default constructor - initialises to 0/NULL */
    ParticleTrigger();

    /** Copy constructor - any pointers are deep copied */
    ParticleTrigger(const ParticleTrigger& trigger);

    /** Equality operator - any pointers are deep copied */
    ParticleTrigger& operator=(const ParticleTrigger& trigger);

    /** Destructor - any member pointers are deleted */
    virtual ~ParticleTrigger();

    /** Returns PhysEventNumber */
    int GetParticleTriggerNumber() const;

    /** Sets PhysEventNumber */
    void SetParticleTriggerNumber(int ptrigger_number);

    /** Returns TimeStamp */
    int GetTimeStamp() const;

    /** Sets TimeStamp */
    void SetTimeStamp(int time_stamp);

    /** Returns Tof0 Pattern */
    int GetTof0Pattern() const;

    /** Sets Tof0 Pattern */
    void SetTof0Pattern(int pattern);

    /** Returns Tof1 Pattern */
    int GetTof1Pattern() const;

    /** Sets Tof1 Pattern */
    void SetTof1Pattern(int pattern);

    /** Returns Tof2 Pattern */
    int GetTof2Pattern() const;

    /** Sets Tof2 Pattern */
    void SetTof2Pattern(int pattern);

  private:
    int _ptrigger_number;
    int _time_stamp;
    int _tof0Pattern;
    int _tof1Pattern;
    int _tof2Pattern;

    MAUS_VERSIONED_CLASS_DEF(ParticleTrigger)
};

typedef std::vector<ParticleTrigger> ParticleTriggerArray;
}

#endif  // _SRC_COMMON_CPP_DATASTRUCTURE_ParticleTrigger_

