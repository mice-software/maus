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

#include "src/common_cpp/DataStructure/Primary.hh"
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
  enum Algorithm {kNone, kLeastSquares, kConstrainedLeastSquares,
                  kConstrainedChiSquared, kSweepingChiSquared,
                  kSweepingChiSquaredWithVariableWalls};
  // *************************
  //  Constructors
  // *************************

  explicit PolynomialOpticsModel(const Json::Value & configuration);

  /** @brief no memory allocated so doesn't do anything.
   */
  ~PolynomialOpticsModel() { }

  void Build();

 protected:
  Algorithm algorithm_;
  size_t polynomial_order_;
  std::vector<double> weights_;

  // override base class version
  const std::vector<PhaseSpaceVector> PrimaryVectors();

  const TransferMap * CalculateTransferMap(
    const std::vector<PhaseSpaceVector> & start_plane_hits,
    const std::vector<PhaseSpaceVector> & end_plane_hits)
    const;

  void SetupAlgorithm();
};


} // namespace MAUS

#endif




