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

#include "src/map/MapCppTrackReconstructor/MapCppTrackReconstructor.hh"

// C++
#include <vector>
#include <string>

// External
#include "TMinuit.h"
#include "json/json.h"

// Legacy/G4MICE
#include "Interface/Squeal.hh"
#include "Interface/dataCards.hh"

// MAUS
#include "src/common_cpp/Optics/CovarianceMatrix.hh"
//#include "src/common_cpp/Optics/LeastSquaresOpticsModel.hh"
#include "src/common_cpp/Optics/LinearApproximationOpticsModel.hh"
#include "src/common_cpp/Reconstruction/Detector.hh"
#include "src/common_cpp/Reconstruction/MinuitTrackFitter.hh"
#include "src/common_cpp/Reconstruction/Particle.hh"
#include "src/common_cpp/Reconstruction/ReconstructionInput.hh"
#include "src/common_cpp/Reconstruction/Track.hh"
#include "src/common_cpp/Reconstruction/TrackFitter.hh"
#include "src/common_cpp/Reconstruction/TrackPoint.hh"
#include "src/common_cpp/Simulation/MAUSGeant4Manager.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"

namespace MAUS {

MapCppTrackReconstructor::MapCppTrackReconstructor()
    : optics_model_(NULL), track_fitter_(NULL), reconstruction_input_(NULL) {
}

MapCppTrackReconstructor::~MapCppTrackReconstructor() {
  if (reconstruction_input_ != NULL) {
    delete reconstruction_input_;
  }

  if (optics_model_ != NULL) {
    delete optics_model_;
  }

  if (track_fitter_ != NULL) {
    delete track_fitter_;
  }
}

bool MapCppTrackReconstructor::birth(std::string configuration) {
std::cout << "Entering MapCppTrackReconstructor::birth()" << std::endl;
  // parse the JSON document.
  try {
    configuration_ = JsonWrapper::StringToJson(configuration);
    SetupOpticsModel();
    SetupTrackFitter();
  } catch(Squeal& squee) {
    MAUS::CppErrorHandler::getInstance()->HandleSquealNoJson(
      squee, MapCppTrackReconstructor::kClassname);
  } catch(std::exception& exc) {
    MAUS::CppErrorHandler::getInstance()->HandleStdExcNoJson(
      exc, MapCppTrackReconstructor::kClassname);
  }

std::cout << "Leaving MapCppTrackReconstructor::birth()" << std::endl;
  return true;  // Sucessful parsing
}

std::string MapCppTrackReconstructor::process(std::string run_data) {
std::cout << "Entering MapCppTrackReconstructor::process()" << std::endl;
  // parse the JSON document.
  try {
    run_data_ = Json::Value(JsonWrapper::StringToJson(run_data));
  } catch(Squeal& squee) {
    MAUS::CppErrorHandler::getInstance()->HandleSquealNoJson(squee, kClassname);
  } catch(std::exception& exc) {
    MAUS::CppErrorHandler::getInstance()->HandleStdExcNoJson(exc, kClassname);
  }

std::cout << "CHECKPOINT(-1) MapCppTrackReconstructor::process()" << std::endl;
  // Populate ReconstructionInput instance from JSON data
  Json::Value data_acquisition_mode_names = JsonWrapper::GetProperty(
      configuration_,
      "data_acquisition_modes",
      JsonWrapper::arrayValue);
  Json::Value data_acquisition_mode = JsonWrapper::GetProperty(
      configuration_,
      "data_acquisition_mode",
      JsonWrapper::stringValue);
  if (data_acquisition_mode == "Testing") {
    LoadTestingData();
  } else if (data_acquisition_mode == "Simulation") {
    LoadSimulationData();
  } else if (data_acquisition_mode == "Live") {
    LoadLiveData();
  } else {
    std::string message = "Invalid data acquisition mode: ";
    message += data_acquisition_mode.asString();
    throw(Squeal(Squeal::recoverable,
                 message,
                 "MapCppTrackReconstructor::process()"));
  }

  if (reconstruction_input_ == NULL) {
  Json::FastWriter writer;
  std::string output = writer.write(run_data_);
return output;
/*
    throw(Squeal(Squeal::recoverable,
                 "Null reconstruction input.",
                 "MapCppTrackReconstructor::process()"));
*/
  }

std::cout << "CHECKPOINT(-0.5) MapCppTrackReconstructor::process()" << std::endl;
  // TODO(plane1@hawk.iit.edu) Implement Kalman Filter and TOF track fitters
  //  in addition to Minuit, and select between them based on the configuration
 
  // associate tracks with individual particles
  std::vector<Track> tracks;
  CorrelateTrackPoints(tracks);
  
  int particle_id;
std::cout << "CHECKPOINT(0) MapCppTrackReconstructor::process()" << std::endl;
  if (reconstruction_input_->beam_polarity_negative()) {
    particle_id = Particle::kMuMinus;
  } else {
    particle_id = Particle::kMuPlus;
  }

  Json::Value global_tracks;

  // Find the best fit track for each particle traversing the lattice
  Track best_fit_track;
  for (std::vector<Track>::const_iterator measured_tracks = tracks.begin();
       measured_tracks < tracks.end();
       ++measured_tracks) {
    best_fit_track.set_particle_id(particle_id);
    best_fit_track.clear();
    track_fitter_->Fit(*measured_tracks, best_fit_track);

    // TODO(plane1@hawk.iit.edu) Reconstruct track at the desired locations
    //  specified in the configuration.

    reconstructed_tracks_.push_back(best_fit_track);
    global_tracks.append(TrackToJson(best_fit_track));
  }

  // TODO(plane1@hawk.iit.edu) Update the run data with reconstruction results.
  run_data_["global_tracks"] = global_tracks;

  // pass on the updated run data to the next map in the workflow
  Json::FastWriter writer;
  std::string output = writer.write(run_data_);

std::cout << "Leaving MapCppTrackReconstructor::process()" << std::endl;
  return output;
}

void MapCppTrackReconstructor::SetupOpticsModel() {
  Json::Value optics_model_names = JsonWrapper::GetProperty(
      configuration_,
      "reconstruction_optics_models",
      JsonWrapper::arrayValue);
  Json::Value optics_model_name = JsonWrapper::GetProperty(
      configuration_,
      "reconstruction_optics_model",
      JsonWrapper::stringValue);
  size_t model;
  for (model = 0; model < optics_model_names.size(); ++model) {
    if (optics_model_name == optics_model_names[model]) {
      break;  // leave the current index into optics_model_names in model
    }
  }
  
  switch (model) {
    case 0: {
      // "Differentiating"
      /* TODO(plane1@hawk.iit.edu)
      optics_model_ = new DifferentiatingOpticsModel();
      */
      throw(Squeal(Squeal::nonRecoverable,
                   "DifferentiatingOpticsModel is not yet implemented.",
                   "MapCppTrackReconstructor::process()"));
      break;
    }
    case 1: {
      // "Integrating"
      /* TODO(plane1@hawk.iit.edu)
      optics_model_ = new IntegratingOpticsModel();
      */
      throw(Squeal(Squeal::nonRecoverable,
                   "IntegratingOpticsModel is not yet implemented.",
                   "MapCppTrackReconstructor::SetupOpticsModel()()"));
      break;
    }
    case 2: {
    /*
      // "Least Squares"
      Json::Value algorithm_names = JsonWrapper::GetProperty(
          configuration_,
          "LeastSquaresOpticsModel_algorithms",
          JsonWrapper::stringValue);
      Json::Value algorithm_name = JsonWrapper::GetProperty(
          configuration_,
          "LeastSquaresOpticsModel_algorithm",
          JsonWrapper::stringValue);
      int algorithm_number;
      for (algorithm_number = 0;
           algorithm_number < algorithm_names.size();
           ++algorithm_number) {
        if (algorithm_name == algorithm_names[algorithm_number]) {
          // leave the current index into algorithm_names in algorithm_number
          break;
        }
      }

      Algorithm algorithm;

      switch (algorithm_number) {
        case 4: algorithm = Algorithm::kSweepingChiSquaredWithVariableWalls;
                break;
        case 3: algorithm = Algorithm::kSweepingChiSquared;       break;
        case 2: algorithm = Algorithm::kConstrainedChiSquared;    break;
        case 1: algorithm = Algorithm::kConstrainedPolynomial;    break;
        case 0:
        default: algorithm = Algorithm::kUnconstrainedPolynomial;      
      }
      optics_model_ = new LeastSquaresOpticsModel(algorithm);
    */
      break;
    }
    case 3: {
      // "Runge Kutta"
      /* TODO(plane1@hawk.iit.edu)
      optics_model_ = new RungeKuttaOpticsModel();
      */
      throw(Squeal(Squeal::nonRecoverable,
                   "RungeKuttaOpticsModel is not yet implemented.",
                   "MapCppTrackReconstructor::SetupOpticsModel()()"));
      break;
    }
    case 4: {
      // "Linear Approximation"
      optics_model_ = new LinearApproximationOpticsModel();
      break;
    }
    default: {
      std::string message("Unsupported optics model \"");
      message += optics_model_name.asString();
      message += std::string("\" in reconstruction configuration.");
      throw(Squeal(Squeal::nonRecoverable,
                   message.c_str(),
                   "MapCppTrackReconstructor::SetupOpticsModel()()"));
    }
  }
  optics_model_->Build(configuration_);
}

void MapCppTrackReconstructor::SetupTrackFitter() {
  Json::Value fitter_names = JsonWrapper::GetProperty(
      configuration_,
      "reconstruction_track_fitters",
      JsonWrapper::arrayValue);
  Json::Value fitter_name = JsonWrapper::GetProperty(
      configuration_,
      "reconstruction_track_fitter",
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
      double start_plane = 0.0;
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
                   "MapCppTrackReconstructor::SetupTrackFitter()"));
      break;
    }
    case 2: {
      // TOF
      /* TODO(plane1@hawk.iit.edu)
      track_fitter_ = new TOFTrackFitter(optics_model_);
      */
      throw(Squeal(Squeal::nonRecoverable,
                   "TOFTrackFitter is not yet implemented.",
                   "MapCppTrackReconstructor::SetupTrackFitter()"));
      break;
    }
    default: {
      std::string message("Unsupported track fitter \"");
      message += fitter_name.asString();
      message += std::string("\" in reconstruction configuration.");
      throw(Squeal(Squeal::nonRecoverable,
                   message.c_str(),
                   "MapCppTrackReconstructor::SetupTrackFitter()"));
    }
  }
}

void MapCppTrackReconstructor::LoadTestingData() {
std::cout << "Entering MapCppTrackReconstructor::LoadTestingData()" << std::endl;
  // TODO(plane1@hawk.iit.edu) reconstruction_input_ = new ReconstructionInput(...);
  // Create random track points for TOF0, TOF1, Tracker 1, Tracker 2, and TOF 2
  srand((unsigned)time(NULL));

  const bool beam_polarity_negative = true;  // e-, mu-, pi-

  std::vector<Detector> detectors;
  double plane;
  double uncertainty_data[36];
  std::vector<TrackPoint> events;
  double position[4], momentum[4];

  // generate mock detector info and random muon detector event data
  for (size_t id = Detector::kTOF0; id <= Detector::kCalorimeter; ++id) {
    plane = ((double)rand()/(double)RAND_MAX) * 20;  // 0.0 - 20.0 meters
    for (int index = 0; index < 36; ++index) {
      uncertainty_data[index] = ((double)rand()/(double)RAND_MAX) * 100;
    }
    CovarianceMatrix uncertainties(uncertainty_data);
    Detector detector(id, plane, uncertainties);
    detectors.push_back(detector);

std::cout << "CHECKPOINT(0) MapCppTrackReconstructor::LoadTestingData()" << std::endl;

    // skip detectors we're not using
    if ((id == Detector::kCherenkov1) ||
        (id == Detector::kCherenkov2) ||
        (id == Detector::kCalorimeter)) {
      continue;
    }

    for (int coordinate = 0; coordinate < 4; ++coordinate) {
      position[coordinate] = ((double)rand()/(double)RAND_MAX) * 20;  // meters
    }

    for (int coordinate = 0; coordinate < 4; ++coordinate) {
      momentum[coordinate] = ((double)rand()/(double)RAND_MAX) * 500;  // MeV
    }

std::cout << "CHECKPOINT(+1) MapCppTrackReconstructor::LoadTestingData()" << std::endl;

    TrackPoint track_point(position[0], momentum[0],
                           position[1], momentum[1],
                           position[2], momentum[2],
                           position[3], momentum[3],
                           detector);
std::cout << "CHECKPOINT(+1.25) MapCppTrackReconstructor::LoadTestingData()" << std::endl;
    events.push_back(track_point);
std::cout << "CHECKPOINT(+1.5) MapCppTrackReconstructor::LoadTestingData()" << std::endl;
  }


std::cout << "CHECKPOINT(+2) MapCppTrackReconstructor::LoadTestingData()" << std::endl;
  reconstruction_input_ = new ReconstructionInput(beam_polarity_negative,
                                                  detectors,
                                                  events);
std::cout << "Leaving MapCppTrackReconstructor::LoadTestingData()" << std::endl;
}

void MapCppTrackReconstructor::LoadSimulationData() {
  try {
    // MapCppSimulation puts the simulated spill in run_data_["mc"]
    Json::Value mc = JsonWrapper::GetProperty(run_data_,
                                              "mc",
                                              JsonWrapper::arrayValue);
    //mc --> Json::Value[mc.size()] --> "primary"

    // TODO(plane1@hawk.iit.edu) Collect tracks from the detectors and
    // dynamically allocate the ReconstructionInput instance
    // reconstruction_input_ = new ReconstructionInput(...);
  } catch(Squeal& squee) {
    run_data_ = MAUS::CppErrorHandler::getInstance()->HandleSqueal(
        run_data_, squee, "MAUS::MapCppTrackReconstructor");
  } catch(std::exception& exc) {
    run_data_ = MAUS::CppErrorHandler::getInstance()->HandleStdExc(
        run_data_, exc, "MAUS::MapCppTrackReconstructor");
  }
}

void MapCppTrackReconstructor::LoadLiveData() {
  // reconstruction_input_ = new ReconstructionInput(...);
}


void MapCppTrackReconstructor::CorrelateTrackPoints(
    std::vector<Track> & tracks) {
  const std::vector<TrackPoint> & track_points
      = reconstruction_input_->events();

  // TODO(plane1@hawk.iit.edu) create sets of events where each set corresponds
  // to a different particle. For now assume that all events are from a single
  // particle.
  tracks.push_back(Track(track_points, Particle::kNone));
}

bool MapCppTrackReconstructor::death() {
  return true;  // successful
}

Json::Value MapCppTrackReconstructor::TrackToJson(const Track & track) {
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

Json::Value MapCppTrackReconstructor::TrackPointToJson(
    const TrackPoint & track_point) {
  Json::Value track_point_node;

  // coordinates
  Json::Value coordinates;
  coordinates["t"] = track_point.t();
  coordinates["E"] = track_point.E();
  coordinates["x"] = track_point.x();
  coordinates["Px"] = track_point.Px();
  coordinates["y"] = track_point.y();
  coordinates["Py"] = track_point.Py();
  coordinates["z"] = track_point.z();
  coordinates["Pz"] = track_point.Pz();
  coordinates["Pz"] = track_point.Pz();
  track_point_node["coordinates"] = coordinates;
  
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

const std::string MapCppTrackReconstructor::kClassname
  = "MapCppTrackReconstructor";

}  // namespace MAUS

