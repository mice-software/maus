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
#include "src/common_cpp/DataStructure/GlobalTrack.hh"
#include "src/common_cpp/DataStructure/GlobalTrackPoint.hh"
#include "src/common_cpp/DataStructure/SciFiSpacePoint.hh"
#include "src/common_cpp/DataStructure/TOFEventSpacePoint.hh"
#include "src/common_cpp/DataStructure/ThreeVector.hh"
#include "src/common_cpp/JsonCppProcessors/GlobalTrackProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/SciFiSpacePointProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/TOFEventSpacePointProcessor.hh"
#include "src/common_cpp/Optics/CovarianceMatrix.hh"
#include "src/common_cpp/Recon/Global/DataStructureHelper.hh"
#include "src/common_cpp/Recon/Global/Detector.hh"
#include "src/common_cpp/Recon/Global/Particle.hh"
#include "src/common_cpp/Recon/Global/Track.hh"
#include "src/common_cpp/Recon/Global/TrackPoint.hh"
#include "src/common_cpp/Simulation/MAUSGeant4Manager.hh"
#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"

namespace MAUS {

using MAUS::recon::global::DataStructureHelper;
using MAUS::recon::global::Detector;
using MAUS::recon::global::Particle;
using MAUS::recon::global::Track;
using MAUS::recon::global::TrackPoint;

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

std::string MapCppGlobalRawTracks::process(std::string run_data) {
  // parse the JSON document.
  try {
    run_data_ = Json::Value(JsonWrapper::StringToJson(run_data));

    Json::Value data_acquisition_mode;
    try {
      data_acquisition_mode= JsonWrapper::GetProperty(
          configuration_,
          "data_acquisition_mode",
          JsonWrapper::stringValue);
    } catch (Squeal squeal) {
      data_acquisition_mode = Json::Value("Live");
    }
    if (data_acquisition_mode == "Random") {
      LoadRandomData();
    } else if (data_acquisition_mode == "Simulation") {
      LoadSimulationData();
    } else if (data_acquisition_mode == "Smeared") {
      LoadSmearedData();
    } else if (data_acquisition_mode == "Live") {
      LoadLiveData();
    } else {
      std::string message = "Invalid data acquisition mode: ";
      message += data_acquisition_mode.asString();
      throw(Squeal(Squeal::recoverable,
                  message,
                  "MapCppGlobalRawTracks::process()"));
    }

std::cout << "DEBUG MapCppGlobalRawTracks::process(): "
          << "Loaded " << tracks_.size() << " tracks." << std::endl;
    if (tracks_.size() == 0) {
  /*
      Json::FastWriter writer;
      std::string output = writer.write(run_data_);
      return output;
      throw(Squeal(Squeal::recoverable,
                  "No tracks found.",
                  "MapCppGlobalRawTracks::process()"));
  */
    }
  } catch(Squeal& squee) {
    MAUS::CppErrorHandler::getInstance()->HandleSquealNoJson(squee, kClassname);
  } catch(std::exception& exc) {
    MAUS::CppErrorHandler::getInstance()->HandleStdExcNoJson(exc, kClassname);
  }

  std::cout << "DEBUG MapCppGlobalRawTracks::process(): "
            << "Processing " << tracks_.size()
            << " raw tracks" << std::endl;
  size_t event_index = 0;
  for (std::vector<MAUS::recon::global::Track>::iterator
          raw_track = tracks_.begin();
       raw_track < tracks_.end();
       ++raw_track) {
    std::sort(raw_track->begin(), raw_track->end());  // sort in z-plane order

  std::cout << "DEBUG MapCppGlobalRawTracks::process(): "
            << "Appending a track of size " << raw_track->size()
            << " to raw_tracks" << std::endl;
    Json::Value raw_tracks;
    // Only one raw track per event for now...
    raw_tracks.append(
      DataStructureHelper::GetInstance().TrackToJson(*raw_track));
    run_data_["recon_events"][event_index]["global_event"]["raw_tracks"]
      = raw_tracks;
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
  return true;  // successful
}

void MapCppGlobalRawTracks::LoadRandomData() {
  // Create random track points for TOF0, TOF1, Tracker 1, Tracker 2, and TOF 2
  srand((unsigned)time(NULL));

  std::map<int, Detector> detectors;
  double plane;
  double uncertainty_data[36];
  double position[3], momentum[3];
  Track track;

  // generate mock detector info and random muon detector event data
  for (size_t id = Detector::kTOF0; id <= Detector::kCalorimeter; ++id) {
    // plane = ((double)rand()/(double)RAND_MAX) * 20;  // 0.0 - 20.0 meters
    plane = id * 1.4;  // 0.0 - 20.0 meters
    for (int index = 0; index < 36; ++index) {
      uncertainty_data[index] = static_cast<double>(rand()) / RAND_MAX
                              * 100.0;
    }
    CovarianceMatrix uncertainties(uncertainty_data);
    Detector detector(Detector::ID(id), plane, uncertainties);
    detectors.insert(std::pair<Detector::ID, Detector>(Detector::ID(id),
                                                       detector));

    // skip detectors we're not using
    if ((id == Detector::kCherenkov1) ||
        (id == Detector::kCherenkov2) ||
        (id == Detector::kCalorimeter)) {
      continue;
    }

    for (int coordinate = 0; coordinate < 3; ++coordinate) {
      position[coordinate] = static_cast<double>(rand()) / RAND_MAX
                           * 20.0;  // ns or mm
    }

    for (int coordinate = 0; coordinate < 3; ++coordinate) {
      momentum[coordinate] = static_cast<double>(rand()) / RAND_MAX
                           * 500.0;  // MeV(/c)
    }

    // force the positions to be monotonically increasing
    if (!track.empty()) {
      TrackPoint const & last_point = track.back();
      position[0] += last_point.t();
      position[1] += last_point.x();
      position[2] += last_point.y();
    }

    track.push_back(TrackPoint(position[0], momentum[0]/100.0,
                               position[1], momentum[1]/100.0,
                               position[2], momentum[2],
                               detector));
  }
  tracks_.push_back(track);
}

void MapCppGlobalRawTracks::LoadSimulationData() {
  LoadSimulationData("mc_events");
}

void MapCppGlobalRawTracks::LoadSmearedData() {
  LoadSimulationData("mc_smeared");
}

void MapCppGlobalRawTracks::LoadSimulationData(
    const std::string mc_branch_name) {
  std::map<Detector::ID, Detector> detectors;
  DataStructureHelper::GetInstance().GetDetectorAttributes(
      configuration_, detectors);
  LoadMonteCarloData(mc_branch_name, detectors);
}

/* Currently we just glob all spills together.
 */
void MapCppGlobalRawTracks::LoadMonteCarloData(
    const std::string               branch_name,
    const std::map<Detector::ID, Detector> & detectors) {

  try {
    const Json::Value mc_events = JsonWrapper::GetProperty(
        run_data_, branch_name, JsonWrapper::arrayValue);
    for (Json::Value::UInt particle_index = Json::Value::UInt(0);
         particle_index < mc_events.size();
         ++particle_index) {
      Track track;
      TrackPoint tof0_track_point;
      TrackPoint tof1_track_point;
      const Json::Value mc_event = mc_events[particle_index];
      std::vector<std::string> hit_group_names = mc_event.getMemberNames();
      std::vector<std::string>::const_iterator hit_group_name;
      for (hit_group_name = hit_group_names.begin();
           hit_group_name != hit_group_names.end();
           ++hit_group_name) {
        const Json::Value hit_group = mc_event[*hit_group_name];
        // if (!hit_group.isArray()) {
        if ((*hit_group_name == "primary") ||
            (*hit_group_name == "virtual_hits") ||
            (*hit_group_name == "special_virtual_hits")) {
          // ignore hit groups we don't care about
          continue;
        }

        for (Json::Value::const_iterator hit = hit_group.begin();
              hit != hit_group.end();
              ++hit) {
          const Json::Value particle_id = JsonWrapper::GetProperty(
              *hit, "particle_id", JsonWrapper::intValue);

          double coordinates[6];

          const Json::Value time = JsonWrapper::GetProperty(
              *hit, "time", JsonWrapper::realValue);
          coordinates[0] = time.asDouble();
          const Json::Value position_json = JsonWrapper::GetProperty(
              *hit, "position", JsonWrapper::objectValue);
          const Json::Value x = JsonWrapper::GetProperty(
              position_json, "x", JsonWrapper::realValue);
          coordinates[2] = x.asDouble();
          const Json::Value y = JsonWrapper::GetProperty(
              position_json, "y", JsonWrapper::realValue);
          coordinates[4] = y.asDouble();
          const Json::Value z = JsonWrapper::GetProperty(
              position_json, "z", JsonWrapper::realValue);

          const Json::Value energy = JsonWrapper::GetProperty(
              *hit, "energy", JsonWrapper::realValue);
          coordinates[1] = energy.asDouble();
          const Json::Value momentum_json = JsonWrapper::GetProperty(
              *hit, "momentum", JsonWrapper::objectValue);
          const Json::Value px = JsonWrapper::GetProperty(
              momentum_json, "x", JsonWrapper::realValue);
          coordinates[3] = px.asDouble();
          const Json::Value py = JsonWrapper::GetProperty(
              momentum_json, "y", JsonWrapper::realValue);
          coordinates[5] = py.asDouble();

          TrackPoint track_point(
            time.asDouble(), energy.asDouble(),
            x.asDouble(), px.asDouble(),
            y.asDouble(), py.asDouble(),
            Particle::ID(particle_id.asInt()), z.asDouble());

          const Json::Value channel_id = JsonWrapper::GetProperty(
              *hit, "channel_id", JsonWrapper::objectValue);
          Detector::ID detector_id = Detector::kNone;
          if ((*hit_group_name) == "sci_fi_hits") {
            const Json::Value tracker_number_json = JsonWrapper::GetProperty(
                channel_id, "tracker_number", JsonWrapper::intValue);
            const size_t tracker_number = tracker_number_json.asUInt();
            const Json::Value station_number_json = JsonWrapper::GetProperty(
                channel_id, "station_number", JsonWrapper::intValue);
            const size_t station_number = tracker_number_json.asUInt();
            detector_id = Detector::ID(
              Detector::kTracker1_1 + 5 * tracker_number * station_number - 1);
          } else if ((*hit_group_name) == "tof_hits") {
            const Json::Value station_number_json = JsonWrapper::GetProperty(
                channel_id, "station_number", JsonWrapper::intValue);
            const size_t station_number = station_number_json.asUInt();
            const Json::Value plane_json = JsonWrapper::GetProperty(
                channel_id, "plane", JsonWrapper::intValue);
            switch (station_number) {
              case 0: {
                detector_id = Detector::ID(Detector::kTOF0);
                tof0_track_point = track_point;
                break;
              }
              case 1: {
                detector_id = Detector::ID(Detector::kTOF1);
                tof1_track_point = track_point;
                break;
              }
              case 2: {
                detector_id = Detector::ID(Detector::kTOF2);
                break;
              }
            }
          }
          track_point.set_detector_id(detector_id);
          std::map<Detector::ID, Detector>::const_iterator detector
            = detectors.find(detector_id);
          track_point.set_uncertainties(detector->second.uncertainties());

            track.push_back(track_point);
std::cout << "DEBUG MapCppGlobalRawTracks::LoadMonteCarloData: "
          << "pushed the following hit:" << std::endl << track_point
          << std::endl;
        }
      }

/*
      double trigger_time = tof1_track_point.t();
std::cout << "DEBUG MapCppGlobalRawTracks::LoadMonteCarloData: "
      << "Trigger Time: " << trigger_time << std::endl;
      std::vector<TrackPoint>::iterator track_point;
      for (track_point = track.begin();
           track_point < track.end();
           ++track_point) {
std::cout << "DEBUG MapCppGlobalRawTracks::LoadMonteCarloData: "
      << "Initial Track Point Time: " << (*track_point)[0] << std::endl;
        (*track_point)[0] = (*track_point)[0] - trigger_time;
std::cout << "DEBUG MapCppGlobalRawTracks::LoadMonteCarloData: "
      << "Final Track Point Time: " << (*track_point)[0] << std::endl;
      }
*/

      double delta_x = tof1_track_point.x() - tof0_track_point.x();
      double delta_y = tof1_track_point.y() - tof0_track_point.y();
      double delta_z = tof1_track_point.z() - tof0_track_point.z();
      double distance = std::sqrt(delta_x*delta_x + delta_y*delta_y
                                  + delta_z*delta_z);
      double delta_t = tof1_track_point.time() - tof0_track_point.time();
      double velocity = distance / delta_t;
      double beta = velocity / ::CLHEP::c_light;
std::cout << "DEBUG MapCppGlobalRawTracks::LoadMonteCarloData: "
      << "beta:" << beta << std::endl;

      // Make a cut on beta = P/E that excludes electrons between about
      // 3 MeV to 530 MeV
      if (beta < 0.98) {
        // TODO(plan1@hawk.iit.edu) do an insertion sort instead of
        // sorting afterwards
        std::sort(track.begin(), track.end());  // sort in z-plane order
        tracks_.push_back(track);
std::cout << "DEBUG MapCppGlobalRawTracks::LoadMonteCarloData: "
          << "pushed the following track:" << std::endl << track
          << std::endl;
      }
    }
  } catch(Squeal& squee) {
    run_data_ = MAUS::CppErrorHandler::getInstance()->HandleSqueal(
        run_data_, squee,
        "MAUS::MapCppGlobalRawTracks::LoadMonteCarloData()");
  } catch(std::exception& exc) {
    run_data_ = MAUS::CppErrorHandler::getInstance()->HandleStdExc(
        run_data_, exc,
        "MAUS::MapCppGlobalRawTracks::LoadMonteCarloData()");
  }
}


/** LoadLiveData - load data provided by the DAQ system or via digitized MC
 *
 *  Pseudocode:
 *    1) Deserialize TOF and SciFi events
 *    2) Assemble all raw track permutations
 *    3) Prune any raw tracks that are unlickly or impossible
 *    4) Serialize raw tracks
 */
void MapCppGlobalRawTracks::LoadLiveData() {
  std::map<Detector::ID, Detector> detectors;
  DataStructureHelper::GetInstance().GetDetectorAttributes(
      configuration_, detectors);

  Json::Value recon_events = JsonWrapper::GetProperty(
      run_data_,
      "recon_events",
      JsonWrapper::arrayValue);
  for (size_t event_index = 0;
        event_index < recon_events.size();
        ++event_index) {
    Json::Value recon_event = JsonWrapper::GetItem(
        recon_events,
        event_index,
        JsonWrapper::objectValue);

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
}

void MapCppGlobalRawTracks::LoadTOFTracks(
    std::map<Detector::ID, Detector>& detectors,
    Json::Value& recon_event,
    std::vector<Track>& tof_tracks) {
  Json::Value tof_event = JsonWrapper::GetProperty(
      recon_event,
      "tof_event",
      JsonWrapper::objectValue);
  Json::Value tof_space_points_json = JsonWrapper::GetProperty(
      tof_event,
      "tof_space_points",
      JsonWrapper::objectValue);
  TOFEventSpacePointProcessor deserializer;
  const TOFEventSpacePoint * tof_space_points = deserializer.JsonToCpp(
      tof_space_points_json);

  // FIXME(Lane) Ignoring multiple hits.
  std::cout << "DEBUG MapCppGlobalRawTracks::LoadTOFTracks(): " << std::endl
            << "\tNumber of TOF0 hits: "
            << tof_space_points->GetTOF0SpacePointArraySize() << std::endl
            << "\tNumber of TOF1 hits: "
            << tof_space_points->GetTOF1SpacePointArraySize() << std::endl;
  if ((tof_space_points->GetTOF0SpacePointArraySize() < 1) ||
      (tof_space_points->GetTOF1SpacePointArraySize() < 1)) {
    return;
  }

  Track tof_track;

  // Use TOF0 and TOF1 to calculate the velocity of the particle
  const TOFSpacePoint tof0_space_point
      = tof_space_points->GetTOF0SpacePointArrayElement(0);
  const TOFSpacePoint tof1_space_point
      = tof_space_points->GetTOF1SpacePointArrayElement(0);
  const Detector& tof0 = detectors.find(Detector::kTOF0)->second;
  const Detector& tof1 = detectors.find(Detector::kTOF1)->second;

  // FIXME(Lane) Replace hard coded slab to coordinate conversion once
  // Mark Reiner's code has been incorporated into TOF space point code.
  const double tof0_t = tof0_space_point.GetTime();
  const double tof1_t = tof1_space_point.GetTime();
  const double tof0_x = -18. + 2. * tof0_space_point.GetSlabx();
  const double tof1_x = -18. + 3. * tof1_space_point.GetSlabx();
  const double tof0_y = -18. + 2. * tof0_space_point.GetSlaby();
  const double tof1_y = -18. + 3. * tof1_space_point.GetSlaby();
  const double tof0_z = tof0.plane();
  const double tof1_z = tof1.plane();

  double delta_t = tof1_t - tof0_t;
  double delta_x = tof1_x - tof0_x;
  double delta_y = tof1_y - tof0_y;
  double delta_z = tof1_z - tof0_z;
  double delta_l = std::sqrt(delta_x*delta_x + delta_y*delta_y
                              + delta_z*delta_z);
  double beta = delta_l / delta_t / ::CLHEP::c_light;
  double gamma = 1. / std::sqrt(1 - beta*beta);
  /* FIXME(Lane) Forcing muon PID for now. Should be
   * Particle::ID particle_id = IdentifyParticle(beta);
   * plus code to create multiple hypotheses (i.e. pion/muon ambiguity)
   */
  Particle::ID particle_id = Particle::ID(Particle::kMuPlus * beam_polarity_);

  double mass = Particle::GetInstance()->GetMass(particle_id);
  double px = mass * delta_x / delta_t;
  double py = mass * delta_y / delta_t;
  double energy = gamma * mass;
  std::cout << "DEBUG MapCppGlobalRawTracks::LoadTOFTracks(): "
            << "Particle: " << Particle::GetInstance()->GetName(particle_id)
            << "\tbeta: " << beta << "\t energy: " << energy << std::endl;

  TrackPoint tof0_track_point(tof0_space_point.GetTime(),
                              energy,
                              tof0_x,
                              px,
                              tof0_y,
                              py,
                              tof0);
  tof0_track_point.set_particle_id(particle_id);
  tof_track.push_back(tof0_track_point);

  TrackPoint tof1_track_point(tof1_space_point.GetTime(),
                              energy,
                              tof1_x,
                              px,
                              tof1_y,
                              py,
                              tof1);
  tof1_track_point.set_particle_id(particle_id);
  tof_track.push_back(tof1_track_point);

  if (tof_space_points->GetTOF2SpacePointArraySize() > 1) {
    const TOFSpacePoint tof2_space_point
        = tof_space_points->GetTOF2SpacePointArrayElement(0);
    const Detector& tof2 = detectors.find(Detector::kTOF2)->second;

    const double tof2_t = tof2_space_point.GetTime();
    const double tof2_x = -27. + 3. * tof2_space_point.GetSlabx();
    const double tof2_y = -27. + 3. * tof2_space_point.GetSlaby();
    const double tof2_z = tof2.plane();

    delta_t = tof2_t - tof1_t;
    delta_x = tof2_x - tof1_x;
    delta_y = tof2_y - tof1_y;
    delta_z = tof2_z - tof1_z;
    delta_l = std::sqrt(delta_x*delta_x + delta_y*delta_y
                        + delta_z*delta_z);
    beta = delta_l / delta_t / ::CLHEP::c_light;
    gamma = 1. / std::sqrt(1 - beta*beta);
    particle_id = IdentifyParticle(beta);

    mass = Particle::GetInstance()->GetMass(particle_id);
    energy = gamma * mass;
    px = mass * delta_x / delta_t;
    py = mass * delta_y / delta_t;
std::cout << "DEBUG MapCppGlobalRawTracks::LoadTOFTracks(): TOF2 info..." << std::endl
        << "dx: " << delta_x << "\tdy: " << delta_y << "\tdz: " << delta_z
        << "\tbeta: " << beta << std::endl
        << "\tenergy: " << energy << "\tpx: " << px << "\tpy: " << py << std::endl;

    TrackPoint tof2_track_point(tof2_space_point.GetTime(),
                                energy,
                                tof2_x,
                                px,
                                tof2_y,
                                py,
                                tof2);
    tof2_track_point.set_particle_id(particle_id);
    tof_track.push_back(tof2_track_point);
  }

  std::sort(tof_track.begin(), tof_track.end());  // sort in z-plane order
  tof_tracks.push_back(tof_track);

  delete tof_space_points;
}

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

