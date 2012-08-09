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

#include "src/common_cpp/Optics/CovarianceMatrix.hh"
#include "src/common_cpp/Optics/PhaseSpaceVector.hh"
#include "src/common_cpp/Optics/TransferMap.hh"
#include "Reconstruction/Global/Particle.hh"
#include "Reconstruction/Global/TrackPoint.hh"
#include "Simulation/MAUSGeant4Manager.hh"

#include "src/legacy/Interface/MiceMaterials.hh"
#include "src/legacy/Interface/MICERun.hh"
#include "src/legacy/Simulation/FillMaterials.hh"

namespace MAUS {

using reconstruction::global::TrackPoint;
using reconstruction::global::Particle;

// ##############################
//  TransferMapOpticsModel public
// ##############################

// ******************************
//  Constructors
// ******************************

TransferMapOpticsModel::TransferMapOpticsModel(
    const Json::Value & configuration) {
  // Prepare for simulating First plane particles through MICE
/*
if (maus_configuration_ == NULL) {
  std:cout << "DEBUG TransferMapOpticsModel(): MAUS configuration is NULL" << std::endl;
}
  simulation.jsonConfiguration = new Json::Value(configuration);
  Json::Value& config = *simulation.jsonConfiguration;
  config["verbose_level"] = Json::Value(0);

  config["maximum_number_of_steps"] = Json::Value(10000);
  config["geant4_visualisation"] = Json::Value(false);
  config["keep_steps"] = Json::Value(false);
  config["keep_tracks"] = Json::Value(false);
  config["physics_model"] = Json::Value("QGSP_BERT");
  config["reference_physics_processes"] = Json::Value("mean_energy_loss");
  config["physics_processes"] = Json::Value("standard");
  config["particle_decay"] = Json::Value(true);
  config["charged_pion_half_life"] = Json::Value(-1.);  // use geant4 default
  config["muon_half_life"] = Json::Value(-1.);  // use geant4 default
  // # set the threshold for delta ray production [mm]
  config["production_threshold"] = Json::Value(0.5);

  // Next function disables std::cout, std::clog,
  // std::cerr depending on VerboseLevel
  Squeak::setStandardOutputs();

  // Data Cards setup
  data_cards_ = simulation.DataCards;
if (data_cards_ == NULL) {
  std::cout << "DEBUG TransferMapOpticsModel(): MAUS Data Cards is NULL" << std::endl;
}
  simulation.DataCards = new dataCards("Simulation");

  // Materials
  mice_materials_ = simulation.miceMaterials;
if (data_cards_ == NULL) {
  std::cout << "DEBUG TransferMapOpticsModel(): MAUS Mice Materials is NULL" << std::endl;
}
  simulation.miceMaterials = new MiceMaterials();

  // MICE Model setup
  Json::Value module_name = JsonWrapper::GetProperty(config,
                                                 "simulation_geometry_filename",
                                                 JsonWrapper::stringValue);
  mice_module_ = simulation.miceModule;
if (data_cards_ == NULL) {
  std::cout << "DEBUG TransferMapOpticsModel(): MAUS Mice Module is NULL" << std::endl;
}
  simulation.miceModule = new MiceModule(module_name.asString());

  // G4 Materials
  fillMaterials(simulation);

  MAUSGeant4Manager * const simulator = MAUSGeant4Manager::GetInstance();

  // RF cavity phases
  simulator->SetPhases();
  Squeak::mout(Squeak::info) << "Fields:" << std::endl;
  simulation.btFieldConstructor->Print(Squeak::mout(Squeak::info));
*/
  // Reference Particle
  MAUSGeant4Manager * const simulator = MAUSGeant4Manager::GetInstance();
  reference_pgparticle_ = simulator->GetReferenceParticle();
  first_plane_ = reference_pgparticle_.z;

  reference_particle_ = TrackPoint(
    reference_pgparticle_.time, reference_pgparticle_.energy,
    reference_pgparticle_.x, reference_pgparticle_.px,
    reference_pgparticle_.y, reference_pgparticle_.py,
    reference_pgparticle_.pid, reference_pgparticle_.z);

  // Calculate time offset from t=0 at z=0
  const double first_plane_time = reference_pgparticle_.time;
  time_offset_ = first_plane_time - reference_particle_.time();

  // First plane particle coordinate deltas
  Json::Value delta_values = JsonWrapper::GetProperty(
      configuration,
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
  std::map<double, const TransferMap *>::iterator transfer_map;
  for (transfer_map = transfer_maps_.begin();
       transfer_map != transfer_maps_.end();
       ++transfer_map) {
    delete (*transfer_map).second;
  }
}

void TransferMapOpticsModel::Build() {
std::cout << "CHECKPOINT Build(): BEGIN" << std::endl;
std::cout.flush();
  // Create some test hits at the desired First plane
  const std::vector<TrackPoint> first_plane_hits = BuildFirstPlaneHits();

  // Iterate through each First plane hit
  MAUSGeant4Manager * simulator = MAUSGeant4Manager::GetInstance();
  std::map<int, std::vector<TrackPoint> > station_hits_map;
  std::vector<TrackPoint>::const_iterator first_plane_hit;
  for (first_plane_hit = first_plane_hits.begin();
       first_plane_hit < first_plane_hits.end();
       ++first_plane_hit) {
    // Simulate the current particle (First plane hit) through MICE.
    simulator->RunParticle(
      reconstruction::global::PrimaryGeneratorParticle(*first_plane_hit));

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
std::cout << "DEBUG Build(): station plane = " << station_plane << std::endl;

    // Generate a transfer map between the First plane and the current station
    // and map the station ID to the transfer map
    transfer_maps_[station_plane]
      = CalculateTransferMap(first_plane_hits, station_hits->second);
  }
std::cout << "CHECKPOINT Build(): 187" << std::endl;
std::cout.flush();

  // make sure we don't override mc_events
  Json::Value * configuration = MICERun::getInstance()->jsonConfiguration;
  configuration->removeMember("mc_events");
  /*
  MICERun & simulation = *MICERun::getInstance();
  delete simulation.DataCards;
  simulation.DataCards = const_cast<dataCards *>(data_cards_);
  delete simulation.miceMaterials;
  simulation.miceMaterials = const_cast<MiceMaterials *>(mice_materials_);
  delete simulation.miceModule;
  simulation.miceModule = const_cast<MiceModule *>(mice_module_);
  delete simulation.jsonConfiguration;
  simulation.jsonConfiguration = const_cast<Json::Value *>(maus_configuration_);
  fillMaterials(simulation);
  MAUSGeant4Manager::GetInstance()->SetPhases();
  */
std::cout << "CHECKPOINT Build(): END" << std::endl;
std::cout.flush();
}

const TransferMap * TransferMapOpticsModel::FindTransferMap(
    const double end_plane) const {
  // find the transfer map that transports a particle from the first plane
  // to the station that is nearest to the desired end_plane
  std::map<double, const TransferMap *>::const_iterator transfer_map_entry;
  size_t map_index = 0;
  bool found_entry = false;
  for (transfer_map_entry = transfer_maps_.begin();
       !found_entry && (transfer_map_entry != transfer_maps_.end());
       ++transfer_map_entry) {
    // determine whether the station before or after end_plane is closest
    double station_plane = transfer_map_entry->first;
    if (station_plane >= end_plane) {
      if (transfer_map_entry == transfer_maps_.begin()) {
        std::ostringstream message_buffer;
        message_buffer << "Mapping detectors are all positioned downstream "
                       << "from one or more hits (End Plane: " << end_plane
                       << ", First Station Plane: " << station_plane << ").";
        throw(Squeal(Squeal::nonRecoverable, message_buffer.str(),
                     "MAUS::TransferMapOpticsModel::GenerateTransferMap()"));
      }
      double after_delta = station_plane - end_plane;
      --transfer_map_entry;
      double before_delta = end_plane - transfer_map_entry->first;
      if (after_delta < before_delta) {
        ++transfer_map_entry;
      }

      found_entry = true;
    }
    if (!found_entry) {
      ++map_index;
    }
  }

  // end() is past the end of the vector, so bring the iterator back in
  if (transfer_map_entry == transfer_maps_.end()) {
    --transfer_map_entry;
  }
std::cout << "DEBUG GenerateTransferMap(): generating transfer map between "
          << first_plane_ << " and " << transfer_map_entry->first << std::endl;
  return transfer_map_entry->second;
}

const TransferMap * TransferMapOpticsModel::GenerateTransferMap(
    const double plane) const {
  if (transfer_maps_.size() == 0) {
    throw(Squeal(Squeal::nonRecoverable,
                 "No transfer maps to choose from.",
                 "MAUS::TransferMapOpticsModel::GenerateTransferMap()"));
  }

  return FindTransferMap(plane);
}

const std::vector<TrackPoint> TransferMapOpticsModel::BuildFirstPlaneHits() {
  std::vector<TrackPoint> first_plane_hits;
  first_plane_hits.push_back(reference_particle_);

  for (int coordinate_index = 0; coordinate_index < 6; ++coordinate_index) {
    // Make a copy of the reference trajectory vector
    TrackPoint first_plane_hit = reference_particle_;

    // Add to the current coordinate of the reference trajectory vector
    // the appropriate delta value and save the modified vector
    first_plane_hit[coordinate_index] += deltas_[coordinate_index];
    first_plane_hits.push_back(first_plane_hit);

    // Subtract from the current coordinate of the reference trajectory vector
    // the appropriate delta value and save the modified vector
    first_plane_hit[coordinate_index] -= 2. * deltas_[coordinate_index];
    if ((coordinate_index == 1) && (first_plane_hit[coordinate_index] < 0)) {
      first_plane_hit[coordinate_index] *= -1.;
    }
    first_plane_hits.push_back(first_plane_hit);
  }

  return first_plane_hits;
}

void TransferMapOpticsModel::MapStationsToHits(
    std::map<int, std::vector<TrackPoint> > & station_hits) {
  // Iterate through each event of the simulation
  MAUSGeant4Manager * const simulator = MAUSGeant4Manager::GetInstance();
  const Json::Value events = simulator->GetEventAction()->GetEvents();
  if (events.size() == 0) {
    throw(Squeal(Squeal::nonRecoverable,
                 "No events were generated during simulation.",
                 "MAUS::TransferMapOpticsModel::MapStationsToHits()"));
  }
  if (events.size() == 0) {
    throw(Squeal(Squeal::nonRecoverable,
                  "No virtual hits were generated during simulation.",
                  "MAUS::TransferMapOpticsModel::MapStationsToHits()"));
  }
  for (size_t event_index = 0; event_index < events.size(); ++event_index) {
    // Iterate through each hit recorded during the current event
    const Json::Value hits = events[event_index]["virtual_hits"];
    for (size_t hit_index = 0; hit_index < hits.size(); ++hit_index) {
      const Json::Value hit = hits[hit_index];
      const int particle_id = hit["particle_id"].asInt();
      const double mass = Particle::GetInstance()->GetMass(particle_id);
      const double px = hit["momentum"]["x"].asDouble();
      const double py = hit["momentum"]["y"].asDouble();
      const double pz = hit["momentum"]["z"].asDouble();
      const double momentum = ::sqrt(px*px + py*py + pz*pz);
      const double energy = ::sqrt(mass*mass + momentum*momentum);

      TrackPoint hit_vector(
        hit["time"].asDouble(), energy,
        hit["position"]["x"].asDouble(), px,
        hit["position"]["y"].asDouble(), py,
        particle_id,
        hit["position"]["z"].asDouble());
      const int station_id = hit["station_id"].asInt();
      station_hits[station_id].push_back(hit_vector);
    }
  }
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
