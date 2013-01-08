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

// Author: Peter Lane

#ifndef SRC_COMMON_CPP_OPTICS_LINEAR_APPROXIMATION_MODEL_HH
#define SRC_COMMON_CPP_OPTICS_LINEAR_APPROXIMATION_MODEL_HH

#include <vector>

#include "src/common_cpp/Optics/TransferMap.hh"
#include "src/common_cpp/Optics/TransferMapOpticsModel.hh"
#include "src/common_cpp/Optics/OpticsModel.hh"

namespace MAUS {

namespace recon {
namespace global {
  class WorkingTrackPoint;
}
}

class LinearApproximationOpticsModel : public TransferMapOpticsModel {
 public:
  // *************************
  //  Constructors
  // *************************

  explicit LinearApproximationOpticsModel(const Json::Value & configuration)
      : TransferMapOpticsModel(configuration) { }

  ~LinearApproximationOpticsModel() { }

 protected:
  const TransferMap * CalculateTransferMap(
      const std::vector<recon::global::WorkingTrackPoint> & start_plane_hits,
      const std::vector<recon::global::WorkingTrackPoint> & station_hits)
      const;
};

class LinearApproximationTransferMap : public TransferMap {
 public:
  LinearApproximationTransferMap(double start_plane, double end_plane,
                                 double mass)
    : start_plane_(start_plane), end_plane_(end_plane), mass_(mass) { }

  LinearApproximationTransferMap(const LinearApproximationTransferMap & map)
    : start_plane_(map.start_plane_), end_plane_(map.end_plane_),
      mass_(map.mass_) { }

  TransferMap * Inverse() const;

  CovarianceMatrix Transport(const CovarianceMatrix & covariances) const;

  /* @brief transports a phase space vector ({t, E, x, Px, y, Py}).
   *
   * @params aPhaseSpaceVector the phase space vector to be transported
   */
  PhaseSpaceVector Transport(const PhaseSpaceVector & vector) const;
 private:
  const double start_plane_;
  const double end_plane_;
  double mass_;
};

} // namespace MAUS

#endif




