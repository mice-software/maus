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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_PMT0_
#define _SRC_COMMON_CPP_DATASTRUCTURE_PMT0_

#include <string>

#include "Rtypes.h"  // ROOT

namespace MAUS {

/** @class Pmt0 comment
 *
 *  @var raw_time  <--description-->
 *  @var charge  <--description-->
 *  @var leading_time  <--description-->
 *  @var time  <--description-->
 *  @var tof_key  <--description-->
 *  @var trigger_request_leading_time  <--description-->
 */

class Pmt0 {
  public:
    /** Default constructor - initialises to 0/NULL */
    Pmt0();

    /** Copy constructor - any pointers are deep copied */
    Pmt0(const Pmt0& _pmt0);

    /** Equality operator - any pointers are deep copied */
    Pmt0& operator=(const Pmt0& _pmt0);

    /** Destructor - any member pointers are deleted */
    virtual ~Pmt0();


    /** Returns RawTime */
    double GetRawTime() const;

    /** Sets RawTime */
    void SetRawTime(double raw_time);

    /** Returns Charge */
    int GetCharge() const;

    /** Sets Charge */
    void SetCharge(int charge);

    /** Returns LeadingTime */
    int GetLeadingTime() const;

    /** Sets LeadingTime */
    void SetLeadingTime(int leading_time);

    /** Returns Time */
    double GetTime() const;

    /** Sets Time */
    void SetTime(double time);

    /** Returns TofKey */
    std::string GetTofKey() const;

    /** Sets TofKey */
    void SetTofKey(std::string tof_key);

    /** Returns TriggerRequestLeadingTime */
    int GetTriggerRequestLeadingTime() const;

    /** Sets TriggerRequestLeadingTime */
    void SetTriggerRequestLeadingTime(int trigger_request_leading_time);

  private:
    double _raw_time;
    int _charge;
    int _leading_time;
    double _time;
    std::string _tof_key;
    int _trigger_request_leading_time;

    ClassDef(Pmt0, 1)
};
}

#endif  // _SRC_COMMON_CPP_DATASTRUCTURE_PMT0_

