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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_TOFDIGIT_
#define _SRC_COMMON_CPP_DATASTRUCTURE_TOFDIGIT_

#include <string>

#include "Rtypes.h"  // ROOT

namespace MAUS {

/** @class TOFDigit comment
 *
 *  @var pmt  <--description-->
 *  @var trigger_leading_time  <--description-->
 *  @var trigger_request_leading_time  <--description-->
 *  @var trigger_trailing_time  <--description-->
 *  @var trigger_request_trailing_time  <--description-->
 *  @var charge_mm  <--description-->
 *  @var trailing_time  <--description-->
 *  @var plane  <--description-->
 *  @var charge_pm  <--description-->
 *  @var phys_event_number  <--description-->
 *  @var station  <--description-->
 *  @var leading_time  <--description-->
 *  @var time_stamp  <--description-->
 *  @var tof_key  <--description-->
 *  @var trigger_time_tag  <--description-->
 *  @var part_event_number  <--description-->
 *  @var slab  <--description-->
 */

class TOFDigit {
  public:
    /** Default constructor - initialises to 0/NULL */
    TOFDigit();

    /** Copy constructor - any pointers are deep copied */
    TOFDigit(const TOFDigit& _tofdigit);

    /** Equality operator - any pointers are deep copied */
    TOFDigit& operator=(const TOFDigit& _tofdigit);

    /** Destructor - any member pointers are deleted */
    virtual ~TOFDigit();


    /** Returns Pmt */
    int GetPmt() const;

    /** Sets Pmt */
    void SetPmt(int pmt);

    /** Returns TriggerLeadingTime */
    int GetTriggerLeadingTime() const;

    /** Sets TriggerLeadingTime */
    void SetTriggerLeadingTime(int trigger_leading_time);

    /** Returns TriggerRequestLeadingTime */
    int GetTriggerRequestLeadingTime() const;

    /** Sets TriggerRequestLeadingTime */
    void SetTriggerRequestLeadingTime(int trigger_request_leading_time);

    /** Returns TriggerTrailingTime */
    int GetTriggerTrailingTime() const;

    /** Sets TriggerTrailingTime */
    void SetTriggerTrailingTime(int trigger_trailing_time);

    /** Returns TriggerRequestTrailingTime */
    int GetTriggerRequestTrailingTime() const;

    /** Sets TriggerRequestTrailingTime */
    void SetTriggerRequestTrailingTime(int trigger_request_trailing_time);

    /** Returns ChargeMm */
    int GetChargeMm() const;

    /** Sets ChargeMm */
    void SetChargeMm(int charge_mm);

    /** Returns TrailingTime */
    int GetTrailingTime() const;

    /** Sets TrailingTime */
    void SetTrailingTime(int trailing_time);

    /** Returns Plane */
    int GetPlane() const;

    /** Sets Plane */
    void SetPlane(int plane);

    /** Returns ChargePm */
    int GetChargePm() const;

    /** Sets ChargePm */
    void SetChargePm(int charge_pm);

    /** Returns PhysEventNumber */
    int GetPhysEventNumber() const;

    /** Sets PhysEventNumber */
    void SetPhysEventNumber(int phys_event_number);

    /** Returns Station */
    int GetStation() const;

    /** Sets Station */
    void SetStation(int station);

    /** Returns LeadingTime */
    int GetLeadingTime() const;

    /** Sets LeadingTime */
    void SetLeadingTime(int leading_time);

    /** Returns TimeStamp */
    int GetTimeStamp() const;

    /** Sets TimeStamp */
    void SetTimeStamp(int time_stamp);

    /** Returns TofKey */
    std::string GetTofKey() const;

    /** Sets TofKey */
    void SetTofKey(std::string tof_key);

    /** Returns TriggerTimeTag */
    int GetTriggerTimeTag() const;

    /** Sets TriggerTimeTag */
    void SetTriggerTimeTag(int trigger_time_tag);

    /** Returns PartEventNumber */
    int GetPartEventNumber() const;

    /** Sets PartEventNumber */
    void SetPartEventNumber(int part_event_number);

    /** Returns Slab */
    int GetSlab() const;

    /** Sets Slab */
    void SetSlab(int slab);

  private:
    int _pmt;
    int _trigger_leading_time;
    int _trigger_request_leading_time;
    int _trigger_trailing_time;
    int _trigger_request_trailing_time;
    int _charge_mm;
    int _trailing_time;
    int _plane;
    int _charge_pm;
    int _phys_event_number;
    int _station;
    int _leading_time;
    int _time_stamp;
    std::string _tof_key;
    int _trigger_time_tag;
    int _part_event_number;
    int _slab;

    ClassDef(TOFDigit, 1)
};
}

#endif  // _SRC_COMMON_CPP_DATASTRUCTURE_TOFDIGIT_

