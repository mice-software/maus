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
#include "src/common_cpp/JsonCppProcessors/GlobalRawTrackProcessor.hh"
#include "src/common_cpp/DataStructure/GlobalRawTrack.hh"
#include "src/common_cpp/DataStructure/GlobalTrackPoint.hh"
#include "src/common_cpp/DataStructure/ThreeVector.hh"
#include "src/common_cpp/Optics/CovarianceMatrix.hh"
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

    LoadRawTracks();
  } catch(Squeal& squee) {
    MAUS::CppErrorHandler::getInstance()->HandleSquealNoJson(squee, kClassname);
  } catch(std::exception& exc) {
    MAUS::CppErrorHandler::getInstance()->HandleStdExcNoJson(exc, kClassname);
  }

std::cout << "DEBUG MapCppGlobalTrackReconstructor::process(): "
          << "Loaded " << raw_tracks_.size() << " tracks." << std::endl;
  if (raw_tracks_.size() == 0) {
/*
    Json::FastWriter writer;
    std::string output = writer.write(run_data_);
    return output;
*/
    throw(Squeal(Squeal::recoverable,
                 "Null recon input.",
                 "MapCppGlobalTrackReconstructor::process()"));
  }

  Json::Value global_tracks;

  // Find the best fit track for each particle traversing the lattice
  size_t track_count = 0;
  for (std::vector<Track>::const_iterator raw_track = raw_tracks_.begin();
       raw_track < raw_tracks_.end();
       ++raw_track) {
    // FIXME(Lane) Assuming the track doesn't decay for now.
    Particle::ID particle_id = (*raw_track)[0].particle_id();

    MAUS::recon::global::Track best_fit_track(particle_id);
    track_fitter_->Fit(*raw_track, best_fit_track);
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

bool MapCppGlobalTrackReconstructor::death() {
  return true;  // successful
}

void MapCppGlobalTrackReconstructor::LoadRawTracks() {
  LoadDetectorConfiguration();

  Json::Value global_raw_tracks = JsonWrapper::GetProperty(
      run_data_, "global_raw_tracks", JsonWrapper::arrayValue);
  size_t num_tracks = global_raw_tracks.size();
  for (size_t track_index = 0; track_index < num_tracks; ++track_index) {
    GlobalRawTrackProcessor deserializer;
    GlobalRawTrack * const global_raw_track = deserializer.JsonToCpp(
      global_raw_tracks[track_index]);
    GlobalTrackPointArray global_track_points
      = global_raw_track->track_points();
    delete global_raw_track;

    Track raw_track;
    for (GlobalTrackPointArray::const_iterator global_track_point
            = global_track_points.begin();
         global_track_point < global_track_points.end();
         ++global_track_point) {
      ThreeVector position = global_track_point->position();
      ThreeVector momentum = global_track_point->momentum();
      const Detector& detector = detectors_.find(
        Detector::ID(global_track_point->detector_id()))->second;

      TrackPoint track_point(global_track_point->time(),
                             global_track_point->energy(),
                             position.x(), momentum.x(),
                             position.y(), momentum.y(),
                             detector);
      track_point.set_particle_id(
        Particle::ID(global_track_point->particle_id()));
      raw_track.push_back(track_point);
    }
    raw_tracks_.push_back(raw_track);
  }
}

void MapCppGlobalTrackReconstructor::LoadDetectorConfiguration() {
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
      detectors_.insert(std::pair<Detector::ID, Detector>(id, detector));
    }
  } catch(Squeal& squee) {
    run_data_ = MAUS::CppErrorHandler::getInstance()->HandleSqueal(
        run_data_, squee,
        "MAUS::MapCppGlobalTrackReconstructor::LoadDetectorConfiguration()");
  } catch(std::exception& exc) {
    run_data_ = MAUS::CppErrorHandler::getInstance()->HandleStdExc(
        run_data_, exc,
        "MAUS::MapCppGlobalTrackReconstructor::LoadDetectorConfiguration()");
  }
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

