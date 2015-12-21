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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_EMRPLANEHIT_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_EMRPLANEHIT_HH_

// C++ headers
#include <vector>

// MAUS headers
#include "Utils/VersionNumber.hh"
#include "DataStructure/EMRBarHit.hh"

namespace MAUS {

typedef std::vector<EMRBarHit> EMRBarHitArray;

/** @class EMRPlaneHit comment
 *
 *  @var emrbarhits	<-- array of bar hits in the plane -->
 *  @var plane		<-- plane id / DBB id / fADC id -->
 *  @var orientation	<-- plane orientation, can be 0 or 1 -->
 *  @var time		<-- trigger time w.r.t. the start of the spill recorded by DBB -->
 *  @var deltat		<-- time of a hit in fADC minus trigger hit time measured by fADC -->
 *  @var charge		<-- SAPMT fADC charge -->
 *  @var pedestal_area	<-- integrated charge at the pedestal -->
 *  @var samples	<-- sampled shape of the digitized SAPMT signal -->
 */

class EMRPlaneHit {
 public:
  /** @brief  Default constructor - initialises to 0/NULL */
  EMRPlaneHit();

  /** @brief Copy constructor - any pointers are deep copied */
  EMRPlaneHit(const EMRPlaneHit& emrph);

  /** @brief Equality operator - any pointers are deep copied */
  EMRPlaneHit& operator=(const EMRPlaneHit& emrph);

  /** @brief Destructor - any member pointers are deleted */
  virtual ~EMRPlaneHit();

  /** @brief Returns the array of bar hits in the plane */
  EMRBarHitArray GetEMRBarHitArray() const          { return _emrbarhits; }

  /** @brief Sets the array of bar hits in the plane */
  void SetEMRBarHitArray(EMRBarHitArray emrbarhits) { _emrbarhits = emrbarhits; }

  /** @brief Adds a bar to the plane */
  void AddEMRBarHit(EMRBarHit barhit)               { _emrbarhits.push_back(barhit); }

  /** @brief Returns the amount of bar hits in the plane */
  size_t GetEMRBarHitArraySize()                    { return _emrbarhits.size(); }

  /** @brief Returns the plane ID (0->47) */
  int GetPlane() const                              { return _plane; }

  /** @brief Sets the plane ID */
  void SetPlane(int plane)                          { _plane = plane; }

  /** @brief Returns the plane orientation (0 for x planes, 1 for y planes) */
  int GetOrientation() const                        { return _orientation; }

  /** @brief Sets the plane orientation */
  void SetOrientation(int orientation)              { _orientation = orientation; }

  /** @brief Returns the global time with respect to the spill */
  int GetTime() const                               { return _time; }

  /** @brief Sets the global time with respect to the spill */
  void SetTime(int time)                            { _time = time; }

  /** @brief Returns the time with respect to the trigger time */
  int GetDeltaT() const                             { return _deltat; }

  /** @brief Sets the time with respect to the trigger time */
  void SetDeltaT(int deltat)                        { _deltat = deltat; }

  /** @brief Returns the charge in the SAPMT */
  double GetCharge() const                          { return _charge; }

  /** @brief Sets the charge in the MAPMT */
  void SetCharge(double charge)                     { _charge = charge; }

  /** @brief Returns the integrated charge at the pedestal */
  double GetPedestalArea() const                    { return _pedestal_area; }

  /** @brief Sets the integrated charge at the pedestal */
  void SetPedestalArea(double pedestal_area)        { _pedestal_area = pedestal_area; }

  /** @brief Returns the sampled SAPMT signal */
  std::vector<int> GetSampleArray() const           { return _samples; }

  /** @brief Sets the sampled SAPMT signal */
  void SetSampleArray(std::vector<int> samples)     { _samples = samples; }

  /** @brief Returns the amount of bar hits in the plane */
  size_t GetSampleArraySize()                       { return _samples.size(); }

 private:
  EMRBarHitArray	_emrbarhits;
  int           	_plane;
  int			_orientation;
  int			_time;
  int			_deltat;
  double		_charge;
  double		_pedestal_area;
  std::vector<int>	_samples;

  MAUS_VERSIONED_CLASS_DEF(EMRPlaneHit)
};
} // namespace MAUS

#endif // #define _SRC_COMMON_CPP_DATASTRUCTURE_EMRPLANEHIT_HH_
