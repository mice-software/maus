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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_EMRSPILLDATA_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_EMRSPILLDATA_HH_

#include <vector>

#include "Utils/VersionNumber.hh"
#include "DataStructure/EMRBarHit.hh"
#include "DataStructure/EMREventTrack.hh"

namespace MAUS {

typedef std::vector<EMRBarHit> EMRBarHitArray;
typedef std::vector<EMREventTrack*> EMREventTrackArray;

/** @class EMREvent comment
 *
 *  @var emrbarhits	<-- Array of bars hit outside of the trigger windows -->
 *  @var emreventtracks	<-- Array of event tracks reconstructed from the hits -->
 */

class EMRSpillData {
 public:
  /** Default constructor - initialises to 0/NULL */
  EMRSpillData();

  /** Copy constructor - any pointers are deep copied */
  EMRSpillData(const EMRSpillData& emrsd);

  /** Equality operator - any pointers are deep copied */
  EMRSpillData& operator=(const EMRSpillData& emrsd);

  /** Destructor - any member pointers are deleted */
  virtual ~EMRSpillData();

  /** @brief Returns the array of bar hits */
  EMRBarHitArray GetEMRBarHitArray() const          { return _emrbarhits; }

  /** @brief Sets the array of bar hits */
  void SetEMRBarHitArray(EMRBarHitArray emrbarhits) { _emrbarhits = emrbarhits; }

  /** @brief Adds a bar hit to the array */
  void AddEMRBarHit(EMRBarHit emrbh)                { _emrbarhits.push_back(emrbh); }

  /** @brief Returns the amount of bar hits in the plane */
  size_t GetEMRBarHitArraySize()                    { return _emrbarhits.size(); }

  /** @brief Returns the array of reconstructed event tracks **/
  EMREventTrackArray GetEMREventTrackArray() const  { return _emreventtracks; }

  /** @brief Sets the array of reconstructed event tracks **/
  void SetEMREventTrackArray(EMREventTrackArray emreventtracks);

  /** @brief Adds a track event to the array **/
  void AddEMREventTrack(EMREventTrack* emrte)       { _emreventtracks.push_back(emrte); }

  /** @brief Returns the amount of event tracks **/
  size_t GetEMREventTrackArraySize()                { return _emreventtracks.size(); }

 private:
  EMRBarHitArray	_emrbarhits;
  EMREventTrackArray	_emreventtracks;

  MAUS_VERSIONED_CLASS_DEF(EMRSpillData)
};
} // namespace MAUS

#endif // #define _SRC_COMMON_CPP_DATASTRUCTURE_EMRSPILLDATA_HH_
