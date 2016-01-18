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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_EMREVENT_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_EMREVENT_HH_

#include <vector>

#include "Utils/VersionNumber.hh"
#include "DataStructure/EMREventTrack.hh"
#include "DataStructure/ThreeVector.hh"

namespace MAUS {

typedef std::vector<EMREventTrack*> EMREventTrackArray;

/** @class EMREvent comment
 *
 *  @var emreventtracks	<-- array of event tracks (contains PHs, SPs and track) -->
 *  @var vertex		<-- coordinates of the decay vertex -->
 *  @var evertex        <-- errors on the coordinates of the decay vertex -->
 *  @var deltat		<-- mother decay time -->
 *  @var distance	<-- distance between the mother and the daughter -->
 *  @var polar		<-- global polar angle theta (inclination) -->
 *  @var azimuth	<-- global azimuthal angle phi -->
 */

class EMREvent {
 public:
  /** @brief Default constructor - initialises to 0/NULL */
  EMREvent();

  /** @brief Copy constructor - any pointers are deep copied */
  EMREvent(const EMREvent& emre);

  /** @brief Equality operator - any pointers are deep copied */
  EMREvent& operator=(const EMREvent& emre);

  /** @brief Destructor - any member pointers are deleted */
  virtual ~EMREvent();

  /** @brief Returns the array of event tracks that constitute the event **/
  EMREventTrackArray GetEMREventTrackArray() const { return _emreventtracks; }

  /** @brief Sets the array of tracks that constitute the event **/
  void SetEMREventTrackArray(EMREventTrackArray emreventtracks);

  /** @brief Adds a track event to the array **/
  void AddEMREventTrack(EMREventTrack* emrte)      { _emreventtracks.push_back(emrte); }

  /** @brief Sets the amount of event tracks **/
  size_t GetEMREventTrackArraySize()               { return _emreventtracks.size(); }

  /** @brief Returns the event track of the mother **/
  EMREventTrack* GetMotherPtr();

  /** @brief Returns the event track of the daughter **/
  EMREventTrack* GetDaughterPtr();

  /** @brief Returns the coordinates of the decay vertex */
  ThreeVector GetVertex() const                    { return _vertex; }

  /** @brief Sets the coordinates of the decay vertex */
  void SetVertex(ThreeVector vertex)               { _vertex = vertex; }

  /** @brief Returns the errors on the coordinates of the decay vertex */
  ThreeVector GetVertexErrors() const              { return _evertex; }

  /** @brief Sets the errors on the coordinates of the decay vertex */
  void SetVertexErrors(ThreeVector evertex)        { _evertex = evertex; }

  /** @brief Returns the time difference between the birth and its decay */
  double GetDeltaT() const                         { return _deltat; }

  /** @brief Sets the time difference between the birth and its decay */
  void SetDeltaT(double deltat)                    { _deltat = deltat; }

  /** @brief Returns the distance between the mother and the daughter */
  double GetDistance() const                       { return _distance; }

  /** @brief Sets the distance between the mother and the daughter */
  void SetDistance(double distance)                { _distance = distance; }

  /** @brief Returns the polar angle (inclination) between the mother and the daughter */
  double GetPolar() const                          { return _polar; }

  /** @brief Sets the polar angle (inclination) between the mother and the daughter */
  void SetPolar(double polar)                      { _polar = polar; }

  /** @brief Returns the azimuthal angle between the mother and the daughter */
  double GetAzimuth() const                        { return _azimuth; }

  /** @brief Sets the azimuthal angle between the mother and the daughter */
  void SetAzimuth(double azimuth)                  { _azimuth = azimuth; }

 private:
  EMREventTrackArray	_emreventtracks;
  ThreeVector		_vertex;
  ThreeVector		_evertex;
  double		_deltat;
  double		_distance;
  double		_polar;
  double		_azimuth;

  MAUS_VERSIONED_CLASS_DEF(EMREvent)
};
} // namespace MAUS

#endif // #define _SRC_COMMON_CPP_DATASTRUCTURE_EMREVENT_HH_
