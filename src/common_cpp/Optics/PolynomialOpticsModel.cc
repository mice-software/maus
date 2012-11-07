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
#include <cmath>

#include <iomanip>
#include <sstream>
#include <vector>

#include "Interface/Squeal.hh"
#include "Maths/PolynomialMap.hh"
#include "src/common_cpp/Optics/PolynomialTransferMap.hh"
#include "Recon/Global/TrackPoint.hh"
#include "Simulation/MAUSGeant4Manager.hh"
#include "Simulation/MAUSPhysicsList.hh"

namespace MAUS {

  using recon::global::TrackPoint;

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
std::cout << "DEBUG PolynomialOpticsModel::PolynomialOpticsModel(): "
          << "deltas_ = " << deltas_ << std::endl;
}

void PolynomialOpticsModel::Build() {
  // Create some test hits at the desired First plane
  const std::vector<TrackPoint> first_plane_hits = BuildFirstPlaneHits();

  // Iterate through each First plane hit
  MAUSGeant4Manager * simulator = MAUSGeant4Manager::GetInstance();
  simulator->GetPhysicsList()->BeginOfRunAction();
  std::map<int, std::vector<TrackPoint> > station_hits_map;
  std::vector<TrackPoint>::const_iterator first_plane_hit;
  for (first_plane_hit = first_plane_hits.begin();
       first_plane_hit < first_plane_hits.end();
       ++first_plane_hit) {
    // Simulate the current particle (First plane hit) through MICE.
    simulator->RunParticle(
      recon::global::PrimaryGeneratorParticle(*first_plane_hit));

    // Identify the hits by station and add them to the mappings from stations
    // to the hits they recorded.
    MapStationsToHits(station_hits_map);
  }

  // Iterate through each station
  std::map<int, std::vector<TrackPoint> >::iterator station_hits;
  for (station_hits = station_hits_map.begin();
       station_hits != station_hits_map.end();
       ++station_hits) {
    // find the average z coordinate for the station
    std::vector<TrackPoint>::iterator station_hit;

    double station_plane = station_hits->second.begin()->z();

    // Generate a transfer map between the First plane and the current station
    // and map the station ID to the transfer map
    transfer_maps_[station_plane]
      = CalculateTransferMap(first_plane_hits, station_hits->second);
  }
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

/* Create a set of phase space vectors that produce a linearly independent
 * set of N polynomial vectors where N is the number of polynomial coefficients
 * for the desired number of variables (6) and polynomial order. This is
 * necessary in order to solve the least squares problem which involves
 * the calculation of a Moore-Penrose psuedo inverse.
 */
const std::vector<TrackPoint> PolynomialOpticsModel::BuildFirstPlaneHits() {
  size_t num_poly_coefficients
    = PolynomialMap::NumberOfPolynomialCoefficients(6, polynomial_order_);

    std::vector<TrackPoint> first_plane_hits;
/*
  for (int i = 0; i < 7; ++i) {
    int first_index = i-1;
    if (first_index < 0) first_index = 0;
    int second_index = i;
    if (second_index > 5) second_index = 5;
    for (int j = first_index; j <= second_index; ++j) {
      TrackPoint first_plane_hit;

      first_plane_hit[j] = 1.;

      first_plane_hits.push_back(TrackPoint(first_plane_hit + reference_particle_,
                                 reference_particle_.z(),
                                 reference_particle_.particle_id()));
    }
  }
*/
  for (size_t i = 0; i < 6; ++i) {
    for (size_t j = i; j < 6; ++j) {
      TrackPoint first_plane_hit;

      for (size_t k = 0; k < i; ++k) {
        first_plane_hit[k] = 1.;
      }
      double delta = deltas_[j];
      fprintf(stdout, "Delta %d: %f\n", size_t(j), deltas_[j]); fflush(stdout);
      first_plane_hit[j] = delta;

      first_plane_hits.push_back(TrackPoint(first_plane_hit + reference_particle_,
                                 reference_particle_.z(),
                                 reference_particle_.particle_id()));
    }
  }
  size_t base_block_length = first_plane_hits.size();

  // adjust for the case where the base block size is greater than N(n, v)
  while (base_block_length > num_poly_coefficients) {
    first_plane_hits.pop_back();
    base_block_length = first_plane_hits.size();
  }

  int summand;
  for (size_t row = base_block_length; row < num_poly_coefficients; ++row) {
    summand = row / base_block_length;
    TrackPoint first_plane_hit
      = TrackPoint(first_plane_hits[row % base_block_length] + summand);

    first_plane_hits.push_back(TrackPoint(first_plane_hit,
                                reference_particle_.z(),
                                reference_particle_.particle_id()));
  }
std::cout << "DEBUG PolynomialOpticsModel::BuildFirstPlaneHits(): "
          << "# first plane hits = " << first_plane_hits.size() << std::endl;
  return first_plane_hits;
}

/* Calculate a transfer matrix from an equal number of inputs and output.
 */
const TransferMap * PolynomialOpticsModel::CalculateTransferMap(
    const std::vector<recon::global::TrackPoint> & start_plane_hits,
    const std::vector<recon::global::TrackPoint> & station_hits)
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
/*
    polynomial_map = PolynomialMap::PolynomialLeastSquaresFit(
        points, values, polynomial_order_, weights_);
*/
    // Fit to first order and then fit to higher orders with the
    // first order map as a constraint
    polynomial_map = PolynomialMap::PolynomialLeastSquaresFit(
        points, values, 1, weights_);
    if (polynomial_order_ > 1) {
      PolynomialMap * linear_polynomial_map = polynomial_map;
      polynomial_map = PolynomialMap::ConstrainedPolynomialLeastSquaresFit(
          points, values, polynomial_order_,
          linear_polynomial_map->GetCoefficientsAsVector(), weights_);
    }
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

  Vector<double> x_map_plot = generate_polynomial_2D(
      *polynomial_map, 2, -10., 10., 0.1);
  std::cout << "DEBUG PolynomialOpticsModel::CalculateTransferMap: "
            << "plot data for x = " << x_map_plot << std::endl;

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
