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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_CKOVA_
#define _SRC_COMMON_CPP_DATASTRUCTURE_CKOVA_

#include "Rtypes.h"  // ROOT

namespace MAUS {

/** @class CkovA comment
 *
 *  @var position_min_0  <--description-->
 *  @var arrival_time_2  <--description-->
 *  @var arrival_time_3  <--description-->
 *  @var arrival_time_0  <--description-->
 *  @var arrival_time_1  <--description-->
 *  @var pulse_1  <--description-->
 *  @var pulse_0  <--description-->
 *  @var pulse_3  <--description-->
 *  @var pulse_2  <--description-->
 *  @var coincidences  <--description-->
 *  @var position_min_2  <--description-->
 *  @var position_min_3  <--description-->
 *  @var total_charge  <--description-->
 *  @var position_min_1  <--description-->
 *  @var part_event_number  <--description-->
 *  @var number_of_pes  <--description-->
 */

class CkovA {
  public:
    /** Default constructor - initialises to 0/NULL */
    CkovA();

    /** Copy constructor - any pointers are deep copied */
    CkovA(const CkovA& _ckova);

    /** Equality operator - any pointers are deep copied */
    CkovA& operator=(const CkovA& _ckova);

    /** Destructor - any member pointers are deleted */
    virtual ~CkovA();


    /** Returns PositionMin0 */
    int GetPositionMin0() const;

    /** Sets PositionMin0 */
    void SetPositionMin0(int position_min_0);

    /** Returns ArrivalTime2 */
    int GetArrivalTime2() const;

    /** Sets ArrivalTime2 */
    void SetArrivalTime2(int arrival_time_2);

    /** Returns ArrivalTime3 */
    int GetArrivalTime3() const;

    /** Sets ArrivalTime3 */
    void SetArrivalTime3(int arrival_time_3);

    /** Returns ArrivalTime0 */
    int GetArrivalTime0() const;

    /** Sets ArrivalTime0 */
    void SetArrivalTime0(int arrival_time_0);

    /** Returns ArrivalTime1 */
    int GetArrivalTime1() const;

    /** Sets ArrivalTime1 */
    void SetArrivalTime1(int arrival_time_1);

    /** Returns Pulse1 */
    int GetPulse1() const;

    /** Sets Pulse1 */
    void SetPulse1(int pulse_1);

    /** Returns Pulse0 */
    int GetPulse0() const;

    /** Sets Pulse0 */
    void SetPulse0(int pulse_0);

    /** Returns Pulse3 */
    int GetPulse3() const;

    /** Sets Pulse3 */
    void SetPulse3(int pulse_3);

    /** Returns Pulse2 */
    int GetPulse2() const;

    /** Sets Pulse2 */
    void SetPulse2(int pulse_2);

    /** Returns Coincidences */
    int GetCoincidences() const;

    /** Sets Coincidences */
    void SetCoincidences(int coincidences);

    /** Returns PositionMin2 */
    int GetPositionMin2() const;

    /** Sets PositionMin2 */
    void SetPositionMin2(int position_min_2);

    /** Returns PositionMin3 */
    int GetPositionMin3() const;

    /** Sets PositionMin3 */
    void SetPositionMin3(int position_min_3);

    /** Returns TotalCharge */
    int GetTotalCharge() const;

    /** Sets TotalCharge */
    void SetTotalCharge(int total_charge);

    /** Returns PositionMin1 */
    int GetPositionMin1() const;

    /** Sets PositionMin1 */
    void SetPositionMin1(int position_min_1);

    /** Returns PartEventNumber */
    int GetPartEventNumber() const;

    /** Sets PartEventNumber */
    void SetPartEventNumber(int part_event_number);

    /** Returns NumberOfPes */
    double GetNumberOfPes() const;

    /** Sets NumberOfPes */
    void SetNumberOfPes(double number_of_pes);

  private:
    int _position_min_0;
    int _arrival_time_2;
    int _arrival_time_3;
    int _arrival_time_0;
    int _arrival_time_1;
    int _pulse_1;
    int _pulse_0;
    int _pulse_3;
    int _pulse_2;
    int _coincidences;
    int _position_min_2;
    int _position_min_3;
    int _total_charge;
    int _position_min_1;
    int _part_event_number;
    double _number_of_pes;

    ClassDef(CkovA, 1)
};
}

#endif  // _SRC_COMMON_CPP_DATASTRUCTURE_CKOVA_

