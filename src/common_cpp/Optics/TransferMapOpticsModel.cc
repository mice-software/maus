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

// ##############################
//  TransferMapOpticsModel public
// ##############################

// ******************************
//  Constructors
// ******************************

TransferMapOpticsModel::TransferMapOpticsModel(
    const Json::Value & configuration)
    : reference_particle_(
        TrackPoint(PhaseSpaceVector::PhaseSpaceType::kPositional)) {
  // Prepare for simulating start plane particles through MICE
  MICERun & simulation = *MICERun::getInstance();
  //simulation.jsonConfiguration = new Json::Value(Json::objectValue);
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
  simulation.DataCards = new dataCards("Simulation");

  // Materials
  simulation.miceMaterials = new MiceMaterials();

  // MICE Model setup
  Json::Value module_name = JsonWrapper::GetProperty(config,
                                                 "simulation_geometry_filename",
                                                 JsonWrapper::stringValue);
  simulation.miceModule = new MiceModule(module_name.asString());

  // G4 Materials
  fillMaterials(simulation);

  MAUSGeant4Manager * const simulator = MAUSGeant4Manager::GetInstance();

  // RF cavity phases
  simulator->SetPhases();
  Squeak::mout(Squeak::info) << "Fields:" << std::endl;
  simulation.btFieldConstructor->Print(Squeak::mout(Squeak::info));

  // Reference Particle
  reference_pgparticle_ = simulator->GetReferenceParticle();
  reference_particle_ = TrackPoint(
    reference_pgparticle_.x, reference_pgparticle_.px,
    reference_pgparticle_.y, reference_pgparticle_.py,
    reference_pgparticle_.z, reference_pgparticle_.pz,
    reference_pgparticle_.pid,
    PhaseSpaceVector::PhaseSpaceType::kPositional);

  // Calculate time offset from t=0 at z=0
  const double start_plane_time = reference_pgparticle_.time;
  time_offset_ = start_plane_time - reference_particle_.time();

  // Start plane particle coordinate deltas
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
  // Create some test hits at the desired start plane
  const std::vector<TrackPoint> start_plane_hits = BuildStartPlaneHits();

  // Iterate through each start plane hit
  MAUSGeant4Manager * const simulator = MAUSGeant4Manager::GetInstance();
  std::map<int, std::vector<TrackPoint> > station_hits_map;
  std::vector<TrackPoint>::const_iterator start_plane_hit;
  for (start_plane_hit = start_plane_hits.begin();
       start_plane_hit < start_plane_hits.end();
       ++start_plane_hit) {
    // Simulate the current particle (start plane hit) through MICE.
    simulator->RunParticle(
      reconstruction::global::PrimaryGeneratorParticle(*start_plane_hit));

    // Identify the hits by station and add them to the mappings from stations
    // to the hits they recorded.
    MapStationsToHits(station_hits_map);
  }

std::cout << "CHECKPOINT Build() 1" << std::endl; std::cout.flush();
  // Iterate through each station
std::cout << "DEBUG Build(): # Stations = " << station_hits_map.size() << std::endl; std::cout.flush();
  std::map<int, std::vector<TrackPoint> >::iterator station_hits;
  for (station_hits = station_hits_map.begin();
       station_hits != station_hits_map.end();
       ++ station_hits) {
std::cout << "DEBUG Build(): # Station Hits = " << station_hits->second.size() << std::endl; std::cout.flush();
    // find the average z coordinate for the station
    std::vector<TrackPoint>::iterator station_hit;

    double station_plane = station_hits->second.begin()->z();
std::cout << "DEBUG Build(): # Station Hit Z = " << station_plane << std::endl; std::cout.flush();

    // Generate a transfer map between the start plane and the current station
    // and map the station ID to the transfer map
std::cout << "CHECKPOINT Build() 1.1" << std::endl; std::cout.flush();
    transfer_maps_[station_plane]
      = CalculateTransferMap(start_plane_hits, station_hits->second);
std::cout << "CHECKPOINT Build() 1.2" << std::endl; std::cout.flush();
std::cout << "DEBUG Build(): # Transfer Maps = " << transfer_maps_.size() << std::endl; std::cout.flush();
  }
}

const TransferMap * TransferMapOpticsModel::GenerateTransferMap(
    const double end_plane) const {
std::cout << "DEBUG GenerateTransferMap(): # Transfer Maps = " << transfer_maps_.size() << std::endl;
  if (transfer_maps_.size() == 0) {
    throw(Squeal(Squeal::nonRecoverable,
                 "No transfer maps to choose from.",
                 "MAUS::TransferMapOpticsModel::GenerateTransferMap()"));
  }
  // find the transfer map that transports a particle from the start plane
  // to the station that is nearest to the desired end_plane
std::cout << "DEBUG GenerateTransferMap(): End Plane = " << end_plane << std::endl;
  std::map<double, const TransferMap *>::const_iterator transfer_map_entry;
  size_t map_index = 0;
  for (transfer_map_entry = transfer_maps_.begin();
       transfer_map_entry != transfer_maps_.end();
       ++transfer_map_entry) {
    // determine whether the station before or after end_plane is closest
    double station_plane = transfer_map_entry->first;
std::cout << "DEBUG GenerateTransferMap(): Station Plane = " << station_plane << std::endl;
    if (station_plane >= end_plane) {
std::cout << "DEBUG GenerateTransferMap(): Station # = " << map_index << std::endl;
      if (transfer_map_entry == transfer_maps_.begin()) {
        throw(Squeal(Squeal::nonRecoverable,
                     "Mapping detectors are all positioned downstream from one "
                     "or more hits.",
                     "MAUS::TransferMapOpticsModel::GenerateTransferMap()"));
      }
      double before_delta = end_plane - transfer_map_entry->first;
      double after_delta = 0.0;
      ++transfer_map_entry;
      if (transfer_map_entry != transfer_maps_.end()) {
        after_delta = end_plane - transfer_map_entry->first;
        if (before_delta < after_delta) {
          --transfer_map_entry;
        }
      }

      break;  // found a close station so beak off the search
    }
    ++map_index;
  }

  if (transfer_map_entry == transfer_maps_.end()) {
    --transfer_map_entry;
  }

  return transfer_map_entry->second;
}

const std::vector<TrackPoint> TransferMapOpticsModel::BuildStartPlaneHits() {
  std::vector<TrackPoint> start_plane_hits;
  start_plane_hits.push_back(reference_particle_);

  for(int coordinate_index = 0; coordinate_index < 6; ++coordinate_index) {
    // Make a copy of the reference trajectory vector
    TrackPoint start_plane_hit = reference_particle_;

    // Add to the current coordinate of the reference trajectory vector
    // the appropriate delta value and save the modified vector
    start_plane_hit[coordinate_index] += deltas_[coordinate_index];
    start_plane_hits.push_back(start_plane_hit);

    // Subtract from the current coordinate of the reference trajectory vector
    // the appropriate delta value and save the modified vector
    start_plane_hit[coordinate_index] -= 2. * deltas_[coordinate_index];
    if ((coordinate_index == 1) && (start_plane_hit[coordinate_index] < 0)) {
      start_plane_hit[coordinate_index] *= -1.;
    }
    start_plane_hits.push_back(start_plane_hit);
  }

  return start_plane_hits;
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
std::cout << "DEBUG MapStationsToHits(): # Events = " << events.size() << std::endl;
  if (events.size() == 0) {
    throw(Squeal(Squeal::nonRecoverable,
                  "No virtual hits were generated during simulation.",
                  "MAUS::TransferMapOpticsModel::MapStationsToHits()"));
  }
  for (size_t event_index = 0; event_index < events.size(); ++event_index) {
    // Iterate through each hit recorded during the current event
    const Json::Value hits = events[event_index]["virtual_hits"];
std::cout << "DEBUG MapStationsToHits(): # Virtual Hits = " << hits.size() << std::endl;
    for (size_t hit_index = 0; hit_index < hits.size(); ++hit_index) {
      const Json::Value hit = hits[hit_index];
      TrackPoint hit_vector(
        hit["position"]["x"].asDouble(), hit["momentum"]["x"].asDouble(),
        hit["position"]["y"].asDouble(), hit["momentum"]["y"].asDouble(),
        hit["position"]["z"].asDouble(), hit["momentum"]["z"].asDouble(),
        hit["particle_id"].asInt(),
        PhaseSpaceVector::PhaseSpaceType::kPositional);
      const int station_id = hit["station_id"].asInt();
      station_hits[station_id].push_back(hit_vector);
    }
  }
}

}  // namespace MAUS
