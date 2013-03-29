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
#include "TMinuit.h"
#include "CLHEP/Random/RandGauss.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include "json/json.h"

// Legacy/G4MICE
#include "Interface/Squeal.hh"
#include "Interface/dataCards.hh"

// MAUS
#include "src/common_cpp/Converter/DataConverters/JsonCppSpillConverter.hh"
#include "src/common_cpp/Converter/DataConverters/CppJsonSpillConverter.hh"
#include "src/common_cpp/DataStructure/Data.hh"
#include "src/common_cpp/DataStructure/MCEvent.hh"
#include "src/common_cpp/DataStructure/ReconEvent.hh"
#include "src/common_cpp/DataStructure/SciFiSpacePoint.hh"
#include "src/common_cpp/DataStructure/TOFEventSpacePoint.hh"
#include "src/common_cpp/DataStructure/ThreeVector.hh"
#include "src/common_cpp/DataStructure/Global/ReconEnums.hh"
#include "src/common_cpp/DataStructure/Global/Track.hh"
#include "src/common_cpp/DataStructure/Global/TrackPoint.hh"
#include "src/common_cpp/JsonCppProcessors/GlobalTrackProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/SciFiSpacePointProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/TOFEventSpacePointProcessor.hh"
#include "src/common_cpp/Optics/CovarianceMatrix.hh"
#include "src/common_cpp/Recon/Global/DataStructureHelper.hh"
#include "src/common_cpp/Recon/Global/Detector.hh"
#include "src/common_cpp/Recon/Global/Particle.hh"
#include "src/common_cpp/Simulation/MAUSGeant4Manager.hh"
#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"

namespace MAUS {

using MAUS::recon::global::DataStructureHelper;
using MAUS::recon::global::Detector;
using MAUS::recon::global::Particle;
using MAUS::DataStructure::Global::DetectorPoint;
using MAUS::DataStructure::Global::PID;
using MAUS::DataStructure::Global::Track;
using MAUS::DataStructure::Global::TrackPoint;

typedef std::pair<

MapCppGlobalRawTracks::MapCppGlobalRawTracks() {
}

MapCppGlobalRawTracks::~MapCppGlobalRawTracks() {
}

bool MapCppGlobalRawTracks::birth(std::string configuration) {
  // parse the JSON document.
  try {
    configuration_ = JsonWrapper::StringToJson(configuration);
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
    case Particle::kEPlus:
    case Particle::kMuPlus:
    case Particle::kPiPlus:
      beam_polarity_ = 1;
      break;
    case Particle::kEMinus:
    case Particle::kMuMinus:
    case Particle::kPiMinus:
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
  MAUS::Data * run_data = deserialize(run_data_json);
  if (!data_cpp) {
    return std::string("{\"errors\":{\"failed_json_cpp_conversion\":"
                        "\"Failed to convert Json to C++ Data object\"}}");
  }

  const MAUS::Spill * spill = run_data->GetSpill();
  MAUS::ReconEventArray * recon_events_ = spill->GetReconEvents();
  if (!recon_events_) {
    return run_data_string;
  }

  MAUS::ReconEventArray::const_iterator recon_event;
  for (recon_event = recon_events->begin();
      recon_event < recon_events->end();
      ++recon_event) {
    MAUS::GlobalEvent * global_event = new GlobalEvent();
    // Load the ReconEvent, and import it into the GlobalEvent
    AssembleRawTracks(*recon_event, global_event);
  }

std::cout << "DEBUG MapCppGlobalRawTracks::process(): "
          << "Loaded " << tracks_.size() << " tracks." << std::endl;
  /*
  try {
  } catch(Squeal& squee) {
    MAUS::CppErrorHandler::getInstance()->HandleSquealNoJson(squee, kClassname);
  } catch(std::exception& exc) {
    MAUS::CppErrorHandler::getInstance()->HandleStdExcNoJson(exc, kClassname);
  }
  */

  std::cout << "DEBUG MapCppGlobalRawTracks::process(): "
            << "Processing " << tracks_.size()
            << " raw tracks" << std::endl;
  size_t event_index = 0;
  for (std::vector<MAUS::DataStructure::Global::Track *>::iterator
          raw_track = tracks_.begin();
       raw_track < tracks_.end();
       ++raw_track) {
    raw_track->SortTrackPointsByZ();
    MAUS::GlobalEvent * global_event = new GlobalEvent();
    recon_event->SetGlobalEvent(

    ++event_index;
  }

  // pass on the updated run data to the next map in the workflow
  Json::FastWriter writer;
  std::string output = writer.write(run_data_);
  std::cout << "DEBUG MapCppGlobalRawTracks::process(): "
            << "Output: " << std::endl
            << output << std::endl;

  return output;
}

bool MapCppGlobalRawTracks::death() {
  delete run_data_;
  run_data_ = NULL;
  return true;  // successful
}

/** LoadLiveData - load data provided by the DAQ system or via digitized MC
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
  std::map<Detector::ID, Detector> detectors;
  DataStructureHelper::GetInstance().GetDetectorAttributes(
      configuration_, detectors);

    // Load TOF and SciFi tracks from the appropriate recon event trees
    std::vector<Track> tof_tracks;
std::cout << "DEBUG MapCppGlobalRawTracks::LoadLiveData(): "
          << "Loading TOF tracks..." << std::endl;
    LoadTOFTracks(detectors, recon_event, tof_tracks);
std::cout << "DEBUG MapCppGlobalRawTracks::LoadLiveData(): "
          << "Loaded " << tof_tracks.size() << " TOF tracks." << std::endl;
    std::vector<Track> sci_fi_tracks;
std::cout << "DEBUG MapCppGlobalRawTracks::LoadLiveData(): "
          << "Loading SciFi tracks..." << std::endl;
    LoadSciFiTracks(detectors, recon_event, sci_fi_tracks);
std::cout << "DEBUG MapCppGlobalRawTracks::LoadLiveData(): "
          << "Loaded " << sci_fi_tracks.size() << " SciFi tracks." << std::endl;

    // Assuming there are an equal number of TOF and SciFi tracks,
    // merge each pair of tracks based on z position of the detectors
    std::vector<Track>::iterator tof_track = tof_tracks.begin();
    std::vector<Track>::iterator sci_fi_track = sci_fi_tracks.begin();
    while (tof_track != tof_tracks.end() ||
           sci_fi_track != sci_fi_tracks.end()) {
      std::vector<TrackPoint>::iterator tof_track_point;
      if (tof_track != tof_tracks.end()) {
        tof_track_point = tof_track->begin();
      }
      std::vector<TrackPoint>::iterator sci_fi_track_point;
      if (sci_fi_track != sci_fi_tracks.end()) {
        sci_fi_track_point = sci_fi_track->begin();
      }
      Track combined_track;
      while ((tof_track != tof_tracks.end() &&
              tof_track_point != tof_track->end()) ||
             (sci_fi_track != sci_fi_tracks.end() &&
              sci_fi_track_point != sci_fi_track->end())) {
        if (tof_track != tof_tracks.end() &&
            tof_track_point != tof_track->end() &&
            (sci_fi_track == sci_fi_tracks.end() ||
            sci_fi_track_point == sci_fi_track->end())) {
          combined_track.push_back(*tof_track_point);
          ++tof_track_point;
        } else if (sci_fi_track != sci_fi_tracks.end() &&
                   sci_fi_track_point != sci_fi_track->end() &&
                   (tof_track == tof_tracks.end() ||
                    tof_track_point == tof_track->end())) {
          combined_track.push_back(*sci_fi_track_point);
          ++sci_fi_track_point;
        } else if (tof_track != tof_tracks.end() &&
                   sci_fi_track != sci_fi_tracks.end() &&
                   tof_track_point != tof_track->end() &&
                   sci_fi_track_point != sci_fi_track->end()) {
          if (tof_track_point->z() <= sci_fi_track_point->z()) {
            combined_track.push_back(*tof_track_point);
            ++tof_track_point;
          } else {
            combined_track.push_back(*sci_fi_track_point);
            ++sci_fi_track_point;
          }
        }
      }
      tracks_.push_back(combined_track);
std::cout << "DEBUG MapCppGlobalRawTracks::LoadLiveData(): "
          << "Combined track size: " << combined_track.size() << std::endl;
std::cout << "DEBUG MapCppGlobalRawTracks::LoadLiveData(): "
          << "Global tracks acquired: " << tracks_.size() << std::endl;
      if (tof_track != tof_tracks.end()) {
        ++tof_track;
      }
      if (sci_fi_track != sci_fi_tracks.end()) {
        ++sci_fi_track;
      }
    }
  }

  recon_event->SetGlobalEvent(global_event);
}

std::vector<Track *> MapCppGlobalRawTracks::LoadTOFTracks(
    std::map<Detector::ID, Detector>& detectors,
    MAUS::ReconEvent * recon_event,
    std::vector<Track> * tof_tracks) {
  const TOFEvent * tof_event = recon_event->GetTOFEvent();
  const TOFEventSpacePoint * space_point_events
    = tof_event->GetTOFEventSpacePoint();
  const std::vector<TOFSpacePoint> tof0_space_points
    = space_point_events->GetTOF0SpacePointArray();
  const std::vector<TOFSpacePoint> tof1_space_points
    = space_point_events->GetTOF1SpacePointArray();
  const std::vector<TOFSpacePoint> tof2_space_points
    = space_point_events->GetTOF2SpacePointArray();

  std::vector<TrackPoints *> track_points;
  track->set_mapper_name(kClassname);

  TLorentzVector deltas[2];

  // FIXME(Lane) Replace hard coded slab to coordinate conversion once
  // Mark Reiner's code has been incorporated into TOF space point code.
  const Detector& tof0 = detectors.find(DetectorPoint::kTOF0)->second;
  std::vector<TOFSpacePoint>::const_iterator tof0_space_point;
  for (tof0_space_point = tof0_space_points.begin();
       tof0_space_point < tof0_space_points.end();
       ++tof0_space_point) {
    TrackPoint * track_point = new TrackPoint();
    PopulateTOFTrackPoint(tof0, tof0_space_point, 4., 10, track_point)
    track_points->push_back(track_point);

    deltas[0] = track_point->get_position();
  }

  const Detector& tof1 = detectors.find(DetectorPoint::kTOF1)->second;
  std::vector<TOFSpacePoint>::const_iterator tof1_space_point;
  for (tof1_space_point = tof1_space_points.begin();
       tof1_space_point < tof1_space_points.end();
       ++tof1_space_point) {
    TrackPoint * track_point = new TrackPoint();
    PopulateTOFTrackPoint(tof1, tof1_space_point, 6., 7, track_point)
    track_points->push_back(track_point);

    deltas[0] = track_point->get_position() - deltas[0];
    deltas[1] = track_point->get_position();
  }

  const Detector& tof2 = detectors.find(DetectorPoint::kTOF2)->second;
  std::vector<TOFSpacePoint>::const_iterator tof2_space_point;
  for (tof2_space_point = tof2_space_points.begin();
       tof2_space_point < tof2_space_points.end();
       ++tof2_space_point) {
    TrackPoint * track_point = new TrackPoint();
    PopulateTOFTrackPoint(tof2, tof2_space_point, 6., 10, track_point)
    track_points->push_back(track_point);

    deltas[1] = track_point->get_position() - deltas[1];
  }

  // Approximate momenta by using tof0/tof1 and tof1/tof2 position deltas
  TLorentzVector momenta[2];
  for (size_t index = 0; index < 2; ++index) {
    double delta_x = deltas[index].X()
    double delta_y = deltas[index].Y();
    double delta_z = deltas[index].Z();
    double delta_t = deltas[index].T();
    double delta_l = std::sqrt(delta_x*delta_x + delta_y*delta_y
                                + delta_z*delta_z);
    double beta = delta_l / delta_t / ::CLHEP::c_light;
    double gamma = 1. / std::sqrt(1 - beta*beta);
    /* FIXME(Lane) Forcing muon PID for now. Should be
    * Particle::ID particle_id = IdentifyParticle(beta);
    * plus code to create multiple hypotheses (i.e. pion/muon ambiguity)
    */
    PID particle_id = PID(Particle::kMuPlus * beam_polarity_);
    double mass = Particle::GetInstance()->GetMass(particle_id);

    momenta[index] = TLorentzVector(mass * delta_x / delta_t,
                                    mass * delta_y / delta_t,
                                    gamma * mass * beta,  // pz ~= p
                                    gamma * mass);
    std::cout << "DEBUG MapCppGlobalRawTracks::LoadTOFTracks(): "
              << "Particle: " << Particle::GetInstance()->GetName(particle_id)
              << "\tbeta: " << beta << "\t 4-momentum: " << momenta[index]
              << std::endl;
  }

  // Set each track point's 4-momentum and add to the track
  Track * track = new Track();
  std::vector<TrackPoint *>::iterator track_point;
  for (track_point = track_points.begin();
       track_point < track_points.end();
       ++track_point) {
    const double z = track_point->get_position().Z();
    if (z <= tof1.plane()) {
      (*track_point)->set_position(momenta[0]);
    } else {
      (*track_point)->set_position(momenta[1]);
    }

    track->AddTrackPoint(*track_point);
  }

  track->SortTrackPointsByZ();
  tof_tracks.push_back(tof_track);
}

void MAUS::MapCppGlobalRawTracks::PopulateTOFTrackPoint(
    const Detector & detector,
    const std::vector<TOFSpacePoint>::const_iterator & tof_space_point,
    const double slab_width,
    const size_t number_of_slabs,
    TrackPoint * track_point) {
  const double max_xy = slab_width * (number_of_slabs - 0.5);

  SpacePoint * space_point = new SpacePoint();

  space_point->set_detector(detector.id());

  // FIXME(Lane) not sure what to put here
  space_point->set_geometry_path("");

  const double x = slab_width * tof_space_point->GetSlabx() - max_xy;
  const double y = slab_width * tof_space_point->GetSlabx() - max_xy;
  const double z = detector.plane();
  const double t = tof_space_point->GetTime();
  TLorentzVector position(x, y, z, t);
  space_point->set_position(position);

  CovarianceMatrix covariances = detector.uncertainties();
  TLorentzVector position_errors(::sqrt(covariances(3, 3)),
                                  ::sqrt(covariances(5, 5)),
                                  0.,
                                  ::sqrt(covariances(1, 1)));
  space_point->set_position_errors(position_errors);

  TLorentzVector momentum_error(::sqrt(covariances(4, 4)),
                                  ::sqrt(covariances(6, 6)),
                                  0.,
                                  ::sqrt(covariances(2, 2)));
  space_point->set_momentum_error(momentum_errors);

  space_point->set_charge(tof_space_point->GetChargeProduct());

  (*track_point) = (*space_point);
  track_point->set_space_point(space_point);
  track_point->set_mapper_name(kClassname);
}

// NOTE(Lane) CONTINUE HERE

void MapCppGlobalRawTracks::LoadSciFiTracks(
    std::map<Detector::ID, Detector>& detectors,
    Json::Value& recon_event,
    std::vector<Track>& sci_fi_tracks) {
  const double z_offset
    = detectors.find(Detector::kTracker1_5)->second.plane();
  std::cout << "DEBUG MapCppGlobalRawTracks::LoadSciFiTracks(): "
            << "z offset: " << z_offset << std::endl;

  Json::Value sci_fi_event = JsonWrapper::GetProperty(
      recon_event,
      "sci_fi_event",
      JsonWrapper::objectValue);
  Json::Value space_points;
  try {
    space_points = JsonWrapper::GetProperty(
        sci_fi_event,
        "spacepoints",
        JsonWrapper::arrayValue);
  } catch (Squeal squeal) {
    // No tracker hits.
    return;
  }

  SciFiSpacePointProcessor deserializer;

  Track track;
  const size_t num_points = space_points.size();
  for (size_t point_index = 0; point_index < num_points; ++point_index) {
    Json::Value space_point_json = JsonWrapper::GetItem(
        space_points,
        point_index,
        JsonWrapper::objectValue);
    const SciFiSpacePoint * space_point = deserializer.JsonToCpp(
        space_point_json);

    const int tracker = space_point->get_tracker();
    const int station = space_point->get_station();
    const Detector::ID detector_id = Detector::ID(
        Detector::kTracker1_1 + 5 * tracker + station - 1);
    const Detector& detector = detectors.find(detector_id)->second;

    const double time = space_point->get_time();
    ThreeVector position = space_point->get_position();

    // FIXME(Lane) Replace MC momentum with official track momentum when that
    // information becomes available.
    ThreeVector momentum;
    const SciFiClusterPArray clusters = space_point->get_channels();
    for (SciFiClusterPArray::const_iterator cluster = clusters.begin();
        cluster < clusters.end();
        ++cluster) {
      ThreeVector true_momentum = (*cluster)->get_true_momentum();
std::cout << "DEBUG MapCppGlobalRawTracks::LoadSciFiTracks(): " << std::endl
          << "\tTrue Momentum: " << true_momentum << std::endl;
      momentum += true_momentum;
    }
    if (clusters.size() > 0) {
      momentum /= clusters.size();
    }
    // smear the MC momentum1.293e+04
    momentum.setX(momentum.x() + ::CLHEP::RandGauss::shoot(0., 3.));
    momentum.setY(momentum.y() + ::CLHEP::RandGauss::shoot(0., 3.));
    momentum.setZ(momentum.z() + ::CLHEP::RandGauss::shoot(0., 20.));
std::cout << "DEBUG MapCppGlobalRawTracks::LoadSciFiTracks(): " << std::endl
          << "\tSmeared Momentum: " << momentum << std::endl;

    // FIXME(Lane) For now assume we've selected only muon tracks and no decays
    const Particle::ID particle_id
      = Particle::ID(Particle::kMuPlus * beam_polarity_);
    const double beta = Beta(particle_id, momentum.mag());
    const double energy = momentum.mag() / beta;

    std::cout << "DEBUG MapCppGlobalRawTracks::LoadSciFiTracks(): " << std::endl
              << "\tSciFi Space Point " << point_index << ":"
              << "\tDetector: " << detector_id
              << "\tStation: " << station << std::endl
              << std::setprecision(10)
              << "\tTime: " << time << "\tEnergy: " << energy << std::endl
              << "\tPosition: " << position << std::endl
              << "\tMomentum: " << momentum << std::endl;

    TrackPoint track_point(time, energy, position.x(), momentum.x(),
                           position.y(), momentum.y(), detector);
    track_point.set_particle_id(particle_id);

    track.push_back(track_point);
  }

  sci_fi_tracks.push_back(track);
}

/* Take an educated guess at the particle ID based on the axial velocity
 * of the particle (beta)
 */
Particle::ID MapCppGlobalRawTracks::IdentifyParticle(const double beta) {
  MAUSGeant4Manager * const simulator = MAUSGeant4Manager::GetInstance();
  MAUSPrimaryGeneratorAction::PGParticle reference_pgparticle
    = simulator->GetReferenceParticle();
  const double reference_momentum = reference_pgparticle.pz;  // good enough
  const double beta_pi = Beta(Particle::kPiPlus, reference_momentum);
  const double beta_mu = Beta(Particle::kMuPlus, reference_momentum);
  const double beta_e = Beta(Particle::kEPlus, reference_momentum);
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

  Particle::ID particle_id = Particle::kNone;
  switch(min_index) {
    case 0: particle_id = Particle::ID(Particle::kPiPlus * beam_polarity_);
            break;
    case 1: particle_id = Particle::ID(Particle::kMuPlus * beam_polarity_);
            break;
    case 2: particle_id = Particle::ID(Particle::kEPlus * beam_polarity_);
            break;
  }

  return particle_id;
}

double MapCppGlobalRawTracks::Beta(Particle::ID pid, const double momentum) {
  const double mass = Particle::GetInstance()->GetMass(pid);
  const double one_over_beta_squared
    = 1+ (mass*mass) / (momentum*momentum);
  return 1. / std::sqrt(one_over_beta_squared);
}

const std::string MapCppGlobalRawTracks::kClassname
  = "MapCppGlobalRawTracks";

}  // namespace MAUS

