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
#include "src/common_cpp/DataStructure/Primary.hh"
#include "src/common_cpp/DataStructure/ThreeVector.hh"
#include "src/common_cpp/JsonCppProcessors/PrimaryProcessor.hh"
#include "src/common_cpp/Optics/PhaseSpaceVector.hh"
#include "src/common_cpp/Optics/PolynomialTransferMap.hh"
#include "Recon/Global/DataStructureHelper.hh"
#include "Recon/Global/ParticleOpticalVector.hh"
#include "Simulation/MAUSGeant4Manager.hh"
#include "Simulation/MAUSPhysicsList.hh"

namespace MAUS {

using MAUS::PhaseSpaceVector;
using recon::global::ParticleOpticalVector;

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
  const std::vector<PhaseSpaceVector> primary_vectors = PrimaryVectors();
std::cout << "DEBUG PolynomialOpticsModel::Build: "
          << "# primaries: " << primary_vectors.size() << std::endl;
  Json::Value primaries_json;
  for (std::vector<PhaseSpaceVector>::const_iterator primary_vector
        = primary_vectors.begin();
       primary_vector < primary_vectors.end();
       ++primary_vector) {
    // generate a Primary object
    Primary primary(reference_primary_);
    ThreeVector position(primary_vector->x(), primary_vector->y(),
                         reference_primary_.GetPosition().z());
    ThreeVector momentum(primary_vector->Px(), primary_vector->Py(),
                         reference_primary_.GetMomentum().z());
    primary.SetPosition(position);
    primary.SetMomentum(momentum);
    primary.SetTime(primary_vector->t());
    primary.SetEnergy(primary_vector->E());

    // serialize primary to JSON
    PrimaryProcessor serializer;
    Json::Value * primary_json = serializer.CppToJson(primary, "");
    Json::Value object_value;
    object_value["primary"] = *primary_json;
    primaries_json.append(object_value);
  }

  MAUSGeant4Manager * simulator = MAUSGeant4Manager::GetInstance();

  // Force setting of stochastics
  simulator->GetPhysicsList()->BeginOfRunAction();

  // Simulate on the primaries, generating virtual detector tracks for each
  const Json::Value virtual_tracks
      = MAUSGeant4Manager::GetInstance()->RunManyParticles(primaries_json);
std::cout << "DEBUG PolynomialOpticsModel::Build: "
          << "# virtual tracks: " << virtual_tracks.size() << std::endl;
  if (virtual_tracks.size() == 0) {
    throw(Squeal(Squeal::nonRecoverable,
                 "No events were generated during simulation.",
                 "MAUS::TransferMapOpticsModel::Build()"));
  }

  // Map stations to hits in each virtual track
  std::map<double, std::vector<PhaseSpaceVector> > station_hits_map;
size_t count = 0;
  for (Json::Value::const_iterator virtual_track = virtual_tracks.begin();
       virtual_track != virtual_tracks.end();
       ++virtual_track) {
    MapStationsToHits(station_hits_map, *virtual_track);
    ++count;
  }
std::cout << "DEBUG PolynomialOpticsModel::Build:" << std::endl
          << "\t# virtual tracks mapped: " << count << std::endl
          << "\t# station Zs: " << station_hits_map.size() << std::endl;

  // Calculate transfer maps from the primary plane to each station plane
  std::map<double, std::vector<PhaseSpaceVector> >::iterator station_hits;
  for (station_hits = station_hits_map.begin();
       station_hits != station_hits_map.end();
       ++station_hits) {
    // calculate transfer map and index it by the station z-plane
std::cout << "DEBUG PolynomialOpticsModel::Build: "
          << "# virtual track hits for z = " << station_hits->first
          << ": " << station_hits->second.size() << std::endl;
    transfer_maps_[station_hits->first]
      = CalculateTransferMap(primary_vectors, station_hits->second);
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
const std::vector<PhaseSpaceVector> PolynomialOpticsModel::PrimaryVectors() {
  size_t num_poly_coefficients
    = PolynomialMap::NumberOfPolynomialCoefficients(6, polynomial_order_);
  ThreeVector position = reference_primary_.GetPosition();
  ThreeVector momentum = reference_primary_.GetMomentum();
  PhaseSpaceVector reference_vector(reference_primary_.GetTime(),
                                    reference_primary_.GetEnergy(),
                                    position.x(), momentum.x(),
                                    position.y(), momentum.y());

  std::vector<PhaseSpaceVector> primaries;
  for (size_t i = 0; i < 6; ++i) {
    for (size_t j = i; j < 6; ++j) {
      PhaseSpaceVector primary;

      for (size_t k = 0; k < i; ++k) {
        primary[k] = 1.;
      }
      double delta = deltas_[j];
      primary[j] = delta;

      primaries.push_back(primary + reference_vector);
    }
  }
  size_t base_block_length = primaries.size();

  // adjust for the case where the base block size is greater than N(n, v)
  while (base_block_length > num_poly_coefficients) {
    primaries.pop_back();
    base_block_length = primaries.size();
  }

  int summand;
  for (size_t row = base_block_length; row < num_poly_coefficients; ++row) {
    summand = row / base_block_length;
    PhaseSpaceVector primary
      = primaries[row % base_block_length] + summand;

    primaries.push_back(primary + reference_vector);
  }
  return primaries;
}

/* Calculate a transfer matrix from an equal number of inputs and output.
 */
const TransferMap * PolynomialOpticsModel::CalculateTransferMap(
    const std::vector<PhaseSpaceVector> & start_plane_hits,
    const std::vector<PhaseSpaceVector> & station_hits)
    const {
  MAUSGeant4Manager * const simulator = MAUSGeant4Manager::GetInstance();
  MAUSPrimaryGeneratorAction::PGParticle reference_pgparticle
    = simulator->GetReferenceParticle();
  const double t0 = reference_pgparticle.time;
  const double E0 = reference_pgparticle.energy;
  const double P0 = reference_pgparticle.pz;

  if (start_plane_hits.size() != station_hits.size()) {
    std::stringstream message;
    message << "The number of start plane hits (" << start_plane_hits.size()
            << ") is not the same as the number of hits per station ("
            << station_hits.size() << ").";
    throw(Squeal(Squeal::nonRecoverable,
                  message.str(),
                  "PolynomialOpticsModel::CalculateTransferMap()"));
  }

  std::vector< std::vector<double> > points;
  for (size_t pt_index = 0; pt_index < start_plane_hits.size(); ++pt_index) {
    ParticleOpticalVector start_plane_point(start_plane_hits[pt_index],
                                            t0, E0, P0);
    std::vector<double> point;
    for (size_t coord_index = 0; coord_index < 6; ++coord_index) {
      #if 0
        point.push_back(start_plane_point[coord_index]);
      #else
        point.push_back(start_plane_hits[pt_index][coord_index]);
      #endif
    }
    points.push_back(point);
  }

  std::vector< std::vector<double> > values;
  for (size_t val_index = 0; val_index < station_hits.size(); ++val_index) {
    ParticleOpticalVector station_value(station_hits[val_index], t0, E0, P0);
    std::vector<double> value;
    for (size_t coord_index = 0; coord_index < 6; ++coord_index) {
      #if 0
        value.push_back(station_value[coord_index]);
      #else
        value.push_back(station_hits[val_index][coord_index]);
      #endif
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

  ThreeVector reference_position = reference_primary_.GetPosition();
  ThreeVector reference_momentum = reference_primary_.GetMomentum();
  PhaseSpaceVector reference_particle(reference_primary_.GetTime(),
                                      reference_primary_.GetEnergy(),
                                      reference_position.x(),
                                      reference_momentum.x(),
                                      reference_position.y(),
                                      reference_momentum.y());
  TransferMap * transfer_map = new PolynomialTransferMap(
    *polynomial_map, reference_particle);
  delete polynomial_map;

  return transfer_map;
}
}  // namespace MAUS
