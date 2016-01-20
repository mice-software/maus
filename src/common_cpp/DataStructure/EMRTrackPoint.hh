/* This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
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
 *
 */

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_EMRTRACKPOINT_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_EMRTRACKPOINT_HH_

// C headers
#include <assert.h>
#include <iostream>

// MAUS headers
#include "Utils/VersionNumber.hh"
#include "DataStructure/ThreeVector.hh"

namespace MAUS {

/** @class EMRTrackPoint comment
 *
 *  @var pos	<-- (x,y,z) coordinates of the track point in the local frame -->
 *  @var gpos	<-- (x,y,z) coordinates of the track point in the global frame -->
 *  @var poserr	<-- (ex,ey,ez) uncertainties on the position -->
 *  @var ch	<-- channel id of the SP from which the track point was reconstructed -->
 *  @var resx	<-- residual distance in the xz plane -->
 *  @var resy	<-- residual distance in the yz plane -->
 *  @var chi2	<-- chi squared of the point in the xy plane: SUM_q[(qi - q)^2] -->
 */

class EMRTrackPoint {
 public:
  /** @brief Default constructor - initialises to 0/NULL */
  EMRTrackPoint();

  /** @brief Copy constructor - any pointers are deep copied */
  EMRTrackPoint(const EMRTrackPoint &emrtp);

  /** @brief Equality operator - any pointers are deep copied */
  EMRTrackPoint& operator=(const EMRTrackPoint& emrtp);

  /** @brief Destructor - any member pointers are deleted */
  virtual ~EMRTrackPoint();

  /** @brief Returns the position */
  ThreeVector GetPosition() const            { return _pos; }

  /** @brief Sets the position */
  void SetPosition(ThreeVector pos)          { _pos = pos; }

  /** @brief Returns the global position */
  ThreeVector GetGlobalPosition() const      { return _gpos; }

  /** @brief Sets the global position */
  void SetGlobalPosition(ThreeVector gpos)   { _gpos = gpos; }

  /** @brief Returns the errors on the position */
  ThreeVector GetPositionErrors() const      { return _poserr; }

  /** @brief Sets the errors on the position */
  void SetPositionErrors(ThreeVector poserr) { _poserr = poserr; }

  /** @brief Returns the channel ID (0->2879) */
  int GetChannel() const                     { return _ch; }

  /** @brief Sets the channel ID */
  void SetChannel(int ch)                    { _ch = ch; }

  /** @brief Returns the residual in the xz plane */
  double GetResidualX() const                { return _resx; }

  /** @brief Sets the residual in the xz plane */
  void SetResidualX(double resx)             { _resx = resx; }

  /** @brief Returns the residual in the yz plane */
  double GetResidualY() const                { return _resy; }

  /** @brief Sets the residual in the yz plane */
  void SetResidualY(double resy)             { _resy = resy; }

  /** @brief Sets the residuals in the xz and yz plane */
  void SetResiduals(double resx, double resy);

  /** @brief Returns the chi2 of the point */
  double GetChi2() const                     { return _chi2; }

  /** @brief Sets the chi2 of the point */
  void SetChi2(double chi2)                  { _chi2 = chi2; }

 private:
  ThreeVector	_pos;
  ThreeVector	_gpos;
  ThreeVector	_poserr;
  int		_ch;
  double	_resx;
  double	_resy;
  double	_chi2;

  MAUS_VERSIONED_CLASS_DEF(EMRTrackPoint)
};
} // namespace MAUS

#endif // #define _SRC_COMMON_CPP_DATASTRUCTURE_EMRTRACKPOINT_HH_
