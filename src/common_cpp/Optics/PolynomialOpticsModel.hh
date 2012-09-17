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

#ifndef SRC_COMMON_CPP_OPTICS_POLYNOMIAL_OPTICS_MODEL_HH
#define SRC_COMMON_CPP_OPTICS_POLYNOMIAL_OPTICS_MODEL_HH

#include <vector>

#include "src/common_cpp/Optics/PhaseSpaceVector.hh"
#include "src/common_cpp/Optics/TransferMapOpticsModel.hh"

namespace Json {
  class Value;
}

namespace MAUS {

class TransferMap;

/** @brief Calculates TransferMaps using any of several least squares fit
 *  functions.
 *  
 *  Here we find a polynomial of arbitrary dimension and order from a set of
 *  points by the method of least squares. The particular version of least
 *  squares used is specified in the constructor.
 *
 *  Note that the problem must be overspecified, so the number of points must be
 *  >= number of polynomial coefficients.
 *  A few interesting variations on this theme... have a look!\n
 * \n
 */

class PolynomialOpticsModel : public TransferMapOpticsModel {
 public:
  // *************************
  //  Constructors
  // *************************

  explicit PolynomialOpticsModel(const Json::Value & configuration);

  /** @brief no memory allocated so doesn't do anything.
   */
  ~PolynomialOpticsModel() { }

  void Build();

  /** @brief An algorithm identification class.
   * Use PolynomialOpticsModel::Algorithm::kWhateverAlgorithm
   * and the constructor calls Algorithm.id() to identify the algorithm.
   */
  class Algorithm {
   public:
    const size_t id() const {
      return id_;
    }

    const bool operator==(const Algorithm & rhs) const {
      return id_ == rhs.id_;
    }

    const bool operator!=(const Algorithm & rhs) const {
      return id_ != rhs.id_;
    }

    static const Algorithm kNone;
    static const Algorithm kLeastSquares;
    static const Algorithm kConstrainedLeastSquares;
    static const Algorithm kConstrainedChiSquared;
    static const Algorithm kSweepingChiSquared;
    static const Algorithm kSweepingChiSquaredWithVariableWalls;
   private:
    Algorithm() : id_(0) {}
    explicit Algorithm(size_t id) : id_(id) {}
    size_t id_;
  };

 protected:
  Algorithm algorithm_;
  size_t polynomial_order_;
  std::vector<double> weights_;
  PhaseSpaceVector deltas_;

  // override base class version
  const std::vector<reconstruction::global::TrackPoint> BuildFirstPlaneHits();

  const TransferMap * CalculateTransferMap(
    const std::vector<reconstruction::global::TrackPoint> & start_plane_hits,
    const std::vector<reconstruction::global::TrackPoint> & station_hits)
    const;

  void SetupAlgorithm();
};


} // namespace MAUS

#endif




