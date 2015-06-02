/* This file is part of MAUS: http://   micewww.pp.rl.ac.uk:8080/projects/maus
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
 * along with MAUS.  If not, see <http://   www.gnu.org/licenses/>.
 */

/* Author: Peter Lane
 */
#include "src/common_cpp/Optics/TransferMapOpticsModel.hh"

#include <sstream>
#include <ctime>

#include "TLorentzVector.h"

#include "DataStructure/Primary.hh"
#include "DataStructure/ThreeVector.hh"
#include "DataStructure/Global/ReconEnums.hh"
#include "JsonCppProcessors/PrimaryProcessor.hh"
#include "src/common_cpp/Optics/CovarianceMatrix.hh"
#include "src/common_cpp/Optics/PhaseSpaceVector.hh"
#include "src/common_cpp/Optics/TransferMap.hh"
#include "Recon/Global/DataStructureHelper.hh"
#include "Recon/Global/Particle.hh"
#include "Simulation/MAUSGeant4Manager.hh"
#include "Simulation/MAUSPhysicsList.hh"
#include "Simulation/VirtualPlanes.hh"

#include "src/legacy/Interface/MiceMaterials.hh"
#include "src/legacy/Interface/MICERun.hh"
#include "src/legacy/Simulation/FillMaterials.hh"

namespace MAUS {

using DataStructure::Global::TrackPoint;
using DataStructure::Global::PID;
using recon::global::Particle;
using recon::global::DataStructureHelper;

// ##############################
//  TransferMapOpticsModel public
// ##############################

// ******************************
//  Constructors
// ******************************

TransferMapOpticsModel::TransferMapOpticsModel(
      Json::Value const * const configuration)
    : OpticsModel(configuration), built_(false) {
  // Reference Particle
  MAUSGeant4Manager * const simulator = MAUSGeant4Manager::GetInstance();
  MAUS::MAUSPrimaryGeneratorAction::PGParticle reference_pgparticle
    = simulator->GetReferenceParticle();
  reference_primary_ = reference_pgparticle.GetPrimary();

  primary_plane_ = reference_pgparticle.z;

  ThreeVector position = reference_primary_.GetPosition();
  ThreeVector momentum = reference_primary_.GetMomentum();
  reference_trajectory_ = PhaseSpaceVector(reference_primary_.GetTime(),
                                           reference_primary_.GetEnergy(),
                                           position.x(), momentum.x(),
                                           position.y(), momentum.y());

  // First plane particle coordinate deltas
  Json::Value delta_values = JsonWrapper::GetProperty(
      *configuration_,
      "TransferMapOpticsModel_Deltas",
      JsonWrapper::objectValue);
  deltas_ = PhaseSpaceVector(
    delta_values["t"].asDouble(),
    delta_values["E"].asDouble(),
    delta_values["x"].asDouble(),
    delta_values["Px"].asDouble(),
    delta_values["y"].asDouble(),
    delta_values["Py"].asDouble());
}

TransferMapOpticsModel::~TransferMapOpticsModel() {
  std::map<int64_t, const TransferMap *>::iterator transfer_map;
  for (transfer_map = transfer_maps_.begin();
       transfer_map != transfer_maps_.end();
       ++transfer_map) {
    delete (*transfer_map).second;
  }
}

void TransferMapOpticsModel::Build() {
  // Create some test hits at the desired First plane
  const std::vector<Primary> primaries = Primaries();
  std::vector<PhaseSpaceVector> primary_vectors;
  Json::Value primaries_json;
  for (std::vector<Primary>::const_iterator primary = primaries.begin();
       primary < primaries.end();
       ++primary) {
    // serialize primary to JSON
    PrimaryProcessor serializer;
    Json::Value * primary_json = serializer.CppToJson(*primary, "");
    Json::Value object_value;
    object_value["primary"] = *primary_json;
    primaries_json.append(object_value);

    // generate a phase space vector for the primary
    ThreeVector position = primary->GetPosition();
    ThreeVector momentum = primary->GetMomentum();
    PhaseSpaceVector primary_vector(primary->GetTime(), primary->GetEnergy(),
                                    position.x(), momentum.x(),
                                    position.y(), momentum.y());
    primary_vectors.push_back(primary_vector);
  }

  MAUSGeant4Manager * simulator = MAUSGeant4Manager::GetInstance();

  // Force setting of stochastics
  simulator->GetPhysicsList()->BeginOfRunAction();

  // Simulate on the primaries, generating virtual detector tracks for each
  Json::Value virtual_tracks;
  try {
    Squeak::mout(Squeak::debug) << "DEBUG TransferMapOpticsModel::Build: "
          << "Executing RunManyParticles with the following VirtualPlanes:"
          << std::endl;
    MAUS::VirtualPlaneManager * const virtual_plane_mgr
                        = MAUSGeant4Manager::GetInstance()->GetVirtualPlanes();
    for (size_t i = 0; i < virtual_plane_mgr->GetPlanes().size(); ++i) {
        Squeak::mout(Squeak::debug)
          << virtual_plane_mgr->GetPlanes()[i]->GetPosition() << std::endl;
    }
    virtual_tracks
      = MAUSGeant4Manager::GetInstance()->RunManyParticles(primaries_json);
  } catch (std::exception ex) {
    std::stringstream message;
    message << "Simulation failed." << std::endl << ex.what() << std::endl;
    throw(Exception(Exception::nonRecoverable,
                 message.str(),
                 "MAUS::TransferMapOpticsModel::Build()"));
  }
  if (virtual_tracks.size() == 0) {
    throw(Exception(Exception::nonRecoverable,
                 "No events were generated during simulation.",
                 "MAUS::TransferMapOpticsModel::Build()"));
  }

  // Map stations to hits in each virtual track
  std::map<int64_t, std::vector<PhaseSpaceVector> > station_hits_map;
  Squeak::mout(Squeak::debug) << "DEBUG TransferMapOpticsModel::Build: "
          << "RunManyParticles yielded the following tracks:" << std::endl;
  for (Json::Value::iterator virtual_track = virtual_tracks.begin();
       virtual_track != virtual_tracks.end();
       ++virtual_track) {
    const Json::Value hits = (*virtual_track)["virtual_hits"];
    for (int hit_index = 0; hit_index < int(hits.size()); ++hit_index) {
      Squeak::mout(Squeak::debug) << setprecision(12)
            << "(" << hits[hit_index]["position"]["x"].asDouble() << ", "
            << hits[hit_index]["position"]["y"].asDouble() << ", "
            << hits[hit_index]["position"]["z"].asDouble() << ") ";
    }
    Squeak::mout(Squeak::debug) << std::endl;
    MapStationsToHits(station_hits_map, *virtual_track);
    Squeak::mout(Squeak::debug) << "DEBUG TransferMapOpticsModel::Build: "
          << "station_hits_map size: " << station_hits_map.size() << std::endl;
  }

  // Calculate transfer maps from the primary plane to each station plane
  std::map<int64_t, std::vector<PhaseSpaceVector> >::iterator station_hits;
  for (station_hits = station_hits_map.begin();
       station_hits != station_hits_map.end();
       ++station_hits) {
    // calculate transfer map and index it by the station z-plane
    Squeak::mout(Squeak::debug) << "DEBUG TransferMapOpticsModel::Build: "
          << "Calculating transfer map for plane at z = " << station_hits->first
          << std::endl;
    transfer_maps_[station_hits->first]
      = CalculateTransferMap(primary_vectors, station_hits->second);
  }

  built_ = true;
}

const TransferMap * TransferMapOpticsModel::FindTransferMap(
    const double end_plane) const {
  // find the transfer map that transports a particle from the first plane
  // to the station that is nearest to the desired end_plane
  std::map<int64_t, const TransferMap *>::const_iterator transfer_map_entry;
  bool found_entry = false;
  for (transfer_map_entry = transfer_maps_.begin();
       !found_entry && (transfer_map_entry != transfer_maps_.end());
       ++transfer_map_entry) {
    // determine whether the station before or after end_plane is closest
    double station_plane = static_cast<double>(transfer_map_entry->first);
    if (station_plane >= end_plane) {
      if ((transfer_map_entry == transfer_maps_.begin()) &&
          (station_plane > end_plane)) {
        std::ostringstream message_buffer;
        message_buffer << "Mapping detectors are all positioned downstream "
                       << "from one or more hits (Hit z: " << end_plane
                       << ", First Station Plane: " << station_plane << ").";
        throw(Exception(Exception::nonRecoverable, message_buffer.str(),
                     "MAUS::TransferMapOpticsModel::GenerateTransferMap()"));
      }
      double after_delta = station_plane - end_plane;
      if (transfer_map_entry != transfer_maps_.begin()) {
        --transfer_map_entry;
        double before_delta
          = end_plane - static_cast<double>(transfer_map_entry->first);
        if (after_delta < before_delta) {
          ++transfer_map_entry;
        }
      }

      // check for poorly configured mapping virtual detectors
      const double selection_error
        = ::abs(end_plane - static_cast<double>(transfer_map_entry->first));
      const double max_selection_error = 10.;  // 1 cm
      if (selection_error > max_selection_error) {
        std::ostringstream message_buffer;
        message_buffer << "Difference in z-position between closest mapping "
                       << setprecision(10)
                       << "detector (z=" << transfer_map_entry->first << ") "
                       << " and target detector " << "(z=" << end_plane << ") "
                       << "exceeds tollerance for accurate mapping.";
        throw(Exception(Exception::nonRecoverable, message_buffer.str(),
                     "MAUS::TransferMapOpticsModel::GenerateTransferMap()"));
      }

      found_entry = true;
    }
  }

  if (transfer_map_entry != transfer_maps_.begin()) {
    --transfer_map_entry;  // compensate for the final increment in the loop
  }

  return transfer_map_entry->second;
}

const TransferMap * TransferMapOpticsModel::GenerateTransferMap(
    const double plane) const {
  if (!built_) {
    throw(Exception(Exception::nonRecoverable,
                  "No transfer maps available since the optics model has not "
                  "been built yet. Call Build() first.",
                  "MAUS::TransferMapOpticsModel::GenerateTransferMap()"));
  } else if (transfer_maps_.size() == 0) {
    throw(Exception(Exception::nonRecoverable,
                 "No transfer maps to choose from.",
                 "MAUS::TransferMapOpticsModel::GenerateTransferMap()"));
  }

  return FindTransferMap(plane);
}

const std::vector<Primary> TransferMapOpticsModel::Primaries() {
  std::vector<Primary> primaries;
  primaries.push_back(reference_primary_);

  for (int coordinate_index = 0; coordinate_index < 6; ++coordinate_index) {
    Primary primary1 = reference_primary_;
    Primary primary2 = reference_primary_;

    switch (coordinate_index) {
      case 0: {
        primary1.SetTime(primary1.GetTime() + deltas_[coordinate_index]);
        primary2.SetTime(primary2.GetTime() - deltas_[coordinate_index]);
        break;
      }
      case 1: {
        primary1.SetEnergy(primary1.GetEnergy() + deltas_[coordinate_index]);
        primary2.SetEnergy(primary2.GetEnergy() - deltas_[coordinate_index]);
        break;
      }
      case 2:
      case 3: {
        ThreeVector position = primary1.GetPosition();
        position[coordinate_index-2] += deltas_[coordinate_index];
        primary1.SetPosition(position);

        position = primary2.GetPosition();
        position[coordinate_index-2] -= deltas_[coordinate_index];
        primary2.SetPosition(position);
        break;
      }
      case 4:
      case 5: {
        ThreeVector momentum = primary1.GetMomentum();
        momentum[coordinate_index-4] += deltas_[coordinate_index];
        primary1.SetMomentum(momentum);

        momentum = primary2.GetMomentum();
        momentum[coordinate_index-4] -= deltas_[coordinate_index];
        primary2.SetMomentum(momentum);
        break;
      }
    }

    primaries.push_back(primary1);
    primaries.push_back(primary2);
  }

  return primaries;
}

void TransferMapOpticsModel::MapStationsToHits(
    std::map<int64_t, std::vector<PhaseSpaceVector> > & station_hits,
    const Json::Value & event) {
  // Iterate through each event of the simulation
  const Json::Value hits = event["virtual_hits"];
std::cout << "DEBUG TransferMapOpticsModel::MapStationsToHits: "
          << "# virtual track hits: " << hits.size() << std::endl;
  for (size_t hit_index = 0; hit_index < hits.size(); ++hit_index) {
    const Json::Value hit = hits[Json::Value::ArrayIndex(hit_index)];
    const PID particle_id = PID(hit["particle_id"].asInt());
    const double mass = Particle::GetInstance().GetMass(particle_id);
    const double px = hit["momentum"]["x"].asDouble();
    const double py = hit["momentum"]["y"].asDouble();
    const double pz = hit["momentum"]["z"].asDouble();
    const double momentum = ::sqrt(px*px + py*py + pz*pz);
    const double energy = ::sqrt(mass*mass + momentum*momentum);

    PhaseSpaceVector hit_vector(
      hit["time"].asDouble(),           energy,
      hit["position"]["x"].asDouble(),  px,
      hit["position"]["y"].asDouble(),  py);
    double z = hit["position"]["z"].asDouble();
    // Round to the nearest mm
    int64_t z_key = (z >= 0?static_cast<int64_t>(z+.5):static_cast<int64_t>(z-.5));
    Squeak::mout(Squeak::debug) 
          << "DEBUG TransferMapOpticsModel::MapStationsToHits: "
          << setprecision(8) << "z = " << z << "\tz_key = " << z_key
          << std::endl;
    station_hits[z_key].push_back(hit_vector);

    // Assuming the first hit is at the start plane, save the time offset
    // between what the reference primary specifies and what the simulation's
    // virtual detectors produce
    if (hit_index == 0) {
      const double start_plane_time = hit["time"].asDouble();
      time_offset_ = start_plane_time - reference_primary_.GetTime();
    }
  }
}

CovarianceMatrix TransferMapOpticsModel::Transport(
    const CovarianceMatrix & covariances,
    const double end_plane) const {
  TransferMap const * transfer_map = GenerateTransferMap(end_plane);
  if (transfer_map == NULL) {
    throw(Exception(Exception::nonRecoverable,
                  "Got NULL transfer map.",
                  "MAUS::TransferMapOpticsModel::Transport()"));
  }
  CovarianceMatrix transported_covariances
    = transfer_map->Transport(covariances);

  return transported_covariances;
}

CovarianceMatrix TransferMapOpticsModel::Transport(
    const CovarianceMatrix & covariances,
    const double start_plane,
    const double end_plane) const {
  const TransferMap * start_plane_map = FindTransferMap(start_plane);
  const TransferMap * end_plane_map = FindTransferMap(end_plane);
  const TransferMap * inverted_start_plane_map = start_plane_map->Inverse();
  CovarianceMatrix transported_covariances
    = inverted_start_plane_map->Transport(
      end_plane_map->Transport(covariances));
  delete inverted_start_plane_map;

  return transported_covariances;
}

PhaseSpaceVector TransferMapOpticsModel::Transport(
    const PhaseSpaceVector & vector,
    const double end_plane) const {
  TransferMap const * transfer_map = GenerateTransferMap(end_plane);
  if (transfer_map == NULL) {
    throw(Exception(Exception::nonRecoverable,
                  "Got NULL transfer map.",
                  "MAUS::TransferMapOpticsModel::Transport()"));
  }

  PhaseSpaceVector transported_vector = transfer_map->Transport(vector);

  return transported_vector;
}

PhaseSpaceVector TransferMapOpticsModel::Transport(
    const PhaseSpaceVector & vector,
    const double start_plane,
    const double end_plane) const {
  const TransferMap * start_plane_map = FindTransferMap(start_plane);
  const TransferMap * end_plane_map = FindTransferMap(end_plane);
  const TransferMap * inverted_start_plane_map = start_plane_map->Inverse();
  PhaseSpaceVector transported_vector = inverted_start_plane_map->Transport(
    end_plane_map->Transport(vector));
  delete inverted_start_plane_map;

  return transported_vector;
}
}  // namespace MAUS
