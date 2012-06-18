/* This file is part of MAUS: http://  micewww.pp.rl.ac.uk:8080/projects/maus
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
 * along with MAUS.  If not, see <http://  www.gnu.org/licenses/>.
 */

/* Author: Peter Lane
 */

#ifndef COMMON_CPP_OPTICS_HIT_GENERATOR_MAP_HH
#define COMMON_CPP_OPTICS_HIT_GENERATOR_MAP_HH

#include <ostream>
#include <vector>

#include "Interface/Squeal.hh"

namespace MAUS {

/** @class TransferMapOpticsModel simulates test particles through MICE and
 *  generates intermediate hits which are then used to build optics model
 *  transfer maps. This is a virtual base class for TransferMap based optics
 *  models. The derived classes must implement CalculateTransferMap() which is
 *  called by Build().
 */
class TransferMapOpticsModel {
 public:
  // ******************************
  //  Constructors
  // ******************************

  /* @brief initialize the generator using the configuration.
   * @params configuration the configuration as a Json document
   */
  TransferMapOpticsModel(const Json::Value & configuration);

  /* @brief 
   */
  void Build();
 protected:
  std::vector<TransferMap *> transfer_maps_;
  TransferMapOpticsModel();

  /* @brief Build a set of PhaseSpaceVectors displaced from the start plane
   * reference trajectory by a configuration-specified amount in one of the six
   * phase space coordinates. The set is used as test particles to extrapolate
   * transfer matrices to other detector station planes.
   */
  const std::vector<PhaseSpaceVector> BuildStartPlaneHits();

  /* @brief Identify simulated hits by station and add them to the mappings
   * from station ID to hits recorded by that station.
   */
  void MapStationsToHits(
      std::map<int, std::vector<PhaseSpaceVector> > & station_hits);

  /* @brief called by Build() to calculate transfer maps between the start plane
   * and the station planes. The returned pointer points to a newly allocated
   * TranferMap. The caller assumes all responsibility for the allocated memory.
   */
  virtual TransferMap * CalculateTransferMap(
      std::vector<PhaseSpaceVector> start_plane_hits,
      std::vector<PhaseSpaceVector> station_hits) = 0;
};

std::ostream& operator<<(std::ostream& out, const TransferMapOpticsModel& map);
}  // namespace MAUS

#endif
