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

/* Author: Chris Rogers (G4MICE)
 * Author: Peter Lane (MAUS update)
 */

#include "Optics/PolynomialOpticsModel.hh"

#include <cfloat>
#include <math.h>

#include <iomanip>
#include <sstream>
#include <vector>

#include "Interface/Squeal.hh"
#include "Maths/PolynomialMap.hh"
#include "src/common_cpp/Optics/PolynomialTransferMap.hh"

namespace MAUS {

const PolynomialOpticsModel::Algorithm
  PolynomialOpticsModel::Algorithm::kNone
    = Algorithm(0);
const PolynomialOpticsModel::Algorithm
  PolynomialOpticsModel::Algorithm::kLeastSquares
    = Algorithm(1);
const PolynomialOpticsModel::Algorithm
  PolynomialOpticsModel::Algorithm::kConstrainedLeastSquares
    = Algorithm(2);
const PolynomialOpticsModel::Algorithm
  PolynomialOpticsModel::Algorithm::kConstrainedChiSquared
    = Algorithm(3);
const PolynomialOpticsModel::Algorithm
  PolynomialOpticsModel::Algorithm::kSweepingChiSquared
    = Algorithm(4);
const PolynomialOpticsModel::Algorithm
  PolynomialOpticsModel::Algorithm::kSweepingChiSquaredWithVariableWalls
    = Algorithm(5);

PolynomialOpticsModel::PolynomialOpticsModel(const Json::Value & configuration)
      : TransferMapOpticsModel(configuration), algorithm_(Algorithm::kNone) {
  // Determine which fitting algorithm to use
  SetupAlgorithm();

  polynomial_order_ = JsonWrapper::GetProperty(
      *configuration_,
      "PolynomialOpticsModel_order",
      JsonWrapper::intValue).asInt();

  // TODO(plane1@hawk.iit.edu) Get from configuration
  const std::vector<double> weights_(6, 1.);
  deltas_ = PhaseSpaceVector(1, 1, 1, 1, 1, 1);
}


void PolynomialOpticsModel::SetupAlgorithm() {
  Json::Value algorithm_names = JsonWrapper::GetProperty(
      *configuration_,
      "PolynomialOpticsModel_algorithms",
      JsonWrapper::arrayValue);
  Json::Value algorithm_name = JsonWrapper::GetProperty(
      *configuration_,
      "PolynomialOpticsModel_algorithm",
      JsonWrapper::stringValue);
  size_t algorithm;
  for (algorithm = 0; algorithm < algorithm_names.size(); ++algorithm) {
    if (algorithm_name == algorithm_names[algorithm]) {
      break;  // leave the current index into algorithm_names in algorithm
    }
  }
  ++algorithm;  // first algorithm constant is kNone

  if (algorithm == Algorithm::kLeastSquares.id()) {
    algorithm_ = Algorithm::kLeastSquares;
  } else if (algorithm == Algorithm::kConstrainedLeastSquares.id()) {
    algorithm_ = Algorithm::kConstrainedLeastSquares;
  } else if (algorithm == Algorithm::kConstrainedChiSquared.id()) {
    algorithm_ = Algorithm::kConstrainedChiSquared;
  } else if (algorithm == Algorithm::kSweepingChiSquared.id()) {
    algorithm_ = Algorithm::kSweepingChiSquared;
  } else if (algorithm == Algorithm::kSweepingChiSquaredWithVariableWalls.id()) {
    algorithm_ = Algorithm::kSweepingChiSquaredWithVariableWalls;
  } else {
    algorithm_ = Algorithm::kNone;
  }
}

const TransferMap * PolynomialOpticsModel::CalculateTransferMap(
    const std::vector<reconstruction::global::TrackPoint> & start_plane_hits,
    const std::vector<reconstruction::global::TrackPoint> & station_hits)
    const {

  if (start_plane_hits.size() != station_hits.size()) {
    throw(Squeal(Squeal::nonRecoverable,
                  "The number of start plane hits is not the same as the number "
                  "of hits per station.",
                  "PolynomialOpticsModel::CalculateTransferMap()"));
  }

  std::vector< std::vector<double> > points;
  for (size_t pt_index = 0; pt_index < start_plane_hits.size(); ++pt_index) {
    std::vector<double> point;
    for (size_t coord_index = 0; coord_index < 6; ++coord_index) {
      point.push_back(start_plane_hits[pt_index][coord_index]);
    }
    points.push_back(point);
  }

  std::vector< std::vector<double> > values;
  for (size_t val_index = 0; val_index < station_hits.size(); ++val_index) {
    std::vector<double> value;
    for (size_t coord_index = 0; coord_index < 6; ++coord_index) {
      value.push_back(station_hits[val_index][coord_index]);
    }
    values.push_back(value);
  }

  PolynomialMap * polynomial_map = NULL;

  if (algorithm_ == Algorithm::kNone) {
      throw(Squeal(Squeal::nonRecoverable,
                   "No fitting algorithm specified in configuration.",
                   "PolynomialOpticsModel::CalculateTransferMap()"));
  } else if (algorithm_ == Algorithm::kLeastSquares) {
    // unconstrained least squares
    polynomial_map = PolynomialMap::PolynomialLeastSquaresFit(
        points, values, polynomial_order_, weights_);
  } else if (algorithm_ == Algorithm::kConstrainedLeastSquares) {
    // constrained least squares
    // ConstrainedLeastSquaresFit(...);
    throw(Squeal(Squeal::nonRecoverable,
                  "Constrained Polynomial fitting algorithm "
                  "is not yet implemented.",
                  "PolynomialOpticsModel::CalculateTransferMap()"));
  } else if (algorithm_ == Algorithm::kConstrainedChiSquared) {
    // constrained chi squared least squares
    // Chi2ConstrainedLeastSquaresFit(...);
    throw(Squeal(Squeal::nonRecoverable,
                  "Constrained Chi Squared fitting algorithm "
                  "is not yet implemented.",
                  "PolynomialOpticsModel::CalculateTransferMap()"));
  } else if (algorithm_ == Algorithm::kSweepingChiSquared) {
    // sweeping chi squared least squares
    // Chi2SweepingLeastSquaresFit(...);
    throw(Squeal(Squeal::nonRecoverable,
                  "Sweeping Chi Squared fitting algorithm "
                  "is not yet implemented.",
                  "PolynomialOpticsModel::CalculateTransferMap()"));
  } else if (algorithm_ == Algorithm::kSweepingChiSquaredWithVariableWalls) {
    // sweeping chi squared with variable walls
    // Chi2SweepingLeastSquaresFitVariableWalls(...);
    throw(Squeal(Squeal::nonRecoverable,
                  "Sweeping Chi Squared Variable Walls fitting algorithm "
                  "is not yet implemented.",
                  "PolynomialOpticsModel::CalculateTransferMap()"));
  } else {
    throw(Squeal(Squeal::nonRecoverable,
                  "Unrecognized fitting algorithm in configuration.",
                  "PolynomialOpticsModel::CalculateTransferMap()"));
  }

  TransferMap * transfer_map = new PolynomialTransferMap(
    *polynomial_map, reference_particle_);
  delete polynomial_map;

  return transfer_map;
}

    /*
  static PolynomialMap* PolynomialLeastSquaresFit(
    const std::vector< std::vector<double> >& points,
    const std::vector< std::vector<double> >& values,
    unsigned int                                        polynomialOrder,
    const VectorMap*                          weightFunction);

  static PolynomialMap* PolynomialLeastSquaresFit(
    const std::vector< std::vector<double> >& points,
    const std::vector< std::vector<double> >& values,
    unsigned int                                        polynomialOrder,
    const std::vector<double>& weights = std::vector<double>());

  static PolynomialMap* ConstrainedPolynomialLeastSquaresFit
      (const std::vector< std::vector<double> >&  points,
       const std::vector< std::vector<double> >& values,
       unsigned int polynomialOrder,
       std::vector< PolynomialCoefficient > coeffs,
       const std::vector<double>& weights = std::vector<double>());

  static PolynomialMap* Chi2ConstrainedLeastSquaresFit
      (const std::vector< std::vector<double> >&  xin,
       const std::vector< std::vector<double> >& xout,
       unsigned int polynomialOrder,
       std::vector< PolynomialMap::PolynomialCoefficient > coeffs,
       double chi2Start, double discardStep, double* chi2End, double chi2Limit,
       std::vector<double> weights, bool firstIsMean = false);

  static PolynomialMap* Chi2SweepingLeastSquaresFit
      (const VectorMap& vec, unsigned int polynomialOrder,
       std::vector< PolynomialMap::PolynomialCoefficient > coeffs,
       double chi2Max, std::vector<double>& delta, double deltaFactor,
       int maxNumberOfSteps);

  static PolynomialMap* Chi2SweepingLeastSquaresFitVariableWalls
      (const VectorMap& vec, unsigned int polynomialOrder,
       std::vector< PolynomialMap::PolynomialCoefficient > coeffs,
       double chi2Max, std::vector<double>& delta, double deltaFactor,
       int maxNumberOfSteps, std::vector<double> max_delta);

  double GetAvgChi2OfDifference(std::vector< std::vector<double> > in,
                                std::vector< std::vector<double> > out);
    int algorithm_id = algorithm_->id();
    switch {
      case 5: {  // sweeping chi squared with variable walls
        // Chi2SweepingLeastSquaresFitVariableWalls(...);
        break;
      }
      case 4: {  // sweeping chi squared
        // Chi2SweepingLeastSquaresFit(...);
        break;
      }
      case 3: {  // constrained chi squared
        // Chi2ConstrainedLeastSquaresFit(...);
        break;
      }
      case 2: {  // constrained polynomial
        // ConstrainedPolynomialLeastSquaresFit(...);
        break;
      }
      case 1:    // unconstrained polynomial
      default: {
        // PolynomialLeastSquaresFit(...);
      }
    }
  }
*/
}  // namespace MAUS
