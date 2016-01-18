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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_EMRBARHIT_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_EMRBARHIT_HH_

#include "Utils/VersionNumber.hh"

namespace MAUS {

/** @class EMRBarHit comment
 *
 *  @var ch	<-- DBB channel of the hit / 2880 elements (2832 bars, 48 test ch.) -->
 *  @var tot	<-- time over threshold measurement -->
 *  @var time	<-- trigger time w.r.t. start of the spill recorded by DBB -->
 *  @var deltat	<-- time of a hit in the DBB minus trigger hit time measured by fADC -->
 */

class EMRBarHit {
 public:
  /** @brief Default constructor - initialises to 0/NULL */
  EMRBarHit();

  /** @brief Copy constructor - any pointers are deep copied */
  EMRBarHit(const EMRBarHit& _emrbh);

  /** @brief Equality operator - any pointers are deep copied */
  EMRBarHit& operator=(const EMRBarHit& _emrbh);

  /** @brief Destructor - any member pointers are deleted */
  virtual ~EMRBarHit();

  /** @brief Returns the global bar ID */
  int GetChannel() const     { return _ch; }

  /** @brief Sets the global bar ID */
  void SetChannel(int ch)    { _ch = ch; }

  /** @brief Returns the time-over-threshold */
  int GetTot() const         { return _tot; }

  /** @brief Sets time-over-threshold */
  void SetTot(int tot)       { _tot = tot; }

  /** @brief Returns the global time with respect to the spill */
  int GetTime() const        { return _time; }

  /** @brief Sets the global time with respect to the spill */
  void SetTime(int time)     { _time = time; }

  /** @brief Returns the time with respect to the trigger time */
  int GetDeltaT() const      { return _deltat; }

  /** @brief Sets the time with respect to the trigger time */
  void SetDeltaT(int deltat) { _deltat = deltat; }

 private:
  int	_ch;
  int	_tot;
  int	_time;
  int	_deltat;

  MAUS_VERSIONED_CLASS_DEF(EMRBarHit)
};
} // namespace MAUS

#endif  // #define _SRC_COMMON_CPP_DATASTRUCTURE_EMRBARHIT_HH_
