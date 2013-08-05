/* This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
 *
 * MAUS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MAUS is distributedTrackReconstructor in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
 /* Author: Peter Lane
 */

#include "src/map/MapCppGlobalRawTracks/MapCppGlobalRawTracks.hh"

// C++
#include <algorithm>
#include <cmath>
#include <map>
#include <string>
#include <vector>

// External
#include "TLorentzVector.h"
#include "TMinuit.h"
#include "CLHEP/Random/RandGauss.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include "json/json.h"

// Legacy/G4MICE
#include "Config/MiceModule.hh"
#include "Interface/Squeal.hh"

// MAUS
#include "Converter/DataConverters/JsonCppSpillConverter.hh"
#include "Converter/DataConverters/CppJsonSpillConverter.hh"
#include "DataStructure/Data.hh"
#include "DataStructure/GlobalEvent.hh"
#include "DataStructure/MCEvent.hh"
#include "DataStructure/ReconEvent.hh"
#include "DataStructure/SciFiSpacePoint.hh"
#include "DataStructure/TOFEventSpacePoint.hh"
#include "DataStructure/ThreeVector.hh"
#include "DataStructure/Global/ReconEnums.hh"
#include "DataStructure/Global/BasePoint.hh"
#include "DataStructure/Global/SpacePoint.hh"
#include "DataStructure/Global/Track.hh"
#include "DataStructure/Global/TrackPoint.hh"
#include "src/common_cpp/Optics/CovarianceMatrix.hh"
#include "Recon/Global/DataStructureHelper.hh"
#include "Recon/Global/Detector.hh"
#include "Recon/Global/Particle.hh"
#include "src/common_cpp/Simulation/MAUSGeant4Manager.hh"
#include "Utils/Globals.hh"
#include "Utils/JsonWrapper.hh"
#include "Utils/CppErrorHandler.hh"

namespace MAUS {

namespace GlobalDS = ::MAUS::DataStructure::Global;
using MAUS::recon::global::DataStructureHelper;
using MAUS::recon::global::Detector;
using MAUS::recon::global::DetectorMap;
using MAUS::recon::global::Particle;

MapCppGlobalRawTracks::MapCppGlobalRawTracks() {
}

MapCppGlobalRawTracks::~MapCppGlobalRawTracks() {
}

bool MapCppGlobalRawTracks::birth(std::string configuration_string) {
  // parse the JSON document.
  try {
    const Json::Value configuration
      = JsonWrapper::StringToJson(configuration_string);

    DataStructureHelper::GetInstance().GetDetectorAttributes(
        configuration, detectors_);
  } catch(Squeal& squee) {
    MAUS::CppErrorHandler::getInstance()->HandleSquealNoJson(
      squee, MapCppGlobalRawTracks::kClassname);
    return false;
  } catch(std::exception& exc) {
    MAUS::CppErrorHandler::getInstance()->HandleStdExcNoJson(
      exc, MapCppGlobalRawTracks::kClassname);
    return false;
  }

  MAUSGeant4Manager * const simulator = MAUSGeant4Manager::GetInstance();
  MAUSPrimaryGeneratorAction::PGParticle reference_pgparticle
    = simulator->GetReferenceParticle();
  switch (reference_pgparticle.pid) {
    case MAUS::DataStructure::Global::kEPlus:
    case MAUS::DataStructure::Global::kMuPlus:
    case MAUS::DataStructure::Global::kPiPlus:
      beam_polarity_ = 1;
      break;
    case MAUS::DataStructure::Global::kEMinus:
    case MAUS::DataStructure::Global::kMuMinus:
    case MAUS::DataStructure::Global::kPiMinus:
      beam_polarity_ = -1;
      break;
    default:
      throw(Squeal(Squeal::nonRecoverable,
                   "Reference particle is not a pion+/-, muon+/-, or e+/-.",
                   "MapCppGlobalRawTracks::birth()"));

  }

  return true;  // Sucessful parsing
}

std::string MapCppGlobalRawTracks::process(std::string run_data_string) {
  // parse the JSON document.
  Json::Value run_data_json
    = Json::Value(JsonWrapper::StringToJson(run_data_string));
  if (run_data_json.isNull() || run_data_json.empty()) {
    return std::string("{\"errors\":{\"bad_json_document\":"
                        "\"Failed to parse input document\"}}");
  }

  JsonCppSpillConverter deserialize;
  MAUS::Data * run_data = deserialize(&run_data_json);
  if (!run_data) {
    return std::string("{\"errors\":{\"failed_json_cpp_conversion\":"
                        "\"Failed to convert Json to C++ Data object\"}}");
  }

  const MAUS::Spill * spill = run_data->GetSpill();
  MAUS::ReconEventPArray * recon_events = spill->GetReconEvents();
  if (!recon_events) {
    return run_data_string;
  }

  MAUS::ReconEventPArray::const_iterator recon_event;
  MAUS::GlobalEvent * global_event;
  for (recon_event = recon_events->begin();
      recon_event != recon_events->end();
      ++recon_event) {
    global_event = new GlobalEvent();
    // Load the ReconEvent, and import it into the GlobalEvent
    AssembleRawTracks(*recon_event, global_event);
  }

  // Serialize the Spill for passing on to the next map in the workflow
  CppJsonSpillConverter serialize;
  Json::FastWriter writer;
  std::string output = writer.write(*serialize(run_data));
  /*
  std::cout << "DEBUG MapCppGlobalRawTracks::process(): "
            << "Output: " << std::endl
            << output << std::endl;
  */
  delete run_data;

  return output;
}

bool MapCppGlobalRawTracks::death() {
  return true;  // successful
}

/** AssembleRawTracks - load data provided by the DAQ system or via digitized MC
 *
 *  Pseudocode:
 *    1) Deserialize TOF and SciFi events
 *    2) Assemble all raw track permutations
 *    3) Prune any raw tracks that are unlickly or impossible
 *    4) Serialize raw tracks
 */
void MapCppGlobalRawTracks::AssembleRawTracks(
    MAUS::ReconEvent * recon_event,
    MAUS::GlobalEvent * global_event) {

  // Load TOF and SciFi tracks from the appropriate recon event trees
  GlobalDS::TrackPArray tracks;
  /*
  std::cout << "DEBUG MapCppGlobalRawTracks::AssembleRawTracks(): "
            << "Loading TOF track..." << std::endl;
  */
  LoadTOFTrack(recon_event, tracks);
  size_t tof_track_count = tracks.size();
  /*
  std::cout << "DEBUG MapCppGlobalRawTracks::AssembleRawTracks(): "
            << "Loaded " << tof_track_count << " TOF tracks." << std::endl;
  */
  /*
  GlobalDS::TrackPArray sci_fi_tracks;
  std::cout << "DEBUG MapCppGlobalRawTracks::AssembleRawTracks(): "
            << "Loading SciFi track..." << std::endl;
  */
  LoadSciFiTrack(recon_event, tracks);
  /*
  size_t sci_fi_track_count = tracks.size() - tof_track_count;
  std::cout << "DEBUG MapCppGlobalRawTracks::AssembleRawTracks(): "
            << "Loaded " << sci_fi_track_count << " SciFi tracks."
            << std::endl;
  */
  // FIXME(Lane) currently assuming only one track...
  if (tracks.size() > 0) {
    tracks[0]->SortTrackPointsByZ();
    tracks[0]->set_mapper_name(kClassname);
    global_event->add_track_recursive(tracks[0]);
  }

  recon_event->SetGlobalEvent(global_event);
}

/** LoadTOFTrack
 */
void MapCppGlobalRawTracks::LoadTOFTrack(
    MAUS::ReconEvent const * const recon_event,
    GlobalDS::TrackPArray & tof_tracks) {
  const TOFEvent * tof_event = recon_event->GetTOFEvent();
  const TOFEventSpacePoint space_point_events
    = tof_event->GetTOFEventSpacePoint();
  const std::vector<TOFSpacePoint> tof0_space_points
    = space_point_events.GetTOF0SpacePointArray();
  const std::vector<TOFSpacePoint> tof1_space_points
    = space_point_events.GetTOF1SpacePointArray();
  const std::vector<TOFSpacePoint> tof2_space_points
    = space_point_events.GetTOF2SpacePointArray();
  
  /*
  std::cout << "DEBUG LoadTOFTrack: space points in TOF0: "
            << tof0_space_points.size() << "\tTOF1: "
            << tof1_space_points.size() << "\tTOF2: "
            << tof2_space_points.size() << std::endl;
  */
  if (tof0_space_points.size() != tof1_space_points.size()) {
    std::cout << "DEBUG LoadTOFTrack: track has different number of "
              << "space points in TOF0 (" << tof0_space_points.size() << ")"
              << " and TOF1 (" << tof1_space_points.size() << ")...skipping."
              << std::endl;
    return;
  } else if (tof0_space_points.size() == 0) {
    std::cout << "DEBUG LoadTOFTrack: track has no space points...skipping."
              << std::endl;
    return;
  }

  GlobalDS::Track * track = new GlobalDS::Track();

  TLorentzVector last_position[3];
  TLorentzVector deltas[3];

  GlobalDS::TrackPointPArray track_points;

  // FIXME(Lane) Replace hard coded slab to coordinate conversion once
  // Mark Reiner's code has been incorporated into TOF space point code.
  MAUS::recon::global::DetectorMap::const_iterator tof0_mapping
    = detectors_.find(GlobalDS::kTOF0);
  if (tof0_mapping == detectors_.end()) {
      throw(Squeal(Squeal::nonRecoverable,
                   "Unable to find info for detector TOF0",
                   "MapCppGlobalRawTracks::LoadTOFTrack()"));
  }
  const Detector& tof0 = tof0_mapping->second;
  std::vector<TOFSpacePoint>::const_iterator tof0_space_point;
  for (tof0_space_point = tof0_space_points.begin();
       tof0_space_point != tof0_space_points.end();
       ++tof0_space_point) {
    GlobalDS::TrackPoint * track_point = new GlobalDS::TrackPoint();
    PopulateTOFTrackPoint(tof0, tof0_space_point, 40., 10, track_point);
    track_points.push_back(track_point);

    last_position[0] = track_point->get_position();
    /*
    std::cout << "DEBUG MapCppGlobalRawTracks::LoadTOFTrack(): "
              << "\t TOF0 4-position: (" << last_position[0].T()
              << ", " << last_position[0].X()
              << ", " << last_position[0].Y() << ", " << last_position[0].Z()
              << ")" << std::endl;
    */
  }

  MAUS::recon::global::DetectorMap::const_iterator tof1_mapping
    = detectors_.find(GlobalDS::kTOF1);
  if (tof1_mapping == detectors_.end()) {
      throw(Squeal(Squeal::nonRecoverable,
                   "Unable to find info for detector TOF1",
                   "MapCppGlobalRawTracks::LoadTOFTrack()"));
  }
  const Detector& tof1 = tof1_mapping->second;
  std::vector<TOFSpacePoint>::const_iterator tof1_space_point;
  for (tof1_space_point = tof1_space_points.begin();
       tof1_space_point != tof1_space_points.end();
       ++tof1_space_point) {
    GlobalDS::TrackPoint * track_point = new GlobalDS::TrackPoint();
    PopulateTOFTrackPoint(tof1, tof1_space_point, 60., 7, track_point);
    track_points.push_back(track_point);

    last_position[1] = track_point->get_position();
    /*
    std::cout << "DEBUG MapCppGlobalRawTracks::LoadTOFTrack(): "
              << "\t TOF1 4-position: (" << last_position[1].T()
              << ", " << last_position[1].X()
              << ", " << last_position[1].Y() << ", " << last_position[1].Z()
              << ")" << std::endl;
    */
  }
  deltas[0] = last_position[1] - last_position[0];
  deltas[1] = deltas[0];

  MAUS::recon::global::DetectorMap::const_iterator tof2_mapping
    = detectors_.find(GlobalDS::kTOF2);
  if (tof2_mapping == detectors_.end()) {
      throw(Squeal(Squeal::nonRecoverable,
                   "Unable to find info for detector TOF2",
                   "MapCppGlobalRawTracks::LoadTOFTrack()"));
  }
  const Detector& tof2 = tof2_mapping->second;
  std::vector<TOFSpacePoint>::const_iterator tof2_space_point;
  for (tof2_space_point = tof2_space_points.begin();
       tof2_space_point != tof2_space_points.end();
       ++tof2_space_point) {
    GlobalDS::TrackPoint * track_point = new GlobalDS::TrackPoint();
    PopulateTOFTrackPoint(tof2, tof2_space_point, 60., 10, track_point);
    track_points.push_back(track_point);

    last_position[2] = track_point->get_position();
    /*
    std::cout << "DEBUG MapCppGlobalRawTracks::LoadTOFTrack(): "
              << "\t TOF2 4-position: (" << last_position[2].T()
              << ", " << last_position[2].X()
              << ", " << last_position[2].Y() << ", " << last_position[2].Z()
              << ")" << std::endl;
    */
  }
  deltas[2] = last_position[2] - last_position[1];

  // Approximate momenta by using tof0/tof1 and tof1/tof2 position deltas
  TLorentzVector momenta[3];
  for (size_t index = 0; index < 3; ++index) {
    double delta_x = deltas[index].X();
    /*
    std::cout << "DEBUG MapCppGlobalRawTracks::LoadTOFTrack(): "
              << "delta_x: " << delta_x << std::endl;
    */
    double delta_y = deltas[index].Y();
    double delta_z = deltas[index].Z();
    /*
    double delta_t = deltas[index].T();
    std::cout << "DEBUG MapCppGlobalRawTracks::LoadTOFTrack(): "
              << "delta_t: " << delta_t << std::endl;
    */
    /*
    double delta_l = std::sqrt(delta_x*delta_x + delta_y*delta_y
                                + delta_z*delta_z);
    std::cout << "DEBUG MapCppGlobalRawTracks::LoadTOFTrack(): "
              << "delta_l: " << delta_l << std::endl;
    */
    /*
    double beta = delta_l / delta_t / ::CLHEP::c_light;
    std::cout << "DEBUG MapCppGlobalRawTracks::LoadTOFTrack(): "
              << "beta: " << beta << std::endl;
    double gamma = 1. / std::sqrt(1 - beta*beta);
    std::cout << "DEBUG MapCppGlobalRawTracks::LoadTOFTrack(): "
              << "gamma: " << gamma << std::endl;
    */
    /* FIXME(Lane) Forcing muon PID for now. Should be
    * GlobalDS::PID particle_id = IdentifyParticle(beta);
    * plus code to create multiple hypotheses (i.e. pion/muon ambiguity)
    */
    /*
    GlobalDS::PID particle_id
      = GlobalDS::PID(GlobalDS::kMuPlus * beam_polarity_);
    */
    MAUSGeant4Manager * const simulator = MAUSGeant4Manager::GetInstance();
    MAUSPrimaryGeneratorAction::PGParticle reference_pgparticle
      = simulator->GetReferenceParticle();
    const GlobalDS::PID particle_id = GlobalDS::PID(reference_pgparticle.pid);
    /*
    double mass = Particle::GetInstance().GetMass(particle_id);
    std::cout << "DEBUG MapCppGlobalRawTracks::LoadTOFTrack(): "
              << "mass: " << mass << std::endl;
    */
    /*
    double energy = FindEnergy(mass, delta_z, delta_t);
    double beta = ::sqrt(1 - ::pow(mass / energy, 2));
    for (size_t slab = 0; slab < (2*index); ++slab) {
      energy -= TOFSlabEnergyLoss(beta, mass);
      beta = ::sqrt(1 - ::pow(mass / energy, 2));
    }
    std::cout << "DEBUG MapCppGlobalRawTracks::LoadTOFTrack(): "
              << "beta: " << beta << std::endl;
    std::cout << "DEBUG MapCppGlobalRawTracks::LoadTOFTrack(): "
              << "energy: " << energy << std::endl;
    std::cout << "DEBUG MapCppGlobalRawTracks::LoadTOFTrack(): "
              << "beta: " << beta << std::endl;
    double gamma = 1. / std::sqrt(1 - beta*beta);
    std::cout << "DEBUG MapCppGlobalRawTracks::LoadTOFTrack(): "
              << "gamma: " << gamma << std::endl;

    momenta[index] = TLorentzVector(gamma * mass * delta_x / delta_t
                                          / ::CLHEP::c_light,
                                    gamma * mass * delta_y / delta_t
                                          / ::CLHEP::c_light,
                                    beta * energy,
                                    energy);
    */
    momenta[index] = TLorentzVector(reference_pgparticle.px,
                                    reference_pgparticle.py,
                                    reference_pgparticle.pz,
                                    reference_pgparticle.energy);
    /*
    std::cout << "DEBUG MapCppGlobalRawTracks::LoadTOFTrack(): "
              << "Particle: " << Particle::GetInstance().GetName(particle_id)
              << "\t 4-momentum: (" << momenta[index].T()
              << ", " << momenta[index].X()
              << ", " << momenta[index].Y() << ", " << momenta[index].Z()
              << ")" << std::endl;
    */
  }

  // Set each track point's 4-momentum and add to the track
  std::vector<GlobalDS::TrackPoint *>::iterator track_point;
  size_t point_index = 0;
  for (track_point = track_points.begin();
       track_point != track_points.end();
       ++track_point) {
    (*track_point)->set_momentum(momenta[point_index++]);

    track->AddTrackPoint(*track_point);
  }

  //track->SortTrackPointsByZ();

  // FIXME(Lane) For now assume we've selected only muon tracks and no decays
  /*
  const GlobalDS::PID particle_id
    = GlobalDS::PID(GlobalDS::kMuPlus * beam_polarity_);
  */
  MAUSGeant4Manager * const simulator = MAUSGeant4Manager::GetInstance();
  MAUSPrimaryGeneratorAction::PGParticle reference_pgparticle
    = simulator->GetReferenceParticle();
  const GlobalDS::PID particle_id = GlobalDS::PID(reference_pgparticle.pid);
  track->set_pid(particle_id);

  tof_tracks.push_back(track);
}

/** PopulateTOFTrackPoint
 */
void MAUS::MapCppGlobalRawTracks::PopulateTOFTrackPoint(
    const Detector & detector,
    const std::vector<TOFSpacePoint>::const_iterator & tof_space_point,
    const double slab_width,
    const size_t number_of_slabs,
    GlobalDS::TrackPoint * track_point) {
  DataStructureHelper helper = DataStructureHelper::GetInstance();
  const double z = helper.GetDetectorZPosition(detector.id());
  std::cerr << "DEBUG MapCppGlobalRawTracks::PopulateTOFTrackPoint: "
            << std::endl
            << "\tPhys. Event #: " << tof_space_point->GetPhysEventNumber()
            << std::endl;
  std::cerr << "DEBUG MapCppGlobalRawTracks::PopulateTOFTrackPoint: " << std::endl
            << "\tID: " << detector.id() << std::endl
            << "\tz-position: " << z << std::endl
            << "\tUncertainties: " << detector.uncertainties() << std::endl;
  double max_xy = slab_width * ::floor(number_of_slabs / 2.0);
  if (number_of_slabs % 2 == 0) {
    max_xy -= slab_width / 2.0;
  }
  std::cerr << "DEBUG MapCppGlobalRawTracks::PopulateTOFTrackPoint: " << std::endl
            << "\t# Slabs: " << number_of_slabs << std::endl
            << "\tSlab Width: " << slab_width << std::endl
            << "\tMax X/Y Value: " << max_xy << std::endl;

  GlobalDS::SpacePoint * space_point = new GlobalDS::SpacePoint();

  space_point->set_detector(detector.id());

  // FIXME(Lane) not sure what to put here
  space_point->set_geometry_path("");
  
  // GetSlaby() gets the number of the slab that is *oriented* in the y
  // direction For TOF0 this is the slab that gives an approximate x coordinate)
  // and vice versa. For TOF1 and TOF2 it yields the y coordinate.
  /*
  std::cout << "DEBUG MapCppGlobalRawTracks::PopulateTOFTrackPoint(): "
            << "Vertical Slab: " << tof_space_point->GetSlaby()
            << "\tHorizontal Slab: " << tof_space_point->GetSlabx() << std::endl;
  */
  const double x = slab_width * tof_space_point->GetSlaby() - max_xy;
  const double y = slab_width * tof_space_point->GetSlabx() - max_xy;
  const double t = tof_space_point->GetTime();
  TLorentzVector position(x, y, z, t);
  space_point->set_position(position);
  /*
  std::cout << "DEBUG MapCppGlobalRawTracks::PopulateTOFTrackPoint(): "
            << "position: (" << space_point->get_position().T() << ","
            << space_point->get_position().X() << ","
            << space_point->get_position().Y() << ","
            << space_point->get_position().Z() << ")" << std::endl;
  */
  dynamic_cast<GlobalDS::BasePoint*>(track_point)->operator=(*space_point);
  track_point->set_space_point(space_point);
  track_point->set_particle_event(tof_space_point->GetPhysEventNumber());
  track_point->set_mapper_name(kClassname);

  track_point->set_position(position);
  CovarianceMatrix covariances = detector.uncertainties();
  TLorentzVector position_errors(::sqrt(covariances(3, 3)),
                                  ::sqrt(covariances(5, 5)),
                                  0.,
                                  ::sqrt(covariances(1, 1)));
  track_point->set_position_error(position_errors);

  TLorentzVector momentum_errors(::sqrt(covariances(4, 4)),
                                  ::sqrt(covariances(6, 6)),
                                  0.,
                                  ::sqrt(covariances(2, 2)));
  track_point->set_momentum_error(momentum_errors);

  track_point->set_charge(tof_space_point->GetChargeProduct());
}

/** FindEnergy
 */
double MapCppGlobalRawTracks::FindEnergy(const double mass,
                                         const double delta_z,
                                         const double actual_delta_t) const {
  MAUSGeant4Manager * const simulator = MAUSGeant4Manager::GetInstance();
  MAUSPrimaryGeneratorAction::PGParticle reference_pgparticle
    = simulator->GetReferenceParticle();
  const double reference_energy = reference_pgparticle.energy;
  const double slab_length = 25.;  // mm
  const double drift_length = delta_z - 2 * slab_length;
  const double beta_0 = delta_z / actual_delta_t / ::CLHEP::c_light;
  /*
  std::cout << "DEBUG FindEnergy: delta_z: " << delta_z << "\tdelta_t: "
            << actual_delta_t << std::endl;
  */
  const double gamma_0 = 1. / std::sqrt(1 - beta_0*beta_0);
  const double E_0 = gamma_0 * mass;
  double E = E_0;
  double beta[4];
  double delta_t[3];
  double E_max = reference_energy + 50.;  // allow for momentum spread
  double E_min = 0;
  if (beta_0 > 1) {
    E = E_max;
  }
  /*
  std::cout << "DEBUG FindEnergy: beta0: " << beta_0 << "\tE0: " << E
            << std::endl;
  */
  bool energy_too_low = false;

  for (size_t iteration = 0; iteration < 10; ++iteration) {
    double slab_energy_loss = 0.;
    for (size_t index = 0; index < 4; ++index) {
      beta[index] = ::sqrt(1 - ::pow(mass / (E - slab_energy_loss), 2));
      if (beta[index] == 0.) {
        energy_too_low = true;
      }
      slab_energy_loss += TOFSlabEnergyLoss(beta[index], mass);
      if ((slab_energy_loss > E) || (slab_energy_loss != slab_energy_loss)) {
        energy_too_low = true;
      }
    }
    /*
    std::cout << "DEBUG FindEnergy: beta[0] = " << beta[0]
              << "\tbeta[1] = " << beta[1]
              << "\tbeta[2] = " << beta[2]
              << "\tbeta[3] = " << beta[3] << std::endl;
    std::cout << "DEBUG FindEnergy: dEdx[0] = " << TOFMeanStoppingPower(beta[0], mass)
              << "\tdEdx[1] = " << TOFMeanStoppingPower(beta[1], mass)
              << "\tdEdx[2] = " << TOFMeanStoppingPower(beta[2], mass)
              << "\tdEdx[3] = " << TOFMeanStoppingPower(beta[3], mass) << std::endl;
    std::cout << "DEBUG FindEnergy: dE[0] = " << TOFSlabEnergyLoss(beta[0], mass)
              << "\tdE[1] = " << TOFSlabEnergyLoss(beta[1], mass)
              << "\tdE[2] = " << TOFSlabEnergyLoss(beta[2], mass)
              << "\tdE[3] = " << TOFSlabEnergyLoss(beta[3], mass) << std::endl;
    */
    delta_t[0] = slab_length / ((beta[0] + beta[1]) / 2) / ::CLHEP::c_light;
    delta_t[1] = drift_length / beta[1] / ::CLHEP::c_light;
    delta_t[2] = slab_length / ((beta[1] + beta[2]) / 2) / ::CLHEP::c_light;
    /*
    std::cout << "DEBUG FindEnergy: delta_t[0] = " << delta_t[0]
              << "\tdelta_t[1] = " << delta_t[1]
              << "\tdelta_t[2] = " << delta_t[2] << std::endl;
    */
    double total_delta_t = 0.;
    for (size_t index = 0; index < 3; ++index) {
      total_delta_t += delta_t[index];
    }
    const double residual_delta_t = actual_delta_t - total_delta_t;
    std::cout << "DEBUG FindEnergy: Actual dt = " << actual_delta_t
              << "\tdt = " << total_delta_t << std::endl;
    std::cout << "DEBUG FindEnergy: Min E = " << E_min
              << "\tMax E = " << E_max << std::endl;
    if (energy_too_low || (total_delta_t != total_delta_t)
        || (residual_delta_t < 0.)){
      // energy is too low
      E_min = E;
      E = (E + E_max) / 2;
      energy_too_low = false;
    } else if (residual_delta_t > 0.) {
      // energy is too high
      E_max = E;
      E = (E_min + E) / 2;
    }
    std::cout << "DEBUG FindEnergy: E[" << iteration << "] = " << E << std::endl;
  }

  return E;
}

/** TOFSlabEnergyLoss
 */
double MapCppGlobalRawTracks::TOFSlabEnergyLoss(const double beta,
                                                const double mass) const {
  const size_t slab_length = 25;  // mm steps
  double E_0 = mass / ::sqrt(1 - beta*beta);
  double beta_i = beta;
  double E_i = E_0;
  for (size_t step = 0; step < slab_length; ++step) {
    const double dEdx = TOFMeanStoppingPower(beta_i, mass);
    E_i -= dEdx;
    if (E_i < mass) {
      E_i = mass;
      break;
    }
    beta_i = ::sqrt(1 - ::pow(mass / E_i, 2));
    std::cout << "DEBUG TOFSlabEnergyLoss: dE/dx[" << step << "]: " << dEdx
              << "\t E_i: " << E_i
              << "\t mass: " << mass << "\tbeta_i: " << beta_i << std::endl;
  }
  return E_0 - E_i;
}

/** TOFMeanStoppingPower
 */
double MapCppGlobalRawTracks::TOFMeanStoppingPower(const double beta,
                                                   const double mass) const {
  const double gamma = 1. / ::sqrt(1-beta*beta);
  const double beta2 = beta*beta;
  const double gamma2 = gamma*gamma;
  // values are for polystyrene
  const double density = 1.060;  // g cm^-3
  const double me = 0.510998918;  // electron mass (MeV/c^2)
  const double K = 0.307075;  // MeV g^-2 cm^2 mol
  const double Tmax = 2 * me * beta2 * gamma2
                    / (1 + 2 * gamma * me/mass + ::pow(me/mass, 2));
  const double Z_over_A = 0.53768;
  const double I = 6.87e-5;  // MeV (68.7 eV)

  std::cout << "DEBUG TOFMeanStoppingPower: beta: " << beta << "\tgamma: "
            << gamma << "\tTmax: " << Tmax << std::endl;
  double density_effect = 0.;
  const double x0 = 0.1647;
  const double x1 = 2.5031;
  const double Cbar = 3.2999;
  const double a = 0.1645;
  const double k = 3.2224;
  const double x = beta * gamma;
  if (x >= x1) {
    density_effect = 2 * ::log(10) * x - Cbar;
  } else if ((x0 <= x) && (x < x1)) {
    density_effect = 2 * ::log(10) * x - Cbar + a * ::pow(x1-x, k);
  }  // (x < x0)

  const double log_operand = 2 * me * beta2 * gamma2 * Tmax / (I*I);
  std::cout << "DEBUG TOFMeanStoppingPower: ln operand: " << log_operand
            << std::endl;

  // Bethe-Bloch equation times the density of polystyrene in MeV / mm
  const double stopping_power
    = K * Z_over_A / beta2
    * (  0.5 * ::log(log_operand) - beta2 - density_effect / 2)
    * density / 10.;
  std::cout << "DEBUG TOFMeanStoppingPower: Stopping Power: " << stopping_power
            << std::endl;

  return stopping_power;
}

/** LoadSciFiTrack
 */
void MapCppGlobalRawTracks::LoadSciFiTrack(
    MAUS::ReconEvent const * const recon_event,
    GlobalDS::TrackPArray & tracks) {
  if (tracks.size() == 0) {
    std::cout << "DEBUG LoadSciFiTrack: no TOF tracks to add SciFi tracks to!"
              << std::endl;
    return;
  }

  SciFiEvent const * const sci_fi_event = recon_event->GetSciFiEvent();
  SciFiSpacePointPArray space_points = sci_fi_event->spacepoints();

  // FIXME(Lane) currently assuming only one track...
  GlobalDS::Track * track = tracks[0];
  for (SciFiSpacePointPArray::const_iterator space_point = space_points.begin();
       space_point != space_points.end();
       ++space_point) {

    const int tracker = (*space_point)->get_tracker();
    const int station = (*space_point)->get_station();
    /*
    std::cout << "DEBUG MapCppGlobalRawTracks::LoadSciFiTrackPoint(): "
              << "\tTracker: " << tracker
              << "\tStation: " << station << std::endl;
    */
    const GlobalDS::DetectorPoint detector_id = GlobalDS::DetectorPoint(
        GlobalDS::kTracker0 + 6 * tracker + station);
    MAUS::recon::global::DetectorMap::const_iterator detector_mapping
      = detectors_.find(detector_id);
    if (detector_mapping == detectors_.end()) {
      std::stringstream message;
      message << "Couldn't find configuration for detector Tracker "
              << tracker << " Station " << station
              << "(id=" << detector_id << ")";
      throw(Squeal(Squeal::nonRecoverable,
                   message.str(),
                   "MapCppGlobalRawTracks::LoadSciFiTrack()"));
    }
    const Detector& detector = detector_mapping->second;
  /*
  std::cout << "DEBUG MapCppGlobalRawTracks::LoadSciFiTrackPoint(): "
            << "\tSciFi Space Point:" << std::endl
            << "\tDetector: " << detector_id
            << "\tStation: " << station << std::endl;
    */
    GlobalDS::TrackPoint * track_point = new GlobalDS::TrackPoint();
    PopulateSciFiTrackPoint(detector, space_point, track_point);
    track->AddTrackPoint(track_point);
  }

  //track->SortTrackPointsByZ();

  MAUSGeant4Manager * const simulator = MAUSGeant4Manager::GetInstance();
  MAUSPrimaryGeneratorAction::PGParticle reference_pgparticle
    = simulator->GetReferenceParticle();
  const GlobalDS::PID particle_id = GlobalDS::PID(reference_pgparticle.pid);
  track->set_pid(particle_id);

  tracks.push_back(track);
}

/** PopulateSciFiTrackPoint
 */
void MapCppGlobalRawTracks::PopulateSciFiTrackPoint(
    const MAUS::recon::global::Detector & detector,
    const SciFiSpacePointPArray::const_iterator & scifi_space_point,
    GlobalDS::TrackPoint * track_point) {
  GlobalDS::SpacePoint * space_point = new GlobalDS::SpacePoint();

  space_point->set_detector(detector.id());

  // FIXME(Lane) not sure what to put here
  space_point->set_geometry_path("");

  MAUSGeant4Manager * const simulator = MAUSGeant4Manager::GetInstance();
  MAUSPrimaryGeneratorAction::PGParticle reference_pgparticle
    = simulator->GetReferenceParticle();

  ThreeVector position = (*scifi_space_point)->get_position();
  DataStructureHelper helper = DataStructureHelper::GetInstance();
  const double z = helper.GetDetectorZPosition(detector.id());
  // TODO(Lane) need tracker timestamp that is synched with TOF timestamp
  // const double time = (*scifi_space_point)->get_time();
  const double time = 0.;
  TLorentzVector four_position;
  if (detector.id() <= GlobalDS::kTracker0_5) {
    four_position = TLorentzVector(-position.x(), position.y(), z, time);
  } else {
    four_position = TLorentzVector(position.x(), position.y(), z, time);
  }
  space_point->set_position(four_position);

  // FIXME(Lane) Replace MC momentum with official track momentum when that
  // information becomes available.
  ThreeVector momentum;
  const SciFiClusterPArray clusters = (*scifi_space_point)->get_channels();
  //double time = 0;
  for (SciFiClusterPArray::const_iterator cluster = clusters.begin();
      cluster != clusters.end();
      ++cluster) {
    //time += (*cluster)->get_time();
    ThreeVector true_momentum = (*cluster)->get_true_momentum();
    /*
    std::cout << "DEBUG MapCppGlobalRawTracks::PopulateSciFiTrack(): " << std::endl
            << "\tTrue Momentum: " << true_momentum << std::endl;
    */
    momentum += true_momentum;
  }
  //time /= clusters.size();
  if (clusters.size() > 0) {
    momentum /= clusters.size();
  }
  // smear the MC momentum1.293e+04
  momentum.setX(momentum.x() + ::CLHEP::RandGauss::shoot(0., 3.));
  momentum.setY(momentum.y() + ::CLHEP::RandGauss::shoot(0., 3.));
  momentum.setZ(momentum.z() + ::CLHEP::RandGauss::shoot(0., 20.));
  /*
  std::cout << "DEBUG MapCppGlobalRawTracks::PopulateSciFiTrack(): " << std::endl
          << "\tSmeared Momentum: " << momentum << std::endl;
  */
  dynamic_cast<GlobalDS::BasePoint*>(track_point)->operator=(*space_point);
  track_point->set_space_point(space_point);

  track_point->set_particle_event((*scifi_space_point)->get_event());
  track_point->set_mapper_name(kClassname);

  // FIXME(Lane) For now assume we've selected only muon tracks and no decays
  /*
  const GlobalDS::PID particle_id
    = GlobalDS::PID(GlobalDS::kMuPlus * beam_polarity_);
  */
  const GlobalDS::PID particle_id = GlobalDS::PID(reference_pgparticle.pid);
  const double beta = Beta(particle_id, momentum.mag());
  const double energy = momentum.mag() / beta;
  TLorentzVector four_momentum(
    momentum.x(), momentum.y(), momentum.z(), energy);
  track_point->set_momentum(four_momentum);
  /*
  std::cout << "DEBUG MapCppGlobalRawTracks::PopulateSciFiTrackPoint(): "
            << "\tSciFi Space Point:" << std::endl
            << std::setprecision(10)
            << "\tTime: " << time << "\tEnergy: " << energy << std::endl
            << "\t4-Position: (" << four_position.X() << ", "
            << four_position.Y() << ", " << four_position.Z() << ", "
            << four_position.T() << ")" << std::endl
            << "\tMomentum: (" << four_momentum.Px() << ", "
            << four_momentum.Py() << ", " << four_momentum.Pz() << ", "
            << four_momentum.Pt() << ")" << std::endl;
  */
  CovarianceMatrix covariances = detector.uncertainties();
  TLorentzVector position_errors(::sqrt(covariances(3, 3)),
                                  ::sqrt(covariances(5, 5)),
                                  0.,
                                  ::sqrt(covariances(1, 1)));
  track_point->set_position_error(position_errors);

  TLorentzVector momentum_errors(::sqrt(covariances(4, 4)),
                                  ::sqrt(covariances(6, 6)),
                                  0.,
                                  ::sqrt(covariances(2, 2)));
  track_point->set_momentum_error(momentum_errors);

  track_point->set_charge((*scifi_space_point)->get_npe());
}

/* Take an educated guess at the particle ID based on the axial velocity
 * of the particle (beta)
 */
GlobalDS::PID MapCppGlobalRawTracks::IdentifyParticle(const double beta) {
  MAUSGeant4Manager * const simulator = MAUSGeant4Manager::GetInstance();
  MAUSPrimaryGeneratorAction::PGParticle reference_pgparticle
    = simulator->GetReferenceParticle();
  const double reference_momentum = reference_pgparticle.pz;  // good enough
  const double beta_pi = Beta(GlobalDS::kPiPlus, reference_momentum);
  const double beta_mu = Beta(GlobalDS::kMuPlus, reference_momentum);
  const double beta_e = Beta(GlobalDS::kEPlus, reference_momentum);
  std::cout << "DEBUG MapCppGlobalRawTracks::IdentifyParticle(): " << std::endl
            << "\tbeta = " << beta << "\tbeta_pi = " << beta_pi << std::endl
            << "\tbeta_mu = " << beta_mu << "\tbeta_e = " << beta_e << std::endl;

  const double beta_fit[3] = {
    std::abs(1. - beta / beta_pi),
    std::abs(1. - beta / beta_mu),
    std::abs(1. - beta / beta_e)
  };
  std::cout << "DEBUG MapCppGlobalRawTracks::IdentifyParticle(): "
            << "beta fits {" << std::endl << std::setprecision(4)
            << "\t" << 1. - beta / beta_pi
            << "\t" << beta_fit[1]
            << "\t" << beta_fit[2]
            << std::endl << "}" << std::endl;
  size_t min_index = 0;
  for (size_t index = 1; index < 3; ++index) {
    if (beta_fit[index] < beta_fit[min_index]) {
      min_index = index;
    }
  }

  GlobalDS::PID particle_id = GlobalDS::kNoPID;
  switch(min_index) {
    case 0: particle_id = GlobalDS::PID(GlobalDS::kPiPlus * beam_polarity_);
            break;
    case 1: particle_id = GlobalDS::PID(GlobalDS::kMuPlus * beam_polarity_);
            break;
    case 2: particle_id = GlobalDS::PID(GlobalDS::kEPlus * beam_polarity_);
            break;
  }

  return particle_id;
}

double MapCppGlobalRawTracks::Beta(GlobalDS::PID pid, const double momentum) {
  const double mass = Particle::GetInstance().GetMass(pid);
  const double one_over_beta_squared
    = 1+ (mass*mass) / (momentum*momentum);
  return 1. / std::sqrt(one_over_beta_squared);
}

const std::string MapCppGlobalRawTracks::kClassname
  = "MapCppGlobalRawTracks";

}  // namespace MAUS

