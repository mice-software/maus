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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_CKOVB_
#define _SRC_COMMON_CPP_DATASTRUCTURE_CKOVB_

#include "Rtypes.h"  // ROOT

namespace MAUS {

/** @class CkovB comment
 *
 *  @var arrival_time_6  <--description-->
 *  @var arrival_time_7  <--description-->
 *  @var arrival_time_4  <--description-->
 *  @var arrival_time_5  <--description-->
 *  @var pulse_5  <--description-->
 *  @var pulse_4  <--description-->
 *  @var pulse_7  <--description-->
 *  @var pulse_6  <--description-->
 *  @var position_min_6  <--description-->
 *  @var coincidences  <--description-->
 *  @var total_charge  <--description-->
 *  @var part_event_number  <--description-->
 *  @var position_min_7  <--description-->
 *  @var number_of_pes  <--description-->
 *  @var position_min_5  <--description-->
 *  @var position_min_4  <--description-->
 */

class CkovB {
  public:
    /** Default constructor - initialises to 0/NULL */
    CkovB();

    /** Copy constructor - any pointers are deep copied */
    CkovB(const CkovB& _ckovb);

    /** Equality operator - any pointers are deep copied */
    CkovB& operator=(const CkovB& _ckovb);

    /** Destructor - any member pointers are deleted */
    ~CkovB();


    /** Returns ArrivalTime6 */
    int GetArrivalTime6() const;

    /** Sets ArrivalTime6 */
    void SetArrivalTime6(int arrival_time_6);

    /** Returns ArrivalTime7 */
    int GetArrivalTime7() const;

    /** Sets ArrivalTime7 */
    void SetArrivalTime7(int arrival_time_7);

    /** Returns ArrivalTime4 */
    int GetArrivalTime4() const;

    /** Sets ArrivalTime4 */
    void SetArrivalTime4(int arrival_time_4);

    /** Returns ArrivalTime5 */
    int GetArrivalTime5() const;

    /** Sets ArrivalTime5 */
    void SetArrivalTime5(int arrival_time_5);

    /** Returns Pulse5 */
    int GetPulse5() const;

    /** Sets Pulse5 */
    void SetPulse5(int pulse_5);

    /** Returns Pulse4 */
    int GetPulse4() const;

    /** Sets Pulse4 */
    void SetPulse4(int pulse_4);

    /** Returns Pulse7 */
    int GetPulse7() const;

    /** Sets Pulse7 */
    void SetPulse7(int pulse_7);

    /** Returns Pulse6 */
    int GetPulse6() const;

    /** Sets Pulse6 */
    void SetPulse6(int pulse_6);

    /** Returns PositionMin6 */
    int GetPositionMin6() const;

    /** Sets PositionMin6 */
    void SetPositionMin6(int position_min_6);

    /** Returns Coincidences */
    int GetCoincidences() const;

    /** Sets Coincidences */
    void SetCoincidences(int coincidences);

    /** Returns TotalCharge */
    int GetTotalCharge() const;

    /** Sets TotalCharge */
    void SetTotalCharge(int total_charge);

    /** Returns PartEventNumber */
    int GetPartEventNumber() const;

    /** Sets PartEventNumber */
    void SetPartEventNumber(int part_event_number);

    /** Returns PositionMin7 */
    int GetPositionMin7() const;

    /** Sets PositionMin7 */
    void SetPositionMin7(int position_min_7);

    /** Returns NumberOfPes */
    double GetNumberOfPes() const;

    /** Sets NumberOfPes */
    void SetNumberOfPes(double number_of_pes);

    /** Returns PositionMin5 */
    int GetPositionMin5() const;

    /** Sets PositionMin5 */
    void SetPositionMin5(int position_min_5);

    /** Returns PositionMin4 */
    int GetPositionMin4() const;

    /** Sets PositionMin4 */
    void SetPositionMin4(int position_min_4);

  private:
    int _arrival_time_6;
    int _arrival_time_7;
    int _arrival_time_4;
    int _arrival_time_5;
    int _pulse_5;
    int _pulse_4;
    int _pulse_7;
    int _pulse_6;
    int _position_min_6;
    int _coincidences;
    int _total_charge;
    int _part_event_number;
    int _position_min_7;
    double _number_of_pes;
    int _position_min_5;
    int _position_min_4;

    ClassDef(CkovB, 1)
};
}

#endif  // _SRC_COMMON_CPP_DATASTRUCTURE_CKOVB_

