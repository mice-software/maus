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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_EMREVENTTRACK_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_EMREVENTTRACK_HH_

#include <string>
#include <vector>

#include "Utils/VersionNumber.hh"
#include "DataStructure/EMRPlaneHit.hh"
#include "DataStructure/EMRSpacePoint.hh"
#include "DataStructure/EMRTrack.hh"

namespace MAUS {

typedef std::vector<EMRPlaneHit*> EMRPlaneHitArray;
typedef std::vector<EMRSpacePoint*> EMRSpacePointArray;

/** @class EMREventTrack comment
 *
 *  @var emrplanehits	<-- array of planes hit by the track -->
 *  @var emrspacepoints	<-- array of reconstructed space points -->
 *  @var track		<-- fitted track of the particle -->
 *  @var type		<-- type of particle: mother, daughter, candidate -->
 *  @var id		<-- unique identifier of the track of type "type" -->
 *  @var time		<-- returns the global time of the event track in nanoseconds -->
 *  @var plane_density	<-- density of planes hit on the path of the particle -->
 *  @var total_charge	<-- Integrated charge over all of the space points -->
 *  @var charge_ratio	<-- Ratio of charge density of the core over the tail -->
 */

class EMREventTrack {
 public:
  /** @brief Default constructor - initialises to 0/NULL */
  EMREventTrack();

  /** @brief Copy constructor - any pointers are deep copied */
  EMREventTrack(const EMREventTrack& emret);

  /** @brief Equality operator - any pointers are deep copied */
  EMREventTrack& operator=(const EMREventTrack& emret);

  /** @brief Destructor - any member pointers are deleted */
  virtual ~EMREventTrack();

  /** @brief Returns the array of planes hit **/
  EMRPlaneHitArray GetEMRPlaneHitArray() const     { return _emrplanehits; }

  /** @brief Sets the array of planes hit **/
  void SetEMRPlaneHitArray(EMRPlaneHitArray emrplanehits);

  /** @brief Adds a plane hit to the array **/
  void AddEMRPlaneHit(EMRPlaneHit* emrph)          { _emrplanehits.push_back(emrph); }

  /** @brief Returns the amount of planes hit **/
  size_t GetEMRPlaneHitArraySize()                 { return _emrplanehits.size(); }

  /** @brief Returns the array of reconstructed space points **/
  EMRSpacePointArray GetEMRSpacePointArray() const { return _emrspacepoints; }

  /** @brief Sets the array of planes hit **/
  void SetEMRSpacePointArray(EMRSpacePointArray emrspacepoints);

  /** @brief Adds a space point to the array **/
  void AddEMRSpacePoint(EMRSpacePoint* emrsp)      { _emrspacepoints.push_back(emrsp); }

  /** @brief Returns the amount of space points **/
  size_t GetEMRSpacePointArraySize()               { return _emrspacepoints.size(); }

  /** @brief Returns the particle's track **/
  EMRTrack GetEMRTrack() const                     { return _track; }

  /** @brief Returns the particle's track **/
  EMRTrack* GetEMRTrackPtr()                       { return &_track; }

  /** @brief Sets the particle's track **/
  void SetEMRTrack(EMRTrack track)                 { _track = track; }

  /** @brief Returns type of particle **/
  std::string GetType() const                      { return _type; }

  /** @brief Sets the type of particle **/
  void SetType(std::string type)                   { _type = type; }

  /** @brief Returns the identifier of the track **/
  int GetTrackId() const                           { return _id; }

  /** @brief Sets the identifier of the track **/
  void SetTrackId(int id)                          { _id = id; }

  /** @brief Returns the global time of the event track **/
  double GetTime() const                           { return _time; }

  /** @brief Sets the global time of the event track **/
  void SetTime(double time)                        { _time = time; }

  /** @brief Returns the plane density in the MAPMT */
  double GetPlaneDensityMA() const                 { return _plane_density_ma; }

  /** @brief Sets the plane density in the MAPMT */
  void SetPlaneDensityMA(double plane_density_ma)  { _plane_density_ma = plane_density_ma; }

  /** @brief Returns the plane density in the SAPMT */
  double GetPlaneDensitySA() const                 { return _plane_density_sa; }

  /** @brief Sets the plane density in the SAPMT */
  void SetPlaneDensitySA(double plane_density_sa)  { _plane_density_sa = plane_density_sa; }

  /** @brief Returns the total charge in the MAPMT */
  double GetTotalChargeMA() const                  { return _total_charge_ma; }

  /** @brief Sets the total charge in the MAPMT */
  void SetTotalChargeMA(double total_charge_ma)    { _total_charge_ma = total_charge_ma; }

  /** @brief Returns the total charge in the SAPMT */
  double GetTotalChargeSA() const                  { return _total_charge_sa; }

  /** @brief Sets the total charge in the SAPMT */
  void SetTotalChargeSA(double total_charge_sa)    { _total_charge_sa = total_charge_sa; }

  /** @brief Returns the charge ratio in the SAPMT */
  double GetChargeRatioMA() const                  { return _charge_ratio_ma; }

  /** @brief Sets the charge ratio in the SAPMT */
  void SetChargeRatioMA(double charge_ratio_ma)    { _charge_ratio_ma = charge_ratio_ma; }

  /** @brief Returns the charge ratio in the MAPMT */
  double GetChargeRatioSA() const                  { return _charge_ratio_sa; }

  /** @brief Sets the charge ratio in the MAPMT */
  void SetChargeRatioSA(double charge_ratio_sa)    { _charge_ratio_sa = charge_ratio_sa; }

 private:
  EMRPlaneHitArray	_emrplanehits;
  EMRSpacePointArray	_emrspacepoints;
  EMRTrack		_track;
  std::string		_type;
  int			_id;
  double		_time;
  double		_plane_density_ma;
  double		_plane_density_sa;
  double		_total_charge_ma;
  double		_total_charge_sa;
  double		_charge_ratio_ma;
  double		_charge_ratio_sa;

  MAUS_VERSIONED_CLASS_DEF(EMREventTrack)
};
} // namespace MAUS

#endif // #define _SRC_COMMON_CPP_DATASTRUCTURE_EMREVENTTRACK_HH_
