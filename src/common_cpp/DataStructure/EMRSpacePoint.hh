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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_EMRSPACEPOINT_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_EMRSPACEPOINT_HH_

#include "Utils/VersionNumber.hh"
#include "DataStructure/ThreeVector.hh"

namespace MAUS {

/** @class EMRSpacePoint comment
 *
 *  @var pos		<-- (x,y,z) coordinates of the space point in the local frame -->
 *  @var gpos		<-- (x,y,z) coordinates of the space point in the global frame -->
 *  @var errors		<-- (ex,ey,ez) uncertainties on the position -->
 *  @var ch		<-- channel id of the bar from which the space point was reconstructed -->
 *  @var time		<-- average time w.r.t. the start of the spill recorded by DBB -->
 *  @var deltat		<-- average time of a hit in the DBB minus trigger hit time measured by fADC -->
 *  @var charge_ma	<-- MAPMT reconstructed charge -->
 *  @var charge_sa	<-- SAPMT corrected fADC charge -->
 */

class EMRSpacePoint {
 public:
  /** @brief Default constructor - initialises to 0/NULL */
  EMRSpacePoint();

  /** @brief Copy constructor - any pointers are deep copied */
  EMRSpacePoint(const EMRSpacePoint& _emrsp);

  /** @brief Equality operator - any pointers are deep copied */
  EMRSpacePoint& operator=(const EMRSpacePoint& _emrsp);

  /** @brief Destructor - any member pointers are deleted */
  virtual ~EMRSpacePoint();

  /** @brief  Returns the position */
  ThreeVector GetPosition() const            { return _pos; }

  /** @brief  Sets the position */
  void SetPosition(ThreeVector pos)          { _pos = pos; }

  /** @brief  Returns the global position */
  ThreeVector GetGlobalPosition() const      { return _gpos; }

  /** @brief  Sets the global position */
  void SetGlobalPosition(ThreeVector gpos)   { _gpos = gpos; }

  /** @brief  Returns the errors on the position */
  ThreeVector GetPositionErrors() const      { return _poserr; }

  /** @brief  Sets the errors on the position */
  void SetPositionErrors(ThreeVector poserr) { _poserr = poserr; }

  /** @brief Returns the channel ID (0->2879) */
  int GetChannel() const                     { return _ch; }

  /** @brief Sets the channel ID */
  void SetChannel(int ch)                    { _ch = ch; }

  /** @brief Returns the global time with respect to the spill */
  double GetTime() const                     { return _time; }

  /** @brief Sets the global time with respect to the spill */
  void SetTime(double time)                  { _time = time; }

  /** @brief Returns the time with respect to the trigger time */
  double GetDeltaT() const                   { return _deltat; }

  /** @brief Sets the time with respect to the trigger time */
  void SetDeltaT(double deltat)              { _deltat = deltat; }

  /** @brief Returns the reconstructed MAPMT charge */
  double GetChargeMA() const                 { return _chargema; }

  /** @brief Sets the reconstructed MAPMT charge */
  void SetChargeMA(double chargema)          { _chargema = chargema; }

  /** @brief Returns the SAPMT charge */
  double GetChargeSA() const                 { return _chargesa; }

  /** @brief Sets the SAPMT charge */
  void SetChargeSA(double chargesa)          { _chargesa = chargesa; }

 private:
  ThreeVector	_pos;
  ThreeVector	_gpos;
  ThreeVector	_poserr;
  int		_ch;
  double	_time;
  double	_deltat;
  double	_chargema;
  double	_chargesa;

  MAUS_VERSIONED_CLASS_DEF(EMRSpacePoint)
};
} // namespace MAUS

#endif  // #define _SRC_COMMON_CPP_DATASTRUCTURE_EMRSPACEPOINT_HH_
