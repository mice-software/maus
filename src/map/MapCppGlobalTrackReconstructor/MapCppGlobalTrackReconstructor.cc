/* This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
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
 * along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
 /* Author: Peter Lane
 */

#include "src/map/MapCppGlobalTrackReconstructor/MapCppGlobalTrackReconstructor.hh"

// C++
#include <algorithm>
#include <map>
#include <vector>
#include <string>

// External
#include "TMinuit.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include "json/json.h"

// Legacy/G4MICE
#include "Interface/Squeal.hh"
#include "Interface/dataCards.hh"

// MAUS
#include "src/common_cpp/JsonCppProcessors/TOFSpacePointProcessor.hh"
##include "src/common_cpp/Optics/CovarianceMatrix.hh"
#include "src/common_cpp/Optics/LinearApproximationOpticsModel.hh"
#include "src/common_cpp/Optics/PolynomialOpticsModel.hh"
#include "src/common_cpp/Recon/Global/Detector.hh"
#include "src/common_cpp/Recon/Global/MinuitTrackFitter.hh"
#include "src/common_cpp/Recon/Global/Particle.hh"
#include "src/common_cpp/Recon/Global/ReconstructionInput.hh"
#include "src/common_cpp/Recon/Global/Track.hh"
#include "src/common_cpp/Recon/Global/TrackFitter.hh"
#include "src/common_cpp/Recon/Global/TrackPoint.hh"
#include "src/common_cpp/Simulation/MAUSGeant4Manager.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"

namespace MAUS {

using MAUS::recon::global::Detector;
using MAUS::recon::global::MinuitTrackFitter;
using MAUS::recon::global::Particle;
using MAUS::recon::global::ReconstructionInput;
using MAUS::recon::global::Track;
using MAUS::recon::global::TrackFitter;
using MAUS::recon::global::TrackPoint;

MapCppGlobalTrackReconstructor::MapCppGlobalTrackReconstructor()
    : optics_model_(NULL), track_fitter_(NULL) {
}

MapCppGlobalTrackReconstructor::~MapCppGlobalTrackReconstructor() {
  if (optics_model_ != NULL) {
    delete optics_model_;
  }

  if (track_fitter_ != NULL) {
    delete track_fitter_;
  }
}

bool MapCppGlobalTrackReconstructor::birth(std::string configuration) {
  // parse the JSON document.
  try {
    configuration_ = JsonWrapper::StringToJson(configuration);
    Json::Value physics_processes = configuration_["physics_processes"];
    if ((physics_processes != Json::Value("mean_energy_loss"))
        && (physics_processes != Json::Value("none"))) {
      throw(Squeal(Squeal::nonRecoverable,
                  "The \"physics_processes\" configuration variable should be "
                  "set to \"mean_energy_loss\" or \"none\" to avoid collisions "
                  "of the test particles with walls.",
                  "MAUS::MapCppGlobalTrackReconstructor::birth()"));
    }
    SetupOpticsModel();
    SetupTrackFitter();
  } catch(Squeal& squee) {
    MAUS::CppErrorHandler::getInstance()->HandleSquealNoJson(
      squee, MapCppGlobalTrackReconstructor::kClassname);
    return false;
  } catch(std::exception& exc) {
    MAUS::CppErrorHandler::getInstance()->HandleStdExcNoJson(
      exc, MapCppGlobalTrackReconstructor::kClassname);
    return false;
  }

  return true;  // Sucessful parsing
}

void MapCppGlobalTrackReconstructor::SetupOpticsModel() {
fprintf(stdout, "CHECKPOINT: SetupOpticsModel() 0\n");
fflush(stdout);
  Json::Value optics_model_names = JsonWrapper::GetProperty(
      configuration_,
      "recon_optics_models",
      JsonWrapper::arrayValue);
  Json::Value optics_model_name = JsonWrapper::GetProperty(
      configuration_,
      "recon_optics_model",
      JsonWrapper::stringValue);
  size_t model;
  for (model = 0; model < optics_model_names.size(); ++model) {
    if (optics_model_name == optics_model_names[model]) {
      break;  // leave the current index into optics_model_names in model
    }
  }

fprintf(stdout, "CHECKPOINT: SetupOpticsModel() 1\n");
fflush(stdout);
  switch (model) {
    case 0: {
      // "Differentiating"
      /* TODO(plane1@hawk.iit.edu)
      optics_model_ = new DifferentiatingOpticsModel();
      */
      throw(Squeal(Squeal::nonRecoverable,
                   "DifferentiatingOpticsModel is not yet implemented.",
                   "MapCppGlobalTrackReconstructor::process()"));
      break;
    }
    case 1: {
      // "Integrating"
      /* TODO(plane1@hawk.iit.edu)
      optics_model_ = new IntegratingOpticsModel();
      */
      throw(Squeal(Squeal::nonRecoverable,
                   "IntegratingOpticsModel is not yet implemented.",
                   "MapCppGlobalTrackReconstructor::SetupOpticsModel()()"));
      break;
    }
    case 2: {
fprintf(stdout, "CHECKPOINT SetupOpticsModel() 1.1c\n");
fflush(stdout);
     optics_model_ = new PolynomialOpticsModel(configuration_);
      break;
    }
    case 3: {
      // "Runge Kutta"
      /* TODO(plane1@hawk.iit.edu)
      optics_model_ = new RungeKuttaOpticsModel();
      */
      throw(Squeal(Squeal::nonRecoverable,
                   "RungeKuttaOpticsModel is not yet implemented.",
                   "MapCppGlobalTrackReconstructor::SetupOpticsModel()()"));
      break;
    }
    case 4: {
      // "Linear Approximation"
fprintf(stdout, "CHECKPOINT SetupOpticsModel() 1.1e\n");
fflush(stdout);
     optics_model_ = new LinearApproximationOpticsModel(configuration_);
      break;
    }
    default: {
      std::string message("Unsupported optics model \"");
      message += optics_model_name.asString();
      message += std::string("\" in recon configuration.");
      throw(Squeal(Squeal::nonRecoverable,
                   message.c_str(),
                   "MapCppGlobalTrackReconstructor::SetupOpticsModel()()"));
    }
  }
fprintf(stdout, "CHECKPOINT: SetupOpticsModel() 2\n");
fflush(stdout);
  optics_model_->Build();

  // make sure we don't override mc_events in the output data structure
  configuration_.removeMember("mc_events");
fprintf(stdout, "CHECKPOINT: SetupOpticsModel() 3\n");
fflush(stdout);
}

void MapCppGlobalTrackReconstructor::SetupTrackFitter() {
  Json::Value fitter_names = JsonWrapper::GetProperty(
      configuration_,
      "recon_track_fitters",
      JsonWrapper::arrayValue);
  Json::Value fitter_name = JsonWrapper::GetProperty(
      configuration_,
      "recon_track_fitter",
      JsonWrapper::stringValue);
  size_t fitter;
  for (fitter = 0; fitter < fitter_names.size(); ++fitter) {
    if (fitter_name == fitter_names[fitter]) {
      break;  // leave the current index into fitter_names in fitter
    }
  }

  switch (fitter) {
    case 0: {
      // Minuit
      double start_plane = optics_model_->first_plane();
      track_fitter_ = new MinuitTrackFitter(*optics_model_, start_plane);
      break;
    }
    case 1: {
      // Kalman Filter
      /* TODO(plane1@hawk.iit.edu)
      track_fitter_ = new KalmanFilterTrackFitter(optics_model_);
      */
      throw(Squeal(Squeal::nonRecoverable,
                   "KalmanFilterTrackFitter is not yet implemented.",
                   "MapCppGlobalTrackReconstructor::SetupTrackFitter()"));
      break;
    }
    case 2: {
      // TOF
      /* TODO(plane1@hawk.iit.edu)
      track_fitter_ = new TOFTrackFitter(optics_model_);
      */
      throw(Squeal(Squeal::nonRecoverable,
                   "TOFTrackFitter is not yet implemented.",
                   "MapCppGlobalTrackReconstructor::SetupTrackFitter()"));
      break;
    }
    default: {
      std::string message("Unsupported track fitter \"");
      message += fitter_name.asString();
      message += std::string("\" in recon configuration.");
      throw(Squeal(Squeal::nonRecoverable,
                   message.c_str(),
                   "MapCppGlobalTrackReconstructor::SetupTrackFitter()"));
    }
  }
}

std::string MapCppGlobalTrackReconstructor::process(std::string run_data) {
  // parse the JSON document.
  try {
    run_data_ = Json::Value(JsonWrapper::StringToJson(run_data));

    // Populate ReconstructionInput instance from JSON data
    Json::Value data_acquisition_mode_names = JsonWrapper::GetProperty(
        configuration_,
        "data_acquisition_modes",
        JsonWrapper::arrayValue);
    Json::Value data_acquisition_mode = JsonWrapper::GetProperty(
        configuration_,
        "data_acquisition_mode",
        JsonWrapper::stringValue);
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
                  "MapCppGlobalTrackReconstructor::process()"));
    }
  } catch(Squeal& squee) {
    MAUS::CppErrorHandler::getInstance()->HandleSquealNoJson(squee, kClassname);
  } catch(std::exception& exc) {
    MAUS::CppErrorHandler::getInstance()->HandleStdExcNoJson(exc, kClassname);
  }

std::cout << "DEBUG MapCppGlobalTrackReconstructor::process(): "
          << "Loaded " << tracks_.size() << " tracks." << std::endl;
  if (tracks_.size() == 0) {
/*
    Json::FastWriter writer;
    std::string output = writer.write(run_data_);
    return output;
*/
    throw(Squeal(Squeal::recoverable,
                 "Null recon input.",
                 "MapCppGlobalTrackReconstructor::process()"));
  }

  // TODO(plane1@hawk.iit.edu) Implement Kalman Filter and TOF track fitters
  //  in addition to Minuit, and select between them based on the configuration

  // associate tracks with individual particles
  // std::vector<MAUS::recon::global::Track> tracks;
  // CorrelateTrackPoints(tracks);


  MAUSGeant4Manager * const simulator = MAUSGeant4Manager::GetInstance();
  MAUSPrimaryGeneratorAction::PGParticle reference_pgparticle
    = simulator->GetReferenceParticle();
  Particle::ID particle_id = Particle::ID(reference_pgparticle.pid);

  Json::Value global_tracks;

  // Find the best fit track for each particle traversing the lattice
  size_t track_count = 0;
  for (std::vector<MAUS::recon::global::Track>::const_iterator
          measured_track = tracks_.begin();
       measured_track < tracks_.end();
       ++measured_track) {
    MAUS::recon::global::Track best_fit_track(particle_id);

    track_fitter_->Fit(*measured_track, best_fit_track);
    // TODO(plane1@hawk.iit.edu) Reconstruct track at the desired locations
    //  specified in the configuration.

std::cout << "DEBUG MapCppGlobalTrackReconstructor::process(): "
          << "Appending a best fit track of size " << best_fit_track.size()
          << " to global_tracks" << std::endl;
    global_tracks.append(TrackToJson(best_fit_track));
    track_count += best_fit_track.size() - 1;
  }

  // TODO(plane1@hawk.iit.edu) Update the run data with recon results.
  run_data_["global_tracks"] = global_tracks;

  // pass on the updated run data to the next map in the workflow
  Json::FastWriter writer;
  std::string output = writer.write(run_data_);

  return output;
}

void MapCppGlobalTrackReconstructor::LoadRandomData() {
  // Create random track points for TOF0, TOF1, Tracker 1, Tracker 2, and TOF 2
  srand((unsigned)time(NULL));

  std::map<int, Detector> detectors;
  double plane;
  double uncertainty_data[36];
  double position[3], momentum[3];
  Track track;

  // generate mock detector info and random muon detector event data
  for (size_t id = Detector::kTOF0_1; id <= Detector::kCalorimeter; ++id) {
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

void MapCppGlobalTrackReconstructor::LoadSimulationData() {
  LoadSimulationData("mc_events");
}

void MapCppGlobalTrackReconstructor::LoadSmearedData() {
  LoadSimulationData("mc_smeared");
}

void MapCppGlobalTrackReconstructor::LoadSimulationData(
    const std::string mc_branch_name) {
  std::map<Detector::ID, Detector> detectors;
  LoadDetectorConfiguration(detectors);
  LoadMonteCarloData(mc_branch_name, detectors);
}

void MapCppGlobalTrackReconstructor::LoadDetectorConfiguration(
    std::map<Detector::ID, Detector> & detectors) {
  try {
    // FIXME(plane1@hawk.iit.edu) Once the detector groups provide this
    // information this will need to be changed
    Json::Value detector_attributes_json = JsonWrapper::GetProperty(
        configuration_, "recon_detector_attributes",
        JsonWrapper::arrayValue);

    // *** Get detector info ***
    const Json::Value::UInt detector_count = detector_attributes_json.size();
    for (Json::Value::UInt index = 0; index < detector_count; ++index) {
      const Json::Value detector_json = detector_attributes_json[index];
      const Json::Value id_json = JsonWrapper::GetProperty(
          detector_json, "id", JsonWrapper::intValue);
      const Detector::ID id = Detector::ID(id_json.asInt());

      const Json::Value plane_json = JsonWrapper::GetProperty(
          detector_json, "plane", JsonWrapper::realValue);
      const double plane = plane_json.asDouble();

      const Json::Value uncertainties_json = JsonWrapper::GetProperty(
          detector_json, "uncertainties", JsonWrapper::arrayValue);
      const CovarianceMatrix uncertainties
          = GetJsonCovarianceMatrix(uncertainties_json);

      const Detector detector(id, plane, uncertainties);
      detectors.insert(std::pair<Detector::ID, Detector>(id, detector));
    }
  } catch(Squeal& squee) {
    run_data_ = MAUS::CppErrorHandler::getInstance()->HandleSqueal(
        run_data_, squee,
        "MAUS::MapCppGlobalTrackReconstructor::LoadMonteCarloData()");
  } catch(std::exception& exc) {
    run_data_ = MAUS::CppErrorHandler::getInstance()->HandleStdExc(
        run_data_, exc,
        "MAUS::MapCppGlobalTrackReconstructor::LoadMonteCarloData()");
  }
}

/* Currently we just glob all spills together.
 */
void MapCppGlobalTrackReconstructor::LoadMonteCarloData(
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
            // FIXME detector IDs have changed
            detector_id = Detector::ID(tracker_number + 4 + station_number);
          } else if ((*hit_group_name) == "tof_hits") {
            const Json::Value station_number_json = JsonWrapper::GetProperty(
                channel_id, "station_number", JsonWrapper::intValue);
            const size_t station_number = station_number_json.asUInt();
            const Json::Value plane_json = JsonWrapper::GetProperty(
                channel_id, "plane", JsonWrapper::intValue);
            const size_t plane = plane_json.asUInt();
            switch (station_number) {
              case 0: {
                detector_id = Detector::ID(Detector::kTOF0_1 + plane);
                tof0_track_point = track_point;
                break;
              }
              case 1: {
                detector_id = Detector::ID(Detector::kTOF1_1 + plane);
                tof1_track_point = track_point;
                break;
              }
              case 2: {
                detector_id = Detector::ID(Detector::kTOF2_2 + plane);
                break;
              }
            }
          }
          track_point.set_detector_id(detector_id);
          std::map<Detector::ID, Detector>::const_iterator detector
            = detectors.find(detector_id);
          track_point.set_uncertainties(detector->second.uncertainties());

            track.push_back(track_point);
std::cout << "DEBUG MapCppGlobalTrackReconstructor::LoadMonteCarloData: "
          << "pushed the following hit:" << std::endl << track_point
          << std::endl;
        }
      }

/*
      double trigger_time = tof1_track_point.t();
std::cout << "DEBUG MapCppGlobalTrackReconstructor::LoadMonteCarloData: "
      << "Trigger Time: " << trigger_time << std::endl;
      std::vector<TrackPoint>::iterator track_point;
      for (track_point = track.begin();
           track_point < track.end();
           ++track_point) {
std::cout << "DEBUG MapCppGlobalTrackReconstructor::LoadMonteCarloData: "
      << "Initial Track Point Time: " << (*track_point)[0] << std::endl;
        (*track_point)[0] = (*track_point)[0] - trigger_time;
std::cout << "DEBUG MapCppGlobalTrackReconstructor::LoadMonteCarloData: "
      << "Final Track Point Time: " << (*track_point)[0] << std::endl;
      }
*/

      double delta_x = tof1_track_point.x() - tof0_track_point.x();
      double delta_y = tof1_track_point.y() - tof0_track_point.y();
      double delta_z = tof1_track_point.z() - tof0_track_point.z();
      double distance = ::sqrt(delta_x*delta_x + delta_y*delta_y
                                + delta_z*delta_z);
      double delta_t = tof1_track_point.time() - tof0_track_point.time();
      double velocity = distance / delta_t;
      double beta = velocity / ::CLHEP::c_light;
std::cout << "DEBUG MapCppGlobalTrackReconstructor::LoadMonteCarloData: "
      << "beta:" << beta << std::endl;

      // Make a cut on beta = P/E that excludes electrons between about
      // 3 MeV to 530 MeV
      if (beta < 0.98) {
        // TODO(plan1@hawk.iit.edu) do an insertion sort instead of
        // sorting afterwards
        std::sort(track.begin(), track.end());  // sort in chronological order
        tracks_.push_back(track);
std::cout << "DEBUG MapCppGlobalTrackReconstructor::LoadMonteCarloData: "
          << "pushed the following track:" << std::endl << track
          << std::endl;
      }
    }
  } catch(Squeal& squee) {
    run_data_ = MAUS::CppErrorHandler::getInstance()->HandleSqueal(
        run_data_, squee,
        "MAUS::MapCppGlobalTrackReconstructor::LoadMonteCarloData()");
  } catch(std::exception& exc) {
    run_data_ = MAUS::CppErrorHandler::getInstance()->HandleStdExc(
        run_data_, exc,
        "MAUS::MapCppGlobalTrackReconstructor::LoadMonteCarloData()");
  }
}

/*
void ExtractTOFSpacePoints(
    const Json::Value& tof_space_points,
    std::vector<MAUS::TOFSpacePoint>& tof0_space_points,
    std::vector<MAUS::TOFSpacePoint>& tof1_space_points,
    std::vector<MAUS::TOFSpacePoint>& tof2_space_points) {
  stations.push_back(JsonWrapper::GetProperty(tof_space_points,
                                              "tof0",
                                              JsonWrapper::arrayValue));
  stations.push_back(JsonWrapper::GetProperty(tof_space_points,
                                              "tof1",
                                              JsonWrapper::arrayValue));

  for (unsigned int station = 0; station <=2; ++station) {
    std::stringstream station_name
    station_name << "tof" << station;
    Json::Value space_points = JsonWrapper::GetProperty(
        tof_space_points,
        station_name.str(),
        JsonWrapper::arrayValue);
    std::vector<MAUS::TOFSpacePoint>& tofn_space_points;
    switch {
      case 0: tofn_space_points = tof0_space_points; break;
      case 1: tofn_space_points = tof1_space_points; break;
      case 2: tofn_space_points = tof2_space_points; break;
    }

    for (size_t point_index = 0;
          point_index < space_points.size();
          ++point_index) {
      Json::Value space_point_json = JsonWrapper::GetItem(
          space_points,
          point_index,
          JsonWrapper::objectValue);
      TOFSpacePoint space_point;

      Json::Value time_json = JsonWrapper::GetProperty(
          _json,
          "time",
          JsonWrapper::realValue);
      const double time = time_json.asDouble();

      Json::Value station = JsonWrapper::GetProperty(
          _json,
          "station",
          JsonWrapper::uintValue);
      unsigned int tof_station = station.asUInt();

      Json::Value x_slab = JsonWrapper::GetProperty(
          _json,
          "slabX",
          JsonWrapper::uintValue);
      space_point.SetSlabx(y_slab.asInt());
      double max_x_value = 0.;
      double x_slab_half_width = 0.;
      switch (tof_station) {
        // 10 4cm-wide slabs between -18cm and +18cm
        case 0:
          max_x_value = -180.;
          x_slab_half_width = 2.;
          break;
        // 7 6cm-wide slabs between -18cm and +18cm
        case 1:
          max_x_value = -180.;
          x_slab_half_width = 3.;
          break;
        // 10 6cm-wide slabs between -27cm and +27cm
        case 2:
          max_x_value = -270.;
          x_slab_half_width = 3.;
          break;
      }
      const double x = max_x_value + x_slab_half_width * x_slab.asUInt();

      Json::Value y_slab = JsonWrapper::GetProperty(
          _json,
          "slabY",
          JsonWrapper::uintValue);
      double max_y_value = 0.;
      double y_slab_half_width = 0.;
      switch (tof_station) {
        // 10 4cm-wide slabs between -18cm and +18cm
        case 0:
          max_y_value = -180.;
          y_slab_half_width = 2.;
          break;
        // 7 6cm-wide slabs between -18cm and +18cm
        case 1:
          max_y_value = -180.;
          y_slab_half_width = 3.;
          break;
        // 10 6cm-wide slabs between -27cm and +27cm
        case 2:
          max_y_value = -270.;
          y_slab_half_width = 3.;
          break;
      }
      const double y = max_y_value + y_slab_half_width * y_slab.asUInt();

      tofn_space_points.
    }
  }
}
*/

void MapCppGlobalTrackReconstructor::LoadTOFTracks(
    std::map<Detector::ID, Detector>& detectors,
    Json::Value& event,
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
  /*
  std::vector<MAUS::TOFSpacePoint> tof0_space_points;
  std::vector<MAUS::TOFSpacePoint> tof1_space_points;
  std::vector<MAUS::TOFSpacePoint> tof2_space_points;
  ExtractTOFSpacePoints(tof_space_points,
                        tof0_space_points,
                        tof1_space_points,
                        tof2_space_points);
  */
  MAUSGeant4Manager * const simulator = MAUSGeant4Manager::GetInstance();
  MAUSPrimaryGeneratorAction::PGParticle reference_pgparticle
    = simulator->GetReferenceParticle();
  const size_t num_space_points
      = tof_space_points->GetTOF0SpacePointArraySize();
  // TODO(Lane) match up tof0, tof1, and tof2 space points based on DAQ window
  // For now assume an equal number of space points for each TOF detector
  for (size_t index = 0; index < tof_space_points->GetTOF0SpacePointArraySize();
       ++index) {
    // Use TOF0 and TOF1 to calculate the velocity of the particle
    const TOFSpacePoint tof0_space_point
        = tof_space_points->GetTOF0SpacePointArrayElement(index);
    const TOFSpacePoint tof1_space_point
        = tof_space_points->GetTOF0SpacePointArrayElement(index);
    const double delta_t
        = tof1_space_point.GetTime() - tof0_space_point.GetTime();
    const Detector& tof0 = detectors.find(Detector::kTOF0)->second;
    const Detector& tof1 = detectors.find(Detector::kTOF1)->second;
    const double delta_z = tof1.plane() - tof0.plane();
    const double beta = delta_z / delta_t / ::CLHEP::c_light;
    const double gamma = 1. / ::sqrt(1 - beta*beta);

    // Assume particle is a muon and calculate energy and momentum.
    const double mass = Particle::GetInstance()->GetMass(Particle::kMuMinus);
    const double energy = gamma * mass;
    const double momentum = beta * energy;

    // Ignore if the energy or momentum inconsistant with a muon.
    const double reference_energy = reference_pgparticle.energy;
    const double reference_momentum = reference_pgparticle.pz;  // good enough
    // m_mu / m_pi = 0.7572 and m_mu / m_e = 206.8
    const double energy_ratio = reference_energy / energy;
    const double momentum_ratio = reference_momentum / momentum;
    if ((energy_ratio < 0.8) || (energy_ratio > 10.) ||
        (momentum_ratio < 0.8) || (momentum_ratio > 10.)) {
      return;
    }

    // Otherwise, populate TrackPoints for each space point
  }
}

void MapCppGlobalTrackReconstructor::LoadLiveData() {
  Json::Value event_type = JsonWrapper::GetProperty(run_data_,
                                                    "daq_event_type",
                                                    JsonWrapper::stringValue);
  std::map<Detector::ID, Detector> detectors;
  LoadDetectorConfiguration(detectors);

  if (event_type == "physics_event") {
    Json::Value recon_events = JsonWrapper::GetProperty(
        run_data_,
        "recon_events",
        JsonWrapper::arrayValue);
    for (size_t event_index = 0; event < recon_events.size() ++event_index) {
      Json::Value recon_event = JsonWrapper::GetItem(
          recon_events,
          event_index,
          JsonWrapper::objectValue);
      std::vector<Track> tof_tracks;
      LoadTOFTracks(detectors, recon_event, tof_tracks);
    }
  }
}


/* We assume that muons traverse the MICE lattice one at a time, so we can
 * sort out tracks based on the requirement that hits will occur in the same
 * order as the detector positions.
 */
/*
void MapCppGlobalTrackReconstructor::CorrelateTrackPoints(
    std::vector<Track> & tracks) {
  const std::vector<TrackPoint> & track_points
      = recon_input_->events();

  if (track_points.size() == 0) {
    return;
  }
std::cout << "DEBUG MapCppGlobalTrackReconstructor::CorrelateTrackPoints: "
          << "correlating " << track_points.size() << " track points ..."
          << std::endl;

  const double muon_mass
    = Particle::GetInstance()->GetMass(Particle::kMuMinus);

  // TODO(plane1@hawk.iit.edu) Handle multiple particles per spill, still
  // assuming the particles navigate the MICE lattice one at a time
  Track track;
  double last_z = track_points[0].z();
std::cout << "DEBUG MapCppGlobalTrackReconstructor::CorrelateTrackPoints: "
          << "correlating the following track point:" << std::endl
          << *event << std::endl;
    if (event->energy() < muon_mass) {
      // reject particles with energy < muon mass (electrons)
      continue;
std::cout << "DEBUG MapCppGlobalTrackReconstructor::CorrelateTrackPoints: "
          << "rejected particle with energy < muon mass" << std::endl;
    } else if (event->z() < last_z) {
      // A new track begins when a hit is upstream from the last hit
      tracks.push_back(track);
std::cout << "DEBUG MapCppGlobalTrackReconstructor::CorrelateTrackPoints: "
          << "completed a track with " << track.size() << "track points."
          << std::endl;
      track.clear();
      track.push_back(*event);
std::cout << "DEBUG MapCppGlobalTrackReconstructor::CorrelateTrackPoints: "
          << "pushed the following track point:" << std::endl
          << *event << std::endl;
    } else {
      track.push_back(*event);
std::cout << "DEBUG MapCppGlobalTrackReconstructor::CorrelateTrackPoints: "
          << "pushed the following track point:" << std::endl
          << *event << std::endl;
    }
    last_z = event->z();
  }
  if (track.size() > 0) {
    tracks.push_back(track);
std::cout << "DEBUG MapCppGlobalTrackReconstructor::CorrelateTrackPoints: "
          << "completed a track with " << track.size() << "track points."
          << std::endl;
  }
}
*/
bool MapCppGlobalTrackReconstructor::death() {
  return true;  // successful
}

CovarianceMatrix const MapCppGlobalTrackReconstructor::GetJsonCovarianceMatrix(
    Json::Value const & value) {
  if (value.size() < static_cast<Json::Value::UInt>(6)) {
    throw(Squeal(Squeal::recoverable,
                 "Not enough row elements to convert JSON to CovarianceMatrix",
                 "MapCppGlobalTrackReconstructor::GetJsonCovarianceMatrix()"));
  }

  const size_t size = 6;
  double matrix_data[size*size];
  for (size_t row = 0; row < 6; ++row) {
    const Json::Value row_json = value[row];
    if (row_json.size() < static_cast<Json::Value::UInt>(6)) {
      throw(Squeal(
          Squeal::recoverable,
          "Not enough column elements to convert JSON to CovarianceMatrix",
          "MapCppGlobalTrackReconstructor::GetJsonCovarianceMatrix()"));
    }
    for (size_t column = 0; column < 6; ++column) {
      const Json::Value element_json = row_json[column];
      matrix_data[row*size+column] = element_json.asDouble();
    }
  }

  return CovarianceMatrix(matrix_data);
}

Json::Value MapCppGlobalTrackReconstructor::TrackToJson(const Track & track) {
  Json::Value track_node;

  // track points
  Json::Value track_points;
  for (size_t index = 0; index < track.size(); ++index) {
    track_points.append(TrackPointToJson(track[index]));
  }
  track_node["track_points"] = track_points;

  // particle ID
  track_node["PID"] = Json::Value(track.particle_id());

  return track_node;
}

Json::Value MapCppGlobalTrackReconstructor::TrackPointToJson(
    const TrackPoint & track_point) {
  Json::Value track_point_node;

  // coordinates
  Json::Value position;
  position["x"] = track_point.x();
  position["y"] = track_point.y();
  position["z"] = track_point.z();
  track_point_node["position"] = position;

  Json::Value momentum;
  momentum["x"] = track_point.Px();
  momentum["y"] = track_point.Py();
  momentum["z"] = track_point.Pz();
  track_point_node["momentum"] = momentum;

  track_point_node["time"] = track_point.t();
  track_point_node["energy"] = track_point.E();

  // detector ID
  track_point_node["detector_id"] = Json::Value(track_point.detector_id());

  // uncertainty matrix
  Json::Value uncertainties;
  size_t size = track_point.uncertainties().size();
  for (size_t row = 1; row <= size; ++row) {
    Json::Value row_node;
    for (size_t column = 1; column <= size; ++column) {
      row_node.append(Json::Value(track_point.uncertainties()(row, column)));
    }
    uncertainties.append(row_node);
  }
  track_point_node["uncertainties"] = uncertainties;

  return track_point_node;
}

const std::string MapCppGlobalTrackReconstructor::kClassname
  = "MapCppGlobalTrackReconstructor";

}  // namespace MAUS

