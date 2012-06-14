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

#ifndef SRC_COMMON_CPP_OPTICS_LEAST_SQUARES_MODEL_HH
#define SRC_COMMON_CPP_OPTICS_LEAST_SQUARES_MODEL_HH

#include <vector>

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

class LeastSquaresOpticsModel : public OpticsModel {
 public:
  // *************************
  //  Constructors
  // *************************

  LeastSquaresOpticsModel()
      : algorithm_(Algorithm.kNone), transfer_map_(NULL) { }

  /** @brief no memory allocated so doesn't do anything.
   */
  ~LeastSquaresOpticsModel() { }
  
  void Build(const Json::Value & configuration);

  TransferMap * GenerateTransferMap(
      double start_plane, double end_plane, double mass) const;
  
 private:
  Algorithm algorithm_;
  std::vector< std::vector<double> > hits_in_;
  TransferMap * transfer_map_;

  void SetupAlgorithm(const Json::Value & configuration);
  void BuildHitsIn(const Json::Value & configuration);
  void Simulate(const Json::Value & configuration);
  void CalculateTransferMaps(const Json::Value & configuration);

  /** @brief An algorithm identification class.
   * Use LeastSquaresOpticsModel.Algorithm.kWhateverAlgorithm in the constructor
   * and the constructor calls Algorithm.id() to identify the algorithm.
   */
  class Algorithm {
   public:
    Algorithm() : id_(kNone) { }
    Algorithm(size_t id) : id_(id) {}
    const size_t id() const {
      return id_;
    }

    static const Algorithm kNone;
    static const Algorithm kUnconstrainedPolynomial;
    static const Algorithm kConstrainedPolynomial;
    static const Algorithm kConstrainedChiSquared;
    static const Algorithm kSweepingChiSquared;
    static const Algorithm kSweepingChiSquaredWithVariableWalls;
   private:
    Algorithm() : id_(0) {}
    size_t id_;
  };
  const Algorithm kNone                                 = Algorithm(0);
  const Algorithm kUnconstrainedPolynomial              = Algorithm(1);
  const Algorithm kConstrainedPolynomial                = Algorithm(2);
  const Algorithm kConstrainedChiSquared                = Algorithm(3);
  const Algorithm kSweepingChiSquared                   = Algorithm(4);
  const Algorithm kSweepingChiSquaredWithVariableWalls  = Algorithm(5);
};


} // namespace MAUS

#endif




