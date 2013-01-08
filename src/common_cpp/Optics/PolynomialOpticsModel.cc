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
#include <map>

#include <iomanip>
#include <sstream>
#include <vector>

#include "Interface/Squeal.hh"
#include "Maths/PolynomialMap.hh"
#include "src/common_cpp/Optics/PolynomialTransferMap.hh"
#include "Recon/Global/WorkingTrackPoint.hh"
#include "Simulation/MAUSGeant4Manager.hh"
#include "Simulation/MAUSPhysicsList.hh"

namespace MAUS {

using recon::global::WorkingTrackPoint;

PolynomialOpticsModel::PolynomialOpticsModel(const Json::Value & configuration)
      : TransferMapOpticsModel(configuration), algorithm_(kNone) {
  // Determine which fitting algorithm to use
  SetupAlgorithm();

  polynomial_order_ = JsonWrapper::GetProperty(
      *configuration_,
      "PolynomialOpticsModel_order",
      JsonWrapper::intValue).asInt();

  // TODO(plane1@hawk.iit.edu) Get from configuration
  const std::vector<double> weights_(6, 1.);
}

void PolynomialOpticsModel::Build() {
  // Create some test hits at the desired First plane
  const std::vector<WorkingTrackPoint> first_plane_hits = BuildFirstPlaneHits();

  // Iterate through each First plane hit
  MAUSGeant4Manager * simulator = MAUSGeant4Manager::GetInstance();
  simulator->GetPhysicsList()->BeginOfRunAction();
  std::map<int, std::vector<WorkingTrackPoint> > station_hits_map;
  std::vector<WorkingTrackPoint>::const_iterator first_plane_hit;
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
  std::map<int, std::vector<WorkingTrackPoint> >::iterator station_hits;
  for (station_hits = station_hits_map.begin();
       station_hits != station_hits_map.end();
       ++station_hits) {
    // find the average z coordinate for the station
    std::vector<WorkingTrackPoint>::iterator station_hit;

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
  for (algorithm = 0; algorithm <= algorithm_names.size(); ++algorithm) {
    if (algorithm_name == algorithm_names[algorithm]) {
      break;  // leave the current index into algorithm_names in algorithm
    }
  }

  switch (++algorithm) {  // the first element in Algorithm is kNone
    case kLeastSquares:
      algorithm_ = kLeastSquares;
      break;
    case kConstrainedLeastSquares:
      algorithm_ = kConstrainedLeastSquares;
      break;
    case kConstrainedChiSquared:
      algorithm_ = kConstrainedChiSquared;
      break;
    case kSweepingChiSquared:
      algorithm_ = kSweepingChiSquared;
      break;
    case kSweepingChiSquaredWithVariableWalls:
      algorithm_ = kSweepingChiSquaredWithVariableWalls;
      break;
    default: algorithm_ = kNone;
  }
}

/* Create a set of phase space vectors that produce a linearly independent
 * set of N polynomial vectors where N is the number of polynomial coefficients
 * for the desired number of variables (6) and polynomial order. This is
 * necessary in order to solve the least squares problem which involves
 * the calculation of a Moore-Penrose psuedo inverse.
 */
const std::vector<WorkingTrackPoint> PolynomialOpticsModel::BuildFirstPlaneHits() {
  size_t num_poly_coefficients
    = PolynomialMap::NumberOfPolynomialCoefficients(6, polynomial_order_);

    std::vector<WorkingTrackPoint> first_plane_hits;
  for (size_t i = 0; i < 6; ++i) {
    for (size_t j = i; j < 6; ++j) {
      WorkingTrackPoint first_plane_hit;

      for (size_t k = 0; k < i; ++k) {
        first_plane_hit[k] = 1.;
      }
      double delta = deltas_[j];
      first_plane_hit[j] = delta;

      first_plane_hits.push_back(WorkingTrackPoint(first_plane_hit + reference_particle_,
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
    WorkingTrackPoint first_plane_hit
      = WorkingTrackPoint(first_plane_hits[row % base_block_length] + summand);

    first_plane_hits.push_back(WorkingTrackPoint(first_plane_hit + reference_particle_,
                                reference_particle_.z(),
                                reference_particle_.particle_id()));
  }
  return first_plane_hits;
}

/* Calculate a transfer matrix from an equal number of inputs and output.
 */
const TransferMap * PolynomialOpticsModel::CalculateTransferMap(
    const std::vector<recon::global::WorkingTrackPoint> & start_plane_hits,
    const std::vector<recon::global::WorkingTrackPoint> & station_hits)
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

  switch (algorithm_) {
    case kNone:
      throw(Squeal(Squeal::nonRecoverable,
                    "No fitting algorithm specified in configuration.",
                    "PolynomialOpticsModel::CalculateTransferMap()"));
    case kLeastSquares:
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
      break;
    case kConstrainedLeastSquares:
      // constrained least squares
      // ConstrainedLeastSquaresFit(...);
      throw(Squeal(Squeal::nonRecoverable,
                    "Constrained Polynomial fitting algorithm "
                    "is not yet implemented.",
                    "PolynomialOpticsModel::CalculateTransferMap()"));
    case kConstrainedChiSquared:
      // constrained chi squared least squares
      // Chi2ConstrainedLeastSquaresFit(...);
      throw(Squeal(Squeal::nonRecoverable,
                    "Constrained Chi Squared fitting algorithm "
                    "is not yet implemented.",
                    "PolynomialOpticsModel::CalculateTransferMap()"));
    case kSweepingChiSquared:
      // sweeping chi squared least squares
      // Chi2SweepingLeastSquaresFit(...);
      throw(Squeal(Squeal::nonRecoverable,
                    "Sweeping Chi Squared fitting algorithm "
                    "is not yet implemented.",
                    "PolynomialOpticsModel::CalculateTransferMap()"));
    case kSweepingChiSquaredWithVariableWalls:
      // sweeping chi squared with variable walls
      // Chi2SweepingLeastSquaresFitVariableWalls(...);
      throw(Squeal(Squeal::nonRecoverable,
                    "Sweeping Chi Squared Variable Walls fitting algorithm "
                    "is not yet implemented.",
                    "PolynomialOpticsModel::CalculateTransferMap()"));
    default:
      throw(Squeal(Squeal::nonRecoverable,
                    "Unrecognized fitting algorithm in configuration.",
                    "PolynomialOpticsModel::CalculateTransferMap()"));
  }

  TransferMap * transfer_map = new PolynomialTransferMap(
    *polynomial_map, reference_particle_);
  delete polynomial_map;

  return transfer_map;
}
}  // namespace MAUS
