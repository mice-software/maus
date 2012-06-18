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

namespace MAUS {

using reconstruction::global::PrimaryGeneratorParticle;
// ##############################
//  TransferMapOpticsModel public
// ##############################

// ******************************
//  Constructors
// ******************************

TransferMapOpticsModel::TransferMapOpticsModel(
    const Json::Value & configuration) {
  // Prepare for simulating start plane particles through MICE
  MICERun& simulation = *MICERun::getInstance();
  simulation.jsonConfiguration = new Json::Value(Json::objectValue);
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

  // Materials
  simulation.miceMaterials = new MiceMaterials();

  // MICE Model setup
  Json::Value modname = JsonWrapper::GetProperty
             (config, "simulation_geometry_filename", JsonWrapper::stringValue);
  simulation.miceModule = new MiceModule(modname.asString());

  // G4 Materials
  fillMaterials(simulation);

  MAUSGeant4Manager * const simulator = MAUSGeant4Manager.GetInstance();

  // RF cavity phases
  simulator->SetPhases();
  Squeak::mout(Squeak::info) << "Fields:" << std::endl;
  simulation.btFieldConstructor->Print(Squeak::mout(Squeak::info));

  MAUSPrimaryGeneratorAction::PGParticle reference_pgparticle
    = simulator->GetReferenceParticle();
}

void TransferMapOpticsModel::Build() {
  // Create some test hits at the desired start plane
  const std::vector<PhaseSpaceVector> start_plane_hits = BuildStartPlaneHits();

  // Iterate through each start plane hit
  MAUSGeant4Manager * const simulator = MAUSGeant4Manager.GetInstance();
  std::map<int, std::vector<PhaseSpaceVector> > station_hits;
  std::vector<PhaseSpaceVector>::iterator start_plane_hit;
  for (start_plane_hit = start_plane_hits.begin();
       start_plane_hit < start_plane_hits.end();
       ++start_plane_hit) {
    // Simulate the current particle (start plane hit) through MICE.
    simulator->RunParticle(
      reconstruction::global::PrimaryGeneratorParticle(&(*start_plane_hit)));

    // Identify the hits by station and add them to the mappings from stations
    // to the hits they recorded.
    MapStationsToHits(station_hits);
  }

  // Iterate through each station
  std::vector<PhaseSpaceVector>::iterator station_hits;
  for (station_hits = station_hits.begin();
       station_hits < station_hits.end();
       ++ station_hits) {
    // Generate a transfer map between the start plane and the current station
    transfer_maps_.push_back(CalculateTransferMap(start_plane_hits,
                                                  &(*station_hits)));
  }
}

const std::vector<PhaseSpaceVector> BuildStartPlaneHits() {
  std::vector<PhaseSpaceVector> start_plane_hits;

/*
simulation_reference_particle = { # used for setting particle phase
    "position":{"x":0.0, "y":0.0, "z":-500.0},
    "momentum":{"x":0.0, "y":0.0, "z":1.0},
    "particle_id":-13, "energy":226.0, "time":0.0, "random_seed":10
}
*/
  const Json::Value & configuration
    = *MICERun::getInstance()->jsonConfiguration;
  // Extract reference particle information
  Json::Value reference_particle = JsonWrapper::GetProperty(
      configuration,
      "simulation_reference_particle",
      JsonWrapper::objectValue);
  Json::Value position = JsonWrapper::GetProperty(
      reference_particle,
      "position",
      JsonWrapper::doubleValue);
  Json::Value momentum = JsonWrapper::GetProperty(
      reference_particle,
      "momentum",
      JsonWrapper::objectValue);
  Json::Value time = JsonWrapper::GetProperty(
      reference_particle,
      "time",
      JsonWrapper::realValue);
  Json::Value energy = JsonWrapper::GetProperty(
      reference_particle,
      "energy",
      JsonWrapper::realValue);

  // Extract deltas
  Json::Value delta_values = JsonWrapper::GetProperty(
      configuration,
      "TransferMapOpticsModel_Deltas",
      JsonWrapper::objectValue);
  Json::Value delta_t = JsonWrapper::GetProperty(
      reference_particle,
      "t",
      JsonWrapper::realValue);
  Json::Value delta_E = JsonWrapper::GetProperty(
      reference_particle,
      "E",
      JsonWrapper::realValue);
  Json::Value delta_x = JsonWrapper::GetProperty(
      reference_particle,
      "x",
      JsonWrapper::realValue);
  Json::Value delta_Px = JsonWrapper::GetProperty(
      reference_particle,
      "Px",
      JsonWrapper::realValue);
  Json::Value delta_y = JsonWrapper::GetProperty(
      reference_particle,
      "x",
      JsonWrapper::realValue);
  Json::Value delta_Py = JsonWrapper::GetProperty(
      reference_particle,
      "Px",
      JsonWrapper::realValue);

  // TODO(plane1@hawk.iit.edu) Get the following from the configuration
  const PhaseSpaceVector reference_trajectory_in(
    PhaseSpaceVector::PhaseSpaceType::kPositional);
  const std::vector<double> deltas(1.0, 1.0, 1.0, 1.0, 1.0, 1.0);

  start_plane_hits.push_back(reference_trajectory);

  for(int coordinate_index = 0; coordinate_index < 6; ++coordinate_index)
  {
    // Make a copy of the reference trajectory vector
    PhaseSpaceVector start_plane_hit = reference_trajectory;

    // Add to the current coordinate of the reference trajectory vector
    // the appropriate delta value and save the modified vector
    start_plane_hit[coordinate_index] += delta[coordinate_index];
    start_plane_hits.push_back(hit);

    // Subtract from the current coordinate of the reference trajectory vector
    // the appropriate delta value and save the modified vector
    start_plane_hit[coordinate_index] -= 2. * deltas[coordinate_index];
    if ((coordinate_index == 1) && (start_plane_hit[coordinate_index] < 0)) {
      start_plane_hit[coordinate_index] *= -1.;
    }
    start_plane_hits.push_back(hit);
  }

  return start_plane_hits;
}

void TransferMapOpticsModel::MapStationsToHits(
    std::map<int, std::vector<PhaseSpaceVector> > & station_hits) {
  // Iterate through each event of the simulation
  const Json::Value events = simulator->GetEventAction()->GetEvents();
  for (size_t event_index = 0; event_index < events.size(); ++event_index) {
    // Iterate through each hit recorded during the current event
    const Json::Value hits = events[event_index]["virtual_hits"];
    for (size_t hit_index = 0; hit_index < hits.size(); ++hit_index) {
      const Json::Value hit = hits[hit_index];
      PhaseSpaceVector hit_vector;
      hit_vector.set_t(hit["time"].asDouble());
      hit_vector.set_x(hit["position"]["x"].asDouble());    
      hit_vector.set_Px(hit["momentum"]["x"].asDouble());    
      hit_vector.set_y(hit["position"]["y"].asDouble());    
      hit_vector.set_Py(hit["momentum"]["y"].asDouble());    
      hit_vector.set_z(hit["position"]["z"].asDouble());    
      hit_vector.set_Pz(hit["momentum"]["z"].asDouble());    
      double mass = hit["mass"].asDouble();
      hit_vector.set_E(::sqrt(hit_vector.Px()*hit_vector.Px() +
                              hit_vector.Py()*hit_vector.Py() +
                              hit_vector.Pz()*hit_vector.Pz() +
                              mass*mass));
      int id(hit["station_id"].asInt(), StationId::virt);
      station_hits[id].push_back(hit_vector);
    }
  }
}

const std::map<int, std::vector<PhaseSpaceVector> > &
TransferMapOpticsModel::station_hit_map() {
  return &station_hit_map_;
}

}  // namespace MAUS
