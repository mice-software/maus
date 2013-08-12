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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_KLDIGIT_
#define _SRC_COMMON_CPP_DATASTRUCTURE_KLDIGIT_

#include <string>

#include "src/common_cpp/Utils/VersionNumber.hh"

namespace MAUS {

/** @class KLDigit comment
 *
 *  @var pmt  <which pmt is flashed, possible values 0,1>
 *  @var charge_mm  <charge in flash adc units, max value - min value>
 *  @var charge_pm  <charge in flash adc units, max value - pedestal value>
 *  @var phys_event_number  <number of events in the spill>
 *  @var kl_key  <which kl channel>
 *  @var part_event_number  <number of particle in the event>
 *  @var cell  <which cell is flashed, possible values from 0 to 21>
 *  @var position_max  <the position of max value of charge in flash adc>
 */

class KLDigit {
  public:
    /** Default constructor - initialises to 0/NULL */
    KLDigit();

    /** Constructor - initialises member variables from arguments */
    KLDigit(int pmt, int charge_mm, int charge_pm, int phys_event_number,
            std::string kl_key, int part_event_number, int cell, int position_max );

    /** Copy constructor - any pointers are deep copied */
    KLDigit(const KLDigit& _kldigit);

    /** Equality operator - any pointers are deep copied */
    KLDigit& operator=(const KLDigit& _kldigit);

    /** Destructor - any member pointers are deleted */
    virtual ~KLDigit();


    /** Returns Pmt */
    int GetPmt() const;

    /** Sets Pmt */
    void SetPmt(int pmt);

    /** Returns ChargeMm */
    int GetChargeMm() const;

    /** Sets ChargeMm */
    void SetChargeMm(int charge_mm);

    /** Returns ChargePm */
    int GetChargePm() const;

    /** Sets ChargePm */
    void SetChargePm(int charge_pm);

    /** Returns PhysEventNumber */
    int GetPhysEventNumber() const;

    /** Sets PhysEventNumber */
    void SetPhysEventNumber(int phys_event_number);

    /** Returns KlKey */
    std::string GetKlKey() const;

    /** Sets KlKey */
    void SetKlKey(std::string kl_key);

    /** Returns PartEventNumber */
    int GetPartEventNumber() const;

    /** Sets PartEventNumber */
    void SetPartEventNumber(int part_event_number);

    /** Returns Cell */
    int GetCell() const;

    /** Sets Cell */
    void SetCell(int cell);

    /** Returns Position of fADC maximum */
    int GetPositionMax() const;

    /** Sets Position of fADC maximum */
    void SetPositionMax(int position_max);

  private:
    int _pmt;
    int _charge_mm;
    int _charge_pm;
    int _phys_event_number;
    std::string _kl_key;
    int _part_event_number;
    int _cell;
    int _position_max;

    MAUS_VERSIONED_CLASS_DEF(KLDigit)
};
}

#endif  // _SRC_COMMON_CPP_DATASTRUCTURE_KLDIGIT_

