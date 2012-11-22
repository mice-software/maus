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
#include <map>

#include "Interface/Squeal.hh"
#include "Simulation/MAUSPrimaryGeneratorAction.hh"
#include "src/common_cpp/Optics/OpticsModel.hh"
#include "src/common_cpp/Optics/PhaseSpaceVector.hh"
#include "Recon/Global/TrackPoint.hh"

class dataCards;
class MiceMaterials;
class MiceModule;

namespace Json {
  class Value;
}

namespace MAUS {

class TransferMap;

/** @class TransferMapOpticsModel simulates test particles through MICE and
 *  generates intermediate hits which are then used to build optics model
 *  transfer maps. This is a virtual base class for TransferMap based optics
 *  models. The derived classes must implement CalculateTransferMap() which is
 *  called by Build().
 */
class TransferMapOpticsModel : public OpticsModel {
 public:
  // ******************************
  //  Constructors
  // ******************************

  /* @brief initialize the generator using the configuration.
   * @params configuration the configuration as a Json document
   */
  explicit TransferMapOpticsModel(const Json::Value & configuration);

  ~TransferMapOpticsModel();

  /* @brief 
   */
  void Build();

  // ******************************
  //  OpticsModel functions
  // ******************************

  CovarianceMatrix Transport(const CovarianceMatrix & covariances,
                                     const double end_plane) const;

  CovarianceMatrix Transport(const CovarianceMatrix & covariances,
                                     const double start_plane,
                                     const double end_plane) const;

  PhaseSpaceVector Transport(const PhaseSpaceVector & vector,
                                     const double end_plane) const;

  PhaseSpaceVector Transport(const PhaseSpaceVector & vector,
                                     const double start_plane,
                                     const double end_plane) const;

 protected:
  const TransferMap * GenerateTransferMap(const double plane) const;

  const dataCards * data_cards_;
  const MiceMaterials * mice_materials_;
  const MiceModule * mice_module_;
  const Json::Value * maus_configuration_;

  std::map<double, const TransferMap *> transfer_maps_;
  MAUSPrimaryGeneratorAction::PGParticle reference_pgparticle_;
  recon::global::TrackPoint reference_particle_;
  double time_offset_;
  PhaseSpaceVector deltas_;

  TransferMapOpticsModel();

  /* @brief Build a set of TrackPoints displaced from the start plane
   * reference trajectory by a configuration-specified amount in one of the six
   * phase space coordinates. The set is used as test particles to extrapolate
   * transfer matrices to other detector station planes.
   */
  const std::vector<recon::global::TrackPoint> BuildFirstPlaneHits();

  /* @brief Identify simulated hits by station and add them to the mappings
   * from station ID to hits recorded by that station.
   */
  void MapStationsToHits(
      std::map<int, std::vector<recon::global::TrackPoint> > & station_hits,
      const Json::Value & event);

  const TransferMap * FindTransferMap(const double end_plane) const;

  /* @brief called by Build() to calculate transfer maps between the start plane
   * and the station planes. The returned pointer points to a newly allocated
   * TranferMap. The caller assumes all responsibility for the allocated memory.
   */
  virtual const TransferMap * CalculateTransferMap(
      const std::vector<recon::global::TrackPoint> & start_plane_hits,
      const std::vector<recon::global::TrackPoint> & station_hits)
      const = 0;
};

std::ostream& operator<<(std::ostream& out, const TransferMapOpticsModel& map);
}  // namespace MAUS

#endif
